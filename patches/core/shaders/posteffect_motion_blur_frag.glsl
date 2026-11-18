////////////////////////////////////////////////////////
// PostEffect Motion Blur - Fragment Shader
//
// Port of the HPL1 (Penumbra) "Motion" program.
// HPL1 got the motion of each pixel by drawing all objects again with
// their previous matrices. Here it comes from the linear depth in the
// G-buffer, projected with the camera of a moment ago (avReproj*).
// Sampling along the motion is as in HPL1, but spread around the pixel
// (HPL1 went from the pixel to its old position) and with a small
// per-pixel offset, so the samples do not show up as copies.
//
// The alpha of diffuseMap is 1 on the view model (hands, lantern): it is
// not blurred, and it is not smeared into the rest of the picture.
//
// Low quality: 8 samples, fits ATI R300 (Radeon 9500-X850) limits.
// HighQuality: 16 samples.
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

@include helper_float_packing.glsl

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform sampler2DRect depthMap;
@define sampler_depthMap 1

// Rows of the reprojection matrix: (x*d, y*d, d, 1) -> previous window position (x, y) / w
uniform vec4 avReprojX;
uniform vec4 avReprojY;
uniform vec4 avReprojW;

// 1 / (blur length the motion approaches)^2, in pixels
uniform float afInvMaxLengthSqr;

void main()
{
	vec2 vScreenCoord = gl_FragCoord.xy;

	vec4 vCenter = texture2DRect(diffuseMap, vScreenCoord);

	////////////////////////////////////
	// Depth
	@ifdef PackedDepth
		//Depth / far plane. Cleared to 1 where nothing was drawn.
		float fDepth = UnpackVec3ToFloat(texture2DRect(depthMap, vScreenCoord).xyz);
		vec4 vPos = vec4(vScreenCoord * fDepth, fDepth, 1.0);
	@else
		//View space position. Cleared to 0 where nothing was drawn: infinitely far, only rotation counts.
		float fDepth = -texture2DRect(depthMap, vScreenCoord).z;
		float fFar = step(fDepth, 0.0001);
		fDepth += fFar;
		vec4 vPos = vec4(vScreenCoord * fDepth, fDepth, 1.0 - fFar);
	@endif

	////////////////////////////////////
	// Where the pixel was when the exposure started
	vec3 vPrev = vec3(dot(avReprojX, vPos), dot(avReprojY, vPos), dot(avReprojW, vPos));
	vec2 vVelocity = vPrev.xy / vPrev.z - vScreenCoord;

	//Point was behind the camera: no usable motion
	vVelocity *= step(0.0, vPrev.z);

	//Soft limit: short motion is kept as is, long motion approaches the max length
	vVelocity *= inversesqrt(1.0 + dot(vVelocity, vVelocity) * afInvMaxLengthSqr);

	//The view model is not blurred
	vVelocity *= 1.0 - vCenter.w;

	////////////////////////////////////
	// Sample along the motion, centered on the pixel
	float fJitter = fract(dot(vScreenCoord, vec2(0.5, 0.25)));

	@ifdef HighQuality
		vec2 vStep = vVelocity * (1.0 / 16.0);
	@else
		vec2 vStep = vVelocity * (1.0 / 8.0);
	@endif
	vec2 vCoord = vScreenCoord - vVelocity * 0.5 + vStep * fJitter;

	//xyz = color, w = weight. Samples on the view model are left out.
	vec4 vSum = vec4(vCenter.xyz, 1.0);

	@ifdef HighQuality
	for(int i = 0; i < 16; ++i)
	@else
	for(int i = 0; i < 8; ++i)
	@endif
	{
		vec4 vSample = texture2DRect(diffuseMap, vCoord);
		vSum += vec4(vSample.xyz, 1.0) * (1.0 - vSample.w);
		vCoord += vStep;
	}

	gl_FragColor.xyz = vSum.xyz / vSum.w;
	gl_FragColor.w = 1.0;
}
