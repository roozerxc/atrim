////////////////////////////////////////////////////////
// PostEffect Gamma - Fragment Shader
//
// Applies gamma correction to the whole game window,
// drawn right before the buffers are swapped.
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

    vColor = max(vColor * (255.0 / 256.0), vec3(1.0e-10));
    float fSafeGamma = max(afGamma, 0.1);
    vColor = pow(vColor, vec3(1.0 / fSafeGamma));
    
    ////////////////////////
    // Ordered dither (4x4 bayer) before 8bit backbuf
    vec2 vPos = mod(floor(gl_FragCoord.xy), 4.0);
    vec2 vLow = mod(vPos, 2.0);
    vec2 vHigh = floor(vPos * 0.5);

    float fLow = mod(vLow.x * 2.0 + vLow.y * 3.0, 4.0);
    float fHigh = mod(vHigh.x * 2.0 + vHigh.y * 3.0, 4.0);
    float fDither = (fLow * 4.0 + fHigh + 0.5) / 16.0;

    vColor += vec3((fDither - 0.5) / 255.0);

    gl_FragColor.xyz = vColor;
    gl_FragColor.w = 1.0;
}
