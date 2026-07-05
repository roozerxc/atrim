////////////////////////////////////////////////////////
// Water Surface - Fragment Shader
//
// Shades the surface of a water
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

@include helper_reflection.glsl

////////////////////////////////
// Variying input
varying vec3 gvVertexPos;

@ifdef UseReflection
    varying vec3 gvNormal;
    varying vec3 gvTangent;
    varying vec3 gvBinormal;
@endif

////////////////////////////////
// Samplers
uniform sampler2D aDiffuseMap;
@define sampler_aDiffuseMap 0

uniform sampler2D aNormalMap;
@define sampler_aNormalMap 1

@ifdef UseRefraction
    uniform sampler2DRect aRefractionMap;
    @define sampler_aRefractionMap 2
@endif

@ifdef UseReflection
    @ifdef UseCubeMapReflection
        uniform samplerCube aEnvMap;
        @define sampler_aEnvMap 3
    @else
        uniform sampler2DRect aReflectionMap;
        @define sampler_aReflectionMap 3
    @endif
@endif

////////////////////////////////
// Arguments
uniform float afT;
uniform float afWaveAmplitude;
uniform float afWaveFreq;
uniform float afRefractionScale;

@ifdef UseFog
    uniform vec2 avFogStartAndLength;
    uniform vec4 avFogColor;
    uniform float afFalloffExp;
@endif

@ifdef UseReflection
    uniform vec2 avFresnelBiasPow;

    @ifdef UseCubeMapReflection
        uniform mat4 a_mtxInvViewRotation;
    @else
        uniform vec2 avReflectionMapSizeMul;
    @endif

    @ifdef UseReflectionFading
        uniform vec2 avReflectionFadeStartAndLength;
    @endif
@endif

////////////////////////////////
// Main
void main()
{
    ///////////////////////////////
    //Get the two uv coords
    float fT1 = afT*0.8;
    vec2 vUv1 = gl_TexCoord[0].xy+afT*0.01f;
    vUv1.x += sin(fT1 + vUv1.y * afWaveFreq)* afWaveAmplitude;
    vUv1.y += sin(fT1 + vUv1.x * afWaveFreq)* afWaveAmplitude;

    float fT2 = afT*-2.6;
    vec2 vUv2 = gl_TexCoord[0].xy+afT*-0.012f;
    vUv2.x += sin(fT2 + vUv2.y * afWaveFreq*1.2) * afWaveAmplitude*0.75;
    vUv2.y += sin(fT2 + vUv2.x * afWaveFreq*1.2) * afWaveAmplitude*0.75;

    ///////////////////////////////
    //Get the normals and combine into final normal
    // (No need for full unpack since there is a normalize later)
    vec3 vNormal1 = texture2D(aNormalMap, vUv1).xyz-0.5;
    vec3 vNormal2 = texture2D(aNormalMap, vUv2).xyz-0.5;

    vec3 vFinalNormal = normalize(vNormal1*0.7 + vNormal2*0.3);

    ///////////////////////////////
    //Get the diffuse color
    vec4 vSurfaceColor = texture2D(aDiffuseMap, vUv1);

    ///////////////////////////////
    //Get the fog amount
    @ifdef UseFog
        float fFogAmount = (-gvVertexPos.z - avFogStartAndLength.x)/ avFogStartAndLength.y;
        fFogAmount = clamp(fFogAmount, 0.0, 1.0);
        fFogAmount = pow(fFogAmount, afFalloffExp) * avFogColor.a;
    @endif

    ///////////////////////////////
    //Get the refraction color
    @ifdef UseRefraction
        float fInvDist = min(1.0/gvVertexPos.z, 10.0);
        vec2 vDistortedScreenPos = gl_FragCoord.xy + vFinalNormal.xy * afRefractionScale * fInvDist;

        vec4 vRefractionColor = texture2DRect(aRefractionMap, vDistortedScreenPos);

        @ifdef UseRefractionEdgeCheck
            if(vRefractionColor.w <0.5)
            {
                vRefractionColor = texture2DRect(aRefractionMap, gl_FragCoord.xy);
            }
        @endif
    @else
        vec4 vRefractionColor = vec4(1);
    @endif

    ///////////////////////////////
    //Get the reflection color
    @ifdef UseReflection
        //////////////////
        //Reflection fading
        @ifdef UseReflectionFading
            float fReflFade = 1.0 - clamp( (gvVertexPos.z - avReflectionFadeStartAndLength.x) / avReflectionFadeStartAndLength.y, 0.0, 1.0);
        @endif

        //////////////////
        //Fresnel
        vec3 vScreenNormal = normalize(vFinalNormal.x * gvTangent + vFinalNormal.y * gvBinormal + vFinalNormal.z * gvNormal);
        vec3 vEyeVec = normalize(gvVertexPos);

        float afEDotN = max(dot(-vEyeVec, vScreenNormal),0.0);
        float fFresnel = Fresnel(afEDotN, avFresnelBiasPow.x, avFresnelBiasPow.y);
    
        @ifdef UseReflectionFading
            fFresnel *= fReflFade;
        @endif

        //////////////////
        //Cubemap
        @ifdef UseCubeMapReflection
            vec3 vEnvUv = reflect(vEyeVec, vScreenNormal);
            vEnvUv = (a_mtxInvViewRotation * vec4(vEnvUv,1)).xyz;
    
            vec4 vReflectionColor = textureCube(aEnvMap,vEnvUv);
        //////////////////
        //World reflection
        @else
            vec4 vReflectionColor = texture2DRect(aReflectionMap, vDistortedScreenPos * avReflectionMapSizeMul);
        @endif

    ///////////////////////////////
    //Add fake "specular" and diffuse shading (use when there is no reflection)
    @else
        vec3 vLightDir = normalize(vec3(0.5, 0.5, 0.5));
        float fLDotN = max(dot(vLightDir, vFinalNormal),0.0);
        float fDiffuse =  fLDotN * 0.5 + 0.5;
        float fSpecular = pow(fLDotN,16.0);
    @endif
    
    ///////////////////////////////
    //Caclulate the final color
    @ifdef UseFog
        @ifdef UseRefraction
            @ifdef UseReflection
                gl_FragColor.xyz = (vRefractionColor.xyz*vSurfaceColor.xyz + vReflectionColor.xyz*fFresnel) * (1.0-fFogAmount) + avFogColor.xyz*fFogAmount;
                gl_FragColor.w = 1.0;
            @else
                gl_FragColor.xyz = (vSurfaceColor.xyz * vRefractionColor.xyz *fDiffuse + vec3(fSpecular)) * (1.0-fFogAmount) + avFogColor.xyz*fFogAmount;
                gl_FragColor.w = 1.0;
            @endif
        @else
            gl_FragColor.xyz = (vSurfaceColor.xyz * vRefractionColor.xyz *fDiffuse + vec3(fSpecular)) * (1.0-fFogAmount) + vec3(fFogAmount);
            gl_FragColor.w = 1.0;
        @endif
    @else
        @ifdef UseReflection
            gl_FragColor.xyz = vRefractionColor.xyz*vSurfaceColor.xyz + vReflectionColor.xyz*fFresnel;
            gl_FragColor.w = 1.0;
        @else
            gl_FragColor.xyz = vSurfaceColor.xyz * vRefractionColor.xyz *fDiffuse + vec3(fSpecular);
            gl_FragColor.w = 1.0;
        @endif
    @endif
}