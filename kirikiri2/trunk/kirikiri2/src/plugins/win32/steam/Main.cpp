#include <windows.h>
#include "tp_stub.h"
#include "ncbind.hpp"
#include <string>
#include <SteamAchievements.h>

#include <steam_api.h>
#pragma comment(lib,"steam_api.lib")

// ttstrをUTF8文字列へ変換
std::string
convertTtstrToUtf8String(ttstr &buf)
{
  int maxlen = buf.length() * 6 + 1;
  char *dat = new char[maxlen];
  int len = TVPWideCharToUtf8String(buf.c_str(), dat);
  std::string result(dat, len);
  delete[] dat;
  return result;
}

// std::stringをttstrに変換
ttstr
convertUtf8StringToTtstr(const char *buf, size_t length)
{
	tjs_uint maxlen = (tjs_uint)length * 2 + 1;
	tjs_char *dat = new tjs_char[maxlen];
	tjs_uint len = TVPUtf8ToWideCharString(buf, dat);
	ttstr result(dat, len);
	delete[] dat;
	return result;
}

ttstr
convertUtf8StringToTtstr(const std::string &buf)
{
	return convertUtf8StringToTtstr(buf.c_str(), buf.length());
}

extern void initStorage();
extern void doneStorage();

/**
 * Steam情報基礎クラス
 */
class Steam : public tTVPContinuousEventCallbackIntf {
public:
	// 初期化
	static void registerSteam() {
		instance = new Steam();
		instance->init();
		initStorage();
	}
	// 解除
	static void unregisterSteam() {
		doneStorage();
		delete instance;
	}
	// コンストラクタ用(常に例外)
	static tjs_error Factory(Steam **obj, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		TVPThrowExceptionMessage(TJSGetMessageMapMessage(TJS_W("TVPCannotCreateInstance")).c_str());
		return TJS_E_FAIL;
	}

	// コンストラクタ
	Steam() : inited(false), achieve(0) {
	}

	// デストラクタ
	virtual ~Steam() {
		if (achieve) {
			delete achieve;
		}
		if (inited){
			TVPRemoveContinuousEventHook(this);
			SteamAPI_Shutdown();
		}
	}
	// 初期化
	void init() {
		if ((inited = SteamAPI_Init())) {
			ISteamUtils *utils = SteamUtils();
			if (utils) {
				utils->SetOverlayNotificationPosition(k_EPositionTopLeft);
			}
			achieve = new SteamAchievements();
			TVPAddContinuousEventHook(this);
		} else {
			TVPThrowExceptionMessage(L"Steam must be running to play this game (SteamAPI_Init() failed)");
		}
	}

	// ---------------------------------------------------------
	// 実績
	// ---------------------------------------------------------

	static bool requestInitialize() {
		if (instance && instance->achieve) {
			return instance->achieve->requestInitialize();
		}
		return false;
	}

	static bool getInitialized() {
		if (instance && instance->achieve) {
			return instance->achieve->getInitialized();
		}
		return false;
	}
	
	static int getAchievementsCount() {
		if (instance && instance->achieve) {
			return instance->achieve->getAchievementsCount();
		}
		return 0;
	}
	
	static tTJSVariant getAchievement(tTJSVariant n) {
		if (instance && instance->achieve) {
			return instance->achieve->getAchievement(n);
		}
		return tTJSVariant();
	}

	static bool setAchievement(tTJSVariant n) {
		if (instance && instance->achieve) {
			return instance->achieve->setAchievement(n);
		}
		return false;
	}

	static bool clearAchievement(tTJSVariant n) {
		if (instance && instance->achieve) {
			return instance->achieve->clearAchievement(n);
		}
		return false;
	}

	// ---------------------------------------------------------
	// 情報
	// ---------------------------------------------------------
	
	// 現在の言語を取得
	static ttstr getLanguage() {
		ttstr ret;
		ISteamApps *app = SteamApps();
		if (app) {
			std::string lang = app->GetCurrentGameLanguage();
			ret = convertUtf8StringToTtstr(lang);
		}
		return ret;
	}

	// ---------------------------------------------------------
	// クラウド
	// ---------------------------------------------------------

	static bool getCloudEnabled() {
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			return storage->IsCloudEnabledForApp();
		}
		return false;
	}

	static void setCloudEnabled(bool enabled) {
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			storage->SetCloudEnabledForApp(enabled);
		}
	}

	static tTJSVariant getCloudQuota() {
		tTJSVariant ret;
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			int32 total;
			int32 available;
			if (storage->GetQuota(&total, &available)) {
				iTJSDispatch2 *dict = TJSCreateDictionaryObject();
				if (dict) {
					ncbPropAccessor obj(dict);
					obj.SetValue(L"total", total);
					obj.SetValue(L"available", available);
					ret = tTJSVariant(dict, dict);
					dict->Release();
				}
			}
		}
		return ret;
	}

	static int getCloudFileCount() {
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			return storage->GetFileCount();
		}
		return 0;
	}

	static tTJSVariant getCloudFileInfo(int n) {
		tTJSVariant ret;
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			int32 size;
			std::string name = storage->GetFileNameAndSize(n, &size);
			int64 time = storage->GetFileTimestamp(name.c_str());
			iTJSDispatch2 *dict = TJSCreateDictionaryObject();
			if (dict) {
				ncbPropAccessor obj(dict);
				obj.SetValue(L"filename", convertUtf8StringToTtstr(name));
				obj.SetValue(L"size", size);
				obj.SetValue(L"time", time);
				ret = tTJSVariant(dict, dict);
				dict->Release();
			}
		}
		return ret;
	}

	static bool deleteCloudFile(ttstr name) {
		bool ret = false;
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			ttstr n = name;
			n.ToLowerCase();
			std::string filename = convertTtstrToUtf8String(n);
			ret = storage->FileDelete(filename.c_str());
		}
		return ret;
	}
	

public:
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		SteamAPI_RunCallbacks();
	}

protected:
	static Steam *instance;

	bool inited;
	SteamAchievements *achieve;
};

Steam *Steam::instance = 0;

NCB_REGISTER_CLASS(Steam) {
	Factory(&Class::Factory);
	NCB_METHOD(getLanguage);

	NCB_METHOD(requestInitialize);
	NCB_PROPERTY_RO(initialized, getInitialized);
	NCB_PROPERTY_RO(achievementsCount, getAchievementsCount);
	NCB_METHOD(getAchievement);
	NCB_METHOD(setAchievement);
	NCB_METHOD(clearAchievement);

	NCB_PROPERTY(cloudEnabled, getCloudEnabled, setCloudEnabled);
	NCB_METHOD(getCloudQuota);
	NCB_PROPERTY_RO(cloudFileCount, getCloudFileCount);
	NCB_METHOD(getCloudFileInfo);
	NCB_METHOD(deleteCloudFile);
}

NCB_REGISTER_CALLBACK(PreRegist,  Steam::registerSteam, 0, registerSteam_0);
NCB_REGISTER_CALLBACK(PostRegist, 0, Steam::unregisterSteam, 0_unregisterSteam);
