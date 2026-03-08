#ifndef HPL_RENDERABLE_CONTAINER_LIST_H
#define HPL_RENDERABLE_CONTAINER_LIST_H

#include "scene/RenderableContainer.h"

namespace hpl
{

//-------------------------------------------

class cRCNode_List : public iRenderableContainerNode
{
    friend class cRenderableContainer_List;
public:
    cRCNode_List();
    ~cRCNode_List();

private:
};

//-------------------------------------------

class cRenderableContainer_List : public iRenderableContainer
{
public:
    cRenderableContainer_List();
    ~cRenderableContainer_List();

    void Add(iRenderable *apRenderable);
    void Remove(iRenderable *apRenderable);

    iRenderableContainerNode* GetRoot();

    void Compile();

    void RenderDebug(cRendererCallbackFunctions *apFunctions);

private:
    cRCNode_List mRoot;
};

//-------------------------------------------
};
#endif // HPL_RENDERABLE_CONTAINER_LIST_H
