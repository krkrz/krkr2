#include <windows.h>
#include <DispEx.h>
#include <stdio.h>

#define KRKRDISPWINDOWCLASS _T("TScrollBox")

// ATL
#if _MSC_VER == 1200
// Microsoft SDK のものとかちあうので排除
#define __IHTMLControlElement_INTERFACE_DEFINED__
#endif

#include "ncbind/ncbind.hpp"

#include <atlbase.h>
static CComModule _Module;
#include <atlwin.h>
#include <atlcom.h>
#include <atliface.h>
#define _ATL_DLL
#include <atlhost.h>
#include <ExDispID.h>

// ログ出力用
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
	TVPAddLog(msg);
	va_end(args);
}

//---------------------------------------------------------------------------

#include "IDispatchWrapper.hpp"

extern IDispatch *createEventSink(GUID diid, ITypeInfo *pTypeInfo, iTJSDispatch2 *receiver);
extern HRESULT findIID(IDispatch *pDispatch, const tjs_char *pitf, IID *piid, ITypeInfo **ppTypeInfo);

/*
 * WIN32OLE ネイティブインスタンス
 */
class WIN32OLE // ネイティブインスタンス
{
public:
	iTJSDispatch2 *objthis; //< 自分自身
	IDispatch *pDispatch; //< 保持してるインスタンス

protected:

	struct EventInfo {
		IID diid;
		DWORD cookie;
		EventInfo(REFIID diid, DWORD cookie) : diid(diid), cookie(cookie) {};
	};
	vector<EventInfo> events;

	/**
	 * イベント情報の消去
	 */
	void clearEvent() {
		if (pDispatch) {
			vector<EventInfo>::iterator i = events.begin();
			while (i != events.end()) {
				AtlUnadvise(pDispatch, i->diid, i->cookie);
				i++;
			}
			events.clear();
		}
	}

	/**
	 * 登録情報の消去
	 */
	void clear() {
		clearEvent();
		if (pDispatch) {
			pDispatch->Release();
			pDispatch = NULL;
		}
	}

public:
	/**
	 * コンストラクタ
	 * @param objthis TJS2インスタンス
	 * @param progIdorCLSID
	 */
	WIN32OLE(iTJSDispatch2 *objthis, const tjs_char *progIdOrCLSID) : objthis(objthis), pDispatch(NULL) {
		if (progIdOrCLSID) {
			HRESULT hr;
			CLSID   clsid;
			OLECHAR *oleName = SysAllocString(progIdOrCLSID);
			if (FAILED(hr = CLSIDFromProgID(oleName, &clsid))) {
				hr = CLSIDFromString(oleName, &clsid);
			}
			SysFreeString(oleName);
			if (SUCCEEDED(hr)) {
				// COM 接続してIDispatch を取得する
				/* get IDispatch interface */
				hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&pDispatch);
				if (!SUCCEEDED(hr)) {
					log(L"CoCreateInstance failed %ws", progIdOrCLSID);
				}
			} else {
				log(L"bad CLSID %ws", progIdOrCLSID);
			}
		}
		tTJSVariant name(TJS_W("missing"));
		objthis->ClassInstanceInfo(TJS_CII_SET_MISSING, 0, &name);
	}
	
	// オブジェクトが無効化されるときに呼ばれる
	virtual ~WIN32OLE()	{
		clear();
	}

	/**
	 * インスタンス生成ファクトリ
	 */
	static tjs_error factory(WIN32OLE **result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (result) {
			*result = new WIN32OLE(objthis, param[0]->GetString());
		}
		return S_OK;
	}

