#version 120

uniform float afGamma;

void main()
{
    gl_FragColor = vec4(afGamma / 2.0, 0.0, 0.0, 1.0);
}