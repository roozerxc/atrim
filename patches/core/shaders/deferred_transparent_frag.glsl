////////////////////////////////////////////////////////
// Deferred Base - Fragment Shader
//
// A basic fragment with little fancy stuff.
////////////////////////////////////////////////////////

#version 120

#extension GL_ARB_texture_rectangle : enable

@include helper_reflection.glsl

varying vec4 gvColor;

@ifdef UseFog || UseRefraction  || UseEnvMap
    varying vec3 gvVertexPos;
@endif

////////////////////
//Normal interpolated values
@ifdef UseRefraction  || UseEnvMap
    varying vec3 gvNormal;

    @ifdef UseNormalMapping
        varying vec3 gvTangent;
        varying vec3 gvBinormal;
    @endif
@endif

//////////////////////////////
// Samplers
uniform sampler2D aDiffuseMap;
@define sampler_aDiffuseMap 0

@ifdef UseNormalMapping
    uniform sampler2D aNormalMap;
    @define sampler_aNormalMap 1
@endif

@ifdef UseRefraction
    uniform sampler2DRect aRefractionMap;
    @define sampler_aRefractionMap 2
@endif

@ifdef UseEnvMap
    uniform samplerCube aEnvMap;
    @define sampler_aEnvMap 3
@endif

@ifdef UseCubeMapAlpha
    uniform sampler2D aEnvMapAlphaMap;
    @define sampler_aEnvMapAlphaMap 4
@endif

//////////////////////////////
// Params
uniform float afAlpha;
uniform float afLightLevel;

@ifdef UseRefraction
    uniform float afRefractionScale;
@endif

@ifdef UseFog
    uniform vec2 avFogStartAndLength;
    uniform float afOneMinusFogAlpha;
    uniform float afFalloffExp;
@endif

@ifdef UseEnvMap
    uniform mat4 a_mtxInvViewRotation;
    uniform vec2 avFresnelBiasPow;
    uniform vec2 avRimLightMulPow;
@endif

