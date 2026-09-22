#include "graphics/AnimationTrack.h"

#include "math/Math.h"
#include "graphics/Animation.h"
#include "system/LowLevelSystem.h"
#include "scene/Node3D.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cAnimationTrack::cAnimationTrack(const tString &asName, tAnimTransformFlag aTransformFlags,
                                 cAnimation *apParent)
{
    msName = asName;
    mTransformFlags = aTransformFlags;
    mpParent = apParent;

    mfMaxFrameTime = 0;

    mlNodeIdx = -1;
}

//-----------------------------------------------------------------------

cAnimationTrack::~cAnimationTrack()
{
    STLDeleteAll(mvKeyFrames);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cAnimationTrack::ResizeKeyFrames(int alSize)
{
    mvKeyFrames.reserve(alSize);
}

//-----------------------------------------------------------------------

cKeyFrame* cAnimationTrack::CreateKeyFrame(float afTime)
{
    cKeyFrame* pFrame = hplNew( cKeyFrame,());
    pFrame->time = afTime;

    //Check so that this is the first
    if(afTime > mfMaxFrameTime || mvKeyFrames.empty())
    {
        mvKeyFrames.push_back(pFrame);
        mfMaxFrameTime = afTime;
    }
    else
    {
        tKeyFramePtrVecIt it = mvKeyFrames.begin();
        for(; it != mvKeyFrames.end(); it++)
        {
            if(afTime < (*it)->time)
            {
                break;
            }
        }
        mvKeyFrames.insert(it,pFrame);
    }

    return pFrame;
}

//-----------------------------------------------------------------------

void cAnimationTrack::ClearKeyFrames()
{
    STLDeleteAll(mvKeyFrames);
    mvKeyFrames.clear();
}

//-----------------------------------------------------------------------

void cAnimationTrack::ApplyToNode(cNode3D* apNode, float afTime, float afWeight, bool bLoop)
{
    if(mvKeyFrames.empty())
    {
        return;
    }

    cKeyFrame Frame = GetInterpolatedKeyFrame(afTime, bLoop);

    //Rotation
    cQuaternion qRot = cMath::QuaternionSlerp(afWeight, cQuaternion::Identity, Frame.rotation, true);
    apNode->AddRotation(qRot);

    //Translation
    cVector3f vTrans = Frame.trans * afWeight;
    apNode->AddTranslation(vTrans);
}

//-----------------------------------------------------------------------

cKeyFrame cAnimationTrack::GetInterpolatedKeyFrame(float afTime, bool bLoop)
{
    cKeyFrame ResultKeyFrame;
    ResultKeyFrame.time = afTime;

    if(mvKeyFrames.empty())
    {
        ResultKeyFrame.rotation = cQuaternion(1,0,0,0);
        ResultKeyFrame.trans = 0;
        return ResultKeyFrame;
    }

    cKeyFrame *pKeyFrameA = NULL;
    cKeyFrame *pKeyFrameB = NULL;

    float fT = GetKeyFramesAtTime(afTime, &pKeyFrameA, &pKeyFrameB, bLoop);


    if(fT == 0.0f)
    {
        ResultKeyFrame.rotation = pKeyFrameA->rotation;
        ResultKeyFrame.trans = pKeyFrameA->trans;
    }
    else
    {
        //Do a linear interpolation
        //This should include spline stuff later on.


        cQuaternion rotA = pKeyFrameA->rotation;
        cQuaternion rotB = pKeyFrameB->rotation;
        /*
        if ( rotA.v.x == -1 * rotB.v.x
            && rotA.v.y == -1 * rotB.v.y
            && rotA.v.z == -1 * rotB.v.z
            && rotA.w == -1 * rotB.w )
        {
            ResultKeyFrame.rotation = rotB;
        }
        else
        {
            ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, rotA,
                                                rotB, true);
        }*/

        rotA.Normalize();
        rotB.Normalize();

        if ( rotA.w < 0 )
        {
            rotA = rotA * -1.0f;
        }

        if ( rotB.w < 0 )
        {
            rotB = rotB * -1.0f;
        }


        ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, rotA,
                                  rotB, true);

        /*

        ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, pKeyFrameA->rotation,
                                                pKeyFrameB->rotation, true);
                                                */
        ResultKeyFrame.trans = pKeyFrameA->trans * (1 - fT) + pKeyFrameB->trans * fT;
    }

    return ResultKeyFrame;
}

//-----------------------------------------------------------------------

float cAnimationTrack::GetKeyFramesAtTime(float afTime, cKeyFrame** apKeyFrameA,cKeyFrame** apKeyFrameB, bool bLoop)
{
    float fTotalAnimLength = mpParent->GetLength();

    //Get the number of frames
    const int lSize = (int)mvKeyFrames.size();
    int lFirst = 0, lLast = lSize - 1;

    //Find the second frame.
    int lIdxB=-1;

    while(lFirst <= lLast)
    {
        int lMid = (lFirst + lLast) >> 1;
        int lBefore = lMid > 0 ? lMid - 1 : 0;

        if(afTime < mvKeyFrames[lBefore]->time)
        {
            lLast = lMid - 1;
        }
        else if(afTime > mvKeyFrames[lMid]->time)
        {
            lFirst = lMid + 1;
        }
        else
        {
            lIdxB = lMid;
            break;
        }
    }

    //If first frame was found, the lowest time is not 0.
    //If so return the first frame only.
    if(lIdxB <= 0)
    {
        /////////////////////
        // Return the first and last frame if the animation is looping
        if(bLoop)
        {
            afTime = fmod(afTime, fTotalAnimLength + kEpsilonf);

            if(afTime < mvKeyFrames[0]->time)
            {
                *apKeyFrameA = mvKeyFrames[mvKeyFrames.size() - 1];
                *apKeyFrameB = mvKeyFrames[0];

                return afTime / (mvKeyFrames[0]->time + kEpsilonf);
            }
        }

        *apKeyFrameA = mvKeyFrames[0];
        *apKeyFrameB = mvKeyFrames[0];
        return 0.0f;
    }

    //Get the frames
    *apKeyFrameA = mvKeyFrames[lIdxB - 1];
    *apKeyFrameB = mvKeyFrames[lIdxB];

    float fDeltaT = (*apKeyFrameB)->time - (*apKeyFrameA)->time + kEpsilonf;

    return (afTime - (*apKeyFrameA)->time) / fDeltaT;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
}