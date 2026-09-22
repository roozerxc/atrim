////////////////////////////////////////////////////////
// Heat map a greyscale value
//
// A basic fragment with little fancy stuff.
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable

//////////////
// TEXTURES
uniform sampler2DRect aComplexityMap;
@define sampler_aComplexityMap 0

uniform sampler2DRect aSceneMap;
@define sampler_aSceneMap 1

void main()
{
	float fComplexity = texture2DRect(aComplexityMap, gl_FragCoord.xy).x * 1.4f;
	vec4 vColor = texture2DRect(aSceneMap, gl_FragCoord.xy);

	vec4 vFinalColor = vec4(0,0,0,0.5);

	if(fComplexity > 0)
	{
		/////////////
		// Fade from green to red, where green is good and red is bad
		vFinalColor = mix(vec4(0,1,0,0), vec4(1, 0, 0, 0), min(1, fComplexity));

		float fLength = length(vFinalColor);
		vFinalColor /= fLength;

		vFinalColor.a = 0.25 + fComplexity * 0.65;
	}
		 
	gl_FragColor = mix(vColor, vFinalColor, min(0.9, vFinalColor.a));
}