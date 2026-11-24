/**
	@file OAL_OggSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing Ogg Vorbis Sample data 
*/

#include "OpenAL/OAL_OggSample.h"
#include "OpenAL/OAL_Buffer.h"
#include "system/Platform.h"
#include "system/MemoryManager.h"
#include "system/LowLevelSystem.h"
#include <cstring>

//-------------------------------------------------------------------------------

extern ov_callbacks gCallbacks;

//-------------------------------------------------------------------------------

///////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------

bool cOAL_OggSample::CreateFromFile(const wstring &asFilename)
{
	DEF_FUNC_NAME("cOAL_OggSample::Load()");
	FUNC_USES_AL;

	if(mbStatus == false)
		return false;

	Reset();

	char *pPCMBuffer = NULL;
	bool bEOF = false;
	int lOpenResult;
	int lCurrent_section = 0;
	long lDataSize = 0;

	msFilename = asFilename;

	FILE *fileHandle = hpl::cPlatform::OpenFile(asFilename, L"rb");
	if(!fileHandle)
	{
		mbStatus = false;
		return false;
	}

	OggVorbis_File ovFileHandle;
	memset(&ovFileHandle, 0, sizeof(ovFileHandle));

	if((lOpenResult = ov_open_callbacks(fileHandle, &ovFileHandle, NULL, 0, gCallbacks)) < 0)
	{
		fclose(fileHandle);
		mbStatus = false;
		return false;
	}

	vorbis_info *viFileInfo = ov_info(&ovFileHandle, -1);
	if(viFileInfo == NULL || viFileInfo->channels < 1 || viFileInfo->channels > 2 || viFileInfo->rate <= 0)
	{
		ov_clear(&ovFileHandle);
		mbStatus = false;
		return false;
	}

	mlChannels  = viFileInfo->channels;
	mFormat     = (mlChannels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	mlFrequency = viFileInfo->rate;
	ogg_int64_t llPcmTotal = ov_pcm_total(&ovFileHandle, -1);
	mlSamples   = (long)llPcmTotal;
	mfTotalTime = ov_time_total(&ovFileHandle, -1);

	if(llPcmTotal < 0)
	{
		ov_clear(&ovFileHandle);
		mbStatus = false;
		return false;
	}

	const int kSafetyMargin = STREAMING_BLOCK_SIZE * 8;
	// Work out the size in 64 bits first: samples * channels * bytes can overflow a 32 bit int
	// for a corrupt or huge ov_pcm_total, and wrap to a small positive size that passes the check.
	const long long kMaxBytes = 10LL * 60 * 48000 * 2 * 2;
	long long llSizeInBytes = (long long)(llPcmTotal > 0 ? llPcmTotal : 1) * mlChannels * GetBytesPerSample() + kSafetyMargin;
	if(llSizeInBytes <= 0 || llSizeInBytes > kMaxBytes)
	{
		ov_clear(&ovFileHandle);
		mbStatus = false;
		return false;
	}

	int lSizeInBytes = (int)llSizeInBytes;

	pPCMBuffer = (char*)hplMalloc(lSizeInBytes);
	if(pPCMBuffer == NULL)
	{
		ov_clear(&ovFileHandle);
		mbStatus = false;
		return false;
	}
	memset(pPCMBuffer, 0, lSizeInBytes);

	while(!bEOF)
	{
		long lRemaining = lSizeInBytes - lDataSize;
		if(lRemaining <= 0)
		{
			bEOF = true;
			break;
		}

		long lChunkSize = ov_read(&ovFileHandle,
		                          pPCMBuffer + lDataSize,
		                          (lRemaining < STREAMING_BLOCK_SIZE) ? (int)lRemaining : STREAMING_BLOCK_SIZE,
		                          SYS_ENDIANNESS,
		                          2, 1, &lCurrent_section);

		if(lChunkSize == 0)
		{
			bEOF = true;
		}
		else if(lChunkSize < 0)
		{
			hplFree(pPCMBuffer);
			ov_clear(&ovFileHandle);
			mbStatus = false;
			return false;
		}
		else
		{
			lDataSize += lChunkSize;
		}
	}

	cOAL_Buffer* pBuffer = mvBuffers[0];
	if(lDataSize > 0)
	{
		mbStatus = pBuffer->Feed((ALvoid*)pPCMBuffer, lDataSize);
	}
	else
	{
		mbStatus = false;
	}

	hplFree(pPCMBuffer);
	ov_clear(&ovFileHandle);

	return mbStatus;
}

