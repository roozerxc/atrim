#ifndef LUX_HAND_OBJECT_MELEE_H
#define LUX_HAND_OBJECT_MELEE_H

//----------------------------------------------

#include "LuxHandObject.h"


//----------------------------------------------

class cLuxHandObject_Melee : public iLuxHandObject
{
public:    
    cLuxHandObject_Melee(const tString& asName, cLuxPlayerHands *apHands);
    ~cLuxHandObject_Melee();
    
    void LoadImplementedVars(cXmlElement *apVarsElem);

    void ImplementedCreateEntity(cLuxMap *apMap);
    void ImplementedDestroyEntity(cLuxMap *apMap);

    void RenderSolid(cRendererCallbackFunctions* apFunctions);
    void RenderTrans(cRendererCallbackFunctions* apFunctions);
    
    void ImplementedReset();

    void Update(float afTimeStep);

    bool DoAction(eLuxPlayerAction aAction, bool abPressed);
    bool AnimationIsOver();

private:
    void UpdateCharge(float afTimeStep);
    void UpdateCheckDamageEvent(float afTimeStep);
    
    
    void Charge();
    void Swing();

    tString msHandsAnim_Swing;
    tString msHandsAnim_SwingCharge;
    tString msHandsAnim_Charge;
    tString msSwingSound;
    tString msSwingChargeSound;
    tString msChargeSound;
    tString msChargeDoneSound;
    tString msHitSound;

    cVector3f mvCollideShapeSize;
    cVector3f mvCollideShapeOffset;
    iCollideShape *mpCollideShape;

    float mfChargePlayerSpeedMul;
    float mfChargeTime;
    float mfRelativeDamageTime;

    float mfAttackForce;
    float mfMaxAttackForce;

    float mfAttackMinDamage;
    float mfAttackMaxDamage;
    float mfAttackHitSpeed;
    int mlAttackStrength;
};

//----------------------------------------------



#endif // LUX_HAND_OBJECT_MELEE_H
