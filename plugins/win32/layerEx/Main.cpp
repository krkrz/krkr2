#include <stdio.h>
#include <windows.h>
#include "tp_stub.h"

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

#include "LayerExImage.h"

// クラスID
static tjs_int32 ClassID_LayerExImage = -1;

/**
 * レイヤオブジェクトからレイヤ拡張ネイティブインスタンスを取得する。
 * ネイティブインスタンスを持ってない場合は自動的に割り当てる
 * @param objthis レイヤオブジェクト
 * @return Layer拡張書き込み用ネイティブインスタンス。取得失敗したら NULL
 */
NI_LayerExImage *
getLayerExImageNative(iTJSDispatch2 *layerobj)
{
	if (!layerobj) return NULL;

	NI_LayerExImage *_this;
	if (TJS_FAILED(layerobj->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
												   ClassID_LayerExImage, (iTJSNativeInstance**)&_this))) {
		_this = new NI_LayerExImage(layerobj);
		if (TJS_FAILED(layerobj->NativeInstanceSupport(TJS_NIS_REGISTER,
													   ClassID_LayerExImage, (iTJSNativeInstance **)&_this))) {
			return NULL;
		}
	}
	_this->reset(layerobj);
	return _this;
}

/**
 * 明度とコントラスト
 * @param brightness 明度 -255 ～ 255, 負数の場合は暗くなる
 * @param contrast コントラスト -100 ～100, 0 の場合変化しない
 */
class tLightFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		if (numparams < 1) return TJS_E_BADPARAMCOUNT;

		NI_LayerExImage *image;
		if ((image = getLayerExImageNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		image->light(*param[0], numparams > 1 ? (int)*param[1] : 0);
		image->redraw(objthis);
		
		return TJS_S_OK;
	}
};

/**
 * 色相と彩度
 * @param hue 色相
 * @param sat 彩度
 * @param blend ブレンド 0 (効果なし) ～ 1 (full effect)
 */
class tColorizeFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		if (numparams < 2) return TJS_E_BADPARAMCOUNT;

		NI_LayerExImage *image;
		if ((image = getLayerExImageNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		image->colorize(*param[0], *param[1], numparams > 2 ? *param[2] : 1.0f);
		image->redraw(objthis);
		
		return TJS_S_OK;
	}
};

/**
 * ノイズ追加
 * @param level ノイズレベル 0 (no noise) ～ 255 (lot of noise).
 */
class tNoiseFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_LayerExImage *image;
		if ((image = getLayerExImageNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		image->noise(*param[0]);
		image->redraw(objthis);
		
		return TJS_S_OK;
	}
};


//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

static void
addMethod(iTJSDispatch2 *dispatch, const tjs_char *methodName, tTJSDispatch *method)
{
	tTJSVariant var = tTJSVariant(method);
	method->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		methodName, // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&var, // 登録する値
		dispatch // コンテキスト
		);
}

static void
delMethod(iTJSDispatch2 *dispatch, const tjs_char *methodName)
{
	dispatch->DeleteMember(
		0, // フラグ ( 0 でよい )
		methodName, // メンバ名
		NULL, // ヒント
		dispatch // コンテキスト
		);
}

static void
addClass(iTJSDispatch2 *dispatch, const tjs_char *className, iTJSDispatch2 *classObj)
{
	tTJSVariant var = tTJSVariant(classObj);
	classObj->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		className, // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&var, // 登録する値
		dispatch // コンテキスト
		);
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// クラスオブジェクト登録
	ClassID_LayerExImage = TJSRegisterNativeClass(TJS_W("LayerExImage"));

	{
		// TJS のグローバルオブジェクトを取得する
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		
		// Layer クラスオブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Layer"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			addMethod(dispatch, L"light",  new tLightFunction());
			addMethod(dispatch, L"colorize",  new tColorizeFunction());
			addMethod(dispatch, L"noise",  new tNoiseFunction());
		}
		
		global->Release();
	}
			
	// この時点での TVPPluginGlobalRefCount の値を
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
	// 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
	// 解放時にはこれと同じか、これよりも少なくなってないとならない。
	// そうなってなければ、どこか別のところで関数などが参照されていて、
	// プラグインは解放できないと言うことになる。

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数。

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す

	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// - global の DeleteMember メソッドを用い、オブジェクトを削除する
	if(global)
	{
		// TJS 自体が既に解放されていたときなどは
		// global は NULL になり得るので global が NULL でない
		// ことをチェックする

		// Layer クラスオブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Layer"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			delMethod(dispatch, L"light");
			delMethod(dispatch, L"colorize");
			delMethod(dispatch, L"noise");
		}
		
		// - global を Release する
		global->Release();
	}

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}
