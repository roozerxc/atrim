////////////////////////////////////////////////////////
// Deferred Base - Vertex Shader
//
//
////////////////////////////////////////////////////////
#version 120

@ifdef GLSL_110
@elseif ShaderModel_3 || ShaderModel_4
invariant gl_Position;
@endif

//---------------------------------------------

@ifdef UseNormals
	varying vec3 gvNormal;
@endif
@ifdef UseNormalMapping
	varying vec3 gvTangent;
	varying vec3 gvBinormal;
@endif

@ifdef UseColor
	varying vec4 gvColor;
@endif

//32 bit G-Buffer
@ifdef Deferred_32bit
	varying float gfLinearDepth;
	
	uniform float afInvFarPlane;
//64 bit G-Buffer
@endif
	
@ifdef Deferred_64bit || UseVertexPosition || UseEnvMap || UseFog || UseRefraction
	varying vec3 gvVertexPos;	
@endif

@ifdef DeferredLight
	
	//Only need to cacluate position for 32 bit version
	@ifdef Deferred_32bit
		uniform float afNegFarPlane;
		varying vec3 gvFarPlanePos;
	@endif
	
	@ifdef UseBatching
		varying vec3 gvLightPosition;		
		varying vec4 gvLightColor;
		varying float gfLightRadius;
	@endif
@endif

@ifdef UseParallax
	varying vec3 gvTangentEyePos;
@endif

@ifdef UseUvAnimation
	uniform mat4 a_mtxUV;
@endif

@ifdef UseBoxFogNormalizedRayDir
	uniform mat4 a_mtxBoxInvViewModelRotation;
@endif

@ifdef UseSkeleton
	//Weights in texcoord 2, palette indices in texcoord 3.
	//UseSkinMatrices: three vec4 per bone, the rows of the 3x4 bone matrix.
	//Otherwise two vec4 per bone: rotation quaternion, translation + uniform scale (w).
	uniform vec4 avSkinBones[$GpuSkinBoneVecs];

	@ifdef UseSkinMatrices
	void AddSkinBone(float afIndex, float afWeight, inout vec4 avRow0, inout vec4 avRow1, inout vec4 avRow2)
	{
		int lIdx = int(afIndex + 0.5) * 3;
		avRow0 += avSkinBones[lIdx] * afWeight;
		avRow1 += avSkinBones[lIdx + 1] * afWeight;
		avRow2 += avSkinBones[lIdx + 2] * afWeight;
	}
	@else
	void AddSkinBone(float afIndex, float afWeight, inout vec4 avRow0, inout vec4 avRow1, inout vec4 avRow2)
	{
		int lIdx = int(afIndex + 0.5) * 2;
		vec4 q = avSkinBones[lIdx];
		vec4 t = avSkinBones[lIdx + 1];
		
		vec3 q2 = q.xyz * 2.0;
		vec3 qq = q.xyz * q2;
		float fXY = q.x * q2.y;
		float fXZ = q.x * q2.z;
		float fYZ = q.y * q2.z;
		vec3 vW = q.w * q2;
		
		float fScale = t.w * afWeight;
		avRow0 += vec4(1.0 - qq.y - qq.z, fXY - vW.z, fXZ + vW.y, 0.0) * fScale + vec4(0.0, 0.0, 0.0, t.x * afWeight);
		avRow1 += vec4(fXY + vW.z, 1.0 - qq.x - qq.z, fYZ - vW.x, 0.0) * fScale + vec4(0.0, 0.0, 0.0, t.y * afWeight);
		avRow2 += vec4(fXZ - vW.y, fYZ + vW.x, 1.0 - qq.x - qq.y, 0.0) * fScale + vec4(0.0, 0.0, 0.0, t.z * afWeight);
	}
	@endif
@endif


//---------------------------------------------

