////////////////////////////////////////////////////////
// _TEST_ G-BUFFER LITE FRAG
//
// Used by the start-up test of the lite G-buffer layout
// (cRendererDeferred::TestLiteGBufferLayout).
// Writes a known depth per column, read back on the CPU.
////////////////////////////////////////////////////////

#version 120

@include helper_float_packing.glsl

void main()
{
	float fDepth = fract(gl_FragCoord.x * 0.0137);
	gl_FragData[0] = vec4(0.25, 0.5, 0.75, 1.0);
	@ifdef DepthTarget_2
		gl_FragData[2] = vec4(PackFloatInVec3(fDepth), 0.0);
	@else
		gl_FragData[1] = vec4(PackFloatInVec3(fDepth), 0.0);
	@endif
}
