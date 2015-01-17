#include "SteamAchievements.h"
#include "ncbind.hpp"

extern std::string convertTtstrToUtf8String(ttstr &buf);
extern ttstr convertUtf8StringToTtstr(const std::string &buf);

SteamAchievements::SteamAchievements() :
 m_iAppID( 0),
 m_bInitialized( false),
 m_CallbackUserStatsReceived(this, &SteamAchievements::OnUserStatsReceived),
 m_CallbackUserStatsStored(this, &SteamAchievements::OnUserStatsStored),
 m_CallbackAchievementStored(this, &SteamAchievements::OnAchievementStored)
{
	m_iAppID = SteamUtils()->GetAppID();
	requestInitialize();
}


SteamAchievements::~SteamAchievements()
{
}

bool
SteamAchievements::requestInitialize()
{
	// Is Steam loaded? If not we can't get stats.
	if (NULL == SteamUserStats() || NULL == SteamUser()) {
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if (!SteamUser()->BLoggedOn()) {
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

int
SteamAchievements::getAchievementsCount()
{
	return SteamUserStats()->GetNumAchievements();
}


tTJSVariant
SteamAchievements::getAchievement(tTJSVariant n)
{
	tTJSVariant ret;
	if (m_bInitialized) {
		std::string name;
		getName(n, name);
		if (name != "") {
			const char *ach = name.c_str();
			bool achieved = false;
			uint32 unlockTime;
			if (SteamUserStats()->GetAchievementAndUnlockTime(ach, &achieved, &unlockTime)) {
				std::string name = SteamUserStats()->GetAchievementDisplayAttribute(ach, "name");
				std::string desc = SteamUserStats()->GetAchievementDisplayAttribute(ach, "desc");
				iTJSDispatch2 *dict = TJSCreateDictionaryObject();
				if (dict) {
					ncbPropAccessor obj(dict);
					obj.SetValue(L"ach", ach);
					obj.SetValue(L"achieved", achieved);
					obj.SetValue(L"unlockTime", unlockTime);
					obj.SetValue(L"name", convertUtf8StringToTtstr(name));
					obj.SetValue(L"desc", convertUtf8StringToTtstr(desc));
					ret = tTJSVariant(dict, dict);
					dict->Release();
				}
			}
		}
	}
	return ret;
}

bool
SteamAchievements::setAchievement(tTJSVariant n)
{
	// Have we received a call back from Steam yet?
	if (m_bInitialized)	{
		std::string name;
		getName(n, name);
		if (name != "") {
			SteamUserStats()->SetAchievement(name.c_str());
			return SteamUserStats()->StoreStats();
		}
	}
	// If not then we can't set achievements yet
	return false;
}

bool
SteamAchievements::clearAchievement(tTJSVariant n)
{
	// Have we received a call back from Steam yet?
	if (m_bInitialized)	{
		std::string name;
		getName(n, name);
		if (name != "") {
			return SteamUserStats()->ClearAchievement(name.c_str());
		}
	}
	// If not then we can't set achievements yet
	return false;
}


void
SteamAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
 // we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == pCallback->m_nGameID) {
		if (k_EResultOK == pCallback->m_eResult) {
			OutputDebugString("Received stats and achievements from Steam\n");
			m_bInitialized = true;
		}
	} else {
		char buffer[128];
		_snprintf(buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult);
		OutputDebugString(buffer);
	}
}

void
SteamAchievements::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == pCallback->m_nGameID) {
		if (k_EResultOK == pCallback->m_eResult) {
			OutputDebugString("Stored stats for Steam\n");
		}
	} else {
		char buffer[128];
		_snprintf(buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult);
		OutputDebugString(buffer);
	}
}

void
SteamAchievements::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == pCallback->m_nGameID) {
		OutputDebugString("Stored Achievement for Steam\n");
	}
}

void
SteamAchievements::getName(tTJSVariant n, std::string &name)
{
	if (n.Type() == tvtInteger) {
		tjs_int num = n;
		name = SteamUserStats()->GetAchievementName(num);
	} else if (n.Type() == tvtString) {
		ttstr str = n.GetString();
		name = convertTtstrToUtf8String(str);
	}
}

