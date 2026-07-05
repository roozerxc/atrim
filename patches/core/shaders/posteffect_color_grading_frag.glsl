////////////////////////////////////////////////////////
// PostEffect Color Grading - Fragment Shader
////////////////////////////////////////////////////////

#version 120

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform sampler3D gradingMap;
@define sampler_gradingMap 1

@ifdef IsCrossFading
    uniform sampler3D gradingMap2;
    @define sampler_gradingMap2 2

    uniform float afCrossFadeAlpha;
@endif

void main()
{
    vec3 vDiffuseColor = texture2DRect(diffuseMap, gl_TexCoord[0].xy).xyz;

    @ifdef IsCrossFading
        gl_FragColor.xyz =  (1 - afCrossFadeAlpha ) * texture3D(gradingMap, vDiffuseColor.yxz).xyz + afCrossFadeAlpha * texture3D(gradingMap2, vDiffuseColor.yxz).xyz;
    @else
        gl_FragColor.xyz = texture3D(gradingMap, vDiffuseColor.yxz).xyz;
    @endif
}