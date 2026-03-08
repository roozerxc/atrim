#ifndef HPL_SOUND_TYPES_H
#define HPL_SOUND_TYPES_H


namespace hpl
{

//---------------------------------------

enum eSoundEntityType
{
    eSoundEntityType_Main,
    eSoundEntityType_Start,
    eSoundEntityType_Stop,
    eSoundEntityType_LastEnum
};

//---------------------------------------

class iSoundEntryCallback
{
public:
    virtual void OnPriorityRelease()=0;
};

//---------------------------------------
};

#endif // HPL_SOUND_TYPES_H
