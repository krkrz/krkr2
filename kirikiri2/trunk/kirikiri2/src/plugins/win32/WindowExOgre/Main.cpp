#include <windows.h>
#include <list>
#include <map>
using namespace std;
#include "tp_stub.h"
#include <stdio.h>

#include "Ogre.h"
#include "OgreConfigFile.h"
using namespace Ogre;

class OgreInfo : public tTVPContinuousEventCallbackIntf
{

public:
	Ogre::Root *root;

	/**
	 * コンストラクタ
	 */
	OgreInfo() {
		root = new Ogre::Root();

		// コンフィグ初期化
		ConfigFile cf;
		cf.load("resources.cfg");
		// Go through all sections & settings in the file
		ConfigFile::SectionIterator seci = cf.getSectionIterator();
		
		String secName, typeName, archName;
		while (seci.hasMoreElements()) {
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i) {
				typeName = i->first;
					archName = i->second;
				ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
			}
		}
	}

	/**
	 * デストラクタ
	 */
	~OgreInfo() {
		TVPRemoveContinuousEventHook(this);
		delete root;
	}

public:

	bool config() {
		if (root->showConfigDialog()) {
			root->initialise(false);
			return true;
		}
		return false;
	}

	void start() {
		TVPRemoveContinuousEventHook(this);
		TVPAddContinuousEventHook(this);
	}
	
	/**
	 * Continuous コールバック
	 * 吉里吉里が暇なときに常に呼ばれる
	 * 塗り直し処理
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		root->renderOneFrame();
	}
};

OgreInfo *ogreInfo = NULL;

/**
 * ログ出力用
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
	TVPAddLog(msg);
	va_end(args);
}

#include "WindowExBase.h"

/*
 * ウインドウに直結した OGRE 情報を保持するためのネイティブインスタンス
 */
class NI_OgreBase : public tTJSNativeInstance
{
	
protected:
	/// ウインドウオブジェクト
	iTJSDispatch2 * _windowobj;

	/// OGRE 関係
	string windowName;
	RenderWindow* _renderWindow;
	SceneManager* _sceneManager;
	
private:

	/**
	 * ウインドウの解除
	 */
	void detach();

	/*
	 * ウインドウの設定
	 */
	void attach();

	/**
	 * メッセージのレシーバ
	 */
	static bool __stdcall messageReceiver(void *userdata, tTVPWindowMessage *Message);

	
public:
	/**
	 * コンストラクタ
	 */
	NI_OgreBase(iTJSDispatch2 *windowobj);

	/**
	 * デストラクタ
	 */
	~NI_OgreBase();

public:

	/**
	 * 初期化処理
	 */
	void init() {

		/* nothing to do */

		// XXX テスト用にオブジェクトを配置してみる
		_sceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
		Entity *ent = _sceneManager->createEntity("head", "ogrehead.mesh");
		_sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

        // Green nimbus around Ogre
        ParticleSystem* pSys1 = _sceneManager->createParticleSystem("Nimbus", 
            "Examples/GreenyNimbus");
        _sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(pSys1);

        // Create a rainstorm 
        ParticleSystem* pSys4 = _sceneManager->createParticleSystem("rain", 
            "Examples/Rain");
        SceneNode* rNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
        rNode->translate(0,1000,0);
        rNode->attachObject(pSys4);
        // Fast-forward the rain so it looks more natural
        pSys4->fastForward(5);

        // Aureola around Ogre perpendicular to the ground
        ParticleSystem* pSys5 = _sceneManager->createParticleSystem("Aureola", 
            "Examples/Aureola");
        _sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(pSys5);

		// Set nonvisible timeout
		ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);
	}

};

bool __stdcall
NI_OgreBase::messageReceiver(void *userdata, tTVPWindowMessage *Message)
{
	// この関数には(興味があるないに関わらず)非常に大量のメッセージが通過するため
	// 関係ないメッセージについてはパフォーマンスをあまり低下させないような実装が望まれる
	
	switch(Message->Msg) {
	case TVP_WM_DETACH:
		// ウィンドウハンドルが作り直されるか、あるいは
		// ウィンドウが破棄される直前にこのメッセージが来る。
		// 吉里吉里の Window クラスのウィンドウの子ウィンドウとして
		// ウィンドウを貼り付けた場合はこのタイミングでウィンドウを
		// 取り外さなければならない。
		// WPARAM は 1 ならばウィンドウが破棄されるとき、0 ならば
		// ウィンドウが作り直されるときである。
		// TVP_WM_DETACH と TVP_WM_ATTACH の間にも
		// メッセージが到着することがあるが無視すべき。
		if(Message->WParam == 1)
			TVPAddLog(TJS_W("TVP_WM_DETACH: closing"));
		else if(Message->WParam == 0)
			TVPAddLog(TJS_W("TVP_WM_DETACH: rebuilding"));
		((NI_OgreBase*)userdata)->detach();
		break;
		
	case TVP_WM_ATTACH:
		// ウィンドウが作り直された後にこのメッセージが来る。
		// LPARAM には 新しいウィンドウハンドルが入っている。
		// 子ウィンドウをまた取り付ける場合はこのタイミングでまた取り付ける
		// ことができる。
		TVPAddLog(TJS_W("TVP_WM_ATTACH: new window handle ") +
			TJSInt32ToHex(Message->LParam, 8));
		((NI_OgreBase*)userdata)->attach();
		break;

	default:
		break;
	}
	return false;
}

