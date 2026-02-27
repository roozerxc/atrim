#ifndef HPL_GUI_MATERIAL_H
#define HPL_GUI_MATERIAL_H

#include "system/SystemTypes.h"


namespace hpl {

    class iLowLevelGraphics;
    
    class iGuiMaterial
    {
    public:
        iGuiMaterial(const tString &asName,iLowLevelGraphics *apLowLevelGraphics) :
                        msName(asName), mpLowLevelGraphics(apLowLevelGraphics) {}
        virtual ~iGuiMaterial(){}

        virtual void BeforeRender()=0;
        virtual void AfterRender()=0;

        const tString& GetName() const {return msName;}

    protected:
        tString msName;
        iLowLevelGraphics *mpLowLevelGraphics;
    };

};
#endif // HPL_GUI_MATERIAL_H
