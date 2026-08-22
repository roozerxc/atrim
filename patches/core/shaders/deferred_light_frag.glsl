////////////////////////////////////////////////////////
// Deferred G-Buffer Light - Fragment Shader
//
// A shader applied to the accumlation buffer, using the
// G-buffer targets as input.
//
// Postion retrival from depth:
// The position gvFarPlanePos is at the far plane and the far x,y and z postions can be thought of as a "trinagle"
// The wanted position can also be thought of as a "triangle" with the same ratio as the far one. The ratio is expressed as
// the stored depth value since it is wanted_pos.z/farplane.
//
////////////////////////////////////////////////////////

#version 120

#extension GL_ARB_texture_rectangle : enable

@include helper_float_packing.glsl

//--------------------------------------------------------------


///////////////////////////////
// Shadow helper functions

float ShadowOffsetLookup(sampler2DShadow aShadowMap, vec4 avLocation, vec2 avOffset)
{
    return shadow2DProj(aShadowMap, vec4(avLocation.xy + (avOffset * avLocation.w), avLocation.z, avLocation.w) ).x;
}


//--------------------------------------------------------------



////////////////////
//Varying varaibles
varying vec3 gvFarPlanePos;    //The pixel postion projected to the far plane

@ifdef UseBatching
varying vec3 gvLightPosition;
varying vec4 gvLightColor;
varying float gfLightRadius;
@endif

////////////////////
//Uniform varaibles
@ifdef UseBatching
//Do nothing
@else
    uniform vec3 avLightPos;
uniform float afInvLightRadius;
uniform vec4 avLightColor;
uniform float afFalloff;
@endif

//Division is done with farplane
@ifdef DivideInFrag
uniform float afNegFarPlane;
@endif

uniform float afComplexity;

//Spotlight specfics
@ifdef LightType_Spot
    @ifdef UseGobo || UseShadowMap
        uniform mat4 a_mtxSpotViewProj;
    @endif

    @ifdef UseGobo
    @else
        uniform float afOneMinusCosHalfSpotFOV;
        uniform vec3 avLightForward;
    @endif

    @ifdef UseShadowMap
        @ifdef ShadowMapQuality_Low
        @else
            uniform vec2 avShadowMapOffsetMul;
        @endif
    @endif
//Point specfics
@else
    @ifdef UseGobo
        uniform mat4 a_mtxInvViewRotation;
    @endif
@endif

////////////////////
//Textures
uniform sampler2DRect  aDiffuseMap;
@define sampler_aDiffuseMap 0

uniform sampler2DRect  aNormalMap;
@define sampler_aNormalMap 1

uniform sampler2DRect  aDepthMap;
@define sampler_aDepthMap 2

@ifdef RenderTargets_4
    uniform sampler2DRect  aExtraMap;
    @define sampler_aExtraMap 3
@endif

uniform sampler1D  aAttenuationMap;
@define sampler_aAttenuationMap 4

@ifdef UseSSAO
    uniform sampler2DRect  aSSAOMap;
    @define sampler_aSSAOMap 5
@endif

////////////////////////////
//Additional textures

//Spot light
@ifdef LightType_Spot
    @ifdef UseGobo
        uniform sampler2D aGoboMap;
        @define sampler_aGoboMap 5
    @else
        uniform sampler1D aSpotFalloffMap;
        @define sampler_aSpotFalloffMap 5
    @endif
//Point light
@else
    uniform samplerCube aGoboMap;
    @define sampler_aGoboMap 5
@endif

//Shadow map
@ifdef UseShadowMap
    uniform sampler2DShadow aShadowMap;
    @define sampler_aShadowMap 6

    @ifdef ShadowMapQuality_Low
    @else
        uniform sampler2D aShadowOffsetMap;
        @define sampler_aShadowOffsetMap 7
    @endif
@endif

//--------------------------------------------------------------

