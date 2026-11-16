////////////////////////////////////////////////////////
// A "lite" version of the light fragment shader for
// weaker video cards such as Ati 9550XL.
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

//--------------------------------------------------------------

varying vec3 gvFarPlanePos;

uniform vec3 avLightPos;
uniform float afInvLightRadius;
uniform vec4 avLightColor;

@ifdef DivideInFrag
    uniform float afNegFarPlane;
@endif

@ifdef LightType_Spot
    uniform float afOneMinusCosHalfSpotFOV;
    uniform vec3 avLightForward;
@endif

//--------------------------------------------------------------

uniform sampler2DRect aDiffuseMap;
@define sampler_aDiffuseMap 0

uniform sampler2DRect aDepthMap;
@define sampler_aDepthMap 2

uniform sampler1D aAttenuationMap;
@define sampler_aAttenuationMap 4

@ifdef LightType_Spot
    uniform sampler1D aSpotFalloffMap;
    @define sampler_aSpotFalloffMap 5
@endif

//--------------------------------------------------------------

void main()
{
    vec4 vColorVal = texture2DRect(aDiffuseMap, gl_FragCoord.xy);
    vec4 vDepthVal = texture2DRect(aDepthMap, gl_FragCoord.xy);

    // Manually unpack vDepthVal due to ati 9xxx 16bit mantissa
    float fDepth = dot(vDepthVal.xy, vec2(1.0, 1.0/255.0));

    @ifdef DivideInFrag
        vec3 vPos;
        vPos.xy = (gvFarPlanePos.xy / gvFarPlanePos.z);
        vPos.z = afNegFarPlane;
        vPos.xyz *= fDepth;
    @else
        vec3 vPos = gvFarPlanePos * fDepth;
    @endif

    vec3 vLightDir = (avLightPos - vPos) * afInvLightRadius;
    float fAttenuation = texture1D(aAttenuationMap, dot(vLightDir, vLightDir)).x;
    vLightDir = normalize(vLightDir);

    @ifdef LightType_Spot
        float fOneMinusCos = 1.0 - dot(vLightDir, avLightForward);
        fAttenuation *= texture1D(aSpotFalloffMap, clamp(fOneMinusCos / afOneMinusCosHalfSpotFOV, 0.0, 1.0)).x;
    @endif

    gl_FragColor.xyz = vColorVal.xyz * avLightColor.xyz * fAttenuation;
}

//--------------------------------------------------------------
