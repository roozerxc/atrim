#version 120

#extension GL_ARB_texture_rectangle : enable

uniform float afFarPlane;

uniform float afScatterLengthMul;
uniform vec2 avScatterLengthLimits;
uniform float afDepthDiffMul;
uniform float afSkipEdgeLimit;

uniform vec2 avScreenSize;

uniform sampler2DRect depthMap;
@define sampler_depthMap 0

uniform sampler2D scatterDisk;
@define sampler_scatterDisk 1

////////////////////////////////////////////////////////
// Tweakables (all lengths are multiples of the sample disk radius)

//World radius (meters) of the sample disk when the scatter length is not clamped.
//= 2 * ScatterLengthMul(0.2) * tan(FOV / 2), with the default FOV of about 70 degrees.
const float kfSampleRadius = 0.28;

//Occluders further in front than this are ignored. Fading starts at half of it.
const float kfRange = 2.0;

//Gap (between the surface and the occluder, averaged over a sample pair) that gives full occlusion.
const float kfFullOcclusionGap = 1.5;

//Gaps smaller than this are ignored (removes noise from bumpy surfaces and depth precision).
const float kfBias = 0.03;

//Overall strength. Below 1.0 nothing gets fully black.
const float kfStrength = 0.7;

void main()
{
	///////////////
	// This is the core depth that we compare to
	float fCoreDepth = texture2DRect(depthMap, gl_TexCoord[0].xy).x;
	float fCoreDist = max(fCoreDepth * afFarPlane, 0.001);

	//Have a max limit on the length, or else there will be major slowdowns when many objects are upfront.
	//Multiply with height (y) since width varies with aspect!
	//Also a min length to make stuff darker at a distance to avoid flickering.
	float fUnclampedLength = afScatterLengthMul / fCoreDist;
	float fScreenLength = clamp(fUnclampedLength, avScatterLengthLimits.x, avScatterLengthLimits.y);
	float fScatterLength = fScreenLength * avScreenSize.y;

	//World size of the disk. Follows the clamp: smaller close to the camera, bigger far away.
	float fRadius = kfSampleRadius * (fScreenLength / fUnclampedLength);

	float fRangeEnd = fRadius * kfRange * afSkipEdgeLimit;
	float fRangeStart = fRangeEnd * 0.5;
	float fBias = fRadius * kfBias;
	float fOcclusionMul = (kfStrength * afDepthDiffMul) / (fRadius * kfFullOcclusionGap);

	vec2 vScreenScatterCoord = gl_FragCoord.xy * (1.0 / 4.0);	//4 = size of scatter texture, and this is to get a 1-1 pixel-texture usage.
	vScreenScatterCoord.y = fract(vScreenScatterCoord.y);	//Make sure the coord is in 0 - 1 range
	vScreenScatterCoord.y *= 1.0 / float($SampleNumDiv2);	//Access only first texture piece

	float fScatterDiskZ = 0.0;
	float fOccSum = 0.0;

	//////////////////////////////////////////
	// Go through the samples, 4 at a time!
	for(int i=0; i<$SampleNumDiv2 / 2; i++)
	{
		//Get the scatter coordinates (used to get the randomized postion for each sampling)
		//(Vanilla multiplied fScatterDiskZ by 4 here, which wrapped around to the first piece every time.)
		vec2 vScatterLookupCoord1 = vec2(vScreenScatterCoord.x, vScreenScatterCoord.y + fScatterDiskZ);

		vec4 vOffset1 = (texture2D(scatterDisk, vScatterLookupCoord1) * 2.0 - 1.0) * fScatterLength;

		//Look up the depth at the random samples. x-z and y-w are each others opposites!
		vec4 vDepth = vec4(	texture2DRect(depthMap, gl_TexCoord[0].xy + vOffset1.xy).x,
					texture2DRect(depthMap, gl_TexCoord[0].xy + vOffset1.zw).x,
					texture2DRect(depthMap, gl_TexCoord[0].xy - vOffset1.xy).x,
					texture2DRect(depthMap, gl_TexCoord[0].xy - vOffset1.zw).x);

		//Gap in meters, positive = the sample is in front of the core pixel.
		vec4 vGap = (fCoreDepth - vDepth) * afFarPlane;

		//Out of range occluders: use the negative opposite gap instead (extrapolates the surface behind).
		vGap = mix(vGap, -vGap.zwxy, smoothstep(fRangeStart, fRangeEnd, vGap));

		//Average each pair. Planes cancel out, creases (+) and ridges (-) stay.
		vec2 vPairGap = (vGap.xy + vGap.zw) * 0.5;

		//More gap = more occlusion, up to full, then fading out towards the range end.
		vec2 vAmount = clamp((vPairGap - fBias) * fOcclusionMul, 0.0, 1.0);
		vAmount *= vec2(1.0) - smoothstep(fRangeStart, fRangeEnd, vPairGap);

		//Squaring makes sharper dark areas (same curve as vanilla).
		vec2 vOcc = (vec2(1.0) - vAmount) * (vec2(1.0) - vAmount);
		fOccSum += vOcc.x + vOcc.y;

		//Next piece of the scatter texture (so new values are used on next iteration)
		fScatterDiskZ += (1.0 / float($SampleNumDiv2)) * 2.0;
	}

	//2 pairs per iteration
	float fOcc = fOccSum / float($SampleNumDiv2);

	gl_FragColor.xyz = vec3(fOcc * fOcc);
}
