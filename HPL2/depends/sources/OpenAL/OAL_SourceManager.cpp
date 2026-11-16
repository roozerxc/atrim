#include "OpenAL/OAL_SourceManager.h"
#include "OpenAL/OAL_Source.h"
#include "OpenAL/OAL_Device.h"

#include "system/MemoryManager.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"

#include <vector>
#include <stdio.h>

//-----------------------------------------------------------------------------------

extern cOAL_Device* gpDevice;

//-----------------------------------------------------------------------------------

int UpdaterThread(void*);

//-----------------------------------------------------------------------------------

static SDL_mutex* gpStatsMutex = NULL;
static double gfStatsLockWaitMs = 0;
static int glStatsLockWaits = 0;
static double gfStatsRefillMs = 0;
static wstring gsStatsRefillFile;

void OAL_Stats_RecordLockWait(double afMs)
{
	if(gpStatsMutex==NULL) return;
	SDL_LockMutex(gpStatsMutex);
	if(afMs > gfStatsLockWaitMs) gfStatsLockWaitMs = afMs;
	++glStatsLockWaits;
	SDL_UnlockMutex(gpStatsMutex);
}

void OAL_Stats_RecordRefill(double afMs, const wstring& asFile)
{
	if(gpStatsMutex==NULL) return;
	SDL_LockMutex(gpStatsMutex);
	if(afMs > gfStatsRefillMs)
	{
		gfStatsRefillMs = afMs;
		gsStatsRefillFile = asFile;
	}
	SDL_UnlockMutex(gpStatsMutex);
}

void OAL_Stats_Get(float *apLockWaitMs, int *apLockWaits, float *apRefillMs, char *apFile, int alFileSize)
{
	*apLockWaitMs = 0; *apLockWaits = 0; *apRefillMs = 0;
	if(alFileSize > 0) apFile[0] = 0;
	if(gpStatsMutex==NULL) return;

	SDL_LockMutex(gpStatsMutex);
	*apLockWaitMs = (float)gfStatsLockWaitMs;
	*apLockWaits = glStatsLockWaits;
	*apRefillMs = (float)gfStatsRefillMs;
	if(alFileSize > 0)
	{
		//Only the file name, as plain chars
		size_t lStart = gsStatsRefillFile.find_last_of(L"/\\");
		lStart = lStart == wstring::npos ? 0 : lStart + 1;
		int j = 0;
		for(size_t i = lStart; i < gsStatsRefillFile.size() && j < alFileSize - 1; ++i, ++j)
			apFile[j] = gsStatsRefillFile[i] < 128 ? (char)gsStatsRefillFile[i] : '?';
		apFile[j] = 0;
	}
	gfStatsLockWaitMs = 0;
	glStatsLockWaits = 0;
	gfStatsRefillMs = 0;
	gsStatsRefillFile.clear();
	SDL_UnlockMutex(gpStatsMutex);
}

//-----------------------------------------------------------------------------------

cOAL_SourceManager::cOAL_SourceManager() : mpStreamListMutex(NULL), 
										   mpUpdaterThread(NULL),
										   mlUpdaterThreadId(0),
										   mbUseThreading(false), 
										   mlNumOfVoices(0), 
										   mlAvailableVoices(0), 
										   mbManageVoices(true)
{
}

cOAL_SourceManager::~cOAL_SourceManager()
{
}

//-----------------------------------------------------------------------------------

