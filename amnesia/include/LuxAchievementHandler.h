#ifndef LUX_ACHIEVEMENT_HANDLER_H
#define LUX_ACHIEVEMENT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxTypes.h"

//-------------------------------------------------

class iLuxAchievementHandler : public iLuxUpdateable
{
public:
    iLuxAchievementHandler();
    virtual ~iLuxAchievementHandler();

    ///////////////////////////////
    // General
    void Reset();

    ////////////////
    // Achievements
    void CreateAchievement(int alID, const tString& asName);
    void UnlockAchievement(int alID);
    virtual void RegisterAchievements();
    
    tString GetAchievementName(int alID);
    virtual bool GetAchievementUnlocked(int alID);
    virtual tString GetAchievementDisplayName(int alID){return "";}
    virtual tString GetAchievementDescription(int alID){return "";}
    virtual int GetAchievementIcon(int alID){return -1;}

    virtual void OnUnlockedAchievement(int alID);

protected:
    std::map<int, tString> m_mapAchievements;
    std::set<int> m_setDebugActivatedAchievements;
    bool mbRegistered;
};

#endif // LUX_ACHIEVEMENT_HANDLER_H

