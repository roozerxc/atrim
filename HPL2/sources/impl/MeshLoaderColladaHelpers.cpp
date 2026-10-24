#include "impl/MeshLoaderCollada.h"
#include "impl/XmlPugi.h"

#include <string.h>

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "system/String.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"
#include "graphics/Material.h"

#include "graphics/Bone.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"


#include "math/Math.h"

namespace hpl
{

#define GetAdress(sStr) if(sStr.length()>0 && sStr[0]=='#') sStr = cString::Sub(sStr,1);
//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//This fucntions checks if y is up and if not switches palces with z and y.
cVector3f cMeshLoaderCollada::GetVectorPos(const cVector3f &avVec)
{
    if(mbZToY)
    {
        return cVector3f(-avVec.x, avVec.z, avVec.y);
    }
    else
    {
        return avVec;
    }
}

cVector3f cMeshLoaderCollada::GetVectorNormal(const cVector3f &avVec)
{
    if(mbZToY)
    {
        return cVector3f(-avVec.x, avVec.z, avVec.y);
    }
    else
    {
        return avVec;
    }
}

cMatrixf cMeshLoaderCollada::GetMatrix(const cMatrixf &amtxA)
{
    return amtxA;
}

cVector3f cMeshLoaderCollada::GetVectorPosFromPtr(float *apVec)
{
    if(mbZToY)
    {
        return cVector3f(apVec[0], apVec[2], apVec[1]);
    }
    else
    {
        return cVector3f(apVec[0], apVec[1], apVec[2]);
    }
}

cVector3f cMeshLoaderCollada::GetVectorRotationFromPtr(float *apVec)
{
    return cVector3f(apVec[0], apVec[1], apVec[2]);
}

cVector3f cMeshLoaderCollada::GetVectorScaleFromPtr(float *apVec)
{
    if(mbZToY)
    {
        return cVector3f(apVec[0], apVec[2], apVec[1]);
    }
    else
    {
        return cVector3f(apVec[0], apVec[1], apVec[2]);
    }
}

//-----------------------------------------------------------------------

class cTempAnimData
{
public:
    cTempAnimData(): mvTrans(0, 0, 0), mvRot(0, 0, 0), mvScale(1, 1, 1) {}

    cVector3f mvTrans;
    cVector3f mvRot;
    cVector3f mvScale;
    float mfTime;

    int mlIndex;
};

typedef std::vector<cTempAnimData> tTempAnimDataVec;

typedef std::set<float> tTempTimesSet;
typedef std::set<float>::iterator tTempTimesSetIt;

static cTempAnimData *GetTempAnimData(float afTime, tTempAnimDataVec &avTempData)
{
    for(size_t i = 0; i < avTempData.size(); ++i)
    {
        if(avTempData[i].mfTime == afTime)
        {
            return &avTempData[i];
        }
    }

    return NULL;
}

/////////////////

//Get times in TimeVec closes to afTime.
static void GetAnimTimes(float afTime, float *apTimeBefore, float *apTimeAfter, tFloatVec* apTimeVec)
{
    *apTimeBefore = -1;
    *apTimeAfter = -1;
    for(size_t i = 0; i < apTimeVec->size(); ++i)
    {
        float fTime = (*apTimeVec)[i];

        if(fTime <= afTime)
        {
            *apTimeBefore = fTime;
        }
        else if(fTime >= afTime)
        {
            *apTimeAfter = fTime;
        }
    }
}

/////////////////

cAnimationTrack *cMeshLoaderCollada::CreateAnimTrack(cAnimation *apAnimation, cSkeleton *apSkeleton,
        cColladaAnimation &aAnim, cColladaScene *apScene)
{
    tTempAnimDataVec vTempData;
    tTempTimesSet setTempTimes;

    //Log("Animation: %s Target: %s\n",aAnim.msId.c_str(), aAnim.msTargetNode.c_str());

    //Get the node that will be animated by this track
    cColladaNode* pNode = apScene->GetNode(aAnim.msTargetNode);
    if(pNode == NULL)
    {
        Error("Couldn't find node '%s' for animation id '%s'\n", aAnim.msTargetNode.c_str(), aAnim.msId.c_str());
        return NULL;
    }

    cBone* pBone = NULL;

    if(apSkeleton)
    {
        pBone = apSkeleton->GetBoneByName(aAnim.msTargetNode);
        if(pBone == NULL)
        {
            Error("Couldn't find bone '%s'\n", aAnim.msTargetNode.c_str());
            return NULL;
        }
    }
    else
    {

    }

    /////////////////////////////////////////////////
    //Go through all sample and add the different times.
    for(size_t i = 0; i < aAnim.mvSamplers.size(); i++)
    {
        cColladaSampler &Sampler = aAnim.mvSamplers[i];

        tFloatVec *pValueVec = aAnim.GetSourceVec(Sampler.msTimeArray);
        if(pValueVec == NULL)
        {
            Error("Time array not found in sampler '%s'!\n", Sampler.msId.c_str());
            return NULL;
        }

        //Go through all and add to set.
        for(tFloatVecIt it = pValueVec->begin(); it != pValueVec->end(); ++it)
        {
            setTempTimes.insert(*it);
        }
    }

    ///////////////////////////////
    //Resize temp data and fill with times used.
    vTempData.resize(setTempTimes.size());
    int lCount = 0;
    for(tTempTimesSetIt it = setTempTimes.begin(); it != setTempTimes.end(); ++it, ++lCount)
    {
        vTempData[lCount].mfTime = *it;
        vTempData[lCount].mlIndex = lCount;
    }

    bool bLoadedTranslation = false;
    bool bLoadedRotation = false;

    /////////////////////////////////////////////////
    //Go through all samples and get data
    for(size_t i = 0; i < aAnim.mvSamplers.size(); i++)
    {
        cColladaSampler &Sampler = aAnim.mvSamplers[i];

        //Log("Sampler %s\n",Sampler.msId.c_str());

        //////////////////////////
        //Get the times
        tFloatVec *pTimeVec = aAnim.GetSourceVec(Sampler.msTimeArray);
        if(pTimeVec == NULL)
        {
            Error("Time array not found!\n");
            return NULL;
        }

        //Get the Sid of the transformation that this sampler changes.
        tString sTarget = cString::SetFileExt(cString::GetFileName(Sampler.msTarget), "");
        tString sExt = cString::ToLowerCase(cString::GetFileExt(Sampler.msTarget));

        ////////////////////
        //Get the transformation and add the values accordingly
        cColladaTransform *pTrans = pNode->GetTransform(sTarget);
        if(pTrans == NULL)
        {
            //Error("Transform '%s' not found!\n",sTarget.c_str());
            continue;
        }

        ///////////////////////////
        //Get the values for the transformation
        tFloatVec *pValueVec = aAnim.GetSourceVec(Sampler.msValueArray);
        if(pValueVec == NULL)
        {
            Error("Value array not found!\n");
            return NULL;
        }

        ////////////////////////////////
        //Set the translation in this sampler
        if(pTrans->msType == "translate")
        {
            //Log("Loading Translation\n");
            bLoadedTranslation = true;

            for(size_t j = 0; j < pTimeVec->size(); j++)
            {
                //Check the translation type
                //If only a single axis is changes the others are set to
                //the node translation. THIS MIGHT BE WRONG!
                //TODO: Maybe the bind matrix should be here:
                cVector3f vTrans = pNode->m_mtxTransform.GetTranslation();

                //Get the temp data:
                cTempAnimData * pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
                if(pTempData == NULL)
                {
                    Error("Code error at %d\n", __LINE__);
                    return NULL;
                }

                // XYZ
                if(sExt == "")
                {
                    pTempData->mvTrans = cVector3f((*pValueVec)[j * 3 + 0],
                                                   (*pValueVec)[j * 3 + 1],
                                                   (*pValueVec)[j * 3 + 2]);
                }
                // X
                else if(sExt == "x")
                {
                    pTempData->mvTrans = cVector3f((*pValueVec)[j], vTrans.y, vTrans.z);
                }
                // Y
                else if(sExt == "y")
                {
                    pTempData->mvTrans = cVector3f(vTrans.x, (*pValueVec)[j], vTrans.z);
                }
                // Z
                else if(sExt == "z")
                {
                    pTempData->mvTrans = cVector3f(vTrans.x, vTrans.y, (*pValueVec)[j]);
                }

                //Scale and rotate data
                pTempData->mvTrans = (pTempData->mvTrans);

                //What we wanna use is the relative movement of the bone.
                //The exported translation sets new local translation!.
                if(pBone)
                {
                    pTempData->mvTrans -= pBone->GetLocalTransform().GetTranslation();
                }
            }

            ////////////////////////////////
            //Go through all of Temp data and find times not added
            float fTimeBefore = -1, fTimeAfter = -1;
            for(size_t j = 0; j < vTempData.size(); ++j)
            {
                GetAnimTimes(vTempData[j].mfTime, &fTimeBefore, &fTimeAfter, pTimeVec);

                //Time exists
                if(fTimeBefore == vTempData[j].mfTime)
                {
                    //Log("Sample %s time %f is in place!\n",Sampler.msId.c_str(), vTempData[j].mfTime);
                    continue;
                }
                //Time between two keys
                else if(fTimeBefore >= 0 && fTimeAfter >= 0)
                {
                    cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);
                    cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

                    float fT = (vTempData[j].mfTime - pBefore->mfTime) / (pAfter->mfTime -  pBefore->mfTime);

                    vTempData[j].mvTrans = pBefore->mvTrans * (1.0f-fT) + pAfter->mvTrans * fT;

                    //Log("Interpolated sample %s time %f between %f and %f. T=%f\n",
                    //                        Sampler.msId.c_str(), vTempData[j].mfTime,
                    //                        pBefore->mfTime,pAfter->mfTime,fT);
                }
                //Time before start
                else if(fTimeBefore < 0 && fTimeAfter >= 0)
                {
                    cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

                    vTempData[j].mvTrans = pAfter->mvTrans;

                    //Log("Added before at sample %s time %f using %f\n",
                    //                    Sampler.msId.c_str(), vTempData[j].mfTime,
                    //                    pAfter->mfTime);
                }
                //Time after end
                else if(fTimeBefore >= 0 && fTimeAfter < 0)
                {
                    cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);

                    vTempData[j].mvTrans = pBefore->mvTrans;

                    //Log("Added after at sample %s time %f using %f\n",
                    //                        Sampler.msId.c_str(), vTempData[j].mfTime,
                    //                        pBefore->mfTime);
                }
            }
        }
        //////////////////////////////////
        //Add the rotation in this sampler
        else if(pTrans->msType == "rotate")
        {
            bLoadedRotation = true;

            for(size_t j = 0; j < pTimeVec->size(); j++)
            {
                //Get the temp data:
                cTempAnimData * pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
                if(pTempData == NULL)
                {
                    Error("Code error at %d\n", __LINE__);
                    return NULL;
                }

                pTempData->mvRot += (cVector3f(pTrans->mvValues[0],
                                               pTrans->mvValues[1],
                                               pTrans->mvValues[2]) * (*pValueVec)[j]);
            }

            size_t lVecNum = 0;
            if(pTrans->mvValues[0] > 0.001f)
            {
                lVecNum = 0;
            }
            else if(pTrans->mvValues[1] > 0.001f)
            {
                lVecNum = 1;
            }
            else if(pTrans->mvValues[2] > 0.001f)
            {
                lVecNum = 2;
            }

            ////////////////////////////////
            //Go through all of Temp data and find times not added
            float fTimeBefore = -1, fTimeAfter = -1;
            for(size_t j = 0; j < vTempData.size(); ++j)
            {
                GetAnimTimes(vTempData[j].mfTime, &fTimeBefore, &fTimeAfter, pTimeVec);

                /////////////////////
                //Time exists
                if(fTimeBefore == vTempData[j].mfTime)
                {
                    //Log("Sample %s time %f is in place!\n",Sampler.msId.c_str(), vTempData[j].mfTime);
                    continue;
                }
                /////////////////////
                //Time between two keys
                else if(fTimeBefore >= 0 && fTimeAfter >= 0)
                {
                    cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);
                    cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

                    float fT = (vTempData[j].mfTime - pBefore->mfTime) / (pAfter->mfTime -  pBefore->mfTime);

                    vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum] * (1.0f-fT) +
                                                    pAfter->mvRot.v[lVecNum] * fT;

                    //Log("Interpolated sample %s time %f between %f and %f. T=%f\n",
                    //    Sampler.msId.c_str(), vTempData[j].mfTime,
                    //    pBefore->mfTime,pAfter->mfTime,fT);
                }
                /////////////////////
                //Time before start
                else if(fTimeBefore < 0 && fTimeAfter >= 0)
                {
                    cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

                    vTempData[j].mvRot.v[lVecNum] = pAfter->mvRot.v[lVecNum];

                    //Log("Added before at sample %s time %f using %f\n",
                    //    Sampler.msId.c_str(), vTempData[j].mfTime,
                    //    pAfter->mfTime);
                }
                /////////////////////
                //Time after end
                else if(fTimeBefore >= 0 && fTimeAfter < 0)
                {
                    cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);

