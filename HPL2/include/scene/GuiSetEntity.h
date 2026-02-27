#ifndef HPL_GUI_SET_ENTITY_H
#define HPL_GUI_SET_ENTITY_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

#include "scene/Entity3D.h"
#include "graphics/Renderable.h"

namespace hpl {

    class cResources;
    class cGraphics;
    class cGui;

    class cGuiSet;

    //------------------------------------------

    class cGuiSetEntity : public iRenderable
    {
    #ifdef __GNUC__
        typedef iRenderable __super;
    #endif
    public:
        cGuiSetEntity(const tString asName,cGuiSet *apSet);
        ~cGuiSetEntity();

        void SetGuiSet(cGuiSet *apSet);
        cGuiSet* GetGuiSet(){ return mpGuiSet;}
        
        /////////////////////////////////
        //Entity implementation
        tString GetEntityType(){ return "GuiSetEntity";}

        /////////////////////////////////
        //Renderable implementations
        cMaterial *GetMaterial(){ return NULL;}
        iVertexBuffer* GetVertexBuffer(){return NULL;}

        eRenderableType GetRenderType(){ return eRenderableType_GuiSet;}
        cMatrixf* GetModelMatrix(cFrustum *apFrustum);
        int GetMatrixUpdateCount();

    private:
        cGuiSet *mpGuiSet;

        cMatrixf m_mtxTemp;
    };

};
#endif // HPL_GUI_SET_ENTITY_H
