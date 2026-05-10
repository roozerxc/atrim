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

    void Render(float afFrameTime, cFrustum *apFrustum, iTexture *apInputTexture, cRenderTarget *apRenderTarget);

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

    float GetCurrentFrameTime()
    {
        return mfCurrentFrameTime;
    }

private:
    void BeginRendering(float afFrameTime, cFrustum *apFrustum, iTexture *apInputTexture, cRenderTarget *apRenderTarget);
    void EndRendering();
    void CopyToFrameBuffer(iTexture *apOutputTexture);

    tPostEffectMap m_mapPostEffects;
    std::vector<iPostEffect*> mvPostEffects;

    iFrameBuffer *mpFinalTempBuffer[2];

    float mfCurrentFrameTime;
};

//------------------------------------------

};
#endif // HPL_POSTEFFECT_COMPOSITE_H
