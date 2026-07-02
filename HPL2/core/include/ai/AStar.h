#ifndef HPL_A_STAR_H
#define HPL_A_STAR_H

#include "system/SystemTypes.h"
#include "engine/EngineTypes.h"
#include "math/MathTypes.h"

namespace hpl
{

class cAINodeContainer;
class cAINode;

//--------------------------------------

typedef std::set<cAINode*> tAINodeSet;
typedef tAINodeSet::iterator tAINodeSetIt;

//--------------------------------------

typedef std::list<cAINode*> tAINodeList;
typedef tAINodeList::iterator tAINodeListIt;

//--------------------------------------

#define eAStarNodeFlag_Open			 (0x01)
#define eAStarNodeFlag_CastRay		 (0x02)

class cAStarNode
{
public:
    cAStarNode(cAINode *apAINode);

    float mfCost;
    float mfDistance;

    unsigned int mlAStarCount;
    unsigned char mFlags;

    cAStarNode *mpParent;
    cAINode *mpAINode;
};

class cAStarNodeCompare
{
public:
    bool operator()(cAStarNode* apNodeA,cAStarNode* apNodeB) const;
};

class cAStarNodePresort
{
public:
    bool operator()(cAStarNode* apNodeA,cAStarNode* apNodeB) const;
};

typedef std::list<cAStarNode*> tAStarNodeList;
typedef std::set<cAStarNode*,cAStarNodeCompare> tAStarNodeSet;
typedef tAStarNodeSet::iterator tAStarNodeSetIt;

typedef std::vector<cAStarNode> tAStarNodeVec;
typedef tAStarNodeVec::iterator tAStarNodeVecIt;

typedef std::set<cAStarNode*,cAStarNodePresort> tAStarNodePresortSet;
typedef tAStarNodePresortSet::iterator tAStarNodePresortSetIt;

//--------------------------------------
class cAStarHandler;

class iAStarCallback
{
public:
    virtual ~iAStarCallback() {}

    virtual bool CanAddNode(cAINode *apParentNode,cAINode *apChildNode)=0;
};

//--------------------------------------

class cAStarHandler
{
public:
    cAStarHandler(cAINodeContainer *apContainer);
    ~cAStarHandler();

    bool GetPath(const cVector3f& avStart, const cVector3f& avGoal, tAINodeList *apNodeList);

    /**
     * Set max number of times the algorithm is iterated.
     * \param alX -1 = until OpenList is empty
     */
    void SetMaxIterations(int alX)
    {
        mlMaxIterations = alX;
    }

    void SetCallback(iAStarCallback *apCallback)
    {
        mpCallback = apCallback;
    }

private:
    void IterateAlgorithm();

    void AddOpenNode(cAINode *apAINode, cAStarNode *apParent, float afDistance);

    cAStarNode* GetBestNode();
    cAStarNode* GetNode(cAINode *apAINode);

    float Cost(float afDistance, cAINode *apAINode, cAStarNode *apParent);
    float Heuristic(const cVector3f& avStart, const cVector3f& avGoal);

    bool IsGoalNode(cAINode *apAINode);

    void ClearUnlistedNodes(bool abGoals, bool abOpen);

    cVector3f mvGoal;
    cVector3f mvStart;

    cAStarNode* mpGoalNode;
    tAINodeSet m_setGoalNodes;

    cAINodeContainer *mpContainer;

    tUIntVec mvOpenListCount;
    tUIntVec mvGoalListCount;

    int mlMaxIterations;
    unsigned int mlAStarCount;

    iAStarCallback *mpCallback;

    tAStarNodeSet m_setOpenList;
    tAStarNodeSet m_setClosedList;

    tAStarNodeVec mvNodes;
    tAStarNodePresortSet m_setPresortNodes;
};

};
#endif // HPL_A_STAR_H
