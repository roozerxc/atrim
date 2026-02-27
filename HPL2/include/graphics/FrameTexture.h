#ifndef HPL_FRAMETEXTURE_H
#define HPL_FRAMETEXTURE_H

#include "graphics/FrameBase.h"

namespace hpl {

    class iTexture;
    class cFrameSubImage;
    class cImageManager;

    class cFrameTexture : public iFrameBase
    {
    public:
        cFrameTexture(iTexture *pTex, int alHandle,cImageManager *apImageManager, bool abIsCustom);
        ~cFrameTexture();

        iTexture* GetTexture();
        int GetHandle(){return mlHandle;}

        inline bool IsCustom(){ return mbIsCustom;}

        /**
        * Creates a subimage in frame at pos and with size messured in pixel. Only works on custom frames!
        */
        cFrameSubImage* CreateCustomImage(const cVector2l& avPixelPos,const cVector2l& avPixelSize);
        
    private:
        

        iTexture *mpTexture;
        int mlHandle;

        bool mbIsCustom;

        cImageManager *mpImageManager;
    };

};
#endif // HPL_FRAMETEXTURE_H
