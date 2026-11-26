#ifndef _OAL_SOURCEMANAGER_H
#define _OAL_SOURCEMANAGER_H

#include "OAL_Types.h"
#include "OAL_Helper.h"
#include "OAL_LoggerObject.h"

#include <SDL/SDL.h>

class cOAL_SourceManager : public iOAL_LoggerObject
{
public:

	cOAL_SourceManager();
	~cOAL_SourceManager();

	cOAL_Source* GetSource ( int alSourceHandle, bool abSkipRefCountCheck = false );
	cOAL_Source* GetAvailableSource ( unsigned int alPriority, int alNumOfVoices );

	bool Initialize( bool abManageVoices, int alNumSourcesHint, bool abUseThreading, int alUpdateFreq, int alEFXSends = 0 );
	void Destroy ( );

	void AddActiveStream ( cOAL_Source *apSource );
	void UpdateStreaming ( );

	inline void ReserveVoices ( int alNum ) { if (!mbManageVoices) alNum = 1; mlAvailableVoices -= alNum; }
	inline void ReleaseVoices ( int alNum ) { if (!mbManageVoices) alNum = 1; mlAvailableVoices += alNum; }

	inline bool IsThreadAlive ( ) { return mbUseThreading; }
	bool IsUpdaterThread ( );
	inline int	GetThreadWaitTime ( ) { return mlThreadWaitTime; }

	inline int GetNumVoices()	{ return mlNumOfVoices; }

private:

	void LockStreamList();
	void UnlockStreamList();

	int GetUnpackedSourceId(int alHandle);
	int GetUnpackedRefCount(int alHandle);
	
	bool mbManageVoices; 

	int mlNumOfVoices;
	int mlAvailableVoices;

    SDL_mutex*			mpStreamListMutex;
	SDL_Thread*			mpUpdaterThread;
	Uint32				mlUpdaterThreadId;
	int					mlThreadWaitTime;
	bool				mbUseThreading;

	tSourceVec mvSources;
	tSourceList mlstStreamingSources;

};

void OAL_Stats_RecordLockWait(double afMs);
void OAL_Stats_RecordRefill(double afMs, const wstring& asFile);
void OAL_Stats_Get(float *apLockWaitMs, int *apLockWaits, float *apRefillMs, char *apFile, int alFileSize);
void OAL_Stats_RecordSlowRefill(double afMs, const wstring& asFile, int alBuffers, double afSeekMs,
								int alReadErrors, int alHoles);
bool OAL_Stats_GetSlowRefill(char *apText, int alTextSize);

#endif 


