////////////////////////////////////////////////////////
// Deferred G-Buffer Solid - Fragment Shader
//
//
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable
@ifdef UseParallax && Feature_TextureLod
#extension GL_ARB_shader_texture_lod : enable
@endif

@include helper_float_packing.glsl
@include helper_reflection.glsl

////////////////////
//Normal interpolated values
varying vec3 gvNormal;

@ifdef UseNormalMapping
    varying vec3 gvTangent;
    varying vec3 gvBinormal;
@endif

/////////////////////
//Extra interpolated values

//32 bit G-Buffer
@ifdef Deferred_32bit
    varying float gfLinearDepth;
@endif

//64 bit G-Buffer
@ifdef Deferred_64bit || UseEnvMap
    varying vec3 gvVertexPos;
@endif

////////////////////
//Textures
uniform sampler2D aDiffuseMap;
@define sampler_aDiffuseMap 0

@ifdef UseNormalMapping
    uniform sampler2D aNormalMap;
    @define sampler_aNormalMap 1
@endif

@ifdef UseSpecular
    uniform sampler2D aSpecularMap;
    @define sampler_aSpecularMap 2
@endif

@ifdef UseParallax
    uniform sampler2D aHeightMap;
    @define sampler_aHeightMap 3

    uniform vec2 avHeightMapScaleAndBias;

    varying vec3 gvTangentEyePos;
@endif

@ifdef UseEnvMap
    uniform samplerCube aEnvMap;
    @define sampler_aEnvMap 4

    uniform vec2 avFresnelBiasPow;
    uniform mat4 a_mtxInvViewRotation;
@endif

@ifdef UseCubeMapAlpha
    uniform sampler2D aEnvMapAlphaMap;
    @define sampler_aEnvMapAlphaMap 5
@endif

//--------------------------------------------------

///////////////////////////////
// Texture sampling with parallax
//
// Parallax moves the texture coordinates per pixel, and at the edges of relief steps
// neighbouring pixels can be far apart in the texture. Mip-mapping then sees a huge
// texture coordinate change and picks a tiny mip level, which gives blocky 2x2 pixel
// edges. With GL_ARB_shader_texture_lod the textures are sampled with the derivatives
// of the unmodified coordinates instead, so the mip level is the same as without parallax.
// Without it, this is plain texture2D() (same as before).

@ifdef UseParallax && Feature_TextureLod
    vec2 mvUvDx;
    vec2 mvUvDy;
@endif

vec4 SampleUv(sampler2D aMap, vec2 avUv)
{
    @ifdef UseParallax && Feature_TextureLod
        return texture2DGradARB(aMap, avUv, mvUvDx, mvUvDy);
    @else
        return texture2D(aMap, avUv);
    @endif
}

//--------------------------------------------------

///////////////////////////////
// Relief mapping helpers

@ifdef UseParallax && Feature_TextureLod
    //Linear search with the number of steps picked by view angle: few steps when
    //looking straight at the surface, many at grazing angles where the ray is long.
    //(Dynamic loop: fine here, since the gradients are given explicitly.)
    void RayLinearIntersection(sampler2D aHeightMap, float afViewZ, inout vec3 avPosition, inout vec3 avEyeVec)
    {
        const int lMaxSearchSteps = 32;
        float fSearchSteps = floor(mix(float(lMaxSearchSteps), 8.0, afViewZ));

        avEyeVec /= fSearchSteps;     //Split up the eye vector into the number of steps

        for(int i=0; i<lMaxSearchSteps; i++)
        {
            if(float(i) >= fSearchSteps-1.0) break;

            float fDepth = SampleUv(aHeightMap, avPosition.xy).w;
            if(avPosition.z < fDepth) avPosition += avEyeVec;
        }
    }
@else
    //Fixed number of steps (unrolled), for cards without explicit gradients.
    void RayLinearIntersection(sampler2D aHeightMap, float afViewZ, inout vec3 avPosition, inout vec3 avEyeVec)
    {
        const int lSearchSteps = 16;

        avEyeVec /= float(lSearchSteps);     //Split up the eye vector into the number of steps

        for(int i=0; i<lSearchSteps-1; i++)
        {
            float fDepth = SampleUv(aHeightMap, avPosition.xy).w;
            if(avPosition.z < fDepth) avPosition += avEyeVec;
        }
    }
@endif

void RayBinaryIntersection(sampler2D aHeightMap, inout vec3 avPosition, inout vec3 avEyeVec)
{
    const int lSearchSteps = 6;
    for(int i=0; i<lSearchSteps; i++)
    {
        float fDepth = SampleUv(aHeightMap, avPosition.xy).w;
        if(avPosition.z < fDepth)
            avPosition += avEyeVec;

        avEyeVec *= 0.5;
        avPosition -= avEyeVec;
    }
}

//--------------------------------------------------