protected:

	/**
	 * メソッド実行
	 * メンバ名を直接指定
	 * @param wFlag 実行フラグ
	 * @param membername メンバ名
	 * @param result 結果
	 * @param numparams 引数の数
	 * @param param 引数
	 */
	tjs_error invoke(DWORD wFlags,
					 const tjs_char *membername,
					 tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param) {
		if (pDispatch) {
			return iTJSDispatch2Wrapper::Invoke(pDispatch,
												wFlags,
												membername,
												result,
												numparams,
												param);
		}
		return TJS_E_FAIL;
	}
	
	/**
	 * メソッド実行
	 * パラメータの１つ目がメソッド名
	 * @param wFlag 実行フラグ
	 * @param result 結果
	 * @param numparams 引数の数
	 * @param param 引数
	 */
	tjs_error invoke(DWORD wFlags,
					 tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param) {
		//log(L"native invoke %d", numparams);
		if (pDispatch) {
			if (numparams > 0) {
				if (param[0]->Type() == tvtString) {
					return iTJSDispatch2Wrapper::Invoke(pDispatch,
														wFlags,
														param[0]->GetString(),
														result,
														numparams - 1,
														param ? param + 1 : NULL);
				} else {
					return TJS_E_INVALIDPARAM;
				}
			} else {
				return TJS_E_BADPARAMCOUNT;
			}
		}
		return TJS_E_FAIL;
	}

	/**
	 * メソッド実行
	 */
	tjs_error missing(tTJSVariant *result, tjs_int numparams, tTJSVariant **param) {
		
		if (numparams < 3) {return TJS_E_BADPARAMCOUNT;};
		bool ret = false;
		const tjs_char *membername = param[1]->GetString();
		if ((int)*param[0]) {
			// put
			ret = TJS_SUCCEEDED(invoke(DISPATCH_PROPERTYPUT, membername, NULL, 1, &param[2]));
		} else {
			// get
			tTJSVariant result;
			tjs_error err;
			ret = TJS_SUCCEEDED(err = invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, membername, &result, 0, NULL));
			if (err == TJS_E_BADPARAMCOUNT) {
				result = new iTJSDispatch2WrapperForMethod(pDispatch, membername);
				ret = true;
			}
			if (ret) {
				iTJSDispatch2 *value = param[2]->AsObject();
				if (value) {
					value->PropSet(0, NULL, NULL, &result, NULL);
					value->Release();
				}
			}
		}
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}

public:
	static tjs_error invokeMethod(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, WIN32OLE *self) {
		return self->invoke(DISPATCH_PROPERTYGET|DISPATCH_METHOD, result, numparams, param);
	}
	
	static tjs_error setMethod(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, WIN32OLE *self) {
		return self->invoke(DISPATCH_PROPERTYPUT, result, numparams, param);
	}
	
	static tjs_error getMethod(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, WIN32OLE *self) {
		return self->invoke(DISPATCH_PROPERTYGET, result, numparams, param);
	}

	static tjs_error missingMethod(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, WIN32OLE *self) {
		return self->missing(result, numparams, param);
	}
	
protected:

	/**
	 * イベントを登録
	 */
	bool addEvent(const tjs_char *diidName, iTJSDispatch2 *receiver) {
		bool ret = false;
		IID diid;
		ITypeInfo *pTypeInfo;
		if (SUCCEEDED(findIID(pDispatch, diidName, &diid, &pTypeInfo))) {
			IDispatch *eventSink = createEventSink(diid, pTypeInfo, receiver);
			DWORD cookie;
			if (SUCCEEDED(AtlAdvise(pDispatch, eventSink, diid, &cookie))) {
				events.push_back(EventInfo(diid, cookie));
				ret = true;
			}
			eventSink->Release();
			if (pTypeInfo) {
				pTypeInfo->Release();
			}
		}
		return ret;
	}

public:

	/**
	 * イベント登録
	 */
	static tjs_error addEventMethod(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		WIN32OLE *self = ncbInstanceAdaptor<WIN32OLE>::GetNativeInstance(objthis);
		if (!self) {
			return TJS_E_NATIVECLASSCRASH;
		}
		bool success = false;
		const tjs_char *diidName = param[0]->GetString();
		if (numparams > 1) {
			iTJSDispatch2 *receiver = param[1]->AsObject();
			if (receiver) {
				success = self->addEvent(diidName, receiver);
				receiver->Release();
			}
		} else {
			success = self->addEvent(diidName, objthis);
		}
		if (!success) {
			log(L"イベント[%ws]の登録に失敗しました", diidName);
		}
		return TJS_S_OK;
	}
	
	/**
	 * 定数の取得
	 */