                    vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum];
                }
            }
        }
        //////////////////////////////////
        //Add the rotation and translation from a matrix in this sampler
        else if(pTrans->msType == "matrix")
        {
            bLoadedRotation = true;
            bLoadedTranslation = true;

            for(size_t j = 0; j < pTimeVec->size(); j++)
            {
                //Get the temp data:
                cTempAnimData * pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
                if(pTempData == NULL)
                {
                    Error("Code error at %d\n", __LINE__);
                    return NULL;
                }

                cMatrixf mtxFrame = GetMatrix(cMatrixf(&(*pValueVec)[j * 16]));

                if(pBone)
                {
                    cMatrixf mtxBone = pBone->GetLocalTransform();
                    cMatrixf mtxInvBone = cMath::MatrixInverse(mtxBone);
                    cVector3f vChange = mtxFrame.GetTranslation() - mtxBone.GetTranslation();
                    mtxFrame = cMath::MatrixMul(mtxInvBone.GetRotation(), mtxFrame.GetRotation());
                    mtxFrame.SetTranslation(vChange);
                }

                pTempData->mvTrans += (mtxFrame.GetTranslation());

                pTempData->mvRot += (cMath::Vector3ToDeg(cMath::MatrixToEulerAngles(mtxFrame, eEulerRotationOrder_XYZ)));

            }



            ////////////////////////////////
            //Go through all of Temp data and find times not added
            float fTimeBefore = -1, fTimeAfter = -1;
            for(size_t j = 0; j < vTempData.size(); ++j)
            {
                GetAnimTimes(vTempData[j].mfTime, &fTimeBefore, &fTimeAfter, pTimeVec);

                /////////////////////
                //Time exists
                if(fTimeBefore == vTempData[j].mfTime)
                {
                    //Log("Sample %s time %f is in place!\n",Sampler.msId.c_str(), vTempData[j].mfTime);
                    continue;
                }
                /////////////////////
                //Time between two keys
                else if(fTimeBefore >= 0 && fTimeAfter >= 0)
                {
                    cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);
                    cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

                    float fT = (vTempData[j].mfTime - pBefore->mfTime) / (pAfter->mfTime -  pBefore->mfTime);

                    for(size_t lVecNum = 0; lVecNum < 3; ++lVecNum)
                        vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum] * (1.0f-fT) +
                                                        pAfter->mvRot.v[lVecNum] * fT;

                    vTempData[j].mvTrans = pBefore->mvTrans * (1.0f-fT) + pAfter->mvTrans * fT;
                }
                /////////////////////
                //Time before start
                else if(fTimeBefore < 0 && fTimeAfter >= 0)
                {
                    cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

                    for(size_t lVecNum = 0; lVecNum < 3; ++lVecNum)
                    {
                        vTempData[j].mvRot.v[lVecNum] = pAfter->mvRot.v[lVecNum];
                    }

                    vTempData[j].mvTrans = pAfter->mvTrans;
                }
                /////////////////////
                //Time after end
                else if(fTimeBefore >= 0 && fTimeAfter < 0)
                {
                    cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);

                    for(size_t lVecNum = 0; lVecNum < 3; ++lVecNum)
                    {
                        vTempData[j].mvRot.v[lVecNum] = pBefore->mvRot.v[lVecNum];
                    }

                    vTempData[j].mvTrans = pBefore->mvTrans;
                }
            }
        }
    }

    //Log(" Loaded trans: %d, rot: %d\n", bLoadedTranslation, bLoadedRotation);


    /*Log("Animation %s\n",aAnim.msName.c_str());
    for(size_t i=0; i <vTempData.size(); i++)
    {
        Log("Time: %f T: (%s) R: (%s) S: (%s)\n", vTempData[i].mfTime,
            vTempData[i].mvTrans.ToString().c_str(),
            vTempData[i].mvRot.ToString().c_str(),
            vTempData[i].mvScale.ToString().c_str());
    }*/

    ////////////////////////////////////
    //Create new animation track
    cAnimationTrack * pTrack = apAnimation->CreateTrack(aAnim.msTargetNode,
                               eAnimTransformFlag_Rotate |
                               eAnimTransformFlag_Translate);
    ////////////////////////////////////
    //Iterate the temporary data and add to the track.
    for(size_t i = 0; i < vTempData.size(); i++)
    {
        cKeyFrame *pFrame = pTrack->CreateKeyFrame(vTempData[i].mfTime - apScene->mfStartTime);

        //////////////////
        //Get the the node matrix offset
        cMatrixf mtxLocal = pNode->m_mtxTransform;
        cMatrixf mtxRotChange = mtxLocal.GetRotation();
        cVector3f vTransChange = mtxLocal.GetTranslation();
        if(pBone)
        {
            cMatrixf mtxBone = pBone->GetLocalTransform();
            cMatrixf mtxInvBone = cMath::MatrixInverse(mtxBone);

            mtxRotChange = cMath::MatrixMul(mtxLocal.GetRotation(), mtxInvBone.GetRotation());
            vTransChange = mtxLocal.GetTranslation() - mtxBone.GetTranslation();
        }

        ///////////////////////////
        //Set Translation
        if(bLoadedTranslation)
        {
            pFrame->trans = vTempData[i].mvTrans;
        }
        else
        {
            pFrame->trans = vTransChange;
        }

        ///////////////////////////
        //Set Rotation
        if(bLoadedRotation)
        {
            //Create the quaternion from rotations.
            cVector3f vRadRot = vTempData[i].mvRot;
            vRadRot = cVector3f(cMath::ToRad(vRadRot.x), cMath::ToRad(vRadRot.y), cMath::ToRad(vRadRot.z) );
            cMatrixf mtxRot = cMath::MatrixRotate(vRadRot, eEulerRotationOrder_XYZ);
            cQuaternion qRot;
            qRot.FromRotationMatrix(mtxRot);

            pFrame->rotation = qRot;
            //pFrame->base_angles = vRadRot;
        }
        else
        {
            pFrame->rotation.FromRotationMatrix(mtxRotChange);
            //pFrame->base_angles = cMath::MatrixToEulerAngles(mtxRotChange, eEulerRotationOrder_XYZ);
        }
    }

    return pTrack;
}

static tString gsTemp;
static const char *GetTabs(int alDepth)
{
    gsTemp = "";

    for(int i = 0; i < alDepth; i++)
    {
        gsTemp += "\t";
    }

    return gsTemp.c_str();
}

//-----------------------------------------------------------------------


void cMeshLoaderCollada::CalcLocalMatrixAfterControllerBindMatrixRec(cBone* apBone, cMatrixf a_mtxParentGlobal, int alDepth)
{
    //Log("%s %s \n",GetTabs(alDepth),apBone->GetName().c_str());

    cMatrixf mtxGlobal;
    //////////////////////
    //The bone is bound to the skin and has a world matrix
    if(apBone->GetValue() != 0)
    {
        mtxGlobal = apBone->GetLocalTransform();
        cMatrixf mtxInvParent = cMath::MatrixInverse(a_mtxParentGlobal);

        cMatrixf mtxLocal = cMath::MatrixMul(mtxInvParent, mtxGlobal);

        apBone->SetTransform(mtxLocal);
    }
    ////////////////////////
    // Bone is not bound and has local matrix.
    else
    {
        mtxGlobal = apBone->GetWorldTransform();
    }


    cBoneIterator it = apBone->GetChildIterator();
    while(it.HasNext())
    {
        CalcLocalMatrixAfterControllerBindMatrixRec(it.Next(), mtxGlobal, alDepth + 1);
    }
}


//-----------------------------------------------------------------------

void cMeshLoaderCollada::CreateSkeletonBone(cColladaNode* apColladaNode, cBone* apParentBone)
{
    cBone *pBone = apParentBone->CreateChildBone(apColladaNode->msId, apColladaNode->msSid);

    pBone->SetTransform(apColladaNode->m_mtxTransform);

    tColladaNodeListIt it = apColladaNode->mlstChildren.begin();
    for(; it != apColladaNode->mlstChildren.end(); it++)
    {
        CreateSkeletonBone(*it, pBone);
    }
}

//-----------------------------------------------------------------------

iVertexBuffer *cMeshLoaderCollada::CreateVertexBuffer(cColladaGeometry & aGeometry,
        eVertexBufferUsageType aUsageType)
//,tColladaExtraVtxListVec &vExtraVtxVec)
{
    //tVertexVec vVertexVec;
    //tUIntVec vIndexVec;
    //SplitVertices(aGeometry,vExtraVtxVec,vVertexVec,vIndexVec);

    //Log("Creating vertex buffer for '%s'\n",aGeometry.msName.c_str());

    //TEMP: use the one in geometry directly.
    //vExtraVtxVec = aGeometry.mvExtraVtxVec;

    //Create vertex buffer and fill it
    iVertexBuffer *pVtxBuff = mpLowLevelGraphics->CreateVertexBuffer(
                                  eVertexBufferType_Hardware,
                                  eVertexBufferDrawType_Tri, aUsageType,
                                  (int)aGeometry.mvVertexVec.size(), (int)aGeometry.mvIndexVec.size());
    pVtxBuff->CreateElementArray( eVertexBufferElement_Position, eVertexBufferElementFormat_Float, 4);
    pVtxBuff->CreateElementArray( eVertexBufferElement_Normal, eVertexBufferElementFormat_Float, 3);
    pVtxBuff->CreateElementArray( eVertexBufferElement_Texture0, eVertexBufferElementFormat_Float, 3);
    pVtxBuff->CreateElementArray( eVertexBufferElement_Color0, eVertexBufferElementFormat_Float, 4);
    pVtxBuff->CreateElementArray( eVertexBufferElement_Texture1Tangent, eVertexBufferElementFormat_Float, 4);

    pVtxBuff->ResizeArray(eVertexBufferElement_Texture1Tangent, (int)aGeometry.mvTangents.size());

    //Add vertices
    for(size_t j = 0; j < aGeometry.mvVertexVec.size(); j++)
    {
        pVtxBuff->AddVertexVec3f(eVertexBufferElement_Position, aGeometry.mvVertexVec[j].pos);
        pVtxBuff->AddVertexVec3f(eVertexBufferElement_Normal, aGeometry.mvVertexVec[j].norm);
        pVtxBuff->AddVertexVec3f(eVertexBufferElement_Texture0, aGeometry.mvVertexVec[j].tex);

        pVtxBuff->AddVertexColor(eVertexBufferElement_Color0, cColor(1, 1));
    }

    //Add tangents
    memcpy(pVtxBuff->GetFloatArray(eVertexBufferElement_Texture1Tangent), &aGeometry.mvTangents[0], aGeometry.mvTangents.size()*sizeof(float));

    //Add indices
    for(size_t j = 0; j < aGeometry.mvIndexVec.size(); j++)
    {
        //Flip order of indices
        size_t idx = (j / 3) * 3 + (2 - (j % 3));

        pVtxBuff->AddIndex(aGeometry.mvIndexVec[idx]);
    }

    //Compile the vertex buffer
    pVtxBuff->Compile(0);//eVertexCompileFlag_CreateTangents);

    return pVtxBuff;
}



