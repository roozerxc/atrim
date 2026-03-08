#ifndef HPL_GUI_GFX_ELEMENT_H
#define HPL_GUI_GFX_ELEMENT_H

#include "gui/GuiTypes.h"

namespace hpl
{

class iGuiMaterial;
class iTexture;
class cFrameSubImage;
class cResources;

class cGui;

#define kMaxGuiTextures (4)

//------------------------------------------

enum eGuiGfxAnimationType
{
    eGuiGfxAnimationType_Loop,
    eGuiGfxAnimationType_StopAtEnd,
    eGuiGfxAnimationType_Oscillate,
    eGuiGfxAnimationType_Random,
    eGuiGfxAnimationType_LastEnum
};

class cGuiGfxAnimation
{
    friend class cGuiGfxElement;
public:
    cGuiGfxAnimation(): mfFrameLength(1), mType(eGuiGfxAnimationType_Loop) {}

    void AddFrame(int alNum);
    void SetType(eGuiGfxAnimationType aType);
    void SetFrameLength(float afLength);

private:
    tString msName;
    std::vector<int> mvFrames;
    float mfFrameLength;
    eGuiGfxAnimationType mType;
};

//------------------------------------------

class cGuiGfxElement
{
    friend class cGuiSet;
    friend class cGuiRenderObjectCompare;
public:
    cGuiGfxElement(cGui* apGui);
    ~cGuiGfxElement();

    void Update(float afTimeStep);

    void AddImage(cFrameSubImage* apImage);
    void AddTexture(iTexture* apTexture, const cVector2f& avStartUV=0, const cVector2f& avEndUV=1);
    void AddTexture(iTexture* apTexture, const cVector2f& avUVUpperLeft, const cVector2f& avUVUpperRight, const cVector2f& avUVLowerRight, const cVector2f& avUVLowerLeft);

    void AddImageToBuffer(cFrameSubImage* apImage);

    int GetTextureNum()
    {
        return mlTextureNum;
    }

    void SetDestroyTexture(bool abX)
    {
        mbDestroyTexture = abX;
    }
    bool GetDestroyTexture()
    {
        return mbDestroyTexture;
    }

    iTexture * GetTexture(int alIdx)
    {
        return mvTextures[alIdx];
    }
    cFrameSubImage * GetImage(int alIdx)
    {
        return mvImages[alIdx];
    }

    void SetOffset(const cVector3f& avOffset)
    {
        mvOffset = avOffset;
    }
    const cVector3f& GetOffset()const
    {
        return mvOffset;
    }

    void SetActiveSize(const cVector2f& avSize)
    {
        mvActiveSize = avSize;
    }
    const cVector2f& GetActiveSize()
    {
        return mvActiveSize;
    }

    cGuiGfxAnimation* CreateAnimtion(const tString& asName);
    void PlayAnimation(int alNum);
    cGuiGfxAnimation* GetAnimation(int alIdx)
    {
        return mvAnimations[alIdx];
    }
    void SetAnimationTime(float afTime);

    void SetAnimationPaused(bool abX)
    {
        mbAnimationPaused = abX;
    }
    bool GSetAnimationPaused()
    {
        return mbAnimationPaused;
    }

    void SetMaterial(iGuiMaterial *apMat);

    void SetColor(const cColor &aColor);

    void SetFlipUvYAxis(bool abX);
    bool GetFlipUvYAxis()
    {
        return mbFlipUvYAxis;
    }

    cVector2f GetImageSize();

    void Flush();
private:
    void SetImage(cFrameSubImage* apImage, int alNum);

    cVector2f mvImageSize;
    cGui *mpGui;
    tVertexVec mvVtx;

    cVector3f mvOffset;
    cVector2f mvActiveSize;

    iGuiMaterial *mpMaterial;
    iTexture* mvTextures[kMaxGuiTextures];
    cFrameSubImage* mvImages[kMaxGuiTextures];

    bool mbDestroyTexture;

    std::vector<cFrameSubImage*> mvImageBufferVec;

    std::vector<cGuiGfxAnimation*> mvAnimations;
    int mlCurrentAnimation;
    float mfCurrentFrame;
    int mlActiveImage;
    bool mbForwardAnim;
    bool mbAnimationPaused;

    bool mbFlipUvYAxis;

    int mlImageUpdateCount;

    int mlTextureNum;

    bool mbFlushed;

    cResources *mpResources;
};
};
#endif // HPL_GUI_GFX_ELEMENT_H
