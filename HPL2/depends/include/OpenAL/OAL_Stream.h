#ifndef _OAL_STREAM_H
#define _OAL_STREAM_H

#include "OAL_AudioData.h"
#include <SDL/SDL_mutex.h>

//-----------------------------------------------------------------------------------

class cOAL_Stream : public iOAL_AudioData
{
public:

	cOAL_Stream();
	virtual ~cOAL_Stream();

	/////////////////////////////////////

	void AddBoundSource(cOAL_Source* apSource) { if(apSource) mpBoundSource = apSource; }
	void RemoveBoundSource(cOAL_Source* apSource) { if(apSource && (apSource == mpBoundSource)) mpBoundSource = NULL; }

	void Update();

	void DoBuffering();

	virtual void Seek(float afWhere, bool abForceRebuffer=true)=0;
	virtual double GetTime()=0;

	inline bool IsLocked() { return mpBoundSource != NULL; }

	// Streaming Config stuff
	static inline void SetBufferCount(unsigned int alBufferCount)	{ if(alBufferCount >= 1) mlBufferCount = alBufferCount; } 
	static inline void SetBufferSize(unsigned int alBufferSize)		{ if(alBufferSize >= 1) mlBufferSize = alBufferSize; }
	
	static inline unsigned int GetBufferSize()			{ return mlBufferSize; }
	static inline unsigned int GetBufferCount()			{ return mlBufferCount; }

	ALuint* GetOALBufferPointer() { return mvOALBufferIDs; }

	double GetProcessedBuffersTime() { return mfProcessedBuffersTime; }

	bool HasBufferUnderrun();
	bool NeedsRebuffering() { return mbNeedsRebuffering; }

	/////////////////////////////////////
	// Seek asked for by the game (start of play, SetElapsedTime). Waits for a refill in
	// progress, and makes the stream thread throw away buffers it decoded before the seek.
	// Call with the bound source locked.
	void SeekFromGame(float afWhere);

	/////////////////////////////////////
	// Refill done by the stream thread in three steps, so the source is not locked while
	// decoding (the game thread locks it every frame to set volume and position):
	// 1. BeginThreadedUpdate, source locked: takes the played buffers off the source and
	//    takes the decode lock. Returns false if there is nothing to do (no lock taken).
	// 2. DecodeThreaded, source not locked: fills the buffers. Then UnlockDecode.
	// 3. EndThreadedUpdate, source locked again, only if the source still plays this
	//    stream (compare GetAudioData() first, the stream may be gone by then).
	bool BeginThreadedUpdate(cOAL_Source* apSource, tBufferVec& avToFill, bool& abRebuffer,
							 bool& abUnderrun, bool& abLoop, unsigned int& alGeneration);
	void DecodeThreaded(const tBufferVec& avToFill, bool abLoop, tBufferVec& avFilled,
						double& afSeekMs, int& alReadErrors, int& alHoles);
	void UnlockDecode();
	void EndThreadedUpdate(cOAL_Source* apSource, unsigned int alGeneration, bool abRebuffer,
						   bool abUnderrun, const tBufferVec& avFilled);

protected:
	virtual bool Stream(cOAL_Buffer* apDestBuffer)=0;

	//Stops the bound source and waits for a refill in progress. Called first thing by the
	//destructors (the derived one too, before it closes the file).
	void StopAndWaitForDecode();

	cOAL_Source* mpBoundSource;

	static unsigned int mlBufferCount;
	static unsigned int mlBufferSize;

	bool mbNeedsRebuffering;

	double mfProcessedBuffersTime;

	char* mpPCMBuffer;
	ALuint* mvOALBufferIDs;

	//Held while the file is read or seeked (stream thread refill, game seeks, destruction)
	SDL_mutex* mpDecodeMutex;
	//New value on every game seek, from a counter shared by all streams
	unsigned int mlGeneration;
	//Counted by Stream() for the slow refill log
	int mlReadErrors;
	int mlHoles;
};

//-----------------------------------------------------------------------------------

#endif // _OAL_STREAM_H
