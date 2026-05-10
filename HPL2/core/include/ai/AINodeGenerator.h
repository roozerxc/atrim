#ifndef HPL_AI_NODE_GENERATOR_H
#define HPL_AI_NODE_GENERATOR_H

#include "system/SystemTypes.h"
#include "engine/EngineTypes.h"

#include "physics/PhysicsWorld.h"
#include "scene/World.h"

namespace hpl
{

class cWorld;

//-------------------------------

class cAINodeGeneratorParams
{
public:
    cAINodeGeneratorParams();

    tString msNodeType;

    float mfHeightFromGround;
    float mfMinWallDist;

    cVector3f mvMinPos;
    cVector3f mvMaxPos;

    float mfGridSize;
};

//-------------------------------

class cAINodeGenerator : public iPhysicsRayCallback
{
public:
    cAINodeGenerator();
    ~cAINodeGenerator();

    void Generate(cWorld* apWorld,cAINodeGeneratorParams *apParams);

private:
    bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

    void SaveToFile();
    void LoadFromFile();

    cAINodeGeneratorParams *mpParams;
    cWorld* mpWorld;
    tTempAiNodeList *mpNodeList;
    int mlIDCount;
};

};
#endif // HPL_AI_NODE_GENERATOR_H
