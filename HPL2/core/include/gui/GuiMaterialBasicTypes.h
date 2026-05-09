#ifndef HPL_GUI_MATERIAL_BASIC_TYPES_H
#define HPL_GUI_MATERIAL_BASIC_TYPES_H

#include "gui/GuiMaterial.h"

namespace hpl
{

//-----------------------------------------

class cGuiMaterial_Diffuse : public iGuiMaterial
{
public:
    cGuiMaterial_Diffuse(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Diffuse",apLowLevelGraphics) {}
    ~cGuiMaterial_Diffuse() {}

    void BeforeRender();
    void AfterRender();
};

//-----------------------------------------

class cGuiMaterial_Alpha : public iGuiMaterial
{
public:
    cGuiMaterial_Alpha(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Alpha",apLowLevelGraphics) {}
    ~cGuiMaterial_Alpha() {}

    void BeforeRender();
    void AfterRender();
};

//-----------------------------------------

class cGuiMaterial_FontNormal : public iGuiMaterial
{
public:
    cGuiMaterial_FontNormal(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("FontNormal",apLowLevelGraphics) {}
    ~cGuiMaterial_FontNormal() {}

    void BeforeRender();
    void AfterRender();
};

//-----------------------------------------

class cGuiMaterial_Additive : public iGuiMaterial
{
public:
    cGuiMaterial_Additive(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Additive",apLowLevelGraphics) {}
    ~cGuiMaterial_Additive() {}

    void BeforeRender();
    void AfterRender();
};

//-----------------------------------------

class cGuiMaterial_Modulative : public iGuiMaterial
{
public:
    cGuiMaterial_Modulative(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Modulative",apLowLevelGraphics) {}
    ~cGuiMaterial_Modulative() {}

    void BeforeRender();
    void AfterRender();
};

//-----------------------------------------

class cGuiMaterial_PremulAlpha : public iGuiMaterial
{
public:
    cGuiMaterial_PremulAlpha(iLowLevelGraphics *apLowLevelGraphics):iGuiMaterial("Alpha",apLowLevelGraphics) {}
    ~cGuiMaterial_PremulAlpha() {}

    void BeforeRender();
    void AfterRender();
};

//-----------------------------------------

};
#endif // HPL_GUI_MATERIAL_BASIC_TYPES_H
