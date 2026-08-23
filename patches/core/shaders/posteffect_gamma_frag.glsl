////////////////////////////////////////////////////////
// PostEffect Gamma - Fragment Shader
//
// Applies gamma correction to the final image
// Replaces SDL1's hardware gamma functionality
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform float afGamma;

void main()
{
    vec3 vColor = texture2DRect(diffuseMap, gl_TexCoord[0].xy).xyz;

    vColor = max(vColor, vec3(0.001));
    float fSafeGamma = max(afGamma, 0.1);
    vColor = pow(vColor, vec3(1.0 / fSafeGamma));

    gl_FragColor.xyz = vColor;
    gl_FragColor.w = 1.0;
}
