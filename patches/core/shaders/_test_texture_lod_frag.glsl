////////////////////////////////////////////////////////
// _TEST_ TEXTURE LOD FRAG
//
// A shader that tests if GL_ARB_shader_texture_lod works in fragment shaders.
// (texture2DGradARB, used by relief parallax in deferred_gbuffer_solid_frag.glsl)
////////////////////////////////////////////////////////

#version 110
#extension GL_ARB_shader_texture_lod : require

uniform sampler2D aMap;

void main()
{
	vec2 vUv = gl_TexCoord[0].xy;
	gl_FragColor = texture2DGradARB(aMap, vUv, dFdx(vUv), dFdy(vUv));
}
