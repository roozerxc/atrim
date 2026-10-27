////////////////////////////////////////////////////////
// Deferred Illumination - Fragment Shader
//
// Used in a sepperate pass to render illuminating parts of a material.
////////////////////////////////////////////////////////

#version 120

uniform sampler2D aDiffuse;
@define sampler_aDiffuse 0

uniform sampler2D aModulate;
@define sampler_aModulate 1

uniform float afColorMul;
uniform float afTimer;

const float animationSpeed = 50.02f;

void main()
{
    vec4 vDiffuse = texture2D(aDiffuse, gl_TexCoord[0].xy);
    gl_FragColor = vDiffuse * vDiffuse * texture2D(aModulate, gl_TexCoord[0].xy + vec2(animationSpeed * afTimer,0)).x * afColorMul * vec4(8.0);
}