bool cOAL_SourceManager::Initialize ( bool abManageVoices, int alNumSourcesHint, bool abUseThread, int alUpdateFreq, int alEFXSends )
{
	DEF_FUNC_NAME(cOAL_SourceManager::Initialize);
	FUNC_USES_AL;
	// First we must count how many sources are available
	ALuint lTempSource[256];
    
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Initializing Source Manager...\n" );
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Calculating Max Sources\n" );

	while(mlNumOfVoices<alNumSourcesHint)
	{
		RUN_AL_FUNC(alGenSources(1,&lTempSource[mlNumOfVoices]));
		if(!AL_ERROR_OCCURED)
		{
			LogMsg("", eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Source Object successfully created\n" );
			++mlNumOfVoices;
		}
		else
			break;
	}

	if(mlNumOfVoices == 0)
		return false;

	LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Created %d Low Level Sources, %d hinted\n", mlNumOfVoices, alNumSourcesHint);

	RUN_AL_FUNC(alDeleteSources ( mlNumOfVoices, lTempSource ));
	
	mbManageVoices = abManageVoices;
	mlAvailableVoices = mlNumOfVoices;
	mbUseThreading = abUseThread;

	// Then create the cOAL_Source list accordingly 

	LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Creating High Level Sources\n" );

	mvSources.resize(mlNumOfVoices);
	for(int i=0; i<mlNumOfVoices; ++i)
	{
		cOAL_Source *pSource = hplNew( cOAL_Source,(this, i,alEFXSends) );
		mvSources[i] = pSource;
	}

	LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Done creating Sources\n" );

	// Launch updater thread
    if(mbUseThreading)
	{
		LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Launching updater thread...\n" );
		// This converts the desired frequency in aInput to amount of milliseconds to wait.
		// Note that this is an int value, so any freq above 1000 will turn mlThreadWaitTime to 0;
		mlThreadWaitTime = 1000/alUpdateFreq;
		//Mutexes before the thread, it uses them right away.
		mpStreamListMutex = SDL_CreateMutex ();
		if(gpStatsMutex==NULL) gpStatsMutex = SDL_CreateMutex();
		mpUpdaterThread = SDL_CreateThread(UpdaterThread, NULL);
		mlUpdaterThreadId = mpUpdaterThread ? SDL_GetThreadID(mpUpdaterThread) : 0;

		LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Done\n" );
	}
	
	return true;

}

//-----------------------------------------------------------------------------------

void cOAL_SourceManager::Destroy()
{
	// If we are using the Updater thread, kill it
	if(mbUseThreading)							
	{
		LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info,"Stopping updater thread...\n" );
		mbUseThreading = false;
		SDL_WaitThread ( mpUpdaterThread, 0 );
		mpUpdaterThread = NULL;
		SDL_DestroyMutex(mpStreamListMutex);
	}

	//Delete sources
	LogMsg("", eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Deleting Sources...\n" );
	//hpl::Log("Sources: %d\n",mpvSources->size());
	tSourceVecIt it;
	for (it=mvSources.begin(); it!=mvSources.end(); ++it)
	{
		cOAL_Source *pSource = *it;

		//hpl::Log("source: %d\n",pSource);
		//hpl::Log("source id: %d\n",pSource->GetSourceID());
		pSource->Stop();
		hplDelete(pSource);
	}
	mvSources.clear();


	// Clear the streaming sources list
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info,"", "Deleting active streams list...\n" );
	mlstStreamingSources.clear();
}

//-----------------------------------------------------------------------------------

cOAL_Source* cOAL_SourceManager::GetSource(int alSourceHandle, bool abSkipRefCountCheck)
{
	//////////////////////////////////
	//Unpack source ID and refcount from the source
	int lHandleId = GetUnpackedSourceId(alSourceHandle);
	int lHandleRefCount = GetUnpackedRefCount(alSourceHandle);
	
	//////////////////////////////////
	//Check so handle is valid and not out of bounds
	if( (lHandleId < 0) || (lHandleId >= (int)mvSources.size()) )
		return NULL;
    
	cOAL_Source* pSource = mvSources[lHandleId];
	
	//////////////////////////////////
	// Check so that ref count is valid. (this will invalidate sources have been changed)
	if(abSkipRefCountCheck==false)
	{
		pSource->Lock();
		int lSourceRefCount = pSource->GetRefCount();
		pSource->Unlock();

		if(lHandleRefCount!=lSourceRefCount)
		pSource = NULL;
	}

	return pSource;
}

//-----------------------------------------------------------------------------------

void cOAL_SourceManager::LockStreamList()
{
	if(mbUseThreading)
		SDL_LockMutex( mpStreamListMutex );
}

//-----------------------------------------------------------------------------------

void cOAL_SourceManager::UnlockStreamList()
{
	if(mbUseThreading)
		SDL_UnlockMutex( mpStreamListMutex );
}

//-----------------------------------------------------------------------------------

int cOAL_SourceManager::GetUnpackedSourceId(int alHandle)
{
	return (alHandle & 0x00000FFF);
}

int cOAL_SourceManager::GetUnpackedRefCount(int alHandle)
{
	return (alHandle & 0x7FFFF000) >> 12;
}

//-----------------------------------------------------------------------------------

void cOAL_SourceManager::AddActiveStream(cOAL_Source* apSource)
{
	LockStreamList();
	{
		if(apSource) mlstStreamingSources.push_back(apSource);
	}
	UnlockStreamList();
}

//-----------------------------------------------------------------------------------

bool cOAL_SourceManager::IsUpdaterThread()
{
	return mpUpdaterThread != NULL && SDL_ThreadID() == mlUpdaterThreadId;
}

//-----------------------------------------------------------------------------------

