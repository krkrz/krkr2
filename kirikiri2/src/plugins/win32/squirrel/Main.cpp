#include <windows.h>
#include "tp_stub.h"
#include "sqplus.h"

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

//---------------------------------------------------------------------------

static void
addMember(iTJSDispatch2 *dispatch, const tjs_char *name, iTJSDispatch2 *member)
{
	tTJSVariant var = tTJSVariant(member);
	member->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		name, // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&var, // 登録する値
		dispatch // コンテキスト
		);
}

static iTJSDispatch2*
getMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	tTJSVariant val;
	if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
									 name,
									 NULL,
									 &val,
									 dispatch))) {
		ttstr msg = TJS_W("can't get member:");
		msg += name;
		TVPThrowExceptionMessage(msg.c_str());
	}
	return val.AsObject();
}

static bool
isValidMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	return dispatch->IsValid(TJS_IGNOREPROP,
							 name,
							 NULL,
							 dispatch) == TJS_S_TRUE;
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

//---------------------------------------------------------------------------

extern void store(tTJSVariant &result, SquirrelObject &variant);
extern void registglobal(HSQUIRRELVM v, const SQChar *name, iTJSDispatch2 *dispatch);

//---------------------------------------------------------------------------

/**
 */
class tExecFunction : public tTJSDispatch
{
public:	
	/// コンストラクタ
	tExecFunction() {
	}
	/// デストラクタ
	~tExecFunction() {
	}
	
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		// ネイティブオブジェクトの取得
		if (!objthis) return TJS_E_NATIVECLASSCRASH;
		if (membername) return TJS_E_MEMBERNOTFOUND;
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;

		try {
			SquirrelObject script = SquirrelVM::CompileBuffer(param[0]->GetString());
			SquirrelObject ret    = SquirrelVM::RunScript(script);
			if (result) {
				store(*result, ret);
			}
		} catch(SquirrelError e) {
			TVPThrowExceptionMessage(e.desc);
		}
		return S_OK;
	}
};

/**
 */
class tExecStorageFunction : public tTJSDispatch
{
public:	
	/// コンストラクタ
	tExecStorageFunction() {
	}
	/// デストラクタ
	~tExecStorageFunction() {
	}
	
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		// ネイティブオブジェクトの取得
		if (!objthis) return TJS_E_NATIVECLASSCRASH;
		if (membername) return TJS_E_MEMBERNOTFOUND;
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;

		iTJSTextReadStream * stream = TVPCreateTextStreamForRead(param[0]->GetString(), TJS_W(""));
		try {
			ttstr data;
			stream->Read(data, 0);
			SquirrelObject script = SquirrelVM::CompileBuffer(data.c_str());
			SquirrelObject ret    = SquirrelVM::RunScript(script);
			if (result) {
				store(*result, ret);
			}
		} catch(SquirrelError e) {
			stream->Destruct();
			TVPThrowExceptionMessage(e.desc);
		} catch(...) {
			stream->Destruct();
			throw;
		}
		stream->Destruct();
		return S_OK;
	}
};

//---------------------------------------------------------------------------

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// squirrel 初期化
	SquirrelVM::Init();
	
	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	
	if (global) {

		// Scripts オブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Scripts"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			addMember(dispatch, TJS_W("execSQ"), new tExecFunction());
			addMember(dispatch, TJS_W("execStorageSQ"), new tExecStorageFunction());
		}
		
		// グローバルに登録
		registglobal(SquirrelVM::GetVMPtr(), L"krkr", global);
		
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
extern "C" HRESULT _stdcall V2Unlink()
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
	if (global)	{

		// Scripts オブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Scripts"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			delMember(dispatch, TJS_W("execSQ"));
			delMember(dispatch, TJS_W("execStorageSQ"));
		}

		global->Release();
	}

	SquirrelVM::Shutdown();
	
	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}
