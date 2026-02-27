#ifndef HPL_VIDEO_STREAM_THEORA_H
#define HPL_VIDEO_STREAM_THEORA_H

#include "graphics/VideoStream.h"
#include "resources/VideoLoader.h"

#include <theora/theora.h>

namespace hpl {

    //-----------------------------------------
    class  cVideoStreamTheora_Loader;

    class cVideoStreamTheora : public iVideoStream
    {
    public:
        cVideoStreamTheora(const tString& asName, cVideoStreamTheora_Loader* apLoader);
        ~cVideoStreamTheora();

        bool LoadFromFile(const tWString& asFilePath);
        
        void Update(float afTimeStep);

        void Play();
        void Stop();

        void Pause(bool abX);
        bool IsPaused(){ return mbPaused;}

        void SetLoop(bool abX);
        bool IsLooping(){ return mbLooping;}

        void CopyToTexture(iTexture *apTexture);

    private:
        void DrawFrameToBuffer();
        int BufferData(FILE *pFile ,ogg_sync_state *apOggSynchState);
        void QueuePage(ogg_page *apPage);
        bool GetHeaders();
        bool InitDecoders();
        void ResetStreams();
        
        cVideoStreamTheora_Loader *mpLoader;

        FILE *mpFile;

        bool mbLooping;
        bool mbPaused;
        bool mbPlaying;

        float mfTime;

        unsigned char *mpFrameBuffer;
        
        ogg_sync_state   mOggSyncState;
        ogg_stream_state mTheoraStreamState;
        theora_info      mTheoraInfo;
        theora_comment mTheoraComment;
        theora_state    mTheoraState;

        int          mlVideobufReady;
        ogg_int64_t  mlVideobufGranulePos;
        double       mfVideobufTime;

        bool mbVideoLoaded;
        bool mbVideoFrameReady;
        int mlBufferSize;
    };

    //-----------------------------------------

    class cVideoStreamTheora_Loader : public iVideoLoader
    {
    friend class cVideoStreamTheora;
    public:
        cVideoStreamTheora_Loader();
        ~cVideoStreamTheora_Loader();
        
        iVideoStream* LoadVideo(const tWString& asName);
    
    private:
        unsigned char* mpYuvToR;
        unsigned char* mpYuvToB;
        
        unsigned short* mpYuv_G_UV;
        unsigned char* mpYuv_G_Y_UV;
    };

    //-----------------------------------------

};
#endif // HPL_VIDEO_STREAM_THEORA_H