protected:

	/**
	 * 定数の取得
	 * @param pTypeInfo TYPEINFO
	 * @param target 格納先
	 */
	void getConstant(ITypeInfo *pTypeInfo, iTJSDispatch2 *target) {
		// 個数情報
		TYPEATTR  *pTypeAttr = NULL;
		if (SUCCEEDED(pTypeInfo->GetTypeAttr(&pTypeAttr))) {
			for (int i=0; i<pTypeAttr->cVars; i++) {
				VARDESC *pVarDesc = NULL;
				if (SUCCEEDED(pTypeInfo->GetVarDesc(i, &pVarDesc))) {
					if (pVarDesc->varkind == VAR_CONST &&
						!(pVarDesc->wVarFlags & (VARFLAG_FHIDDEN | VARFLAG_FRESTRICTED | VARFLAG_FNONBROWSABLE))) {
						BSTR bstr = NULL;
						unsigned int len;
						if (SUCCEEDED(pTypeInfo->GetNames(pVarDesc->memid, &bstr, 1, &len)) && len >= 0 && bstr) {
							//log(L"const:%s", bstr);
							tTJSVariant result;
							IDispatchWrapper::storeVariant(result, *(pVarDesc->lpvarValue));
							target->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP,
											bstr,
											NULL,
											&result,
											target
											);
							SysFreeString(bstr);
						}
					}
					pTypeInfo->ReleaseVarDesc(pVarDesc);
				}
			}
			pTypeInfo->ReleaseTypeAttr(pTypeAttr);
		}
	}

	/**
	 * 定数の取得
	 * @param pTypeLib TYPELIB
	 * @param target 格納先
	 */
	void getConstant(ITypeLib *pTypeLib, iTJSDispatch2 *target) {
		unsigned int count = pTypeLib->GetTypeInfoCount();
		for (unsigned int i=0; i<count; i++) {
			ITypeInfo *pTypeInfo = NULL;
			if (SUCCEEDED(pTypeLib->GetTypeInfo(i, &pTypeInfo))) {
				getConstant(pTypeInfo, target);
				pTypeInfo->Release();
			}
		}
	}

	/**
	 * 定数の取得
	 * @param target 格納先
	 */
	void getConstant(iTJSDispatch2 *target) {
		if (pDispatch) {
			if (target) {
				ITypeInfo *pTypeInfo = NULL;
				if (SUCCEEDED(pDispatch->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo))) {
					unsigned int index = 0;
					ITypeLib *pTypeLib = NULL;
					if (SUCCEEDED(pTypeInfo->GetContainingTypeLib(&pTypeLib, &index))) {
						getConstant(pTypeLib, target);
						pTypeLib->Release();
					}
					pTypeInfo->Release();
				}
			}
		}
	}

public:
	/**
	 * メソッド実行
	 */
	static tjs_error getConstantMethod(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		WIN32OLE *self = ncbInstanceAdaptor<WIN32OLE>::GetNativeInstance(objthis);
		if (!self) {
			return TJS_E_NATIVECLASSCRASH;
		}
		if (numparams > 0) {
			iTJSDispatch2 *store = param[0]->AsObject();
			if (store) {
				self->getConstant(store);
				store->Release();
			}
		} else {
			self->getConstant(objthis);
		}
		return TJS_S_OK;
	}
};

//---------------------------------------------------------------------------

extern IDocHostUIHandlerDispatch *createExternalUI();

/*
 * ActiveX ネイティブインスタンス
 */
class ActiveX : public WIN32OLE, public CWindowImpl<ActiveX, CAxWindow>
{
protected:
	iTJSDispatch2 *window;  //< オブジェクト情報の参照
	ttstr progId;
	int left;
	int top;
	int width;
	int height;

	IDocHostUIHandlerDispatch *externalUI;

	// イベント処理
	static bool __stdcall messageHandler(void *userdata, tTVPWindowMessage *Message);

