#ifndef HPL_ROPE_ENTITY_H
#define HPL_ROPE_ENTITY_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

#include "graphics/Renderable.h"

namespace hpl {

    class cMaterialManager;
    class cGraphics;
    class cResources;
    class iLowLevelGraphics;
    class cMaterial;
    class iVertexBuffer;
    class iPhysicsRope;
    
    //------------------------------------------

    class cRopeEntity : public iRenderable
    {
    public:
        cRopeEntity(const tString& asName, cResources *apResources,cGraphics *apGraphics, iPhysicsRope *apRope, int alMaxSegments);
        ~cRopeEntity();

        const tString& GetName(){return msName;}

        iPhysicsRope *GetPhysicsRope(){ return mpRope;}

        void SetMaterial(cMaterial * apMaterial);
        
        void SetColor(const cColor &aColor);
        const cColor& GetColor(){ return mColor;}

        void SetMultiplyAlphaWithColor(bool abX);
        bool GetMultiplyAlphaWithColor(){ return mbMultiplyAlphaWithColor;}

        void SetRadius(float afX){ mfRadius = afX;}
        float GetRadius(){ return mfRadius;}
        
        int GetMaxSegments(){ return mlMaxSegments;}

        /**
         * Only used when segment size > 0
         */
        void SetLengthTileAmount(float afX){ mfLengthTileAmount = afX;}
        float GetLengthTileAmount(){ return mfLengthTileAmount;}

        /**
        * Only used when segment size = 0
        */
        void SetLengthTileSize(float afX){ mfLengthTileSize = afX;}
        float GetLengthTileSize(){ return mfLengthTileSize;}

        
        /////////////////////////////////
        //Entity implementation
        tString GetEntityType(){ return "RopeEntity";}

        bool IsVisible();
        
        /////////////////////////////////
        //Renderable implementations
        cMaterial *GetMaterial(){ return mpMaterial;}
        iVertexBuffer* GetVertexBuffer(){return mpVtxBuffer;}

        void UpdateGraphicsForFrame(float afFrameTime);
        bool UpdateGraphicsForViewport(cFrustum *apFrustum,float afFrameTime);

        cBoundingVolume* GetBoundingVolume();

        cMatrixf* GetModelMatrix(cFrustum *apFrustum);

        int GetMatrixUpdateCount();


        eRenderableType GetRenderType(){ return eRenderableType_Rope;}
    private:
        cMaterialManager* mpMaterialManager;
        iLowLevelGraphics* mpLowLevelGraphics;

        iPhysicsRope *mpRope;
        
        cMaterial *mpMaterial;
        iVertexBuffer* mpVtxBuffer;

        int mlMaxSegments;

        float mfRadius;
        float mfLengthTileAmount;
        float mfLengthTileSize;
        
        cColor mColor;
        bool mbMultiplyAlphaWithColor;

        int mlLastUpdateCount;
    };

};
#endif // HPL_ROPE_ENTITY_H
