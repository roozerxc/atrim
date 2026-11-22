////////////////////////////////////////////////////////
// _TEST_ HARDWARE DEPTH FRAG
//
// Used by the start-up test of the hardware depth G-buffer
// mode (cRendererDeferred::TestHardwareDepth).
// ReadDepth: reads the depth texture that is also attached
// to the frame buffer and writes it packed, read back on the CPU.
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

@ifdef ReadDepth
	@include helper_float_packing.glsl

	uniform sampler2DRect aDepthMap;
	@define sampler_aDepthMap 0
@endif

void main()
{
	@ifdef ReadDepth
		float fDepth = texture2DRect(aDepthMap, gl_FragCoord.xy).x;
		gl_FragColor = vec4(PackFloatInVec3(fDepth), 1.0);
	@else
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	@endif
}
