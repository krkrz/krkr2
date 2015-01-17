#ifndef __STEAMACHEIVEMENT_H__
#define __STEAMACHIEVEMENT_H__

#include <string>
#include <windows.h>
#include <steam_api.h>
#include "tp_stub.h"

class SteamAchievements
{
public:
	SteamAchievements();
	~SteamAchievements();

	bool requestInitialize();

	bool getInitialized() { return m_bInitialized; }
	
	int getAchievementsCount();
	
	tTJSVariant getAchievement(tTJSVariant n);
	
	bool setAchievement(tTJSVariant n);
	bool clearAchievement(tTJSVariant n);

	STEAM_CALLBACK( SteamAchievements, OnUserStatsReceived, UserStatsReceived_t, 
		m_CallbackUserStatsReceived );
	STEAM_CALLBACK( SteamAchievements, OnUserStatsStored, UserStatsStored_t, 
		m_CallbackUserStatsStored );
	STEAM_CALLBACK( SteamAchievements, OnAchievementStored, 
		UserAchievementStored_t, m_CallbackAchievementStored );

private:
	int64 m_iAppID; // Our current AppID
	bool m_bInitialized; // Have we called Request stats and received the callback?
	void getName(tTJSVariant n, std::string &name);
};

#endif