void main()
{
    vec4 vFinalColor;

    ////////////////////
    //Diffuse
    @ifdef UseDiffuseMap
        vFinalColor = texture2D(aDiffuseMap, gl_TexCoord[0].xy) * gvColor;
    @else
        vFinalColor = vec4(0.0, 0.0,0.0, 1.0);
    @endif

    ////////////////////
    //Fog
    @ifdef UseFog
        float fFogDepth = length(gvVertexPos);
        float fFogAmount = (fFogDepth - avFogStartAndLength.x)/ avFogStartAndLength.y;
        fFogAmount = clamp(fFogAmount, 0.0, 1.0);
        fFogAmount = pow(fFogAmount, afFalloffExp);

        float fFinalAlpha = afOneMinusFogAlpha * fFogAmount + (1-fFogAmount);
        fFinalAlpha *= afAlpha;
    @else
        float fFinalAlpha = afAlpha;
    @endif

    ////////////////////
    //Calculate new color based on Alpha and Blend mode
    @ifdef BlendMode_Add
        vFinalColor.xyz *= fFinalAlpha*afLightLevel;
    @elseif BlendMode_Mul
        vFinalColor.xyz += (vec3(1.0) - vFinalColor.xyz) * (1.0-fFinalAlpha);
    @elseif BlendMode_MulX2
        float fBlendMulAlpha = afLightLevel*fFinalAlpha;
        vFinalColor.xyz = vFinalColor.xyz*fBlendMulAlpha + vec3(0.5)*(1-fBlendMulAlpha);
    @elseif BlendMode_Alpha
        vFinalColor.a *= fFinalAlpha;
        vFinalColor.xyz *= afLightLevel;
    @elseif BlendMode_PremulAlpha
        vFinalColor *= fFinalAlpha;
        vFinalColor.xyz *= afLightLevel;
    @endif


    ////////////////////
    //Normalmap
    @ifdef UseRefraction  || UseEnvMap
        @ifdef UseNormalMapping
            vec3 vNMapNormal = texture2D(aNormalMap, gl_TexCoord[0].xy).xyz*2.0 - 1.0;

            @ifdef UseScreenNormal || UseEnvMap
                vec3 vScreenNormal = normalize(vNMapNormal.x * gvTangent + vNMapNormal.y * gvBinormal + vNMapNormal.z * gvNormal);
            @endif
        @else
            vec3 vScreenNormal = normalize(gvNormal);
        @endif
    @endif

    ////////////////////
    //Refraction
    @ifdef UseRefraction
        float fInvDist = min(1.0/gvVertexPos.z, 10.0);

        ///////////////////////
        // Sample refaraction map (using distorted coords)
        @ifdef UseNormalMapping
            //Should the screen normal or texture normal be used?
            @ifdef UseScreenNormal
                vec2 vRefractOffset = vScreenNormal.xy;
            @else
                vec2 vRefractOffset = vNMapNormal.xy;
            @endif

            vRefractOffset *= afRefractionScale * fInvDist;
            vec2 vDistortedScreenPos = gl_FragCoord.xy + vRefractOffset;
        @else
            vec2 vDistortedScreenPos = gl_FragCoord.xy + vScreenNormal.xy  * afRefractionScale * fInvDist;
        @endif

        vec4 vRefractionColor = texture2DRect(aRefractionMap, vDistortedScreenPos);

        ///////////////////////
        // Do blending in shader (blend mode is None with refraction)
        @ifdef BlendMode_Add
            vFinalColor.xyz = vFinalColor.xyz + vRefractionColor.xyz;
        @elseif BlendMode_Mul
            vFinalColor.xyz = vFinalColor.xyz * vRefractionColor.xyz;
        @elseif BlendMode_MulX2
            vFinalColor.xyz = vFinalColor.xyz * vRefractionColor.xyz * 2;
        @elseif BlendMode_Alpha
            vFinalColor.xyz = vFinalColor.xyz * vFinalColor.a + vRefractionColor.xyz * (1-vFinalColor.a);
        @elseif BlendMode_PremulAlpha
            vFinalColor.xyz = vFinalColor.xyz + vRefractionColor.xyz * (1-vFinalColor.a);
        @endif
    @endif

    ////////////////////
    //Enviroment Map
    @ifdef UseEnvMap

        ///////////////////////////////
        //Calculate Reflection
        vec3 vEyeVec = normalize(gvVertexPos);
    
        float afEDotN = max(dot(-vEyeVec, vScreenNormal),0.0);
        float fFresnel = Fresnel(afEDotN, avFresnelBiasPow.x, avFresnelBiasPow.y);
    
        vec3 vEnvUv = reflect(vEyeVec, vScreenNormal);
        vEnvUv = (a_mtxInvViewRotation * vec4(vEnvUv,1)).xyz;
    
        vec4 vReflectionColor = textureCube(aEnvMap,vEnvUv);
    
        //Alpha for environment map
        @ifdef UseCubeMapAlpha
            float fEnvMapAlpha = texture2D(aEnvMapAlphaMap, gl_TexCoord[0].xy).w;
            vReflectionColor *= fEnvMapAlpha;
        @endif
    
        vFinalColor.xyz += vReflectionColor.xyz*fFresnel*fFinalAlpha*afLightLevel;

        ///////////////////////////////
        //Rim reflections
        @ifdef ShaderModel_4
            if(avRimLightMulPow.x >0.0)
            {
        @endif

        float fRimLight = dot(vScreenNormal, vec3(0.0, 0.0, -1.0));
        fRimLight = pow(1.0 - abs(fRimLight), avRimLightMulPow.y) * avRimLightMulPow.x;

        vFinalColor.xyz += vReflectionColor.xyz * fRimLight * fFinalAlpha * afLightLevel;

        @ifdef ShaderModel_4
            }
        @endif
    @endif

    gl_FragColor = vFinalColor;
}