	// ユーザメッセージレシーバの登録/解除
	void setReceiver(tTVPWindowMessageReceiver receiver, bool enable) {
		tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
		tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
		tTJSVariant userdata = (tTVInteger)(tjs_int)this;
		tTJSVariant *p[] = {&mode, &proc, &userdata};
		if (window->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, window) != TJS_S_OK) {
			if (enable) {
				TVPThrowExceptionMessage(L"can't regist user message receiver");
			}
		}
	}

	/**
	 * ウインドウを生成
	 * @param krkr 吉里吉里のウインドウ
	 * @return 成功したら true
	 */
	void createWindow(HWND krkr) {
		RECT rect;
		rect.left   = left;
		rect.top    = top;
		rect.right  = left + width;
		rect.bottom = top  + height;
		
		HRESULT hr;
		HWND parent;
		if (krkr && (parent = FindWindowEx(krkr, NULL, KRKRDISPWINDOWCLASS, NULL))) {
			if (width == -1 || height == -1) {
				::GetClientRect(parent, &rect);
			}
			Create(parent, rect, NULL, WS_CHILD|WS_CLIPCHILDREN);
		} else {
			Create(0, rect, NULL, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN);
		}
		if (m_hWnd) {
			// コントロール生成
			OLECHAR *oleName = SysAllocString(progId.c_str());
			hr = CreateControl(oleName);
			if (SUCCEEDED(hr)) {
				// 外部ディスパッチャを登録
				{
					IDispatchEx *dispatchEx = new IDispatchWrapper(objthis);
					SetExternalDispatch(dispatchEx);
					dispatchEx->Release();
				}
				// 外部UIハンドラを登録
				if (externalUI) {
					SetExternalUIHandler(externalUI);
				}
				// IDispatch取得
				hr = QueryControl(IID_IDispatch, (void**)&pDispatch);
			} else {
				log(L"CreateControl failed %ws", progId.c_str());
			}
			SysFreeString(oleName);
		}
		// XXX 生成完了後処理呼び出し
		// if (SUCCEEDED(hr)) { onCreate(); }
	}
		
	/**
	 * 情報を破棄
	 */
	void clear() {
		// XXX 破棄前処理呼び出し
		// if (m_hWnd) {onDestroy();};
		WIN32OLE::clear();
		if (m_hWnd) {
			DestroyWindow();
			m_hWnd = 0;
		}
	}

	// 配置調整用
	void _setPos() {
		if (m_hWnd) {
			SetWindowPos(0, left, top, width, height, 0);
		}
	}
public:

BEGIN_MSG_MAP(ActiveX)
END_MSG_MAP()
	
	/**
	 * コンストラクタ
	 * @param objthis TJS2インスタンス
	 * @param numparams パラメータ数
	 * @param param パラメータ配列
	 */
	ActiveX(iTJSDispatch2 *objthis, tjs_int numparams, tTJSVariant **param) : WIN32OLE(objthis, NULL), window(NULL), externalUI(NULL), left(0), top(0), width(-1), height(-1) {
		
		progId = param[0]->GetString();
		
		if (numparams >= 6) {
			left   = *param[2];
			top    = *param[3];
			width  = *param[4];
			height = *param[5];
		}
		
		HWND handle = 0;
		if (numparams >= 2 && param[1]->Type() == tvtObject) {
			// ウインドウが指定
			iTJSDispatch2 *win = param[1]->AsObjectNoAddRef();
			if (win->IsInstanceOf(0, NULL, NULL, L"Window", win) == TJS_S_TRUE) {
				window = win;
				window->AddRef();
				setReceiver(messageHandler, true);
				tTJSVariant hwnd;
				if (win->PropGet(0, TJS_W("HWND"), NULL, &hwnd, win) == TJS_S_OK) {
					HWND handle = (HWND)(int)hwnd;
					if (handle) {
						// 既に生成済み
						createWindow(handle);
					}
				}
			} else {
				TVPThrowExceptionMessage(L"must set window object");
			}
		} else {
			// 独立ウインドウ
			createWindow(0);
		}
	}

	/**
	 * デストラクタ
	 */
	~ActiveX() {
		clear();
		if (externalUI) {
			delete externalUI;
			externalUI = NULL;
		}
		if (window) {
			setReceiver(messageHandler, false);
			window->Release();
			window = NULL;
		}
	}

	/**
	 * インスタンス生成ファクトリ
	 * @param name  識別名
	 * @param left  表示位置
	 * @param top   表示位置
	 * @param width  表示サイズ
	 * @param height 高さ指定
	 */
	static tjs_error factory(ActiveX **result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		// パラメータは最低ひとつ必須
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (result) {
			*result = new ActiveX(objthis, numparams, param);
		}
		return TJS_S_OK;
	}

	/**
	 * 外部拡張ハンドラの登録
	 * ※事実上 IE 専用の処理
	 */
	void setExternalUI() {
		if (externalUI == NULL) {
			externalUI = createExternalUI();
			if (m_hWnd) {
				SetExternalUIHandler(externalUI);
			}
		}
	}

	void setVisible(bool visible) {
		if (m_hWnd) {
			if (visible) {
				_setPos();
			}
			ShowWindow(visible);
		}
	}

	bool isValidWindow() {
		return m_hWnd != 0;
	}
	
	bool getVisible() {
		return m_hWnd && IsWindowVisible();
	}

	void setLeft(int l) {
		left = l;
		_setPos();
	}

	int getLeft() {
		return left;
	}

	void setTop(int t) {
		top = t;
		_setPos();
	}

	int getTop() {
		return top;
	}
	
	void setWidth(int w) {
		width = w;
		_setPos();
	}

	int getWidth() {
		return width;
	}

	void setHeight(int h) {
		height = h;
		_setPos();
	}

	int getHeight() {
		return height;
	}
	
	/**
	 * 窓場所指定
	 */	
	void setPos(int l, int t) {
		left = l;
		top  = t;
		_setPos();
	}

	/**
	 * 窓サイズ指定
	 */	
	void setSize(int w, int h) {
		width = w;
		height = h;
		_setPos();
	}
};

