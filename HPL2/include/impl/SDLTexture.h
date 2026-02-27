#ifndef HPL_SDL_TEXTURE_H
#define HPL_SDL_TEXTURE_H

#include "graphics/Texture.h"
#include "impl/LowLevelGraphicsSDL.h"

#include <GL/glew.h>
#if defined(__APPLE__)&&defined(__MACH__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

namespace hpl {

    class cBitmapData;

    class cSDLTexture : public iTexture
    {
    public:
        cSDLTexture(const tString& asName, eTextureType aType, eTextureUsage aUsage, iLowLevelGraphics* apLowLevelGraphics);
        ~cSDLTexture();

        bool CreateFromBitmap(cBitmap* pBmp);
        bool CreateAnimFromBitmapVec(std::vector<cBitmap*> *avBitmaps);
        bool CreateCubeFromBitmapVec(std::vector<cBitmap*> *avBitmaps);
        bool CreateFromRawData(const cVector3l &avSize,ePixelFormat aPixelFormat, unsigned char *apData);

        virtual void SetRawData(    int alLevel, const cVector3l& avOffset, const cVector3l& avSize, 
                                    ePixelFormat aPixelFormat, void *apData);
        
        void SetFilter(eTextureFilter aFilter);
        void SetAnisotropyDegree(float afX);

        void SetWrapS(eTextureWrap aMode);
        void SetWrapT(eTextureWrap aMode);
        void SetWrapR(eTextureWrap aMode);
        void SetWrapSTR(eTextureWrap aMode);

        void SetCompareMode(eTextureCompareMode aMode);
        void SetCompareFunc(eTextureCompareFunc aFunc);

        void AutoGenerateMipmaps();

        void Update(float afTimeStep);

        bool HasAnimation();
        void NextFrame();
        void PrevFrame();
        float GetT();
        float GetTimeCount();
        void SetTimeCount(float afX);
        int GetCurrentLowlevelHandle();

        /// SDL / OGL Specific ///////////

        unsigned int GetTextureHandle();
    private:
        void GenerateHandles(int alNumOfHandles);

        bool CreateFromBitmapToIndex(cBitmap* apBmp, int alIdx);

        bool CreateTexture(    int alTextureHandle,
                            cBitmapData* apBitmapImage, int alNumOfMipMaps,
                            const cVector3l avSize, ePixelFormat aPixelFormat, 
                            int alFaceNum,bool abGenerateMipMaps,
                            bool abCheckForResize);

        bool CopyTextureDataToGL(    int alTextureHandle, int alLevel,unsigned char *apData,int alDataSize,
                                    const cVector3l avSize, ePixelFormat aPixelFormat,int alFaceNum);

        void GenerateMipMaps(    GLenum aGLTarget, ePixelFormat aPixelFormat,const cVector3l avSize, 
                                unsigned char *apData,int alDataSize, int alFaceNum);
        
        void SetupProperties(int alTextureHandle);


        unsigned char* ResizePixelData(unsigned char *apData, int alBytesPerPixel);

        
        tUIntVec mvTextureHandles;
        bool mbContainsData;
        cLowLevelGraphicsSDL* mpGfxSDL;

        float mfTimeCount;
        int mlTextureIndex;
        float mfTimeDir;
    };

};
#endif // HPL_SDL_TEXTURE_H
