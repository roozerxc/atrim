////////////////////////////////////////////////////////
// A "lite" version of the light fragment shader for
// weaker video cards such as Ati 9550XL.
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

@ifdef HardwareDepth
    @include helper_hardware_depth.glsl
@endif

//--------------------------------------------------------------

varying vec3 gvFarPlanePos;

uniform vec3 avLightPos;
uniform float afInvLightRadius;
uniform vec4 avLightColor;

@ifdef DivideInFrag
    uniform float afNegFarPlane;
@endif

@ifdef UseSecondLight
    uniform vec3 avLightPos2;
    uniform float afInvLightRadius2;
    uniform vec4 avLightColor2;
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

@ifdef UseSecondLight
    uniform sampler1D aAttenuationMap2;
    @define sampler_aAttenuationMap2 6
@endif

@ifdef LightType_Spot
    uniform sampler1D aSpotFalloffMap;
    @define sampler_aSpotFalloffMap 5
@endif

//--------------------------------------------------------------

void main()
{
    vec4 vColorVal = texture2DRect(aDiffuseMap, gl_FragCoord.xy);
    vec4 vDepthVal = texture2DRect(aDepthMap, gl_FragCoord.xy);

    @ifdef UseLightColorGrade
        vColorVal.xyz = mix(vColorVal.xyz, vColorVal.xyz * vColorVal.xyz, vec3(0.640, 0.764, 0.426));
    @endif

    @ifdef HardwareDepth
        //The depth map is the depth buffer (GBufferMode=2)
        float fDepth = HardwareDepthToLinear(vDepthVal.x);
    @else
        // Manually unpack vDepthVal due to ati 9xxx 16bit mantissa
        float fDepth = dot(vDepthVal.xy, vec2(1.0, 1.0/255.0));
    @endif

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

    @ifdef UseSecondLight
        vec3 vLightDir2 = (avLightPos2 - vPos) * afInvLightRadius2;
        float fAttenuation2 = texture1D(aAttenuationMap2, dot(vLightDir2, vLightDir2)).x;
        gl_FragColor.xyz = vColorVal.xyz * (avLightColor.xyz * fAttenuation + avLightColor2.xyz * fAttenuation2);
    @else
        gl_FragColor.xyz = vColorVal.xyz * avLightColor.xyz * fAttenuation;
    @endif
}

//--------------------------------------------------------------
