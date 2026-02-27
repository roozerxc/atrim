#include "LuxAreaNodes.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// PLAYER START
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxNode_PlayerStart::cLuxNode_PlayerStart(const tString& asName)
{
    msName = asName;
    mvPos =0;
}

//-----------------------------------------------------------------------

cLuxAreaNodeLoader_PlayerStart::cLuxAreaNodeLoader_PlayerStart(const tString& asName) : iAreaLoader(asName)
{
    mbCreatesStaticArea = true;
}

//-----------------------------------------------------------------------

void cLuxAreaNodeLoader_PlayerStart::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
    cLuxMap *pMap = gpBase->mpCurrentMapLoading;
    if(pMap==NULL)
    {
        cStartPosEntity* pPos = apWorld->CreateStartPos(asName);
        pPos->SetMatrix(a_mtxTransform);

        return;
    }

    cLuxNode_PlayerStart *pNode = hplNew(cLuxNode_PlayerStart, (asName));
    pNode->mvPos = a_mtxTransform.GetTranslation() + cVector3f(0,0.05f, 0);
    
    cVector3f vForward = cMath::MatrixMul(a_mtxTransform.GetRotation(), cVector3f(0,0,1));
    
    pNode->mfAngle = -cMath::GetAngleFromPoints2D(0, cVector2f(vForward.x, vForward.z));

    //pNode->mfAngle = cMath::Vector3Angle(vForward, cVector3f(0,0,1));
    //if(cMath::Vector3Dot(vForward, cVector3f(1,0,0)) < 0) pNode->mfAngle = -pNode->mfAngle;    

    pMap->AddPlayerStart(pNode);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PATH NODE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaNodeLoader_PathNode::cLuxAreaNodeLoader_PathNode(const tString& asName)  : iAreaLoader(asName) 
{

    mbCreatesStaticArea = true;
}

//-----------------------------------------------------------------------

void cLuxAreaNodeLoader_PathNode::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
    apWorld->AddAINode(asName,alID, "Default", a_mtxTransform.GetTranslation()+cVector3f(0,0.05f,0));
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// POS NODE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaNodeLoader_PosNode::cLuxAreaNodeLoader_PosNode(const tString& asName) : iAreaLoader(asName)
{
    mbCreatesStaticArea = true;
}

//-----------------------------------------------------------------------

void cLuxAreaNodeLoader_PosNode::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
    cLuxMap *pMap = gpBase->mpCurrentMapLoading;

    cLuxNode_Pos *pNode = hplNew(cLuxNode_Pos, (asName));
    pNode->mvPos = a_mtxTransform.GetTranslation();

    pMap->AddPosNode(pNode);
}

//-----------------------------------------------------------------------


