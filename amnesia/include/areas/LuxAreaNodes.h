#ifndef LUX_AREA_NODES_H
#define LUX_AREA_NODES_H

//----------------------------------------------

#include "LuxTypes.h"

//----------------------------------------------

class cLuxNode_PlayerStart
{
    friend class cLuxAreaNodeLoader_PlayerStart;
public:
    cLuxNode_PlayerStart(const tString& asName);

    const tString& GetName()
    {
        return msName;
    }

    const cVector3f& GetPosition()
    {
        return mvPos;
    }
    float GetAngle()
    {
        return mfAngle;
    }

private:
    tString msName;
    cVector3f mvPos;
    float mfAngle;
};

//----------------------------------------------

class cLuxAreaNodeLoader_PlayerStart : public iAreaLoader
{
public:
    cLuxAreaNodeLoader_PlayerStart(const tString& asName);

    void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld);
};

//----------------------------------------------

class cLuxAreaNodeLoader_PathNode : public iAreaLoader
{
public:
    cLuxAreaNodeLoader_PathNode(const tString& asName);

    void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld);
};

//----------------------------------------------

class cLuxAreaNodeLoader_PosNode : public iAreaLoader
{
public:
    cLuxAreaNodeLoader_PosNode(const tString& asName);

    void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld);
};

//----------------------------------------------


#endif// LUX_AREA_NODES_H
