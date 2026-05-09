#ifndef HPL_IMAGEMANAGER_H
#define HPL_IMAGEMANAGER_H

#include "resources/ResourceManager.h"
#include "math/MathTypes.h"

namespace hpl
{

class cResources;
class cFrameSubImage;
class cFrameTexture;
class cFrameBitmap;
class iLowLevelGraphics;
class cBitmap;
class cBitmapLoaderHandler;
class iTexture;

typedef std::list<cFrameBitmap*> tFrameBitmapList;
typedef tFrameBitmapList::iterator tFrameBitmapListIt;

typedef std::map<int,cFrameTexture*> tFrameTextureMap;
typedef tFrameTextureMap::iterator tFrameTextureMapIt;

class cImageManager :public iResourceManager
{
    friend class cFrameTexture;
public:
    cImageManager(    cResources *mpResources, iLowLevelGraphics *apLowLevelGraphics,
                      iLowLevelSystem *apLowLevelSystem);
    ~cImageManager();

    void Destroy(iResourceBase* apResource);

    void Unload(iResourceBase* apResource);

    //Image specifc
    iResourceBase* CreateInFrame(const tString& asName, int alFrameHandle);
    cFrameSubImage* CreateImage(const tString& asName, int alFrameHandle=-1);
    /**
     * Draws all updated content to textures. THis must be done before a loaded image can be used.
     * Use this as unoften as possible.
     * \return Number of bitmaps flushes
     */
    int FlushAll();
    void ReorganizeAll();

    cFrameSubImage* CreateFromBitmap(const tString &asName,cBitmap* apBmp, int alFrameHandle=-1);

    cFrameTexture* CreateCustomFrame(iTexture *apTexture);

    cFrameTexture* GetFrameTexture(int alHandle);

    int CreateFrame(cVector2l avSize);
    void SetFrameLocked(int alHandle, bool abLocked);
private:
    iLowLevelGraphics *mpLowLevelGraphics;
    cBitmapLoaderHandler *mpBitmapLoaderHandler;

    tFrameBitmapList mlstBitmapFrames;
    tFrameTextureMap m_mapTextureFrames;

    cVector2l mvFrameSize;
    int mlFrameHandle;

    cFrameSubImage *FindImage(const tString &asName, tWString &asFilePath);
    cFrameSubImage *AddToFrame(cBitmap *apBmp, const tWString& asFullPath, int alFrameHandle);
    cFrameBitmap *CreateBitmapFrame(cVector2l avSize);

};

};
#endif // HPL_RESOURCEMANAGER_H
