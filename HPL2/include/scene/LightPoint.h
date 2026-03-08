#ifndef HPL_LIGHT_POINT_H
#define HPL_LIGHT_POINT_H

#include "scene/Light.h"

namespace hpl
{

//------------------------------------------

class cLightPoint : public iLight
{
#ifdef __GNUC__
    typedef iLight __super;
#endif
public:
    cLightPoint(tString asName, cResources *apResources);

private:
    void UpdateBoundingVolume();
};

};
#endif // HPL_LIGHT_POINT_H
