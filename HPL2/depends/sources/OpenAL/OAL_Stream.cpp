#include "OpenAL/OAL_Types.h"
#include "OpenAL/OAL_Device.h"
#include "OpenAL/OAL_Helper.h"
#include "OpenAL/OAL_Stream.h"
#include "OpenAL/OAL_Source.h"
#include "OpenAL/OAL_Buffer.h"

#include "system/MemoryManager.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"

//-----------------------------------------------------------------------------------

unsigned int cOAL_Stream::mlBufferSize = STREAMING_BLOCK_SIZE;
unsigned int cOAL_Stream::mlBufferCount = 4;

//-----------------------------------------------------------------------------------

//Shared by all streams so a stream made at the address of a deleted one never gets the
//value the stream thread saw for the old one. Only changed by the game thread.
static unsigned int glStreamGenerationCounter = 0;

cOAL_Stream::cOAL_Stream() : iOAL_AudioData(eOAL_AudioDataType_Stream, mlBufferCount), mpBoundSource(NULL)
{
	mpPCMBuffer = (char*)malloc(mlBufferSize*sizeof(char));
	mvOALBufferIDs = (ALuint*)malloc(mlBufferCount*sizeof(ALuint));
	for(int i=0;i<(int)mlBufferCount;++i)
		mvOALBufferIDs[i] = mvBuffers[i]->GetObjectID();

	mfProcessedBuffersTime = 0;

	mpDecodeMutex = SDL_CreateMutex();
	mlGeneration = ++glStreamGenerationCounter;
	mlReadErrors = 0;
	mlHoles = 0;
}