// イベント処理
bool __stdcall
ActiveX::messageHandler(void *userdata, tTVPWindowMessage *Message)
{
	ActiveX *self = (ActiveX*)userdata;
	switch (Message->Msg) {
	case TVP_WM_DETACH:
		self->clear();
		break;
	case TVP_WM_ATTACH:
		self->createWindow((HWND)Message->LParam);
		break;
	default:
		break;
	}
	return false;
}



NCB_REGISTER_CLASS(WIN32OLE) {
	Factory(&ClassT::factory);
	RawCallback("invoke",  &ClassT::invokeMethod,  0);
	RawCallback("set",     &ClassT::setMethod,     0);
	RawCallback("get",     &ClassT::getMethod,     0);
	RawCallback("missing", &ClassT::missingMethod, 0);
	RawCallback("addEvent", &ClassT::addEventMethod, 0);
	RawCallback("getConstant", &ClassT::getConstantMethod, 0);
};

NCB_REGISTER_CLASS(ActiveX) {
	Factory(&ClassT::factory);
	RawCallback("invoke",  &ClassT::invokeMethod,  0);
	RawCallback("set",     &ClassT::setMethod,     0);
	RawCallback("get",     &ClassT::getMethod,     0);
	RawCallback("missing", &ClassT::missingMethod, 0);
	RawCallback("addEvent", &ClassT::addEventMethod, 0);
	RawCallback("getConstant", &ClassT::getConstantMethod, 0);
	NCB_METHOD(setExternalUI);
	NCB_METHOD(setPos);
	NCB_METHOD(setSize);
	NCB_PROPERTY_RO(isValidWindow, isValidWindow);
	NCB_PROPERTY(visible, getVisible, setVisible);
	NCB_PROPERTY(left, getLeft, setLeft);
	NCB_PROPERTY(top, getTop, setTop);
	NCB_PROPERTY(width, getWidth, setWidth);
	NCB_PROPERTY(height, getHeight, setHeight);
};

class ScriptsOle {

public
	ScriptsOle() {};

	static tjs_error TJS_INTF_METHOD createOleClass(tTJSVariant *result,
													tjs_int numparams,
													tTJSVariant **param,
													iTJSDispatch2 *objthis) {
		if (numparams <= 1) return TJS_E_BADPARAMCOUNT;
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD createActiveXClass(tTJSVariant *result,
														tjs_int numparams,
														tTJSVariant **param,
														iTJSDispatch2 *objthis) {
		if (numparams <= 1) return TJS_E_BADPARAMCOUNT;
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(ScriptsOle, Scripts) {
	RawCallback("createOleClass", &Class::createOleClass, TJS_STATICMEMBER);
	RawCallback("createActiveXClass", &Class::createActiveXClass, TJS_STATICMEMBER);
};

//---------------------------------------------------------------------------

static BOOL gOLEInitialized = false;

/**
 * 登録処理前
 */
static void PreRegistCallback()
{
	if (!gOLEInitialized) {
		if (SUCCEEDED(OleInitialize(NULL))) {
			gOLEInitialized = true;
		} else {
			log(L"OLE 初期化失敗");
		}
	}
	
	// ATL関連初期化
	_Module.Init(NULL, NULL);
	AtlAxWinInit();
}

/**
 * 開放処理後
 */
static void PostUnregistCallback()
{
	// ATL 終了
	_Module.Term();

	if (gOLEInitialized) {
		OleUninitialize();
		gOLEInitialized = false;
	}
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
