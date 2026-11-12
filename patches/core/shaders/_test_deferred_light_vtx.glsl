////////////////////////////////////////////////////////
// _TEST_ DEFERRED LIGHT VTX
//
// Vertex shader used by the deferred light start-up self-test
// (cRendererDeferred::RunLightShaderSelfTest).
//
// It only feeds the light fragment shader. Vertices are sent
// already in clip space, and the "far plane position" comes
// through texcoord 0 so the test can choose it freely.
////////////////////////////////////////////////////////

#version 120

varying vec3 gvFarPlanePos;

void main()
{
	gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
	gvFarPlanePos = gl_MultiTexCoord0.xyz;
}