//-----------------------------------------------------------------------

void cMeshLoaderCollada::LoadLights(pugi::xml_node apRootElem, tColladaLightVec &avColladaLightVec)
{
    pugi::xml_node pLightElem = apRootElem.child("light");
    for(; pLightElem;    pLightElem = pLightElem.next_sibling("light"))
    {
        cColladaLight Light;
        Light.msId = cString::ToString(Xml::Attr(pLightElem, "id"), "");
        Light.msName = cString::ToString(Xml::Attr(pLightElem, "name"), "");

        pugi::xml_node pTechniqueCommonElem = pLightElem.child("technique_common");

        //////////////////////////////////////////////
        //COLLADA 1.4
        if(pTechniqueCommonElem)
        {
            pugi::xml_node pTypeElem = Xml::FirstElem(pTechniqueCommonElem);
            if(!pTypeElem)
            {
                Log("No Type element found!\n");
                continue;
            }

            Light.msType = cString::ToString(pTypeElem.name(), "");

            /////////////
            //Color
            pugi::xml_node pParamElem = pTypeElem.child("color");
            if(pParamElem)
            {
                const char *pText = Xml::TextOrEmpty(pParamElem);
                tFloatVec vColor;
                cString::GetFloatVec(pText, vColor);
                Light.mDiffuseColor.r = vColor[0];
                Light.mDiffuseColor.g = vColor[1];
                Light.mDiffuseColor.b = vColor[2];
            }
            else
            {
                Light.mDiffuseColor = cColor(1, 1);
            }

            /////////////
            //Angle
            pParamElem = pTypeElem.child("falloff_angle");
            if(pParamElem)
            {
                const char *pText = Xml::TextOrEmpty(pParamElem);
                Light.mfAngle = cString::ToFloat(pText, 0);
            }
            else
            {
                Light.mfAngle = 0;
            }
        }
        ///////////////////////////////////////////////
        //COLLADA 1.3 (NOT SUPPORTED ANY LONGER)
        else
        {
            Light.msType = cString::ToLowerCase(cString::ToString(Xml::Attr(pLightElem, "type"), ""));

            pugi::xml_node pParamElem = pLightElem.child("param");
            for(; pParamElem; pParamElem = pParamElem.next_sibling("param"))
            {
                tString sName = cString::ToString(Xml::Attr(pParamElem, "name"), "");
                const char *pText = Xml::TextOrEmpty(pParamElem);

                if(sName == "COLOR")
                {
                    tFloatVec vColor;
                    cString::GetFloatVec(pText, vColor);
                    Light.mDiffuseColor.r = vColor[0];
                    Light.mDiffuseColor.g = vColor[1];
                    Light.mDiffuseColor.b = vColor[2];
                }
                else if(sName == "ANGLE")
                {
                    Light.mfAngle = cString::ToFloat(pText, 0);
                }
            }
        }

        //Log("Loaded light '%s', type '%s', color: %f %f %f\n", Light.msId.c_str(),
        //                Light.msType.c_str(),
        //                Light.mDiffuseColor.r,Light.mDiffuseColor.g,Light.mDiffuseColor.b);

        avColladaLightVec.push_back(Light);
    }
}

//-----------------------------------------------------------------------

static cColladaAnimation &GetAnimationFromTarget(const tString& asTargetNode,
        tColladaAnimationVec &avAnimations)
{
    for(size_t i = 0; i < avAnimations.size(); i++)
    {
        if(avAnimations[i].msTargetNode == asTargetNode)
        {
            return avAnimations[i];
        }
    }

    //No animation with that target found, create new.
    avAnimations.push_back(cColladaAnimation());
    cColladaAnimation& Anim = avAnimations[avAnimations.size() - 1];
    Anim.msTargetNode = asTargetNode;

    return Anim;
}

void cMeshLoaderCollada::LoadAnimations(pugi::xml_node apRootElem, tColladaAnimationVec &avAnimations,
                                        cColladaScene *apColladaScene)
{
    pugi::xml_node pAnimElem = apRootElem.child("animation");
    for(; pAnimElem; pAnimElem = pAnimElem.next_sibling("animation"))
    {
        /////////////////////////////////////////////
        // Check if there are multiple meshes
        if(pAnimElem.child("animation"))
        {
            /////////////
            // Go down one level
            LoadAnimations(pAnimElem, avAnimations, apColladaScene);
            continue;
        }

        /////////////////////////////////////////////
        //Check what animation to use.
        pugi::xml_node pTestChannelElem = pAnimElem.child("channel");
        if(!pTestChannelElem)
        {
            Warning("Animation missing channel!\n");
            continue;
        }

        //Get target node name
        tString sTestTarget = cString::ToString(Xml::Attr(pTestChannelElem, "target"), "");
        tStringVec vTargetStrings;
        tString sTargetSepp = "/";
        cString::GetStringVec(sTestTarget, vTargetStrings, &sTargetSepp);
        sTestTarget = vTargetStrings[0];

        cColladaAnimation& Anim = GetAnimationFromTarget(sTestTarget, avAnimations);

        //Anim.msName = cString::ToString(Xml::Attr(pAnimElem, "name"),""); No need..
        Anim.msId = cString::ToString(Xml::Attr(pAnimElem, "id"), "");

        //////////////////////////////////
        // Load all Channels
        pugi::xml_node pChannelElem = pAnimElem.child("channel");
        for(; pChannelElem ; pChannelElem = pChannelElem.next_sibling("channel"))
        {
            cColladaChannel    Channel;
            Channel.msId = cString::ToString(Xml::Attr(pChannelElem, "id"), "");
            Channel.msSource = cString::ToString(Xml::Attr(pChannelElem, "source"), "");
            Channel.msTarget  = cString::ToString(Xml::Attr(pChannelElem, "target"), "");
            GetAdress(Channel.msSource);

            Anim.mvChannels.push_back(Channel);
        }

        //////////////////////////////////
        // Load all Samplers
        pugi::xml_node pSamplerElem = pAnimElem.child("sampler");
        for(; pSamplerElem ; pSamplerElem = pSamplerElem.next_sibling("sampler"))
        {
            cColladaSampler Sampler;
            Sampler.msId = cString::ToString(Xml::Attr(pSamplerElem, "id"), "");

            //Iterate the inputs and find the needed types.
            pugi::xml_node pInput = pSamplerElem.child("input");
            for(; pInput ; pInput = pInput.next_sibling("input"))
            {
                tString sSemantic = cString::ToString(Xml::Attr(pInput, "semantic"), "");
                tString sSource = cString::ToString(Xml::Attr(pInput, "source"), "");
                GetAdress(sSource);

                if(sSemantic == "INPUT")
                {
                    Sampler.msTimeArray = sSource;
                }
                else if(sSemantic == "OUTPUT")
                {
                    Sampler.msValueArray = sSource;
                }
            }
            Anim.mvSamplers.push_back(Sampler);
        }

        //////////////////////////////////
        // Set target for the Samplers.
        // This means that the channels can be skipped later on makes things simpler!
        // These values will only work if the samplers are not shared among channels
        for(size_t i = 0; i < Anim.mvChannels.size(); i++)
        {
            for(size_t j = 0; j < Anim.mvSamplers.size(); j++)
            {
                if(Anim.mvChannels[i].msSource == Anim.mvSamplers[j].msId)
                {
                    Anim.mvSamplers[j].msTarget = Anim.mvChannels[i].msTarget;
                }
            }
        }

        //////////////////////////////////
        // Iterate through all the sources
        pugi::xml_node pSourceElem = pAnimElem.child("source");
        for(; pSourceElem ; pSourceElem = pSourceElem.next_sibling("source"))
        {
            Anim.mvSources.push_back(cColladaAnimSource());
            cColladaAnimSource &Source = Anim.mvSources.back();

            Source.msId = cString::ToString(Xml::Attr(pSourceElem, "id"), "");

            pugi::xml_node pArrayElem = pSourceElem.child("float_array");
            if(!pArrayElem)
            {
                //Warning("No array element found for animation data (shouldn't be anything bad)!\n");
                continue;
            }

            int lCount = cString::ToInt(Xml::Attr(pArrayElem, "count"), 0);
            Source.mvValues.reserve(lCount);

            // Count can be 0, check to avoid crashes
            if(lCount > 0)
            {
                const char *pText = Xml::TextOrEmpty(pArrayElem);
                cString::GetFloatVec(pText, Source.mvValues);
            }
        }
    }
}

//-----------------------------------------------------------------------



