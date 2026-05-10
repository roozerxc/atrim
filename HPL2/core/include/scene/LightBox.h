#ifndef HPL_LIGHT_BOX_H
#define HPL_LIGHT_BOX_H

#include "scene/Light.h"
#include "scene/SceneTypes.h"

namespace hpl
{

//------------------------------------------

class cLightBox : public iLight
{
public:
    cLightBox(tString asName, cResources *apResources);

    void SetSize(const cVector3f& avSize);
    inline const cVector3f& GetSize()
    {
        return mvSize;
    }

    void SetBlendFunc(eLightBoxBlendFunc aFunc)
    {
        mBlendFunc = aFunc;
    }
    eLightBoxBlendFunc GetBlendFunc()
    {
        return mBlendFunc;
    }

    bool IsVisible();

    void SetBoxLightPrio(int alX)
    {
        mlBoxLightPrio = alX;
    }
    inline int GetBoxLightPrio()const
    {
        return mlBoxLightPrio;
    }

private:
    void UpdateBoundingVolume();

    cVector3f mvSize;
    eLightBoxBlendFunc mBlendFunc;
    int mlBoxLightPrio;
};

};
#endif // HPL_LIGHT_BOX_H
