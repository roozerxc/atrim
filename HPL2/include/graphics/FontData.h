#ifndef HPL_FONTDATA_H
#define HPL_FONTDATA_H

#include <vector>
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"
#include "resources/LowLevelResources.h"

namespace hpl
{

class cResources;
class iLowLevelGraphics;
class cBitmap;
class cGuiGfxElement;
class cGui;
class cFrameSubImage;

//------------------------------------------------

class cGlyph
{
public:
    cGlyph(    cGuiGfxElement *apGuiGfx,const cVector2f &avOffset,
               const cVector2f &avSize, float afAdvance);
    ~cGlyph();

    cGuiGfxElement *mpGuiGfx;
    cVector2f mvOffset;
    cVector2f mvSize;
    float mfAdvance;
};

typedef std::vector<cGlyph*> tGlyphVec;
typedef tGlyphVec::iterator tGlyphVecIt;

class iFontData : public iResourceBase
{
public:
    iFontData(const tString &asName, iLowLevelGraphics* apLowLevelGraphics);
    ~iFontData();

    virtual bool CreateFromBitmapFile(const tWString &asFileName)=0;


    bool Reload()
    {
        return false;
    }
    void Unload() {}
    void Destroy() {}

    /**
     * Used internally
     */
    void SetUp(cResources* apResources,    cGui *apGui)
    {
        mpResources = apResources;
        mpGui = apGui;
    }

    /**
     * Used internally
     * \param alNum
     * \return
     */
    inline cGlyph* GetGlyph(int alNum)const
    {
        if(alNum<0 || alNum>=(int)mvGlyphs.size())
        {
            return NULL;
        }
        return mvGlyphs[alNum];
    }

    inline unsigned short GetFirstChar()
    {
        return mlFirstChar;
    }
    inline unsigned short GetLastChar()
    {
        return mlLastChar;
    }

    inline const cVector2f& GetSizeRatio()const
    {
        return mvSizeRatio;
    }

    void GetWordWrapRows(float afLength,float afFontHeight,cVector2f avSize,const tWString& asString,
                         tWStringVec *apRowVec);

    /**
     * Get height of the font.
     * \return
     */
    inline float GetHeight()const
    {
        return mfHeight;
    }

    /**
     * Get the length in virtual screen size "pixels" of a formated string
     * \param avSize size of the characters
     * \param fmt
     * \param ...
     * \return
     */
    float GetLengthFmt(const cVector2f& avSize,const wchar_t* fmt,...);
    /**
     * Get the length in virtual screen size "pixels" of a string
     * \param avSize size of the characters
     * \param sText
     * \return
     */
    float GetLength(const cVector2f& avSize,const wchar_t* sText);

protected:
    iLowLevelGraphics* mpLowLevelGraphics;
    cResources* mpResources;
    cGui *mpGui;

    tGlyphVec mvGlyphs;

    float mfHeight;
    unsigned short mlFirstChar;
    unsigned short mlLastChar;

    cVector2f mvSizeRatio;

    cGlyph* CreateGlyph(cFrameSubImage* apImage, const cVector2l &avOffset,const cVector2l &avSize,
                        const cVector2l& avFontSize, int alAdvance);
    void AddGlyph(cGlyph *apGlyph);
};

};
#endif // HPL_FONTDATA_H
