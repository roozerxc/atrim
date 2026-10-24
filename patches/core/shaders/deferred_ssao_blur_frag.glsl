#version 120
#extension GL_ARB_texture_rectangle : enable

////////////////////
// Arguments
uniform float afFarPlane;

////////////////////
// Samplers
uniform sampler2DRect occMap;
@define sampler_occMap 0

uniform sampler2DRect depthMap;
@define sampler_depthMap 1

////////////////////////////////////
// Initialize these down in MAIN due to Mac OS X OpenGL Driver
@ifdef FeatureNotSupported_ConstArray
	float vMul[9];
	float fOffset[9];
@else
	const float vMul[9] = float[9](0.05, 0.1, 0.25, 0.3, 0.1, 0.3, 0.25, 0.1, 0.05);
	const float fOffset[9] = float[9](-3.35, -2.35, -1.35, -0.35, 0.0, 0.35, 1.35, 2.35, 3.35);
@endif

////////////////////
// MAIN
void main()
{
	//Mac cannot initialize outside of main
	@ifdef FeatureNotSupported_ConstArray
		vMul[0] = 0.05; vMul[1] = 0.1; vMul[2] = 0.25; vMul[3] = 0.3; vMul[4] = 0.1; vMul[5] = 0.3; vMul[6] = 0.25; vMul[7] = 0.1; vMul[8] = 0.05;
		fOffset[0] = -3.35; fOffset[1] = -2.35; fOffset[2] = -1.35; fOffset[3] = -0.35; fOffset[4] = 0.0; fOffset[5] = 0.35; fOffset[6] = 1.35; fOffset[7] = 2.35; fOffset[8] = 3.35;
	@endif

	//Get the core (at center) depth and create the minimum depth based on that.
	float fCoreDepth = texture2DRect(depthMap, gl_FragCoord.xy).x;
	float fInvTolerance = 1.0 / max(0.2 / afFarPlane, fCoreDepth * 0.03);
	
	//Offset mul depends on the direction of the blur.	
	float fBlurSize = 2.0;
	@ifdef BlurHorisontal
		vec2 vOffsetMul = vec2(1.0, 0.0) * fBlurSize;
	@else
		vec2 vOffsetMul = vec2(0.0, 1.0) * fBlurSize;
	@endif
		
	
	vec3 vAmount =vec3(0.0);
	float fMulSum =0.0;
	for(int i=0; i<9; i+=1)
	{	
		vec2 vCoordOffset = fOffset[i] * vOffsetMul;
		vec2 vUVPos = gl_TexCoord[0].xy + vCoordOffset;
				
		vec3 vOcc = texture2DRect(occMap, vUVPos).xyz;
		float fDepth = texture2DRect(depthMap, vUVPos).x;
	
		float fMul = vMul[i];
		
		fMul *= clamp(1.0 - abs(fDepth - fCoreDepth) * fInvTolerance, 0.0, 1.0);
		
		vOcc *= fMul;
		
		fMulSum += fMul;
		vAmount += vOcc;
	}
	
	vAmount /= fMulSum;
		
	gl_FragColor.xyz = vAmount;
}