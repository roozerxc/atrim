////////////////////////////////////////////////////////
// Enemy darkness glow - Fragment Shader
//
// Makes a nice flash effect around objects.
////////////////////////////////////////////////////////

#version 120

varying vec3 gvNormal;

uniform sampler2D aDiffuse;
@define sampler_aDiffuse 0

uniform float afColorMul;

void main()
{
    float fRimLight = dot(gvNormal, vec3(0.0, 0.0, -1.0));
    fRimLight = 1.0 - abs(fRimLight);

    fRimLight = min(pow(fRimLight, 0.5), 0.7);

    gl_FragColor = texture2D(aDiffuse, gl_TexCoord[0].xy) * vec4(0.12, 0.12, 0.3, 0.0) * 0.7 * fRimLight * afColorMul;
}