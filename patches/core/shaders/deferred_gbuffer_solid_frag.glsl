////////////////////////////////////////////////////////
// Deferred G-Buffer Solid - Fragment Shader
//
//
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

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
// Relief mapping helpers

//Optimize by getting 4 depths at once somehow?
void RayLinearIntersectionSM2(sampler2D aHeightMap, inout vec3 avPosition, inout vec3 avEyeVec)
{
    const int lSearchSteps = 8;

    avEyeVec /= lSearchSteps;     //Split up the eye vector into the number of steps

    for(int i=0; i<lSearchSteps-1; i++)
    {
        float fDepth = texture2D(aHeightMap, avPosition.xy).w;
        if(avPosition.z < fDepth) avPosition += avEyeVec;
    }
}

void RayLinearIntersectionSM3(sampler2D aHeightMap, float afSearchSteps, inout vec3 avPosition, inout vec3 avEyeVec)
{
    avEyeVec /= afSearchSteps;     //Split up the eye vector into the number of steps

    for(int i=0; i<afSearchSteps-1; i++)
    {
        float fDepth = texture2D(aHeightMap, avPosition.xy).w;
        if(avPosition.z < fDepth) avPosition += avEyeVec;
    }
}

void RayBinaryIntersection(sampler2D aHeightMap, inout vec3 avPosition, inout vec3 avEyeVec)
{
    const int lSearchSteps = 6;
    for(int i=0; i<lSearchSteps; i++)
    {
        float fDepth = texture2D(aHeightMap, avPosition.xy).w;
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
    //////////////////////////////////
    //Diffuse
    @ifdef UseParallax
        //RELIEF PARALLAX:
        @ifdef ParallaxMethod_Relief
            vec3 vEyeVec = normalize(gvTangentEyePos);

            //roozy: Fix hardware freeze from dividing by zero in the eye vector Z
            float fEyeVectorZ = sign(vEyeVec.z) * max(abs(vEyeVec.z), 0.05);

            //Get give eyevec the length so it reaches bottom.
            vEyeVec *= 1.0 / fEyeVectorZ;
            
            //Apply scale and bias
            vEyeVec.xy *= avHeightMapScaleAndBias.x;
            //vec2 vBiasPosOffset = vEyeVec.xy * avHeightMapScaleAndBias.y; <- not working! because the ray casting buggers out when u are really close!
            
            //Get the postion to start the search from. 0 since we start at the top.
            vec3 vHeightMapPos = vec3(gl_TexCoord[0].xy, 0.0); 
            
            //Determine number of steps based on angle between normal and eye
            float fSteps = clamp(floor(( 1 - dot(vEyeVec, normalize(gvNormal)) ) * 18.0) + 2.0, 2.0, 20.0); 
            
            //Do a linear search to find the first intersection.
            RayLinearIntersectionSM2(aHeightMap, vHeightMapPos, vEyeVec);
            
            //Do a binary search between start and first intersection to pinpoint the postion.        
            RayBinaryIntersection(aHeightMap,vHeightMapPos,  vEyeVec);
            
            vec2 vTexCoord = vHeightMapPos.xy;
        @else    
            vec3 vEyeVec = normalize(gvTangentEyePos);
            float fHeight = texture2D(aHeightMap, gl_TexCoord[0].xy).w;

            float fDisplacement = fHeight * avHeightMapScaleAndBias.x;

            vec2 vTexCoord = (vEyeVec * fDisplacement + gl_TexCoord[0].xyz).xy;
        @endif

        vec4 vDiffuseColor = texture2D(aDiffuseMap, vTexCoord);

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
        vec3 vNormal = texture2D(aNormalMap,vTexCoord).xyz - 0.5; //No need for full unpack x*2-1, becuase normal is normalized. (but now we do not normalize...)
        vec3 vScreenNormal = normalize(vNormal.x * gvTangent + vNormal.y * gvBinormal + vNormal.z * gvNormal);
    @else
        vec3 vScreenNormal = normalize(gvNormal);
    @endif

    //If 32 bit we need to pack, else no packing is needed.
    @ifdef Deferred_32bit
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
            float fEnvMapAlpha = texture2D(aEnvMapAlphaMap, vTexCoord).w;
            vReflectionColor *= fEnvMapAlpha;
        @endif

        gl_FragData[0] = vDiffuseColor + vReflectionColor * fFresnel;
    @endif

    //////////////////////////////////
    //Depth
    @ifdef Deferred_32bit
        gl_FragData[2].xyz = PackFloatInVec3(gfLinearDepth);
    @elseif Deferred_64bit
        gl_FragData[2].xyz = gvVertexPos;
    @endif

    //////////////////////////////////
    //Specular
    @ifdef RenderTargets_4
        @ifdef UseSpecular
            gl_FragData[3].xy = texture2D(aSpecularMap, vTexCoord).xy;
        @else
            gl_FragData[3].xy = vec2(0.0);
        @endif
    @else
        @ifdef UseSpecular
            vec2 vSpecVals = texture2D(aSpecularMap, vTexCoord).xy;
            gl_FragData[1].w = vSpecVals.x;
            gl_FragData[2].w = vSpecVals.y;
        @else
            gl_FragData[1].w = 0.0;
            gl_FragData[2].w = 0.0;
        @endif
    @endif
}