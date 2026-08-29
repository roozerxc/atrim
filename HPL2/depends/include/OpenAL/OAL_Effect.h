#ifndef _OAL_EFFECT_H
#define _OAL_EFFECT_H

#include "OpenAL/OAL_LowLevelObject.h"

#include <SDL/SDL.h>

class cOAL_Effect : public iOAL_LowLevelObject
{
public:
	cOAL_Effect();
	~cOAL_Effect();

	bool CreateLowLevelID();
	bool DestroyLowLevelID();

	void SaveObjectState(){}
	void RestoreObjectState(){}

	bool IsValidObject();

	inline ALuint GetEffectID() { return mlEffectId; }

	inline bool	NeedsUpdate () { return mbNeedsUpdate; }
	inline void SetUpdated() { mbNeedsUpdate = false; }

	void Lock();
	void Unlock();
    
protected:
	ALuint	mlEffectId;
	bool	mbNeedsUpdate;

	SDL_mutex* mpMutex;
};

#endif	// _OAL_EFFECT_H


