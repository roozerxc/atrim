#ifndef HPL_VERTEXBUFFER_OGL_ARRAY_H
#define HPL_VERTEXBUFFER_OGL_ARRAY_H

#include "impl/VertexBufferOpenGL.h"

namespace hpl {

    class cVertexBufferOGL_Array : public iVertexBufferOpenGL
    {
    public:
        cVertexBufferOGL_Array(    iLowLevelGraphics* apLowLevelGraphics,
                                eVertexBufferDrawType aDrawType,eVertexBufferUsageType aUsageType,
                                int alReserveVtxSize,int alReserveIdxSize);
        ~cVertexBufferOGL_Array();

        void UpdateData(tVertexElementFlag aTypes, bool abIndices);

        void Draw(eVertexBufferDrawType aDrawType);
        void DrawIndices(unsigned int *apIndices, int alCount,
                        eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum);

        void Bind();
        void UnBind();
        
        
    private:
        void CompileSpecific();
        iVertexBufferOpenGL* CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
                                            eVertexBufferUsageType aUsageType,
                                            int alReserveVtxSize,int alReserveIdxSize);

        void SetVertexStates();
    };

};
#endif // HPL_VERTEXBUFFER_OGL_ARRAY_H
