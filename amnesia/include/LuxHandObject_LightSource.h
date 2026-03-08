#ifndef LUX_HAND_OBJECT_LIGHT_SOURCE_H
#define LUX_HAND_OBJECT_LIGHT_SOURCE_H

//----------------------------------------------

#include "LuxHandObject.h"


//----------------------------------------------

class cLuxHandObject_LightSource : public iLuxHandObject
{
public:
    cLuxHandObject_LightSource(const tString& asName, cLuxPlayerHands *apHands);
    ~cLuxHandObject_LightSource();

    void LoadImplementedVars(cXmlElement *apVarsElem);

    void ImplementedCreateEntity(cLuxMap *apMap);
    void ImplementedDestroyEntity(cLuxMap *apMap);

    void ImplementedReset();

    void Update(float afTimeStep);

    bool DoAction(eLuxPlayerAction aAction, bool abPressed);
    bool AnimationIsOver();

private:
    void UpdateSwayPhysics(float afTimeStep);

    float mfFadeInSpeed;
    float mfFadeOutSpeed;

    bool mbHasSwayPhysics;
    float mfMaxSwayVel;
    cVector2f mvSwayAngleLimits;
    cVector2f mvSwayDownAngleLimits;
    cVector3f mvSwayPinDir;
    float mfSwayGravity;
    float mfSwayFriction;
    float mfSwayPlayerSpeedMul;
    float mfSwayCameraRollMul;
    tString msSkipSwaySubMesh;

    float mfSwayAngle;
    float mfSwayVel;

    std::vector<cColor> mvLightFadeOutColor;
    std::vector<cColor> mvDefaultLightColors;
    std::vector<bool> mvDefaultLightFlicker;
    std::vector<cMatrixf> mvDefaultSubMeshMatrix;
};

//----------------------------------------------



#endif // LUX_HAND_OBJECT_LIGHT_SOURCE_H
