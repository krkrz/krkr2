#include <windows.h>
#include "tp_stub.h"

// fps の制限値
static int fpsLimitValue = 60;

// プロパティ名
#define FPSLIMITNAME L"fpslimit"

/*
 * fps の制限値の参照用のプロパティ
 */
class tFpsLimitProp : public tTJSDispatch
{
public:
	tjs_error TJS_INTF_METHOD PropGet(tjs_uint32 flag,
									  const tjs_char * membername,
									  tjs_uint32 *hint,
									  tTJSVariant *result,
									  iTJSDispatch2 *objthis)	{
		if (result) {
			*result = fpsLimitValue;
		}
		return TJS_S_OK;
	}
	
	tjs_error TJS_INTF_METHOD PropSet(tjs_uint32 flag,
									  const tjs_char *membername,
									  tjs_uint32 *hint,
									  const tTJSVariant *param,
									  iTJSDispatch2 *objthis) {
		fpsLimitValue = *param;
		return TJS_S_OK;
	}
};


// 前回の呼び出し時刻
static tjs_uint64 prevTime;

// リミット処理
class CFPSLimit : public tTVPContinuousEventCallbackIntf {
public:
	CFPSLimit() {};
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		int diff = (int)((prevTime + 1000 / fpsLimitValue) - TVPGetTickCount());
		if (diff > 0) {
			Sleep(diff);
		}
		prevTime = TVPGetTickCount();
	}
};

static CFPSLimit limit;

//---------------------------------------------------------------------------
int WINAPI
DllEntryPoint(HINSTANCE /*hinst*/, unsigned long /*reason*/, void* /*lpReserved*/)
{
	return 1;
}

static tjs_int GlobalRefCountAtInit = 0;
extern "C" __declspec(dllexport) HRESULT __stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// 初期化
	prevTime = TVPGetTickCount();
	TVPAddContinuousEventHook(&limit);

	{
		// TJS のグローバルオブジェクトを取得する
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		
		// Layer クラスオブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("System"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			tTJSDispatch *method = new tFpsLimitProp();
			tTJSVariant var(method);
			dispatch->PropSet(TJS_MEMBERENSURE, FPSLIMITNAME, NULL, &var, dispatch);
			method->Release();
		}
		global->Release();
	}
			
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	return S_OK;
}


//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) HRESULT __stdcall V2Unlink()
{
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;

	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("System"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			dispatch->DeleteMember(0, FPSLIMITNAME, NULL, dispatch);
		}
		global->Release();
	}

	TVPRemoveContinuousEventHook(&limit);
	
	TVPUninitImportStub();

	return S_OK;
}
