#ifndef HPL_POSTEFFECT_COMPOSITE_H
#define HPL_POSTEFFECT_COMPOSITE_H

#include "graphics/RenderFunctions.h"

namespace hpl
{

//------------------------------------------

class cGraphics;
class iLowLevelGraphics;
class iPostEffect;

//------------------------------------------

typedef std::multimap<int, iPostEffect*, std::greater<int> > tPostEffectMap;
typedef tPostEffectMap::iterator tPostEffectMapIt;

//------------------------------------------

class cPostEffectComposite : public iRenderFunctions
{
public:
    cPostEffectComposite(cGraphics *apGraphics);
    ~cPostEffectComposite();

    void Render(double adFrameTime, cFrustum *apFrustum, iTexture *apInputTexture, cRenderTarget *apRenderTarget);

    /**
     * Highest prio is first!
     */
    void AddPostEffect(iPostEffect *apPostEffect, int alPrio);
    inline int GetPostEffectNum()const
    {
        return (int)mvPostEffects.size();
    }
    inline iPostEffect* GetPostEffect(int alIdx)const
    {
        return mvPostEffects[alIdx];
    }

    bool HasActiveEffects();

    //Fix current frame time so it always returns the time in Ms !
    double GetCurrentFrameTime()
    {
        return dCurrentFrameTime / 1000.0;
    }

private:
    void BeginRendering(double adFrameTime, cFrustum *apFrustum, iTexture *apInputTexture, cRenderTarget *apRenderTarget);
    void EndRendering();
    void CopyToFrameBuffer(iTexture *apOutputTexture);

    tPostEffectMap m_mapPostEffects;
    std::vector<iPostEffect*> mvPostEffects;

    iFrameBuffer *mpFinalTempBuffer[2];

    double dCurrentFrameTime;
};

//------------------------------------------

};
#endif // HPL_POSTEFFECT_COMPOSITE_H
