////////////////////////////////////////////////////////
// _TEST_ HARDWARE DEPTH VTX
//
// Used by the start-up test of the hardware depth G-buffer
// mode (cRendererDeferred::TestHardwareDepth).
// Vertices are sent already in clip space, with depth.
////////////////////////////////////////////////////////

#version 120

void main()
{
	gl_Position = vec4(gl_Vertex.xyz, 1.0);
}
