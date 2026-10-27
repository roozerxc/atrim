////////////////////////////////////////////////////////
// Deferred Fog - Fragment Shader
//
// Fragment shader for all things fog!
////////////////////////////////////////////////////////

#version 120

#extension GL_ARB_texture_rectangle : enable

@include helper_float_packing.glsl

//--------------------------------------------------------------

varying vec3 gvVertexPos;

@ifdef OutsideBox && UseBackside
    varying vec3 gvLocalBoxRay;
@endif

//--------------------------------------------------------------

uniform sampler2DRect  aDepthMap;
@define sampler_aDepthMap 0

@ifdef PackedDepth
    uniform float afNegFarPlane;
@endif

@ifdef FogModulation
    uniform sampler2DRect  aDiffuseMap;
    @define sampler_aDiffuseMap 1
@endif

//--------------------------------------------------------------

uniform vec2 avFogStartAndLength;
uniform vec4 avFogColor;
uniform float afFalloffExp;

//--------------------------------------------------------------

@ifdef OutsideBox
    @ifdef UseBackside
        uniform vec3 avRayCastStart;

        uniform vec3 avNegPlaneDistNeg;
        uniform vec3 avNegPlaneDistPos;

        void GetPlaneIntersection(vec3 avPlaneNormal, float afNegPlaneDist, inout float afFinalT)
        {
            //Get T (amount of ray) to intersection
            float fMul  = dot(gvLocalBoxRay, avPlaneNormal);
            float fT = afNegPlaneDist / fMul;

            //Get the intersection and see if inside box
            vec3 vIntersection = abs(gvLocalBoxRay*fT + avRayCastStart);
            if( all( lessThan(vIntersection, vec3(0.5001)) ) )
            {
                afFinalT = max(afFinalT, fT);
            }
        }
    @endif
@endif

//--------------------------------------------------------------

void main()
{
    ////////////////////////////////////
    // Get depth
    @ifdef PackedDepth
        vec4 vDepthVal =  texture2DRect(aDepthMap, gl_FragCoord.xy);
        float fDepth = -UnpackVec3ToFloat(vDepthVal.xyz) * afNegFarPlane;
    @else
        //64 bit stores position directly
        float fDepth = -texture2DRect(aDepthMap, gl_FragCoord.xy).z;
    @endif

    ////////////////////////////////////
    // Outside box
    @ifdef OutsideBox
        //Depth starts at box!
        fDepth = fDepth + gvVertexPos.z; //VertexPos is negative!
    
        ///////////////////////////////////
        // Ray cast to find back of square
        @ifdef UseBackside
            float fFinalT = 0.0;
            GetPlaneIntersection(vec3(-1.0, 0.0, 0.0),    avNegPlaneDistNeg.x,    fFinalT);//Left
            GetPlaneIntersection(vec3(1.0, 0.0, 0.0),     avNegPlaneDistPos.x,    fFinalT);//Right
            GetPlaneIntersection(vec3(0.0, -1.0, 0.0),    avNegPlaneDistNeg.y,     fFinalT);//Bottom
            GetPlaneIntersection(vec3(0.0, 1.0, 0.0 ),    avNegPlaneDistPos.y,     fFinalT);//Top
            GetPlaneIntersection(vec3(0.0, 0.0, -1.0),    avNegPlaneDistNeg.z,     fFinalT);//Back
            GetPlaneIntersection(vec3(0.0, 0.0, 1.0),     avNegPlaneDistPos.z,    fFinalT);//Front
        
            float fLocalBackZ = fFinalT*gvVertexPos.z - gvVertexPos.z;
            fDepth = min(-fLocalBackZ, fDepth);
        @endif

    ////////////////////////////////////
    // Inside box
    @else
        @ifdef UseBackside
            fDepth = min(-gvVertexPos.z, fDepth);
        @endif
    @endif

    fDepth = min(fDepth - avFogStartAndLength.x, avFogStartAndLength.y);
    float fAmount = max(fDepth / avFogStartAndLength.y,0.0);

    @ifdef FogModulation
        vec4 vDiffuseVal = texture2DRect(aDiffuseMap, gl_FragCoord.xy);
        float fogFactor = abs(vDiffuseVal.x);
        gl_FragColor.w = pow(fogFactor * fAmount, afFalloffExp) * avFogColor.w;
    @else
        gl_FragColor.w = pow(fAmount, afFalloffExp) * avFogColor.w;
    @endif

    gl_FragColor.xyz = avFogColor.xyz;
}