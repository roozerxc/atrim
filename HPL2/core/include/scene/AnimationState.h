#ifndef HPL_ANIMATION_STATE_H
#define HPL_ANIMATION_STATE_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

#include "engine/SaveGame.h"

namespace hpl
{

class cAnimation;
class cAnimationManager;

//---------------------------------------------

class cAnimationEvent
{
public:
    float mfTime;
    eAnimationEventType mType;
    tString msValue;
};

//---------------------------------------------

class cAnimationState
{
public:
    cAnimationState(cAnimation* apAnimation,const tString &asName,
                    cAnimationManager *apAnimationManager);
    ~cAnimationState();

    const tString& GetName()
    {
        return msName;
    }

    void Update(float afTimeStep);

    bool DataIsInMeshFile()
    {
        return mpAnimationManager==NULL;
    }

    bool IsFading();

    /**
     * If the animation has reached the end.
     */
    bool IsOver();

    void FadeIn(float afTime);
    void FadeOut(float afTime);

    void SetLength(float afLength);
    float GetLength();

    void SetWeight(float afWeight);
    float GetWeight();

    void SetSpeed(float afSpeed);
    float GetSpeed();

    void SetBaseSpeed(float afSpeed);
    float GetBaseSpeed();

    void SetTimePosition(float afPosition);
    float GetTimePosition();
    float GetPreviousTimePosition();

    /**
     * Set the relative postion. 0 = start, 1 = end
     * \param afPosition
     */
    void SetRelativeTimePosition(float afPosition);

    /**
    * Get the relative postion. 0 = start, 1 = end
    */
    float GetRelativeTimePosition();

    bool IsActive();
    void SetActive(bool abActive);

    bool IsLooping();
    void SetLoop(bool abLoop);

    bool IsPaused();
    void SetPaused(bool abPaused);

    void SetSpecialEventTime(float afT)
    {
        mfSpecialEventTime = afT;
    }
    float GetSpecialEventTime()
    {
        return mfSpecialEventTime;
    }
    bool IsAfterSpecialEvent();
    bool IsBeforeSpecialEvent();

    void AddTimePosition(float afAdd);

    cAnimation* GetAnimation();

    cAnimationEvent *CreateEvent();
    cAnimationEvent *GetEvent(int alIdx);
    int GetEventNum();

    float GetFadeStep()
    {
        return mfFadeStep;
    }
    void SetFadeStep(float afX)
    {
        mfFadeStep = afX;
    }

private:
    tString msName;

    cAnimationManager *mpAnimationManager;

    cAnimation* mpAnimation;

    std::vector<cAnimationEvent*> mvEvents;

    //Properties of the animation
    float mfLength;
    float mfWeight;
    float mfSpeed;
    float mfTimePos;
    float mfPrevTimePos;

    float mfBaseSpeed;

    float mfSpecialEventTime;

    bool mbActive;
    bool mbLoop;
    bool mbPaused;

    //properties for update
    float mfFadeStep;
};

};
#endif // HPL_ANIMATION_STATE_H