void cMeshLoaderCollada::LoadColladaScene(pugi::xml_node apRootElem, cColladaNode *apParentNode,
        cColladaScene *apScene, tColladaLightVec *apColladaLightVec)
{
    cColladaNode *pNode = apParentNode->CreateChild();
    apScene->mlstNodes.push_back(pNode);

    //The local matrix
    cMatrixf mtxTransform = cMatrixf::Identity;

    //Vector to save transform values in later on.
    tFloatVec vValVec;
    vValVec.reserve(5);

    ///////////////////////////////////////////
    //Get properties
    pNode->msId = cString::ToString(Xml::Attr(apRootElem, "id"), "");
    pNode->msName = cString::ToString(Xml::Attr(apRootElem, "name"), "");
    pNode->msSid = cString::ToString(Xml::Attr(apRootElem, "sid"), pNode->msId);
    pNode->msType = cString::ToString(Xml::Attr(apRootElem, "type"), "");

    // XXX
    // Removed as it introduces backwards compatibility issues!!
    //if(pNode->msName.empty())
    //{
    //    pNode->msName = pNode->msId;
    //    Warning("Scene node with id '%s' has empty name! Setting id as name\n", pNode->msId.c_str());
    //}

    /////////////////////////////////////////////
    //Get source, if there is any.
    pugi::xml_node pInstanceElem = apRootElem.child("instance_geometry");
    if(!pInstanceElem)
    {
        pInstanceElem = apRootElem.child("instance_light");
    }
    if(!pInstanceElem)
    {
        pInstanceElem = apRootElem.child("instance_controller");
    }
    if(!pInstanceElem)
    {
        pInstanceElem = apRootElem.child("instance");
    }
    if(pInstanceElem)
    {
        /////////////////////////////////////////////
        //Get geometry instance
        tString sSource = cString::ToString(Xml::Attr(pInstanceElem, "url"), "");
        if(sSource[0] == '#')
        {
            pNode->mbSourceIsFile = false;
        }
        else
        {
            pNode->mbSourceIsFile = true;
        }

        GetAdress(sSource);

        pNode->msSource = sSource;

        /////////////////////////////////////////////
        //Get Material instance, if any
        pugi::xml_node pBindMaterialElem = pInstanceElem.child("bind_material");
        if(pBindMaterialElem)
        {
            pugi::xml_node pTechniqueCommonElem = pBindMaterialElem.child("technique_common");
            if(pTechniqueCommonElem)
            {
                pugi::xml_node pInstanceMaterialElem = pTechniqueCommonElem.child("instance_material");
                if(pInstanceMaterialElem)
                {
                    pNode->msInstanceMaterial = cString::ToString(Xml::Attr(pInstanceMaterialElem, "target"), "");
                    GetAdress(pNode->msInstanceMaterial);
                }
            }
        }
    }



    //Log("Node. %s, type: %s\n",pNode->msId.c_str(),pNode->msType.c_str());

    cVector3f vTranslation = cVector3f(0, 0, 0);

    ///////////////////////////////////////////////////////////
    //Iterate through all of the transforms.
    pugi::xml_node pTransformElem = Xml::FirstElem(apRootElem);
    while(pTransformElem)
    {
        tString sVal = pTransformElem.name();
        tString sSid = cString::ToString(Xml::Attr(pTransformElem, "sid"), "");

        pugi::xml_node pChildElem = pTransformElem.first_child();
        if(!pChildElem)
        {
            pTransformElem = Xml::NextElem(pTransformElem);
            continue;
        }

        //Log("val: %s\n",sVal.c_str());

        const char *pText = (pChildElem.type() == pugi::node_pcdata || pChildElem.type() == pugi::node_cdata) ? pChildElem.value() : NULL;
        if(pText == NULL)
        {
            pTransformElem = Xml::NextElem(pTransformElem);
            continue;
        }

        cString::GetFloatVec(pText, vValVec);

        // Translation
        if(sVal == "translate")
        {
            cVector3f vTrans = GetVectorPosFromPtr(&vValVec[0]);

            mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixTranslate(vTrans));
        }
        // Rotation
        else if(sVal == "rotate")
        {
            cQuaternion qRot;
            cVector3f vRotAxis = GetVectorRotationFromPtr(&vValVec[0]);

            qRot.FromAngleAxis(cMath::ToRad(vValVec[3]), vRotAxis);

            mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixQuaternion(qRot));
        }
        // Scaling
        else if(sVal == "scale")
        {
            cVector3f vScale = GetVectorScaleFromPtr(&vValVec[0]);
            pNode->mvScale = vScale;

            //If this node is a light, do not apply scale.
            if(apColladaLightVec && GetLight(pNode->msSource, *apColladaLightVec))
            {
            }
            //Colliders do not use scale.
            else if(cString::ToLowerCase(cString::Sub(pNode->msName, 1, 8)) == "collider"
                    || cString::ToLowerCase(cString::Sub(pNode->msName, 1, 12)) == "charcollider"
                    ||  cString::ToLowerCase(cString::Sub(pNode->msName, 1, 4)) == "area")
            {

            }
            //This a geometry node (or something else..). Apply scale as normal
            else
            {
                mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixScale(vScale));
            }
        }
        else if(sVal == "matrix")
        {
            cMatrixf mtxBone = GetMatrix(cMatrixf(&vValVec[0]));
            mtxTransform = cMath::MatrixMul(mtxTransform, mtxBone);
        }


        pNode->mlstTransforms.push_back(cColladaTransform());
        cColladaTransform &Transform = pNode->mlstTransforms.back();

        Transform.msSid = sSid;
        Transform.mvValues = vValVec;
        Transform.msType = sVal;

        /*Log("Transform:\n");
        Log("Sid: %s type: %s ",Transform.msSid.c_str(), Transform.msType.c_str());
        Log("Values: ");
        for(size_t i=0; i < Transform.mvValues.size();i++)
        {
        Log("%f ",Transform.mvValues[i]);
        }
        Log("\n");*/

        vValVec.clear();
        pTransformElem = Xml::NextElem(pTransformElem);
    }

    pNode->m_mtxTransform = mtxTransform;
    pNode->m_mtxWorldTransform = cMath::MatrixMul(apParentNode->m_mtxWorldTransform, mtxTransform);

    //Load all children
    pugi::xml_node pNodeElem = apRootElem.child("node");
    while(pNodeElem)
    {
        LoadColladaScene(pNodeElem, pNode, apScene, apColladaLightVec);

        pNodeElem = pNodeElem.next_sibling("node");
    }
}

//-----------------------------------------------------------------------