///////////////////////////////
// Main program
void main()
{
    @ifdef UseParallax && Feature_TextureLod
        mvUvDx = dFdx(gl_TexCoord[0].xy);
        mvUvDy = dFdy(gl_TexCoord[0].xy);
    @endif

    //////////////////////////////////
    //Diffuse
    @ifdef UseParallax
        //SIMPLE PARALLAX, FORCED BY THE MATERIAL:
        //(height map scale outside the range relief works with, see MaterialType_BasicSolid.cpp)
        @ifdef UseSimpleParallax
            vec3 vEyeVec = normalize(gvTangentEyePos);
            float fHeight = texture2D(aHeightMap, gl_TexCoord[0].xy).w;

            float fDisplacement = fHeight * avHeightMapScaleAndBias.x;

            vec2 vTexCoord = (vEyeVec * fDisplacement + gl_TexCoord[0].xyz).xy;

        //RELIEF PARALLAX:
        @elseif ParallaxMethod_Relief
            vec3 vEyeVec = normalize(gvTangentEyePos);

            float fViewZ = abs(vEyeVec.z);

            //roozy: Fix hardware freeze from dividing by zero in the eye vector Z
            //(sign() is 0 for 0, which still divided by zero, so pick the sign by hand.)
            //The limit also caps how far the ray goes at grazing angles (at most ~6.7x the
            //height map scale, was 20x), which is where relief steps and swimming were worst.
            float fEyeVectorZ = (vEyeVec.z < 0.0 ? -1.0 : 1.0) * max(fViewZ, 0.15);

            //Get give eyevec the length so it reaches bottom.
            vEyeVec *= 1.0 / fEyeVectorZ;
            
            //Apply scale and bias
            vEyeVec.xy *= avHeightMapScaleAndBias.x;
            //vec2 vBiasPosOffset = vEyeVec.xy * avHeightMapScaleAndBias.y; <- not working! because the ray casting buggers out when u are really close!
            
            //Get the postion to start the search from. 0 since we start at the top.
            vec3 vHeightMapPos = vec3(gl_TexCoord[0].xy, 0.0); 
            
            
            //Do a linear search to find the first intersection.
            RayLinearIntersection(aHeightMap, fViewZ, vHeightMapPos, vEyeVec);
            
            //Do a binary search between start and first intersection to pinpoint the postion.        
            RayBinaryIntersection(aHeightMap,vHeightMapPos,  vEyeVec);
            
            vec2 vTexCoord = vHeightMapPos.xy;
        @else    
            vec3 vEyeVec = normalize(gvTangentEyePos);
            float fHeight = texture2D(aHeightMap, gl_TexCoord[0].xy).w;

            float fDisplacement = fHeight * avHeightMapScaleAndBias.x;

            vec2 vTexCoord = (vEyeVec * fDisplacement + gl_TexCoord[0].xyz).xy;
        @endif

        vec4 vDiffuseColor = SampleUv(aDiffuseMap, vTexCoord);

    @else
        vec2 vTexCoord = gl_TexCoord[0].xy;
        vec4 vDiffuseColor = texture2D(aDiffuseMap, vTexCoord);
    @endif


    //////////////////////////////////
    //Set Diffuse color if no environemnt mapping is used.
    @ifdef UseEnvMap
    @else
        gl_FragData[0] = vDiffuseColor;
    @endif

    //////////////////////////////////
    //Normal
    @ifdef UseNormalMapping
        vec3 vNormal = SampleUv(aNormalMap, vTexCoord).xyz - 0.5; //No need for full unpack x*2-1, becuase normal is normalized. (but now we do not normalize...)
        vec3 vScreenNormal = normalize(vNormal.x * gvTangent + vNormal.y * gvBinormal + vNormal.z * gvNormal);
    @else
        vec3 vScreenNormal = normalize(gvNormal);
    @endif

    //If 32 bit we need to pack, else no packing is needed.
    @ifdef GBufferLite || GBufferAlbedoOnly
    @elseif Deferred_32bit
        gl_FragData[1].xyz = vScreenNormal*0.5 + 0.5;
    @elseif Deferred_64bit
        gl_FragData[1].xyz = vScreenNormal;
    @endif

    //////////////////////////////////
    //Environment Map
    @ifdef UseEnvMap
        vec3 vCameraSpaceEyeVec = normalize(gvVertexPos);

        vec3 vEnvUv = reflect(vCameraSpaceEyeVec, vScreenNormal);
        vEnvUv = (a_mtxInvViewRotation * vec4(vEnvUv,1)).xyz;

        vec4 vReflectionColor = textureCube(aEnvMap, vEnvUv);

        float afEDotN = max(dot(-vCameraSpaceEyeVec, vScreenNormal),0.0);
        float fFresnel = Fresnel(afEDotN, avFresnelBiasPow.x, avFresnelBiasPow.y);

        @ifdef UseCubeMapAlpha
            float fEnvMapAlpha = SampleUv(aEnvMapAlphaMap, vTexCoord).w;
            vReflectionColor *= fEnvMapAlpha;
        @endif

        gl_FragData[0] = vDiffuseColor + vReflectionColor * fFresnel;
    @endif

    //////////////////////////////////
    //Depth
    @ifdef GBufferAlbedoOnly
    @elseif GBufferLiteSparse
        gl_FragData[2] = vec4(PackFloatInVec3(gfLinearDepth), 0.0);
    @elseif GBufferLite
        gl_FragData[1] = vec4(PackFloatInVec3(gfLinearDepth), 0.0);
    @elseif Deferred_32bit
        gl_FragData[2].xyz = PackFloatInVec3(gfLinearDepth);
    @elseif Deferred_64bit
        gl_FragData[2].xyz = gvVertexPos;
    @endif

    //////////////////////////////////
    //Specular
    @ifdef GBufferLite || GBufferAlbedoOnly
    @elseif RenderTargets_4
        @ifdef UseSpecular
            gl_FragData[3].xy = SampleUv(aSpecularMap, vTexCoord).xy;
        @else
            gl_FragData[3].xy = vec2(0.0);
        @endif
    @else
        @ifdef UseSpecular
            vec2 vSpecVals = SampleUv(aSpecularMap, vTexCoord).xy;
            gl_FragData[1].w = vSpecVals.x;
            gl_FragData[2].w = vSpecVals.y;
        @else
            gl_FragData[1].w = 0.0;
            gl_FragData[2].w = 0.0;
        @endif
    @endif
}