/**
 * ウインドウの解除
 */
void
NI_OgreBase::detach()
{
	if (_renderWindow) {
		_sceneManager->destroyAllCameras();
		_renderWindow->removeAllViewports();
		_renderWindow->removeAllListeners();
		ogreInfo->root->getRenderSystem()->destroyRenderWindow(windowName);
		_renderWindow = NULL;
	}
}

/**
 * ウインドウの再設定
 */
void
NI_OgreBase::attach()
{
	NI_WindowExBase *base;
	if ((base = NI_WindowExBase::getNative(_windowobj))) {
		
		// パラメータ取得
		base->reset(_windowobj);
		
		// ウインドウ生成
		{
			// ハンドルを文字列化
			char hwndName[100];
			snprintf(hwndName, sizeof hwndName, "%d", base->_hwnd);
			
			// 初期化パラメータ
			NameValuePairList params;
			params["parentWindowHandle"] = hwndName;
			params["left"] = "0";
			params["top"] = "0";

			windowName = "window";
			windowName += hwndName;
			
			// ウインドウ生成
			_renderWindow = ogreInfo->root->createRenderWindow(windowName,
															   base->_innerWidth,
															   base->_innerHeight,
															   false,
															   &params);
			ogreInfo->start();
		}

		// XXX もっかい取得パラメータ取得
		base->reset(_windowobj);
	}

	Camera *camera = _sceneManager->createCamera("Player");
	camera->setPosition(Vector3(0,0,500));
	camera->lookAt(Vector3(0,0,-300));
	camera->setNearClipDistance(5);
	
	Viewport* vp = _renderWindow->addViewport(camera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// リソース初期化
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

/**
 * コンストラクタ
 */
NI_OgreBase::NI_OgreBase(iTJSDispatch2 *windowobj)
{
	_windowobj = windowobj; // no addRef
	_renderWindow = NULL;
	
	// registerMessageReceiver を呼ぶ(レシーバ登録)
	tTJSVariant mode, proc, userdata;
	tTJSVariant *p[3] = {&mode, &proc, &userdata};
	mode = (tTVInteger)(tjs_int)wrmRegister;
	proc = (tTVInteger)reinterpret_cast<tjs_int>(messageReceiver);
	userdata = (tTVInteger)(tjs_int)this;
	_windowobj->FuncCall(0, TJS_W("registerMessageReceiver"), NULL, NULL, 3, p, _windowobj);
	
	// シーンマネージャ
	_sceneManager = ogreInfo->root->createSceneManager(ST_GENERIC, "ExampleSMInstance");

	// ウインドウ生成処理
	attach();
}

/**
 * デストラクタ
 */
NI_OgreBase::~NI_OgreBase()
{
	// registerMessageReceiver を呼ぶ(登録解除)
	tTJSVariant mode, proc, userdata;
	tTJSVariant *p[3] = {&mode, &proc, &userdata};
	mode = (tTVInteger)(tjs_int)wrmUnregister;
	proc = (tTVInteger)reinterpret_cast<tjs_int>(messageReceiver);
	userdata = (tTVInteger)(tjs_int)0;
	_windowobj->FuncCall(0, TJS_W("registerMessageReceiver"), NULL, NULL, 3, p, _windowobj);
	detach();
}

// クラスID
static tjs_int32 ClassID_OgreBase = -1;

void
addMember(iTJSDispatch2 *dispatch, const tjs_char *memberName, iTJSDispatch2 *member)
{
	tTJSVariant var = tTJSVariant(member);
	member->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		memberName, // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&var, // 登録する値
		dispatch // コンテキスト
		);
}

static void
delMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	dispatch->DeleteMember(
		0, // フラグ ( 0 でよい )
		name, // メンバ名
		NULL, // ヒント
		dispatch // コンテキスト
		);
}

/**
 * ウインドウオブジェクトから OGRE 用ネイティブインスタンスを取得する。
 * ネイティブインスタンスを持ってない場合は自動的に割り当てる
 * @param windowobj ウインドウオブジェクト
 * @return OGRE 用ネイティブインスタンス。取得失敗したら NULL
 */