void cOAL_SourceManager::UpdateStreaming()
{
	////////////////////////////////
	// Copy the list, so the game can start a stream (AddActiveStream) while streams are
	// being decoded, instead of waiting for all of them. Sources are never deleted while
	// the thread runs, so the pointers stay valid.
	std::vector<cOAL_Source*> vStreams;
	LockStreamList();
	{
		vStreams.assign(mlstStreamingSources.begin(), mlstStreamingSources.end());
	}
	UnlockStreamList();

	bool bRemove = false;
	for(size_t i = 0; i < vStreams.size(); ++i)
	{
		cOAL_Source* pSource = vStreams[i];

		double fStart = hpl::cPlatform::GetApplicationTimeX();
		pSource->Lock();
		{
			if(pSource->GetSourceType()!=eOAL_AudioDataType_Stream )
				bRemove = true;
			else
				pSource->Update();
		}
		double fMs = (hpl::cPlatform::GetApplicationTimeX() - fStart) * 1000.0;
		if(fMs >= 1.0 && pSource->GetAudioData())
			OAL_Stats_RecordRefill(fMs, pSource->GetAudioData()->GetFilename());
		pSource->Unlock();
	}

	////////////////////////////////
	// Drop sources that do not play a stream any more (checked again, it may have got a new one)
	if(bRemove)
	{
		LockStreamList();
		{
			tSourceListIt it = mlstStreamingSources.begin();
			while (it!=mlstStreamingSources.end())
			{
				cOAL_Source* pSource = *it;
				pSource->Lock();
				bool bStream = pSource->GetSourceType()==eOAL_AudioDataType_Stream;
				pSource->Unlock();

				if(bStream)
					++it;
				else
					it = mlstStreamingSources.erase(it);
			}
		}
		UnlockStreamList();
	}
}

//-----------------------------------------------------------------------------------

cOAL_Source* cOAL_SourceManager::GetAvailableSource ( unsigned int alPriority, int alNumOfVoices )
{
	cOAL_Source* pSource;
	int lSourceHandle = -1;
	int lLowestPrioSource = -1;
	unsigned int lLowestPrio = alPriority;
	unsigned int lPriority;
	
	bool bFreeSourceFound = false;
	eOAL_SourceStatus status;

	///////////////////////
	// Debug stuff:
	if(!mbManageVoices)
		alNumOfVoices = 1;
	
	////////////////////
	//If number of available voices, are less than the number wanted voices, 
	//then need to try and get voices from already active ones.
	//Loop through and inactivate voices until the number wanted is available.
	while(mlAvailableVoices < alNumOfVoices)
	{
		lLowestPrioSource = -1;

		//////////////////////
		// Find the voice with the lowest priority
		for(int i=0; i<(int)mvSources.size(); ++i )
		{
			pSource = mvSources[i];
			pSource->Lock();
				lPriority = pSource->GetPriority();
			pSource->Unlock();
			
			if(lPriority < lLowestPrio)
			{
				lLowestPrio = lPriority;
				lLowestPrioSource = i;
			}
		}
		//////////////////////
		// Found a source, stop it.
		if(lLowestPrioSource != -1)
		{
			pSource = mvSources[lLowestPrioSource];
			pSource->Lock();
				pSource->Stop();
				pSource->IncRefCount(); //Do this to make sure the source is invalidated!
			pSource->Unlock();
		}
		//////////////////////
		// No source found, leave loop
		else
		{
			break;
		}
	}

	//////////////////////
	// If there are voices available, get one to use
	if(mlAvailableVoices >= alNumOfVoices)
	{
		for(int i=0; i<(int)mvSources.size(); ++i )
		{
			pSource = mvSources[i];
			
			pSource->Lock();
				status = pSource->GetSourceStatus();
			pSource->Unlock();

			if(status == eOAL_SourceStatus_Free) 
			{
				lSourceHandle = i;
				break;
			}
		}
	}

	////////////////////////////
	// Return source
	if(lSourceHandle != -1)
		return mvSources[lSourceHandle];
	else
		return NULL;
}

//-----------------------------------------------------------------------------------

int UpdaterThread(void* alUnusedArg)
{
	cOAL_SourceManager* pSourceManager = gpDevice->GetSourceManager();
	
	int lWaitTime = pSourceManager->GetThreadWaitTime();

	while(pSourceManager->IsThreadAlive())
	{
		//	While the thread lives, perform the update
        pSourceManager->UpdateStreaming();
		//	And rest a bit
		SDL_Delay(lWaitTime);			
	}
	return 0;
}

//-----------------------------------------------------------------------------------
/*
void cOAL_SourceManager::Log( eOAL_LogVerbose aVerboseLevelReq, eOAL_LogMsg aMessageType, const char* asMessage, ...)
{
	if(asMessage == NULL)
		return;
	if(!mbLogEnabled)
		return;

	if(aVerboseLevelReq > mLogVerboseLevel)
		return;

	string sMessage;

	char text[2048];
	va_list ap;
	va_start(ap, asMessage);
	vsprintf(text, asMessage, ap);
	va_end(ap);

	
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

	sMessage.append("SourceManager - ").append(text);

	Write(sMessage);

}
*/
//-----------------------------------------------------------------------------------
