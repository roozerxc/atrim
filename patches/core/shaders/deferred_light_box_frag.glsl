////////////////////////////////////////////////////////
// Deferred Light Box - Fragment Shader
//
// Fragment program to draw a light box.
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

////////////////////
//Textures
uniform sampler2DRect  aDiffuseMap;
@define sampler_aDiffuseMap 0

@ifdef UseSSAO
    uniform sampler2DRect  aSSAOMap;
    @define sampler_aSSAOMap 5
@endif

//Used when there is no stencil buffer: discard pixels outside of the box.
@ifdef UseBoxClip
    uniform sampler2DRect aDepthMap;
    @define sampler_aDepthMap 2

    uniform mat4 a_mtxInvBoxViewSpace;

    @ifdef Deferred_32bit
        @include helper_float_packing.glsl

        varying vec3 gvFarPlanePos;
        uniform float afNegFarPlane;
    @endif
@endif

////////////////////
//Uniform varaibles
uniform vec4 avLightColor;

uniform float afComplexity;

void main()
{
    vec2 vMapCoords = gl_FragCoord.xy;

    @ifdef UseBoxClip
        vec4 vDepthVal = texture2DRect(aDepthMap, vMapCoords);

        @ifdef Deferred_32bit
            vec3 vPos;
            vPos.xy = gvFarPlanePos.xy / gvFarPlanePos.z;
            vPos.z = afNegFarPlane;
            vPos *= UnpackVec3ToFloat(vDepthVal.xyz);
        @else
            vec3 vPos = vDepthVal.xyz;
        @endif

        //Box is a unit cube in its own space
        vec3 vBoxDist = abs((a_mtxInvBoxViewSpace * vec4(vPos, 1.0)).xyz) - vec3(0.501);
        if(max(vBoxDist.x, max(vBoxDist.y, vBoxDist.z)) > 0.0) discard;
    @endif

    vec4 vColorVal =  texture2DRect(aDiffuseMap, vMapCoords);

    @ifdef UseSSAO
        vColorVal *= texture2DRect(aSSAOMap, vMapCoords * 0.5);    //SSAO should be half the size of the screen.
    @endif

    //Multiply with light color and AO (w).
    gl_FragData[0].xyz = vColorVal.xyz * avLightColor.xyz;
    gl_FragData[1].xyz = vColorVal.xyz * avLightColor.xyz;

    @ifdef DebugComplexity
        gl_FragData[0].xyz = vec3(afComplexity);
        gl_FragData[1].xyz = vec3(afComplexity);
    @endif
}