static NI_OgreBase *
getOgreBaseNative(iTJSDispatch2 *windowobj)
{
	if (!windowobj) return NULL;
	// 情報取得
	NI_OgreBase *_this;
	if (TJS_FAILED(windowobj->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
													ClassID_OgreBase, (iTJSNativeInstance**)&_this))) {
		// ウインドウ拡張部生成
		if (NI_WindowExBase::getNative(windowobj) == NULL) {
			return NULL;
		}
		// OGRE 拡張部登録
		_this = new NI_OgreBase(windowobj);
		if (TJS_FAILED(windowobj->NativeInstanceSupport(TJS_NIS_REGISTER,
														ClassID_OgreBase, (iTJSNativeInstance **)&_this))) {
			delete _this;
			return NULL;
		}
	}
	return _this;
}


#define FUNC(funcname,pnum) \
class funcname : public tTJSDispatch\
{\
protected:\
public:\
	tjs_error TJS_INTF_METHOD FuncCall(\
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,\
		tTJSVariant *result,\
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {\
		NI_OgreBase *_this;\
		if ((_this = getOgreBaseNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;\
		if (numparams < pnum) return TJS_E_BADPARAMCOUNT;

#define FUNCEND \
		return TJS_S_OK;\
	}\
};

#define PROP(funcname) \
class funcname : public tTJSDispatch\
{\
protected:\
public:

#define GETTER \
	tjs_error TJS_INTF_METHOD PropGet(\
		tjs_uint32 flag,\
		const tjs_char * membername,\
		tjs_uint32 *hint,\
		tTJSVariant *result,\
		iTJSDispatch2 *objthis)	{\
		NI_OgreBase *_this;\
		if ((_this = getOgreBaseNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;\
		

#define SETTER \
        return TJS_S_OK;\
    }\
	tjs_error TJS_INTF_METHOD PropSet(\
		tjs_uint32 flag,\
		const tjs_char *membername,\
		tjs_uint32 *hint,\
		const tTJSVariant *param,\
		iTJSDispatch2 *objthis)	{\
		NI_OgreBase *_this;\
		if ((_this = getOgreBaseNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
#define PROPEND \
		return TJS_S_OK;\
	}\
};

// プロパティやメソッドの登録は以下で行う
// _this がネイティブオブジェクトになっている
FUNC(tInitFunction,0)
	_this->init();
FUNCEND

#if 0
// プロパティ用
PROP(tXXXProp)
GETTER
	*result = _this->getXXX();
SETTER
	_this->setXXX(*param);
FUNCEND
#endif

//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;

/**
 * プラグイン初期化処理
 */
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// OGRE の基本情報
	ogreInfo = new OgreInfo();
	if (!ogreInfo->config()) {
		delete ogreInfo;
		ogreInfo = NULL;
		TVPThrowExceptionMessage(TJS_W("can't init OGRE."));
	}
	
	// クラスオブジェクトチェック
	if ((NI_WindowExBase::classId = TJSFindNativeClassID(L"WindowExBase")) <= 0) {
		NI_WindowExBase::classId = TJSRegisterNativeClass(L"WindowExBase");
	}
	
	// クラスオブジェクト登録
	ClassID_OgreBase = TJSRegisterNativeClass(TJS_W("OgreBase"));

	{
		// Window クラスオブジェクトを取得
		tTJSVariant winScripts;
		TVPExecuteExpression(TJS_W("Window"), &winScripts);
		iTJSDispatch2 *window = winScripts.AsObjectNoAddRef();
		
		// Layer クラスオブジェクトを取得
		tTJSVariant layerScripts;
		TVPExecuteExpression(TJS_W("Layer"), &layerScripts);
		iTJSDispatch2 *layer = layerScripts.AsObjectNoAddRef();
		
		if (window && layer) {
			// プロパティ初期化
			NI_WindowExBase::init(window, layer);
			// メンバ登録
			addMember(window, L"initOGRE", new tInitFunction());
		}
	}
	
	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 *global = TVPGetScriptDispatch();
	if (global) {
		global->Release();
	}
			
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	return S_OK;
}

/**
 * プラグイン解放処理
 */
extern "C" HRESULT _stdcall V2Unlink()
{
	if (ogreInfo) {

		if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;

		{
			// Window クラスオブジェクトを取得
			tTJSVariant winScripts;
			TVPExecuteExpression(TJS_W("Window"), &winScripts);
			iTJSDispatch2 *window = winScripts.AsObjectNoAddRef();
			// メンバ削除
			delMember(window, L"initOGRE");
		}
		
		// Window拡張基本プロパティ開放
		NI_WindowExBase::unInit();
		
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		if (global) {
			global->Release();
		}

		delete ogreInfo;
	}
	
	TVPUninitImportStub();

	return S_OK;
}
