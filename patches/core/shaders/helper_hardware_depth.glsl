////////////////////////////////////////////////////////
// Hardware depth helper
//
// For GBufferMode=2: the G-buffer has no linear depth
// target, the depth map is the depth buffer itself.
// avDepthUnproject is set by cRendererDeferred::UpdateDepthUnproject.
////////////////////////////////////////////////////////

uniform vec4 avDepthUnproject;

// Depth buffer value (0-1) to linear depth (view distance / far plane, 0-1),
// the same value the packed depth target holds.
float HardwareDepthToLinear(float afDepth)
{
	return 1.0 / dot(vec4(afDepth, gl_FragCoord.xy, 1.0), avDepthUnproject);
}