cOAL_Stream::~cOAL_Stream()
{
	//hpl::Log(" OAL: Destroying stream %p\n", this);

	StopAndWaitForDecode();

	if(mpDecodeMutex)
		SDL_DestroyMutex(mpDecodeMutex);

	free(mpPCMBuffer);
	free(mvOALBufferIDs);
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::StopAndWaitForDecode()
{
	cOAL_Source* pSource = mpBoundSource;
	if(pSource != NULL)
	{
		pSource->Lock();
		if(mpBoundSource == pSource)
			pSource->Stop();
		pSource->Unlock();
	}

	//The stream is not bound any more, so no new refill can start. Wait for one in progress.
	if(mpDecodeMutex)
	{
		SDL_LockMutex(mpDecodeMutex);
		SDL_UnlockMutex(mpDecodeMutex);
	}
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::SeekFromGame(float afWhere)
{
	if(mpDecodeMutex) SDL_LockMutex(mpDecodeMutex);
	mlGeneration = ++glStreamGenerationCounter;
	Seek(afWhere, true);
	if(mpDecodeMutex) SDL_UnlockMutex(mpDecodeMutex);
}

//-----------------------------------------------------------------------------------

bool cOAL_Stream::BeginThreadedUpdate(cOAL_Source* apSource, tBufferVec& avToFill, bool& abRebuffer,
									  bool& abUnderrun, bool& abLoop, unsigned int& alGeneration)
{
	avToFill.clear();
	abRebuffer = false;
	abUnderrun = false;

	if(apSource == NULL || mpBoundSource != apSource)
		return false;

	////////////////////////////
	// Same checks as cOAL_Source::Update
	eOAL_SourceStatus sourceStatus = apSource->GetSourceStatus();
	if(sourceStatus!=eOAL_SourceStatus_Busy &&
	   sourceStatus!=eOAL_SourceStatus_Busy_BufferUnderrun )
		return false;

	if(GetStatus()==false)
	{
		apSource->Stop();
		return false;
	}
	abUnderrun = sourceStatus==eOAL_SourceStatus_Busy_BufferUnderrun;
	abLoop = mbLoop || apSource->mbLoop;

	if(mpDecodeMutex) SDL_LockMutex(mpDecodeMutex);
	alGeneration = mlGeneration;

	////////////////////////////
	// Start of play or a seek: take all buffers off and refill them all
	if(mbNeedsRebuffering)
	{
		apSource->LowLevelStop();
		mlBuffersUsed=0;
		mfProcessedBuffersTime=GetTime();
		int lQueuedBuffers = apSource->GetQueuedBuffers();
		for(int i=0;i<lQueuedBuffers;++i)
			apSource->Unqueue();

		avToFill.assign(mvBuffers.begin(), mvBuffers.end());
		abRebuffer = true;
		return true;
	}

	////////////////////////////
	// Take the played buffers off
	if(mlBuffersUsed!=0)
	{
		int lProcessedBuffers = apSource->GetProcessedBuffers();
		for(int i=0; i<lProcessedBuffers; ++i)
		{
			cOAL_Buffer* pBuffer = apSource->Unqueue();
			if(pBuffer == NULL)
				continue;
			mfProcessedBuffersTime += pBuffer->GetBufferTime();
			if(mfProcessedBuffersTime > mfTotalTime)
				mfProcessedBuffersTime-=mfTotalTime;
			avToFill.push_back(pBuffer);
		}
	}

	if(avToFill.empty())
	{
		//An underrun with nothing to refill still gets the source started again (as before)
		if(abUnderrun)
			apSource->LowLevelPlay();
		if(mpDecodeMutex) SDL_UnlockMutex(mpDecodeMutex);
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::DecodeThreaded(const tBufferVec& avToFill, bool abLoop, tBufferVec& avFilled,
								 double& afSeekMs, int& alReadErrors, int& alHoles)
{
	avFilled.clear();
	afSeekMs = 0;
	mlReadErrors = 0;
	mlHoles = 0;

	for(size_t i=0; i<avToFill.size(); ++i)
	{
		cOAL_Buffer* pBuffer = avToFill[i];
		if(Stream(pBuffer))
		{
			avFilled.push_back(pBuffer);
		}
		else if(mbEOF && abLoop)
		{
			double fSeekStart = hpl::cPlatform::GetApplicationTimeX();
			Seek(0, false);
			afSeekMs += (hpl::cPlatform::GetApplicationTimeX() - fSeekStart) * 1000.0;
			if(Stream(pBuffer))
				avFilled.push_back(pBuffer);
		}
	}

	alReadErrors = mlReadErrors;
	alHoles = mlHoles;
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::UnlockDecode()
{
	if(mpDecodeMutex) SDL_UnlockMutex(mpDecodeMutex);
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::EndThreadedUpdate(cOAL_Source* apSource, unsigned int alGeneration, bool abRebuffer,
									bool abUnderrun, const tBufferVec& avFilled)
{
	//Stopped, given to another source or seeked while decoding: the next update refills
	//from the new position, these buffers are not used.
	if(mpBoundSource != apSource || mlGeneration != alGeneration)
		return;

	for(size_t i=0; i<avFilled.size(); ++i)
		apSource->Queue(avFilled[i]);

	if(abRebuffer)
	{
		mlBuffersUsed = (int)avFilled.size();
		if(mlBuffersUsed!=0)
			mbNeedsRebuffering = false;
		if(apSource->mbPlaying && apSource->mbPaused==false)
			apSource->LowLevelPlay();
	}
	else if(abUnderrun)
	{
		//It ran out of data and stopped, start it again
		apSource->LowLevelPlay();
	}
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::Update()
{
	if(mpBoundSource==NULL)
		return;

	if(mbNeedsRebuffering)
	{
		bool bPlaying = mpBoundSource->mbPlaying;
		bool bPaused = mpBoundSource->mbPaused;
		mpBoundSource->LowLevelStop();
		mlBuffersUsed=0;
		mfProcessedBuffersTime=GetTime();
		int lQueuedBuffers = mpBoundSource->GetQueuedBuffers();
		for(int i=0;i<lQueuedBuffers;++i)
			mpBoundSource->Unqueue();

		for(int i=0;i<(int)mvBuffers.size();++i)
		{
			cOAL_Buffer* pBuffer = mvBuffers[i];
			if(Stream(pBuffer))
			{
				++mlBuffersUsed;
				mpBoundSource->Queue(pBuffer);
			}
		}
		if(mlBuffersUsed!=0)
			mbNeedsRebuffering = false;
		if(bPlaying && bPaused==false)
			mpBoundSource->LowLevelPlay();
	}

	if(mlBuffersUsed==0)
		return;

	int lProcessedBuffers = mpBoundSource->GetProcessedBuffers();
	// For every buffer that has been played, unqueue it, refill it with streamed data and enqueue it again
	while(lProcessedBuffers)
	{
		// Then perform the actual streaming
		cOAL_Buffer* pBuffer = mpBoundSource->Unqueue();
		mfProcessedBuffersTime += pBuffer->GetBufferTime();
		if(mfProcessedBuffersTime > mfTotalTime)
			mfProcessedBuffersTime-=mfTotalTime;
		
		if(Stream(pBuffer))
		{
			mpBoundSource->Queue(pBuffer);
		}
		else
		{
            if(mbEOF && (mbLoop || mpBoundSource->mbLoop))
			{
				Seek(0, false);
				Stream(pBuffer);
				mpBoundSource->Queue(pBuffer);
			}
		}
		lProcessedBuffers--;
	}
}

//-----------------------------------------------------------------------------------

void cOAL_Stream::DoBuffering()
{
}

//-----------------------------------------------------------------------------------

bool cOAL_Stream::HasBufferUnderrun()
{
	if(mpBoundSource==NULL)
		return false;

	if(IsEOF()==false)
	{
		//hpl::Log("Have not reached end of file\n");
		return true;
	}

	if(mpBoundSource->GetQueuedBuffers()>0)
	{
		//hpl::Log("Source still has %d buffers to play\n", mpBoundSource->GetQueuedBuffers());
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------
/*
void cOAL_Stream::Log( eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMessageType, const char* asMessage, ...)
{
	if(asMessage == NULL)
		return;
	if(!mbLogEnabled)
		return;

	if(aVerboseLevelReq < mLogVerboseLevel)
		return;

	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);
	char streamId[20];

	sprintf(streamId, "Stream %s - ", msFilename.c_str());

	switch(aMessageType)
	{
	case eOAL_LogMsg_Command:
		sMessage.append("[COMMAND] ");
		break;
	case eOAL_LogMsg_Info:
		sMessage.append("[INFO] ");
		break;
	case eOAL_LogMsg_Error:
		sMessage.append("[ERROR] ");
	default:
		break;
	}

	sMessage.append(streamId).append(text);

	Write(sMessage);

}
*/
//-----------------------------------------------------------------------------------