void cMeshLoaderCollada::LoadControllers(pugi::xml_node apRootElem,
        tColladaControllerVec &avColladaControllerVec,
        tColladaGeometryVec *apColladaGeometryVec)
{
    pugi::xml_node pCtrlElem = apRootElem.child("controller");
    for(; pCtrlElem; pCtrlElem = pCtrlElem.next_sibling("controller"))
    {
        avColladaControllerVec.push_back(cColladaController());

        cColladaController &Controller = avColladaControllerVec[avColladaControllerVec.size() - 1];

        Controller.msId = cString::ToString(Xml::Attr(pCtrlElem, "id"), "");

        ///////////////////////////////////////
        // Get Skin element.
        pugi::xml_node pSkinElem = pCtrlElem.child("skin");
        if(!pSkinElem)
        {
            Error("No Skin found in controller!\n");
            continue;
        }

        Controller.msTarget = cString::ToString(Xml::Attr(pSkinElem, "source"), "");
        GetAdress(Controller.msTarget);

        //Get the bind matrix
        pugi::xml_node pBindMatrixElem = pSkinElem.child("bind_shape_matrix");
        if(pBindMatrixElem)
        {
            const char *pText = Xml::TextOrEmpty(pBindMatrixElem);
            tFloatVec vValues;
            cString::GetFloatVec(pText, vValues);

            Controller.m_mtxBindShapeMatrix = GetMatrix(&vValues[0]);
        }
        else
        {
            Warning("No bind matrix in controller '%s' using identity\n", Controller.msId.c_str());
            Controller.m_mtxBindShapeMatrix = cMatrixf::Identity;
        }


        //These are used so you can find what the different sources contain.
        tString sJointNameSource = "";
        tString sJointWeightSource = "";
        tString sJointMatrixSource = "";
        int lJointOffset = -1;
        int lWeightOffset = -1;

        ////////////////////////////////////////
        // Load Joint information
        {
            pugi::xml_node pJointsElem = pSkinElem.child("joints");
            if(!pJointsElem)
            {
                Warning("Couldn't find joint element for controller!\n");
                continue;
            }

            pugi::xml_node pInputElem = pJointsElem.child("input");
            for(; pInputElem; pInputElem = pInputElem.next_sibling("input"))
            {
                tString sSemantic = cString::ToString(Xml::Attr(pInputElem, "semantic"), "");
                tString sSource = cString::ToString(Xml::Attr(pInputElem, "source"), "");
                GetAdress(sSource);

                //The names of the joints
                if(sSemantic == "JOINT")
                {
                    sJointNameSource = sSource;
                }
                else if(sSemantic == "INV_BIND_MATRIX")
                {
                    sJointMatrixSource = sSource;
                }
            }
        }

        ////////////////////////////////////////
        // Load Joint weight info
        {
            pugi::xml_node pJointWeightElem = pSkinElem.child("vertex_weights");
            if(!pJointWeightElem)
            {
                Warning("Couldn't find vertex_weights element for controller!\n");
                continue;
            }

            pugi::xml_node pInputElem = pJointWeightElem.child("input");
            for(; pInputElem; pInputElem = pInputElem.next_sibling("input"))
            {
                tString sSemantic = cString::ToString(Xml::Attr(pInputElem, "semantic"), "");
                tString sSource = cString::ToString(Xml::Attr(pInputElem, "source"), "");
                int lOffset  = cString::ToInt(Xml::Attr(pInputElem, "offset"), -1);
                GetAdress(sSource);

                //The names of the joints
                if(sSemantic == "JOINT")
                {
                    lJointOffset = lOffset;
                }
                //The weight of the joints
                else if(sSemantic == "WEIGHT")
                {
                    lWeightOffset = lOffset;
                    sJointWeightSource = sSource;
                }
            }
        }

        ////////////////////////////////////////
        // Get the sources and load / apply them
        pugi::xml_node pSourceElem = pSkinElem.child("source");
        while(pSourceElem)
        {
            tString sId = cString::ToString(Xml::Attr(pSourceElem, "id"), "");

            //////////////////
            //Name of joints
            if(sId == sJointNameSource)
            {
                pugi::xml_node pNameArrayElem = pSourceElem.child("Name_array");
                if(!pNameArrayElem)
                {
                    Warning("Couldn't find name array!\n");
                    continue;
                }

                int lCount = cString::ToInt(Xml::Attr(pNameArrayElem, "count"), 0);

                //Reserve for faster push_back
                Controller.mvJoints.reserve(lCount);

                const char *pNameText = Xml::Text(pNameArrayElem);
                if(pNameText == NULL)
                {
                    Error("No joint name data found!\n");
                    continue;
                }

                tStringVec vJoints;

                cString::GetStringVec(pNameText, vJoints);

                ///////
                // Support for bones names that have blankspace
                if(vJoints.size() != lCount)
                {
                    // Name has blankspace
                    tString sRootName = vJoints[0];
                    tString sJoint = sRootName;

                    for(size_t i = 1; i < vJoints.size(); ++i)
                    {
                        if(vJoints[i] == sRootName)
                        {
                            ////////////////
                            // Reached a new hierarchy
                            // Push back the joint and start on a new joint
                            Controller.mvJoints.push_back(sJoint);
                            sJoint = sRootName;
                        }
                        else
                        {
                            // Append the part
                            sJoint += " " + vJoints[i];
                        }
                    }

                    if(sJoint != sRootName)
                    {
                        // Add the last joint
                        Controller.mvJoints.push_back(sJoint);
                    }
                }
                else
                {
                    // No blankspace in name, copy the data
                    for(int i = 0; i < lCount; ++i)
                    {
                        Controller.mvJoints.push_back(vJoints[i]);
                    }
                }
            }
            //////////////////
            //Joint weights or matrices
            else if(sId == sJointWeightSource || sId == sJointMatrixSource)
            {
                pugi::xml_node pFloatArrayElem = pSourceElem.child("float_array");
                if(!pFloatArrayElem)
                {
                    Warning("Couldn't find name array!\n");
                    continue;
                }

                int lCount = cString::ToInt(Xml::Attr(pFloatArrayElem, "count"), 0);

                //Get the text data
                const char *pText = Xml::Text(pFloatArrayElem);
                if(pText == NULL)
                {
                    Error("No value data found!\n");
                    return;
                }

                //Convert text to floats
                tFloatVec vValVec;
                vValVec.reserve(lCount);
                cString::GetFloatVec(pText, vValVec);

                //Weights
                if(sId == sJointWeightSource)
                {
                    Controller.mvWeights = vValVec;
                }
                //Matrices
                else
                {
                    Controller.mvMatrices.reserve(lCount / 16);

                    for(int i = 0; i < (lCount / 16); i++)
                    {
                        cMatrixf mtxTemp(&vValVec[i * 16]);
                        Controller.mvMatrices.push_back(GetMatrix(mtxTemp));
                    }
                }
            }

            pSourceElem = pSourceElem.next_sibling("source");
        }

        ////////////////////////////////////////
        // Get joint - vertex pairs.
        {
            pugi::xml_node pJointWeightElem = pSkinElem.child("vertex_weights");
            if(!pJointWeightElem)
            {
                Warning("Couldn't find vertex_weights element for controller!\n");
                continue;
            }

            ////////////////////////////
            // Vcount - get the number of joints for each vertex.
            pugi::xml_node pVCountElem = pJointWeightElem.child("vcount");
            if(!pVCountElem)
            {
                Warning("Couldn't find vertex_weights vcount element for controller!\n");
                continue;
            }

            //Get the text data
            const char *pVCountText = Xml::Text(pVCountElem);
            if(pVCountText == NULL)
            {
                Error("No value data found!\n");
                continue;
            }

            tIntVec vVCount;
            cString::GetIntVec(pVCountText, vVCount);

            /////////////////////////////
            //V - Get the pairs
            pugi::xml_node pVElem = pJointWeightElem.child("v");
            if(!pVElem)
            {
                Warning("Couldn't find vertex_weights v element for controller!\n");
                continue;
            }

            //Get the text data
            const char *pVText = Xml::Text(pVElem);
            if(pVText == NULL)
            {
                Error("No value data found!\n");
                continue;
            }

            tIntVec vV;
            cString::GetIntVec(pVText, vV);

            int lVtx = 0;
            int lNumOfPairs = ((int)vV.size()) / 2;
            int lPairCount = 0;


            Controller.mvPairs.resize(vVCount.size());
            for(int lPair = 0; lPair < lNumOfPairs; ++lPair)
            {
                cColladaJointPair Pair;

                Pair.mlJoint = vV[lPair * 2 + lJointOffset];
                Pair.mlWeight = vV[lPair * 2 + lWeightOffset];

                //Log("Pair: %d, %d vtx: %d\n",Pair.mlJoint, Pair.mlWeight,lVtx);

                Controller.mvPairs[lVtx].push_back(Pair);

                //Check if it is time for a new vertex.
                lPairCount++;
                if(lPairCount >= vVCount[lVtx])
                {
                    lVtx++;
                    lPairCount = 0;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------

static tVector3fVec *gpVertexVec = NULL;

///////////////////////////////////////////

class cColladaTestTri
{
public:
    cColladaTestTri(cColladaVtxIndex *apTriIndex)
    {
        //Log("TestTri:\n");
        //Get the vectors
        for(int i = 0; i < 3; i++)
        {
            mvPos[i] = (*gpVertexVec)[apTriIndex[i].mlVtx];

            //Log("Pos%d: %s\n",i,mvPos[i].ToString().c_str());
        }

        //Sort the vectors
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                if(j == i)
                {
                    continue;
                }

                if(mvPos[i] < mvPos[j])
                {
                    cVector3f vTemp = mvPos[i];
                    mvPos[i] = mvPos[j];
                    mvPos[j] = vTemp;
                }
            }
        }

        //for(int i=0; i<3; i++) Log("Pos%d: %s\n",i,mvPos[i].ToString().c_str());
    }

    cVector3f mvPos[3];
};

///////////////////////////////////////////

class cColladaTestTriCompare
{
public:
    bool operator()(const cColladaTestTri& aTri1, const cColladaTestTri& aTri2)const
    {
        //0
        if(aTri1.mvPos[0].x != aTri2.mvPos[0].x)
        {
            return aTri1.mvPos[0].x < aTri2.mvPos[0].x;
        }
        if(aTri1.mvPos[0].y != aTri2.mvPos[0].y)
        {
            return aTri1.mvPos[0].y < aTri2.mvPos[0].y;
        }
        if(aTri1.mvPos[0].z != aTri2.mvPos[0].z)
        {
            return aTri1.mvPos[0].z < aTri2.mvPos[0].z;
        }
        //1
        if(aTri1.mvPos[1].x != aTri2.mvPos[1].x)
        {
            return aTri1.mvPos[1].x < aTri2.mvPos[1].x;
        }
        if(aTri1.mvPos[1].y != aTri2.mvPos[1].y)
        {
            return aTri1.mvPos[1].y < aTri2.mvPos[1].y;
        }
        if(aTri1.mvPos[1].z != aTri2.mvPos[1].z)
        {
            return aTri1.mvPos[1].z < aTri2.mvPos[1].z;
        }
        //2
        if(aTri1.mvPos[2].x != aTri2.mvPos[2].x)
        {
            return aTri1.mvPos[2].x < aTri2.mvPos[2].x;
        }
        if(aTri1.mvPos[2].y != aTri2.mvPos[2].y)
        {
            return aTri1.mvPos[2].y < aTri2.mvPos[2].y;
        }
        if(aTri1.mvPos[2].z != aTri2.mvPos[2].z)
        {
            return aTri1.mvPos[2].z < aTri2.mvPos[2].z;
        }

        return false;
    }
};

///////////////////////////////////////////

typedef std::set<cColladaTestTri, cColladaTestTriCompare> tColladaTestTriMap;
typedef tColladaTestTriMap::iterator tColladaTestTriMapIt;


///////////////////////////////////////////

void cMeshLoaderCollada::LoadGeometry(pugi::xml_node apRootElem, tColladaGeometryVec &avColladaGeometryVec)
{
    pugi::xml_node pGeomElem = apRootElem.child("geometry");
    for(; pGeomElem; pGeomElem = pGeomElem.next_sibling("geometry"))
    {
        //There should only be one mesh
        pugi::xml_node pMeshElem = pGeomElem.child("mesh");
        if(!pMeshElem)
        {
            //Warning("No Mesh element found in geometry element '%s'!\n",Geometry.msName.c_str());
            continue;
        }

        avColladaGeometryVec.push_back(cColladaGeometry());
        cColladaGeometry &Geometry = avColladaGeometryVec.back();

        //Get main properties
        Geometry.msId = cString::ToString(Xml::Attr(pGeomElem, "id"), "");
        Geometry.msName = cString::ToString(Xml::Attr(pGeomElem, "name"), "");

        if(Geometry.msName == "")
        {
            Geometry.msName = Geometry.msId;
            //Fix to skip a max addition on mesh names
            int lPos = cString::GetLastStringPos(Geometry.msName, "-");
            if(lPos > 0)
            {
                Geometry.msName = cString::Sub(Geometry.msName, 0, lPos);
            }
        }

        ///////////////////////////////////////////////////
        //Check if the geometry should loaded
        if(cString::Sub(Geometry.msName, 0, 4) == "_ref" ||
                cString::Sub(Geometry.msName, 0, 3) == "_bb" ||
                cString::Sub(Geometry.msName, 0, 4) == "_ps" ||
                cString::Sub(Geometry.msName, 0, 6) == "_sound")
        {
            continue;
        }

        ///////////////////////////////////////////////////
        //Iterate through all sources (the vertices)
        pugi::xml_node pSourceElem = pMeshElem.child("source");
        while(pSourceElem)
        {
            //Add a new array
            Geometry.mvArrayVec.push_back(cColladaVtxArray());
            int lPos = (int)Geometry.mvArrayVec.size() - 1;

            //Get id
            tString sSourceId = cString::ToString(Xml::Attr(pSourceElem, "id"), "");
            //Log("Vertex id: '%s' \n",sSourceId.c_str());

            Geometry.mvArrayVec[lPos].msId = sSourceId;

            LoadVertexData(pSourceElem, Geometry.mvArrayVec[lPos].mvArray);

            /*Log("Array: %s\n",sSourceId.c_str());
            for(int i=0; i< (int)Geometry.mvArrayVec[lPos].mvArray.size(); i++)
            {
                Log("(%s) ",Geometry.mvArrayVec[lPos].mvArray[i].ToString().c_str());
            }
            Log("\n");*/

            pSourceElem = pSourceElem.next_sibling("source");
        }

        ///////////////////////////////////////////////////
        //Get the "real" name for the vertices
        //This always includes positions and can include normals and tex coords aswell.
        pugi::xml_node pVerticesElem = pMeshElem.child("vertices");
        if(!pVerticesElem)
        {
            Error("Vertices not found!\n");
            return;
        }

        //Iterate the inputs
        pugi::xml_node pVtxInput = pVerticesElem.child("input");
        while(pVtxInput)
        {
            tString sSemantic = cString::ToString(Xml::Attr(pVtxInput, "semantic"), "");

            if(sSemantic == "POSITION")
            {
                tString sSource = cString::ToString(Xml::Attr(pVtxInput, "source"), "");
                GetAdress(sSource);

                //Log("Position vertex source: %s\n", sSource.c_str());

                for(int i = 0; i < (int) Geometry.mvArrayVec.size(); i++)
                {
                    //If the vertex array is found just change the name and break.
                    if(Geometry.mvArrayVec[i].msId == sSource)
                    {
                        Geometry.mvArrayVec[i].msId = cString::ToString(Xml::Attr(pVerticesElem, "id"), "");
                        break;
                    }
                }
            }
            else
            {
                tString sSource = cString::ToString(Xml::Attr(pVtxInput, "source"), "");
                GetAdress(sSource);

                //Log("%s vertex source: %s\n",sSemantic.c_str(), sSource.c_str());

                //Find array and set some properties
                for(int i = 0; i < (int) Geometry.mvArrayVec.size(); i++)
                    if(Geometry.mvArrayVec[i].msId == sSource)
                    {
                        Geometry.mvArrayVec[i].msType = sSemantic;
                        Geometry.mvArrayVec[i].mbIsInVertex = true;
                        break;
                    }
            }

            pVtxInput = pVtxInput.next_sibling("input");
        }


        ///////////////////////////////////////////////////
        //Get the Triangles, save them in a row
        pugi::xml_node pTriElem = pMeshElem.child("triangles");
        if(!pTriElem)
        {
            pTriElem = pMeshElem.child("polylist");
            if(pTriElem && pTriElem.next_sibling("polylist"))
            {
                Warning("Geometry '%s' seem to have multitexturing!\n", Geometry.msName.c_str());
            }

        }
        else
        {
            if(pTriElem.next_sibling("triangles"))
            {
                Warning("Geometry '%s' seem to have multitexturing!\n", Geometry.msName.c_str());
            }

        }


        if(!pTriElem)
        {
            //Warning("No triangle or polylist element found, testing polygons.\n");
            pTriElem = pMeshElem.child("polygons");
            if(!pTriElem)
            {
                Error("No Polygons found!\n");
                return;
            }
        }
        int lTriElements = 0;

        int lTriCount = cString::ToInt(Xml::Attr(pTriElem, "count"), 0);
        Geometry.msMaterial = cString::ToString(Xml::Attr(pTriElem, "material"), "");
        GetAdress(Geometry.msMaterial);

        //Get the inputs to figure what the indices in he triangles mean.
        pugi::xml_node pTriInputElem = pTriElem.child("input");
        while(pTriInputElem)
        {
            tString sSemantic = cString::ToString(Xml::Attr(pTriInputElem, "semantic"), "");
            tString sSource = cString::ToString(Xml::Attr(pTriInputElem, "source"), "");
            int lIdx = cString::ToInt(Xml::Attr(pTriInputElem, "idx"), -1);
            if(lIdx < 0)
            {
                lIdx = cString::ToInt(Xml::Attr(pTriInputElem, "offset"), -1);
            }

            GetAdress(sSource);

            int lArrayNum = -1;

            //Get what array that belongs to this input.
            for(int i = 0; i < (int)Geometry.mvArrayVec.size(); i++)
            {
                if(Geometry.mvArrayVec[i].msId == sSource)
                {
                    lArrayNum = i;
                    break;
                }
            }

            //Set the properties for the sort
            if(sSemantic == "VERTEX")
            {
                Geometry.mlPosIdxNum = lIdx;
                Geometry.mlPosArrayIdx = lArrayNum;
            }
            else if(sSemantic == "NORMAL")
            {
                Geometry.mlNormIdxNum = lIdx;
                Geometry.mlNormArrayIdx = lArrayNum;
            }
            else if(sSemantic == "TEXCOORD")
            {
                Geometry.mlTexIdxNum = lIdx;
                Geometry.mlTexArrayIdx = lArrayNum;
            }

            //Increase element num
            if(lTriElements < lIdx + 1)
            {
                lTriElements = lIdx + 1;
            }

            //next input
            pTriInputElem = pTriInputElem.next_sibling("input");
        }

        //Go through the arrays and check if any are in the vertex
        for(size_t i = 0; i < Geometry.mvArrayVec.size(); i++)
        {
            if(Geometry.mvArrayVec[i].mbIsInVertex)
            {
                if(Geometry.mvArrayVec[i].msType == "NORMAL")
                {
                    Geometry.mlNormArrayIdx = (int)i;
                    Geometry.mlNormIdxNum = Geometry.mlPosIdxNum;
                }
                else if(Geometry.mvArrayVec[i].msType == "TEXCOORD")
                {
                    Geometry.mlTexArrayIdx = (int)i;
                    Geometry.mlTexIdxNum = Geometry.mlPosIdxNum;
                }
            }
        }

        //Check so that none of the arrays are missing
        if(Geometry.mlNormArrayIdx < 0)
        {
            Warning("No normals for geometry '%s'\n", Geometry.msName.c_str());
            continue;
        }
        if(Geometry.mlTexArrayIdx < 0 && Geometry.msName[0] != '_')
        {
            Warning("No tex coords for geometry '%s'\n", Geometry.msName.c_str());
            continue;
        }

        //////////////////////////////
        // If Z is up axis or the unit scale is not 1, go through all the geometry and convert
        if(mbZToY || mfUnitScale != 1.0f)
        {
            //Positions
            tVector3fVec &vVtxVec = Geometry.mvArrayVec[Geometry.mlPosArrayIdx].mvArray;
            for(size_t i = 0; i < vVtxVec.size(); i++)
            {
                vVtxVec[i] = GetVectorPos(vVtxVec[i]);
            }

            //Normals
            tVector3fVec &vNormVec = Geometry.mvArrayVec[Geometry.mlNormArrayIdx].mvArray;
            for(size_t i = 0; i < vNormVec.size(); i++)
            {
                vNormVec[i] = GetVectorNormal(vNormVec[i]);
            }
        }

        //reserve space for the indices
        Geometry.mvIndices.reserve(lTriElements * 3);

        //Set the vertex array used.
        gpVertexVec = &Geometry.mvArrayVec[Geometry.mlPosArrayIdx].mvArray;

        tColladaTestTriMap map_TestTris;

        // Load all the Indices
        pugi::xml_node pPElem = pTriElem.child("p");
        while(pPElem)
        {
            const char *pText = Xml::Text(pPElem);
            if(pText == NULL)
            {
                Error("No tri data found!\n");
                return;
            }

            //Get the indices for the triangle
            tIntVec vIndexArray;
            cString::GetIntVec(pText, vIndexArray);

            int lTriangleNum = (int)vIndexArray.size() / (3 * lTriElements);
            for(int triangle = 0; triangle < lTriangleNum; triangle++)
            {
                cColladaVtxIndex DataVec[3];
                int lTriangleAdd = triangle * 3 * lTriElements;

                //Iterate the points in triangle
                //If Z is used as y the order must be reversed.
                //Turns out, it is not so... because x is negated.
                /*if(mbZToY)
                {
                    for(int i=2; i>= 0; i--)
                    {
                        DataVec[2-i].mlVtx = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlPosIdxNum];
                        DataVec[2-i].mlNorm = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlNormIdxNum];
                        DataVec[2-i].mlTex = vIndexArray[lTriangleAdd + i*lTriElements + Geometry.mlTexIdxNum];
                    }
                }
                else*/
                {
                    for(int i = 0; i < 3; i++)
                    {
                        DataVec[i].mlVtx = vIndexArray[lTriangleAdd + i * lTriElements + Geometry.mlPosIdxNum];
                        DataVec[i].mlNorm = Geometry.mlNormIdxNum >= 0 ? vIndexArray[lTriangleAdd + i * lTriElements + Geometry.mlNormIdxNum] : 0;
                        DataVec[i].mlTex = Geometry.mlTexIdxNum >= 0 ? vIndexArray[lTriangleAdd + i * lTriElements + Geometry.mlTexIdxNum] : 0;
                    }
                }

                //Skipping this test right now as it is only really interesting for shadow volumes and it will destroy double sided textures!
                //std::pair<tColladaTestTriMapIt,bool> TestPair = map_TestTris.insert(DataVec);
                //if(TestPair.second)
                {
                    //Add the data to the indices
                    for(int i = 0; i < 3; i++)
                    {
                        Geometry.mvIndices.push_back(DataVec[i]);
                    }
                }
                /*else
                {
                    Error("Geometry '%s' has two faces using same vertices! Skipping face.\n",Geometry.msName.c_str());
                }*/
            }

            //Next p
            pPElem = pPElem.next_sibling("p");
        }

        ///////////////////////////////////
        //Split the vertices and make em usable
        SplitVertices(Geometry, Geometry.mvExtraVtxVec, Geometry.mvVertexVec, Geometry.mvIndexVec);
        Geometry.Clear();

        ////////////////////////////////////
        //Create Tangents
        tFloatVec vPosVec;
        vPosVec.resize(Geometry.mvVertexVec.size() * 4);
        tFloatVec vNormVec;
        vNormVec.resize(Geometry.mvVertexVec.size() * 3);
        tFloatVec vTexVec;
        vTexVec.resize(Geometry.mvVertexVec.size() * 3);

        float *pPosData = &vPosVec[0];
        float *pNormData = &vNormVec[0];
        float *pTexData = &vTexVec[0];

        //Fill vectors
        for(size_t i = 0; i < Geometry.mvVertexVec.size(); ++i)
        {
            cVertex &vertex = Geometry.mvVertexVec[i];

            pPosData[0] = vertex.pos.x;
            pPosData[1] = vertex.pos.y;
            pPosData[2] = vertex.pos.z;
            pPosData[3] = 1;

            pNormData[0] = vertex.norm.x;
            pNormData[1] = vertex.norm.y;
            pNormData[2] = vertex.norm.z;

            pTexData[0] = vertex.tex.x;
            pTexData[1] = vertex.tex.y;
            pTexData[2] = vertex.tex.z;

            pPosData += 4;
            pNormData += 3;
            pTexData += 3;
        }

        //Creates tangents
        Geometry.mvTangents.resize(Geometry.mvVertexVec.size() * 4);
        cMath::CreateTriTangentVectors( &Geometry.mvTangents[0],
                                        &Geometry.mvIndexVec[0], (int)Geometry.mvIndexVec.size(),
                                        &vPosVec[0], 4, &vTexVec[0], &vNormVec[0],
                                        (int)Geometry.mvVertexVec.size());
    }
}

//////////////////////////////////////

void cMeshLoaderCollada::LoadVertexData(pugi::xml_node apSourceElem, tVector3fVec &avVtxVec)
{
    //Get some info on the build up of the array
    pugi::xml_node pTechniqueElem = apSourceElem.child("technique_common");
    if(!pTechniqueElem)
    {
        pTechniqueElem = apSourceElem.child("technique");
    }
    if(!pTechniqueElem)
    {
        Warning("No technique or technique_common element found!\n");
        return;
    }

    //Get some attributes from the accessor
    pugi::xml_node pAccessor = pTechniqueElem.child("accessor");
    if(!pAccessor)
    {
        Warning("No accessor element for source data found!\n");
        return;
    }

    int lElements = cString::ToInt(Xml::Attr(pAccessor, "stride"), 0);
    int lVtxCount = cString::ToInt(Xml::Attr(pAccessor, "count"), 0);

    //Log("Elems: %d Count: %d\n",lElements,lVtxCount);

    //Load the array
    pugi::xml_node pDataElem = apSourceElem.child("float_array");
    if(!pDataElem)
    {
        //try with array as well.
        pDataElem = apSourceElem.child("array");
        if(!pDataElem)
        {
            Warning("No data found!\n");
            return;
        }
    }

    const char *pTextElem = Xml::Text(pDataElem);
    if(pTextElem == NULL)
    {
        Warning("No text found!\n");
        return;
    }

    const char *pChars = pTextElem;

    FillVertexVec(pChars, avVtxVec, lElements, lVtxCount);
}

//-----------------------------------------------------------------------

void cMeshLoaderCollada::LoadImages(pugi::xml_node apRootElem, tColladaImageVec &avColladaImageVec)
{
    pugi::xml_node pImageElem = apRootElem.child("image");
    while(pImageElem)
    {
        cColladaImage Image;
        Image.msId = cString::ToString(Xml::Attr(pImageElem, "id"), "");
        Image.msName = cString::ToString(Xml::Attr(pImageElem, "name"), "");

        pugi::xml_node pInitFromElem = pImageElem.child("init_from");
        //COLLADA 1.4
        if(pInitFromElem)
        {
            if(pInitFromElem.first_child())
            {
                Image.msSource = Xml::TextCondensed(pInitFromElem);
            }
            else
            {
                Image.msSource = "";
            }
        }
        //COLLADA 1.3
        else
        {
            Image.msSource = cString::ToString(Xml::Attr(pImageElem, "source"), "");
        }

        avColladaImageVec.push_back(Image);

        pImageElem = pImageElem.next_sibling("image");
    }
}

//-----------------------------------------------------------------------

class cEffectNewParam
{
public:
    tString msId;
    tString msType;
    tString msData;
};

tString *GetFinalSource(std::vector<cEffectNewParam> &avParams, tString& asId)
{
    for(size_t i = 0; i < avParams.size(); ++i)
    {
        if(asId == avParams[i].msId)
        {
            return GetFinalSource(avParams, avParams[i].msData);
        }
    }

    return &asId;
}

void cMeshLoaderCollada::LoadTextures(pugi::xml_node apRootElem, tColladaTextureVec &avColladaTextureVec)
{
    pugi::xml_node pTextureElem = Xml::FirstElem(apRootElem);
    for(; pTextureElem; pTextureElem = Xml::NextElem(pTextureElem))
    {
        cColladaTexture Texture;

        //Get the main properties of the texture
        Texture.msId = cString::ToString(Xml::Attr(pTextureElem, "id"), "");

        /////////////////////////////////////////
        //COLLADA 1.4
        pugi::xml_node pProfileCommon = pTextureElem.child("profile_COMMON");
        if(pProfileCommon)
        {
            std::vector<cEffectNewParam> vNewParams;
            //////////////////////////
            //Iterate all newparams
            pugi::xml_node pNewParamElem = pProfileCommon.child("newparam");
            for(; pNewParamElem; pNewParamElem = pNewParamElem.next_sibling("newparam"))
            {
                vNewParams.push_back(cEffectNewParam());
                cEffectNewParam& newParam = vNewParams.back();

                newParam.msId = Xml::AttrOrEmpty(pNewParamElem, "sid");

                pugi::xml_node pChildElem = Xml::FirstElem(pNewParamElem);
                if(pChildElem)
                {
                    newParam.msType = pChildElem.name();
                    //Log("Newparam '%s' type '%s'\n",newParam.msId.c_str(),newParam.msType.c_str());

                    tString sDataName = "";
                    if(newParam.msType == "surface")
                    {
                        sDataName = "init_from";
                    }
                    else if(newParam.msType == "sampler2D")
                    {
                        sDataName = "source";
                    }

                    if(sDataName == "")
                    {
                        continue;
                    }
                    pugi::xml_node pValueElem = pChildElem.child(sDataName.c_str());
                    if(pValueElem)
                    {
                        newParam.msData = Xml::TextCondensed(pValueElem);
                    }
                    else
                    {
                        Warning("Data element '%s' missing from newparam '%s'\n", sDataName.c_str(),
                                newParam.msId.c_str());
                    }
                }


            }

            //////////////////////////
            //Get the first technique
            pugi::xml_node pTechniqueElem = pProfileCommon.child("technique");
            if(!pTechniqueElem)
            {
                Warning("No effect technique element found!\n");
                continue;
            }

            pugi::xml_node pTypeElem = Xml::FirstElem(pTechniqueElem);
            if(!pTypeElem)
            {
                Warning("No effect type element found!\n");
                continue;
            }
            if(cString::ToString(pTypeElem.name(), "") == "extra")
            {
                pTypeElem = Xml::NextElem(pTypeElem);
                if(!pTypeElem)
                {
                    Warning("No effect type element found!\n");
                    continue;
                }
            }

            ///////////////////////
            //Diffuse
            pugi::xml_node pDiffuseElem = pTypeElem.child("diffuse");
            if(pDiffuseElem)
            {
                pugi::xml_node pLocalTexture = pDiffuseElem.child("texture");
                if(pLocalTexture)
                {
                    tString _tstr = cString::ToString(Xml::Attr(pLocalTexture, "texture"), "");
                    Texture.msImage = *GetFinalSource(vNewParams, _tstr);
                }
                else
                {
                    Texture.msImage = "";
                    //Warning("No diffuse texture effect element for effect '%s'! No file texture file will be loaded.\n",
                    //            Texture.msId.c_str());
                }
            }
            else
            {
                Warning("No diffuse effect element!\n");
            }
        }
        /////////////////////////////////////////
        //COLLADA 1.3
        else
        {
            ////////////////////////////
            //Iterate root params
            pugi::xml_node pParamElem = pTextureElem.child("param");
            while(pParamElem)
            {

                pParamElem = pParamElem.next_sibling("param");
            }

            ///////////////////////////
            //Iterate techniques
            pugi::xml_node pTechniqueElem = pTextureElem.child("technique");
            while(pTechniqueElem)
            {
                tString sProfile = cString::ToString(Xml::Attr(pTechniqueElem, "profile"), "");

                ////////////////////////////
                //Technique params:
                pugi::xml_node pTechParam = pTechniqueElem.child("param");
                while(pTechParam)
                {
                    pTechParam = pTechParam.next_sibling("param");
                }

                ///////////////////////////////
                //Technique inputs:
                pugi::xml_node pTechInput = pTechniqueElem.child("input");
                while(pTechInput)
                {
                    tString sSemantic = cString::ToString(Xml::Attr(pTechInput, "semantic"), "");

                    //Get the image of the texture
                    if(sSemantic == "IMAGE")
                    {
                        Texture.msImage = cString::ToString(Xml::Attr(pTechInput, "source"), "");
                    }

                    pTechInput = pTechInput.next_sibling("input");
                }

                pTechniqueElem = pTechniqueElem.next_sibling("technique");
            }
        }


        //Log("Texture: id: '%s' image: '%s'\n",Texture.msId.c_str(), Texture.msImage.c_str());

        avColladaTextureVec.push_back(Texture);
    }
}
//-----------------------------------------------------------------------

void cMeshLoaderCollada::LoadMaterials(pugi::xml_node apRootElem, tColladaMaterialVec &avColladaMaterialVec)
{
    pugi::xml_node pMaterialElem = apRootElem.child("material");
    while(pMaterialElem)
    {
        cColladaMaterial Material;
        Material.msId = cString::ToString(Xml::Attr(pMaterialElem, "id"), "");
        Material.msName = cString::ToString(Xml::Attr(pMaterialElem, "name"), "");

        ///////////////////////////////////////////
        // COLLADA 1.4
        pugi::xml_node pIstanceEffectElem = pMaterialElem.child("instance_effect");
        if(pIstanceEffectElem)
        {
            Material.msTexture = cString::ToString(Xml::Attr(pIstanceEffectElem, "url"), "");
        }
        ///////////////////////////////////////////
        // COLLADA 1.3
        else
        {
            //The rest of the material loader is gonna be a little lame
            //Just gonna look for a texture.

            pugi::xml_node pShaderElem = pMaterialElem.child("shader");
            if(!pShaderElem)
            {
                Warning("No shader found!\n");
                continue;
            }

            pugi::xml_node pTechElem = pShaderElem.child("technique");
            if(!pTechElem)
            {
                Warning("No technique found!\n");
                continue;
            }

            pugi::xml_node pPassElem = pTechElem.child("pass");
            if(!pPassElem)
            {
                Warning("No pass found!\n");
                continue;
            }

            //Iterate through the inputs and try to find a texture
            pugi::xml_node pInputElem = pPassElem.child("input");
            while(pInputElem)
            {
                tString sSemantic =  cString::ToString(Xml::Attr(pInputElem, "semantic"), "");

                if(sSemantic == "TEXTURE")
                {
                    Material.msTexture = cString::ToString(Xml::Attr(pInputElem, "source"), "");
                }

                pInputElem = pInputElem.next_sibling("input");
            }
        }

        //Log("Material: id: '%s' name: '%s' texture: '%s'\n",Material.msId.c_str(),
        //                                            Material.msName.c_str(),
        //                                            Material.msTexture.c_str());

        //Add to vector
        avColladaMaterialVec.push_back(Material);

        //Next material
        pMaterialElem = pMaterialElem.next_sibling("material");
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// HELPER METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static cVertex IndexDataToVertex(const cColladaVtxIndex &aData, const cColladaGeometry &aGeometry)
{
    cVertex Vtx;

    Vtx.col = cColor(1, 1);
    Vtx.pos = aGeometry.mvArrayVec[aGeometry.mlPosArrayIdx].mvArray[aData.mlVtx];
    if(aGeometry.mlNormArrayIdx >= 0)
    {
        Vtx.norm = aGeometry.mvArrayVec[aGeometry.mlNormArrayIdx].mvArray[aData.mlNorm];
    }
    if(aGeometry.mlTexArrayIdx >= 0)
    {
        Vtx.tex = aGeometry.mvArrayVec[aGeometry.mlTexArrayIdx].mvArray[aData.mlTex];
    }

    //Flip the y coord on the tex
    Vtx.tex.y = 1 - Vtx.tex.y;

    return Vtx;
}

static cColladaExtraVtx IndexDataToExtra(const cColladaVtxIndex &aData, int alNewVtx)
{
    cColladaExtraVtx Extra;
    Extra.mlNewVtx = alNewVtx;
    Extra.mlVtx = aData.mlVtx;
    Extra.mlNorm = aData.mlNorm;
    Extra.mlTex = aData.mlTex;
    return Extra;
}


void cMeshLoaderCollada::SplitVertices(cColladaGeometry &aGeometry, tColladaExtraVtxListVec &avExtraVtxVec,
                                       tVertexVec &avVertexVec, tUIntVec &avIndexVec)
{
    //Resize the extra array and the vertex array
    int lVtxSize = (int)aGeometry.mvArrayVec[aGeometry.mlPosArrayIdx].mvArray.size();
    avExtraVtxVec.resize(lVtxSize);
    avVertexVec.resize(lVtxSize);
    avIndexVec.resize(aGeometry.mvIndices.size());

    tColladaVtxIndexVec &vIndices = aGeometry.mvIndices;

    for(int i = 0; i < (int) vIndices.size(); i++)
    {
        cColladaVtxIndex &Data = vIndices[i];
        //Log("Index %d: ", i);

        //If the vertex extra is empty this is the first of
        //this vertex added, so no need to split.
        if(avExtraVtxVec[Data.mlVtx].empty())
        {
            avExtraVtxVec[Data.mlVtx].push_back(IndexDataToExtra(Data, Data.mlVtx));

            avVertexVec[Data.mlVtx] = IndexDataToVertex(Data, aGeometry);
            avIndexVec[i] = Data.mlVtx;
            //Log("New vertex added");
        }
        //There is already a vertex added at this position, check if this exist
        //else create a new.
        else
        {
            bool bFound = false;
            tColladaExtraVtxListIt it = avExtraVtxVec[Data.mlVtx].begin();
            for(; it != avExtraVtxVec[Data.mlVtx].end(); ++it)
            {
                //If the split has already been made for this combo just set the vertex position.
                if(it->Equals(Data))
                {
                    avIndexVec[i] = it->mlNewVtx;
                    bFound = true;
                    //Log("Old vertex split used");
                    break;
                }
            }

            //If no split was found create a new
            if(bFound == false)
            {
                //The new vertex will be put at the end.
                avExtraVtxVec[Data.mlVtx].push_back(IndexDataToExtra(Data, (int)avVertexVec.size()));
                avIndexVec[i] = (int)avVertexVec.size();

                avVertexVec.push_back(IndexDataToVertex(Data, aGeometry));
                //Log("New split made");
            }
        }

        //Log("\n");
    }
}


//-----------------------------------------------------------------------

tString cMeshLoaderCollada::GetTopString(const tString asPath)
{
    int pos = cString::GetLastStringPos(asPath, "-");

    if(pos < 0)
    {
        return "";
    }

    return asPath.substr(pos + 1);
}

//-----------------------------------------------------------------------

void cMeshLoaderCollada::FillVertexVec(const char* apChars, tVector3fVec &avVtxVec, int alElements, int alVtxCount)
{
    if((int)avVtxVec.size() < alVtxCount)
    {
        avVtxVec.resize(alVtxCount);
    }

    //This is a bit slower but shouldn't matter since we have cache files.
    //Also, the biggie is probably string -> float, which is no different really. Just some extra copying to arrays happen.
    //Plus side is that it is a lot safer!
    tFloatVec vFloatData;
    cString::GetFloatVec(apChars, vFloatData);

    int lLoadedVtxNum = (int)vFloatData.size() / alElements;
    if(lLoadedVtxNum != alVtxCount)
    {
        Warning("Vertex array in does not have correct number of values stored. %d instead of %d\n", vFloatData.size(), alElements * alVtxCount);
    }

    for(int i = 0; i < alVtxCount; ++i)
    {
        if(i >= lLoadedVtxNum)
        {
            break;
        }

        float *fData = &vFloatData[i * alElements];

        avVtxVec[i].x = fData[0];
        if(alElements > 1)
        {
            avVtxVec[i].y = fData[1];
            if(alElements > 2)
            {
                avVtxVec[i].z = fData[2];
            }
        }
    }

    /*
    int lArraySize = alElements * alVtxCount;

    int lArrayCount=0;
    int lStringCount =0;

    float vValArray[3];
    int lValArrayCount=0;
    char vTempChar[256];
    int lTempCharCount=0;

    int lVertexNum =0;

    while(lArrayCount < lArraySize)
    {
        char c= apChars[lStringCount];
        //if a space is found, convert the previous characters to a float.
        if(c == ' ' || c == 0 || c== '\n' || c=='\t')
        {
            if(lTempCharCount>0)
            {
                //Add the float to temp float array
                vTempChar[lTempCharCount] =0;
                vValArray[lValArrayCount] = (float)atof(vTempChar);

                lTempCharCount=0;
                lArrayCount++;

                //If enough values have been added put in vector and add to array.
                lValArrayCount++;
                if(lValArrayCount == alElements)
                {
                    lValArrayCount =0;
                    cVector3f vVec;
                    vVec.x = vValArray[0];vVec.y = vValArray[1];
                    if(alElements==3) vVec.z = vValArray[2];
                    if (lVertexNum >= alVtxCount) {
                        Error("Tried to access beyond end of data structure (%d)\n", alVtxCount);
                        return;
                    }
                    avVtxVec[lVertexNum] = vVec;
                    lVertexNum++;
                    //Log("%s\n", vVec.ToString().c_str());
                }
            }
        }
        //If character is not a space, add to temp char.
        else
        {
            vTempChar[lTempCharCount] = apChars[lStringCount];
            lTempCharCount++;
        }

        lStringCount++;
    }*/
}

//-----------------------------------------------------------------------

tString cMeshLoaderCollada::GetMaterialTextureFile(const tString &asMaterial,
        tColladaMaterialVec &avColladaMaterialVec,
        tColladaTextureVec &avColladaTextureVec,
        tColladaImageVec &avColladaImageVec)
{
    if(asMaterial == "")
    {
        return "";
    }

    //Get the texture Id
    tString sTexId = "";
    for(size_t mat = 0; mat < avColladaMaterialVec.size(); mat++)
    {
        if(avColladaMaterialVec[mat].msId == asMaterial)
        {
            sTexId = avColladaMaterialVec[mat].msTexture;
            GetAdress(sTexId);
            break;
        }
    }

    if(sTexId == "")
    {
        Warning("Material '%s' was not found!\n", asMaterial.c_str());
        return "";
    }

    //Get file Id
    tString sFileId = "";
    for(size_t tex = 0; tex < avColladaTextureVec.size(); tex++)
    {
        if(avColladaTextureVec[tex].msId == sTexId)
        {
            sFileId = avColladaTextureVec[tex].msImage;
            GetAdress(sFileId);
            break;
        }
    }

    if(sFileId == "")
    {
        Warning("Texture '%s' was not found!\n", sTexId.c_str());
        return "";
    }

    //Get file source
    for(size_t img = 0; img < avColladaImageVec.size(); img++)
    {
        if(avColladaImageVec[img].msId == sFileId)
        {
            return cString::GetFileName(avColladaImageVec[img].msSource);
        }
    }

    Warning("Couldn't file image file id '%s'\n", sFileId.c_str());

    return "";
}

//-----------------------------------------------------------------------

/*
OLD SKINNING DATA
void cMeshLoaderCollada::LoadControllers(pugi::xml_node apRootElem,
tColladaControllerVec &avColladaControllerVec,
tColladaGeometryVec &avColladaGeometryVec)
{
pugi::xml_node pCtrlElem = apRootElem.child("controller");
for(;pCtrlElem; pCtrlElem = pCtrlElem.next_sibling("controller"))
{
avColladaControllerVec.push_back(cColladaController());

cColladaController &Controller = avColladaControllerVec[avColladaControllerVec.size()-1];

tString sTarget =cString::ToString(Xml::Attr(pCtrlElem, "target"),"");
tString sId = cString::ToString(Xml::Attr(pCtrlElem, "id"),"");

Controller.msId = sId;

///////////////////////////////////////
// Get Skin element.
pugi::xml_node pSkinElem = pCtrlElem.child("skin");
if(!pSkinElem){ Error("No Skin found in controller!\n"); continue;}

//1.4 support, check skin for source if not found.
if(sTarget == ""){
sTarget = cString::ToString(Xml::Attr(pSkinElem, "source"),"");
GetAdress(sTarget);
}

Controller.msTarget = sTarget;


////////////////////////////////////////
// Find the geometry for the controller.
cColladaGeometry* pGeom =NULL;
for(int i=0;i<(int)avColladaGeometryVec.size();i++)
{
if(sTarget == avColladaGeometryVec[i].msId)
{
if(avColladaGeometryVec[i].mvIndices.empty())
{
Error("Target geometry is empty!\n");
break;
}

pGeom = &avColladaGeometryVec[i];
break;
}
}
if(pGeom==NULL){
Error("Target '%s' for Controller '%s' couldn't be found\n",sTarget.c_str(),sId.c_str());
continue;
}



////////////////////////////////////////
// Get the types of sources used in the controller.
tString sNormalSourceId;
tString sPositionSourceId;
tString sJointSourceId;

pugi::xml_node pVerticesElem = pSkinElem.child("vertices");
if(!pVerticesElem){ Error("No vertices element!\n"); continue;}

pugi::xml_node pVtxInput = pVerticesElem.child("input");
while(pVtxInput)
{
tString sSemantic = cString::ToString(Xml::Attr(pVtxInput, "semantic"),"");
tString sSource = cString::ToString(Xml::Attr(pVtxInput, "source"),"");
GetAdress(sSource);

//Log("Vertex: %s Sem: %s\n",sSource.c_str(), sSemantic.c_str());

//Check the semantic and set the strings to point to right source.
if(sSemantic == "BIND_SHAPE_POSITION")
{
sPositionSourceId = sSource;
}
else if(sSemantic == "BIND_SHAPE_NORMAL")
{
sNormalSourceId = sSource;
}
else if(sSemantic == "JOINTS_AND_WEIGHTS")
{
sJointSourceId = sSource;
}

pVtxInput = pVtxInput.next_sibling("input");
}

////////////////////////////////////////
// Get the sources and load / apply them

pugi::xml_node pSourceElem = pSkinElem.child("source");
while(pSourceElem)
{
tString sId = cString::ToString(Xml::Attr(pSourceElem, "id"),"");

//Log("Loading controller source %s\n",sId.c_str());

//new vectors for positions
if(sId == sPositionSourceId)
{
if(pGeom->mlPosArrayIdx >= 0)
{
LoadVertexData(pSourceElem, pGeom->mvArrayVec[pGeom->mlPosArrayIdx].mvArray);
}
}
//new vectors for normals
else if(sId == sNormalSourceId)
{
if(pGeom->mlNormArrayIdx >= 0)
LoadVertexData(pSourceElem, pGeom->mvArrayVec[pGeom->mlNormArrayIdx].mvArray);
}
//data for joints
else if(sId == sJointSourceId)
{
LoadJointData(pSourceElem, Controller);
}

pSourceElem = pSourceElem.next_sibling("source");
}
}
}

//////////////////////////////////////

void cMeshLoaderCollada::LoadJointData(pugi::xml_node apSourceElem, cColladaController &aController)
{
tString sJointArrayName ="";
tString sMatrixArrayName ="";
tString sWeightArrayName ="";

/////////////////////////////////
//Get technique element
pugi::xml_node pTechniqueElem = apSourceElem.child("technique");
if(!pTechniqueElem){ Warning("No technique element found!\n"); return;}


/////////////////////////////////
//Get the names of joint, matrix and weight array.
pugi::xml_node pAccessorElem = pTechniqueElem.child("accessor");
while(pAccessorElem)
{
tString sSource = cString::ToString(Xml::Attr(pAccessorElem, "source"),"");
GetAdress(sSource);

pugi::xml_node pParamElem = pAccessorElem.child("param");
if(!pParamElem){ Warning("Accessor contains no param!\n"); return;}

tString sParamName = cString::ToString(Xml::Attr(pParamElem, "name"),"");

if(sParamName == "JOINT")
{
sJointArrayName = sSource;
}
else if(sParamName == "INV_BIND_MATRIX")
{
sMatrixArrayName = sSource;
}
else if(sParamName == "WEIGHT")
{
sWeightArrayName = sSource;
}

pAccessorElem = pAccessorElem.next_sibling("accessor");
}

//Log("Joint: %s Matrix: %s Weight: %s\n",sJointArrayName.c_str(),
//    sMatrixArrayName.c_str(),
//    sWeightArrayName.c_str());

/////////////////////////////////
// Get the pairs
pugi::xml_node pCombinerElem = pTechniqueElem.child("combiner");
if(!pCombinerElem){ Error("No combiner found!\n"); return;}

int lPairCount = cString::ToInt(Xml::Attr(pCombinerElem, "count"),0);

aController.mvPairs.resize(lPairCount);
//Log("Pair Count: %d\n",lPairCount);

//Check which element is joint and which is weight.
pugi::xml_node pCombInput = pCombinerElem.child("input");
while(pCombInput)
{
tString sSemantic = cString::ToString(Xml::Attr(pCombInput, "semantic"),"");
int lIdx = cString::ToInt(Xml::Attr(pCombInput, "idx"),0);

if(sSemantic == "JOINT")
{
aController.mlJointPairIdx = lIdx;
//Log("Joint has idx %d\n",lIdx);
}
else if(sSemantic == "WEIGHT")
{
aController.mlWeightPairIdx = lIdx;
//Log("Weight has idx %d\n",lIdx);
}
else
{
Warning("Unknown semantic '%s' found in combiner input\n", sSemantic);
}

pCombInput = pCombInput.next_sibling("input");
}

// Load the data for the pairs
int lPairIdx =0;
pugi::xml_node pCombV = pCombinerElem.child("v");
while(pCombV)
{
const char *pText = Xml::Text(pCombV);
if(pText==NULL){ Error("No index data found!\n"); return;}

//Get the indices joints and weights
tIntVec vIndexArray;
cString::GetIntVec(pText,vIndexArray);

//Add each pair in a list at each vertex
int lNumOfPairs = (int) vIndexArray.size() /2;
for(int i=0;i<lNumOfPairs;i++)
{
cColladaJointPair Pair;

Pair.mlJoint = vIndexArray[i*2 + aController.mlJointPairIdx];
Pair.mlWeight = vIndexArray[i*2 + aController.mlWeightPairIdx];

//Log("Pair: %d, %d\n",Pair.mlJoint, Pair.mlWeight);

aController.mvPairs[lPairIdx].push_back(Pair);
}


lPairIdx++;
pCombV = pCombV.next_sibling("v");
}


/////////////////////////////////
// Get the arrays

////////////////////////////////
//Get the names of the joints:
pugi::xml_node pNameArrayElem = apSourceElem.child("Name_array");
if(!pNameArrayElem){ Error("No name array found!\n"); return;}

tString sNameId = cString::ToString(Xml::Attr(pNameArrayElem, "id"),"");
int lNameCount = cString::ToInt(Xml::Attr(pNameArrayElem, "count"),0);

if(sNameId != sJointArrayName){
Error("Name array and joint array is not the same!\n");
return;
}

const char *pNameText = Xml::Text(pNameArrayElem);
if(pNameText==NULL){ Error("No joint name data found!\n"); return;}

aController.mvJoints.reserve(lNameCount);
cString::GetStringVec(pNameText, aController.mvJoints);


//////////////////////////////
// Get weights and matrices
pugi::xml_node pFloatArrayElem = apSourceElem.child("float_array");
while(pFloatArrayElem)
{
tString sId = cString::ToString(Xml::Attr(pFloatArrayElem, "id"),"");
int lCount = cString::ToInt(Xml::Attr(pFloatArrayElem, "count"),0);

const char *pText = Xml::Text(pFloatArrayElem);
if(pText==NULL){ Error("No value data found!\n"); return;}

tFloatVec vValVec;
vValVec.reserve(lCount);
cString::GetFloatVec(pText, vValVec);

//Get the matrix arrray
if(sId == sMatrixArrayName)
{
aController.mvMatrices.reserve(lCount / 16);

for(int i=0; i< (lCount / 16); i++)
{
cMatrixf mtxTemp(&vValVec[i*16]);

aController.mvMatrices.push_back(mtxTemp);

//Log("Mtx: %s\n",cMath::MatrixToChar(mtxTemp));
}
}
//Get the weight array
else if(sId == sWeightArrayName)
{
aController.mvWeights = vValVec;

//for(int i=0; i <(int)aController.mvWeights.size();i++)
//{
//Log("Weight: %f\n",Controller.mvWeights[i]);
//}
}

pFloatArrayElem = pFloatArrayElem.next_sibling("float_array");
}
}*/

}