///////////////////////////////
// Main program
void main()
{	
	vec4 vInVertex = gl_Vertex;
	vec3 vInNormal = gl_Normal;
	vec4 vInTangent = gl_MultiTexCoord1;
	
	@ifdef UseSkeleton
		vec4 vRow0 = vec4(0.0);
		vec4 vRow1 = vec4(0.0);
		vec4 vRow2 = vec4(0.0);
		AddSkinBone(gl_MultiTexCoord3.x, gl_MultiTexCoord2.x, vRow0, vRow1, vRow2);
		AddSkinBone(gl_MultiTexCoord3.y, gl_MultiTexCoord2.y, vRow0, vRow1, vRow2);
		AddSkinBone(gl_MultiTexCoord3.z, gl_MultiTexCoord2.z, vRow0, vRow1, vRow2);
		AddSkinBone(gl_MultiTexCoord3.w, gl_MultiTexCoord2.w, vRow0, vRow1, vRow2);
		
		vInVertex = vec4(dot(vRow0, gl_Vertex), dot(vRow1, gl_Vertex), dot(vRow2, gl_Vertex), 1.0);
		vInNormal = vec3(dot(vRow0.xyz, gl_Normal), dot(vRow1.xyz, gl_Normal), dot(vRow2.xyz, gl_Normal));
		vInTangent.xyz = vec3(dot(vRow0.xyz, gl_MultiTexCoord1.xyz), dot(vRow1.xyz, gl_MultiTexCoord1.xyz), dot(vRow2.xyz, gl_MultiTexCoord1.xyz));
	@endif
	
	//////////////////////
	// Position
	@ifdef UseDecalDepthPull
		vec4 vDecalViewPos = gl_ModelViewMatrix * vInVertex;
		vDecalViewPos.xyz *= 0.9995;
		gl_Position = gl_ProjectionMatrix * vDecalViewPos;
	@elseif UseSkeleton
		gl_Position = gl_ModelViewProjectionMatrix * vInVertex;
	@else
		gl_Position = ftransform();
	@endif
	
	
	//////////////////////
	// Color
	@ifdef UseColor
		gvColor = gl_Color;
	@endif
	
	//////////////////////
	// Uv
	@ifdef UseUv
		@ifdef UseUvAnimation
			gl_TexCoord[0] = a_mtxUV * gl_MultiTexCoord0;
		@else
			gl_TexCoord[0] = gl_MultiTexCoord0;
		@endif
		@ifdef UseUvCoord1
			gl_TexCoord[1] = gl_MultiTexCoord1;
		@endif
	@endif

	//////////////////////
	// Normals
	@ifdef UseNormals
		gvNormal = normalize(gl_NormalMatrix * vInNormal);
	@endif

	//////////////////////
	// Normalmapping
	@ifdef UseNormalMapping
		//To consider: Is gl_NormalMatrix correct here?
		gvTangent = normalize(gl_NormalMatrix * vInTangent.xyz);
		
		//Need to do it in model space (and not view) because reflection normal mapping will fail otherwise!
		gvBinormal = normalize(gl_NormalMatrix * cross(vInNormal,vInTangent.xyz) * vInTangent.w); 
	@endif
	
	//////////////////////
	// Parallax
	@ifdef UseParallax
		vec3 vViewEyeVec =  (gl_ModelViewMatrix * vInVertex).xyz;
		
		gvTangentEyePos.x = dot(vViewEyeVec, gvTangent);
		gvTangentEyePos.y = dot(vViewEyeVec, gvBinormal);
		gvTangentEyePos.z = dot(-vViewEyeVec, gvNormal);
		
		//Do not normalize yet! Do that in the fragment shader.		
	@endif

	//////////////////////
	// Deferring (G-Buffer)
	@ifdef Deferred_32bit
		gfLinearDepth = -(gl_ModelViewMatrix * vInVertex).z * afInvFarPlane; //Do not use near plane! Doing like this will make the calcs simpler in the light shader.
	@endif
	
	@ifdef Deferred_64bit || UseVertexPosition || UseEnvMap || UseFog || UseRefraction
		gvVertexPos = (gl_ModelViewMatrix * vInVertex).xyz;
	@endif
	
	//////////////////////
	// Deferring (Lights)
	@ifdef DeferredLight
	
		@ifdef Deferred_32bit
			////////////////////////////
			//Light are rendered as shapes
			@ifdef UseDeferredLightShapes
				//Project the position to the farplane
				vec3 vPos = (gl_ModelViewMatrix * vInVertex).xyz;
				
				//Spotlight will divided in fragment shader
				@ifdef DivideInFrag
					gvFarPlanePos = vPos;
					gvFarPlanePos.xy *= afNegFarPlane;
				//Point light does division now
				@else
					vec2 vTanXY = vPos.xy / vPos.z;
					
					gvFarPlanePos.xy = vTanXY * afNegFarPlane;
					gvFarPlanePos.z = afNegFarPlane;
				@endif
			////////////////////////////
			//Light are 2D quads
			@else
				//No need to project, postion is already at far plane.
				gvFarPlanePos = gl_Vertex.xyz;
			@endif
		@endif
		
		////////////////////////////
		//Batching is used
		@ifdef UseBatching
			gvLightPosition = gl_MultiTexCoord0.xyz;
			gvLightColor = 	gl_Color;
			gfLightRadius = gl_MultiTexCoord1.x;			
		@endif
	@endif
}