///////////////////////////////
// Main program
void main()
{
    /////////////////////////////////
    //Get values from samplers
    vec2 vMapCoords = gl_FragCoord.xy;
    vec4 vColorVal = pow(texture2DRect(aDiffuseMap, vMapCoords), vec4(2.2, vec3(2.4, 1.9, 0.1)));
    //vec4 vColorVal =  texture2DRect(aDiffuseMap, vMapCoords);
    vec4 vNormalVal = texture2DRect(aNormalMap, vMapCoords);
    vec4 vDepthVal =  texture2DRect(aDepthMap, vMapCoords);
    
    @ifdef UseSSAO
        vColorVal *= texture2DRect(aSSAOMap, vMapCoords * 0.5);    //SSAO should be half the size of the screen.
    @endif

    @ifdef RenderTargets_4
        vec4 vExtraVal = texture2DRect(aExtraMap, vMapCoords);
    @endif

    /////////////////////////////////
    //When using batching, set up variables differently
    @ifdef UseBatching
        vec3 avLightPos = gvLightPosition;
        float afInvLightRadius = gfLightRadius;
        vec4 avLightColor = gvLightColor;
        float afFalloff = 1.0;
    @endif

    /////////////////////////////////
    // Get postion

    //32 bit has packed depth
    @ifdef Deferred_32bit
        float fDepth = UnpackVec3ToFloat(vDepthVal.xyz);

        @ifdef DivideInFrag
            vec3 vPos;
            vPos.xy = (gvFarPlanePos.xy / gvFarPlanePos.z);
            vPos.z =  afNegFarPlane;
            vPos.xyz *= fDepth;
        @else
            vec3 vPos = gvFarPlanePos * fDepth;
        @endif
    //64 bit stores postion directly
    @elseif Deferred_64bit
        vec3 vPos = vDepthVal.xyz;
    @endif


    /////////////////////////////////
    // Light direction and attenuation
    vec3 vLightDir = (avLightPos - vPos)*afInvLightRadius;
    float fAttenuatuion =  texture1D(aAttenuationMap,dot(vLightDir,vLightDir)).x;
    vLightDir = normalize( vLightDir );

    //////////////////////////////
    //Spot attentuation / gobo
    @ifdef LightType_Spot
        @ifdef UseGobo
            vec4 vProjectedUv = a_mtxSpotViewProj * vec4(vPos,1.0);
            vec3 vGoboVal = texture2DProj(aGoboMap, vProjectedUv).xyz;
        @else
            float fOneMinusCos = 1.0 - dot( vLightDir,  avLightForward);
            fAttenuatuion *= texture1D(aSpotFalloffMap, fOneMinusCos / afOneMinusCosHalfSpotFOV).x;
        @endif
    //////////////////////////////
    //Point gobo
    @else
        @ifdef UseGobo
            vec4 vWorldLightDir = a_mtxInvViewRotation * vec4(vLightDir,1.0);
            vec3 vGoboVal = textureCube(aGoboMap, vWorldLightDir.xyz).xyz;
        @endif
    @endif

    /////////////////////////////////
    //Unpack normal and normalize (if needed)
    @ifdef Deferred_32bit
        vec3 vNormal = vNormalVal.xyz*2.0 - 1.0;
    @elseif Deferred_64bit
        vec3 vNormal = vNormalVal.xyz;
    @endif

    @ifdef UseSpecular
    vNormal = normalize(vNormal);
    @endif

    /////////////////////////////////
    //Calculate diffuse color
    float fLDotN = max( dot( vLightDir, vNormal.xyz), 0.0);
    vec3 vDiffuse = pow(vColorVal.xyz * avLightColor.xyz * fLDotN, vec3(1.0 / 1.6));
    //vec3 vDiffuse = vColorVal.xyz * avLightColor.xyz * fLDotN;

    /////////////////////////////////
    //Calculate specular color
    @ifdef UseSpecular
        @ifdef RenderTargets_4
            float fSpecIntensity = vExtraVal.x;
            float fSpecPower = vExtraVal.y;
        @else
            float fSpecIntensity = vNormalVal.w;
            float fSpecPower = vDepthVal.w;
        @endif

        vec3 vHalfVec = normalize(vLightDir + normalize(-vPos));
        fSpecPower = exp2(fSpecPower * 10.0) + 1.0;//Range 0 - 1024
        vec3 vSpecular = vec3(avLightColor.w * fSpecIntensity *  pow( clamp( dot( vHalfVec, vNormal.xyz), 0.0, 1.0),fSpecPower ) );
        vSpecular *= avLightColor.xyz;
    @endif

    /////////////////////////////////
    // Caclulate shadow (if any)

    @ifdef UseShadowMap && LightType_Spot
        @ifdef UseGobo
        @else
            vPos += vNormal * (1 - dot(vNormal, vLightDir)) * 0.03;
            vec4 vProjectedUv = a_mtxSpotViewProj * vec4(vPos,1.0);
        @endif

        ////////////////////////
        // No Smoothing
        @ifdef ShadowMapQuality_Low
    
        fAttenuatuion *= shadow2DProj(aShadowMap, vProjectedUv).x;

        ///////////////////////
        // Smoothing
        @else
            //Set up variables
            float fShadowSum = 0;
            float fJitterZ =0;
            vec2 vScreenJitterCoord = gl_FragCoord.xy * $ShadowJitterLookupMul;

            vScreenJitterCoord.y = fract(vScreenJitterCoord.y);     //Make sure the coord is in 0 - 1 range
            vScreenJitterCoord.y *= 1.0 / $ShadowJitterSamplesDiv2;     //Access only first texture piece

            ////////////////
            // Shader Model 3, Dynamic Branching available
            @ifdef ShaderModel_4
            ////////////////
            // Cheap pre-test
            //  Note1: division must occur when getting samples else gfx card gets angry.)
            //  Note2: It _must_ be division! doing sample * 1/8 will fail!!
            for(int i=0; i<2.0; i++)
            {
                vec2 vJitterLookupCoord = vec2(vScreenJitterCoord.x, vScreenJitterCoord.y + fJitterZ);
        
                vec4 vOffset = texture2D(aShadowOffsetMap, vJitterLookupCoord) *2.0-1.0;
        
                fShadowSum += ShadowOffsetLookup(aShadowMap, vProjectedUv, vec2(vOffset.xy) * avShadowMapOffsetMul ) / 4.0;
                fShadowSum += ShadowOffsetLookup(aShadowMap, vProjectedUv, vec2(vOffset.zw) * avShadowMapOffsetMul ) / 4.0;
        
                fJitterZ += 1.0 / $ShadowJitterSamplesDiv2;
            }
        
            ////////////////
            // Check if in penumbra
            if(fShadowSum > 0.0 && fShadowSum < 1.0)
            {
                //Multiply, so the X presamples only affect their part (X/all_samples) of samples taken.
                fShadowSum *= 4.0 / $ShadowJitterSamples;
        
                ////////////////
                // Fullscale filtering
                for(int i=0; i<$ShadowJitterSamplesDiv2-2.0; i++)
                {
                    vec2 vJitterLookupCoord = vec2(vScreenJitterCoord.x, vScreenJitterCoord.y + fJitterZ); //Not that coords are 0-1!
        
                    vec4 vOffset = texture2D(aShadowOffsetMap, vJitterLookupCoord) *2.0 - 1.0;
        
                    fShadowSum += ShadowOffsetLookup(aShadowMap, vProjectedUv, vec2(vOffset.xy) * avShadowMapOffsetMul ) / $ShadowJitterSamples;
                    fShadowSum += ShadowOffsetLookup(aShadowMap, vProjectedUv, vec2(vOffset.zw) * avShadowMapOffsetMul ) / $ShadowJitterSamples;
        
                    fJitterZ += 1.0 / $ShadowJitterSamplesDiv2;
                }
        
                //vDiffuse.xyz = vec3(0,0,1);
            }
            /////////////////////
            // No Dynamic Branching
            @else
                for(int i=0; i<$ShadowJitterSamplesDiv2; i++)
                {
                    vec2 vJitterLookupCoord = vec2(vScreenJitterCoord.x, vScreenJitterCoord.y + fJitterZ);
        
                    vec4 vOffset = texture2D(aShadowOffsetMap, vJitterLookupCoord) *2.0 - 1.0;
        
                    fShadowSum += ShadowOffsetLookup(aShadowMap, vProjectedUv, vec2(vOffset.xy) * avShadowMapOffsetMul );
                    fShadowSum += ShadowOffsetLookup(aShadowMap, vProjectedUv, vec2(vOffset.zw) * avShadowMapOffsetMul );
        
                    fJitterZ += 1.0 / $ShadowJitterSamplesDiv2;
                }
        
                fShadowSum /= $ShadowJitterSamples;
            @endif

            /////////////////////
            // Add shadow sum to attenuation
            fAttenuatuion *= fShadowSum;
        @endif
    @endif
        
    /////////////////////////////////
    //Final color
    @ifdef UseSpecular
        @ifdef UseGobo
            gl_FragColor.xyz = (vSpecular + vDiffuse) * vGoboVal * fAttenuatuion;
        @else
            gl_FragColor.xyz = (vSpecular + vDiffuse) * fAttenuatuion;
        @endif
    @else
        @ifdef UseGobo
            gl_FragColor.xyz = vDiffuse * vGoboVal * fAttenuatuion;
        @else
            gl_FragColor.xyz = vDiffuse * fAttenuatuion;
        @endif
    @endif
}

