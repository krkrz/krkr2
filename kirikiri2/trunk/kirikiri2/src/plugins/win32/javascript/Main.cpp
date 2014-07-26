#include <stdio.h>
#include "ncbind/ncbind.hpp"
#include <vector>
#include <v8.h>
#include <v8-debug.h>

using namespace v8;

// 吉里吉里オブジェクトクラス処理
#include "tjsobj.h"
#include "tjsinstance.h"

// javascript 上での TJS2のグローバル空間の参照名
#define KIRIKIRI_GLOBAL L"krkr"
// TJS2 上での javascript のグローバル空間の参照名
#define JAVASCRIPT_GLOBAL L"jsglobal"

// 値の格納・取得用
extern Local<Value> toJSValue(Isolate *isolate, const tTJSVariant &variant);
extern tTJSVariant toVariant(Isolate *isolate, Handle<Value> value);
extern void JSEXCEPTION(TryCatch *try_catch);

// コピーライト表記
static const char *copyright =
#include "LICENSE.h"
;

// Javascriptグローバルコンテキスト
Isolate *isolate = 0;

//---------------------------------------------------------------------------

/**
 * Scripts クラスへの Javascript 実行メソッドの追加
 */
class ScriptsJavascript {

public:
	ScriptsJavascript(){};

	/**
	 * javascript スクリプトの実行
	 * @param script スクリプト
	 * @param ... 引数
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD _exec(const tjs_char *filename,
										   const tjs_char *scriptText,
										   tTJSVariant *result) {
		HandleScope handle_scope(isolate);
		Context::Scope context_scope(isolate->GetCurrentContext());
		TryCatch try_catch;

		Local<Script> script = Script::Compile(String::NewFromTwoByte(isolate, scriptText), filename ? String::NewFromTwoByte(isolate, filename) : String::NewFromTwoByte(isolate, L""));
		if (script.IsEmpty()) {
			// Print errors that happened during compilation.
			JSEXCEPTION(&try_catch);
		} else {
			Local<Value> ret = script->Run();
			if (ret.IsEmpty()) {
				JSEXCEPTION(&try_catch);
			} else {
				// 結果を格納
				if (result) {
					*result = toVariant(isolate, ret);
				}
			}
		}
		return TJS_S_OK;
	}

	
	/**
	 * javascript スクリプトの実行
	 * @param script スクリプト
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD exec(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		return _exec(NULL, param[0]->GetString(), result);
	}

	/**
	 * javascript スクリプトのファイルからの実行
	 * @param filename ファイル名
	 * @param ... 引数
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD execStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		const tjs_char *filename = param[0]->GetString();
		iTJSTextReadStream * stream = TVPCreateTextStreamForRead(filename, TJS_W(""));
		ttstr data;
		try {
			stream->Read(data, 0);
			stream->Destruct();
		}
		catch(...)
		{
			stream->Destruct();
			throw;
		}
		tjs_error ret = _exec(filename, data.c_str(), result);
		return ret;
	}

	// デバッガ駆動用
	static tjs_error TJS_INTF_METHOD processDebug(tTJSVariant *result,
												  tjs_int numparams,
												  tTJSVariant **param,
												  iTJSDispatch2 *objthis) {
		HandleScope handle_scope(isolate);
		Context::Scope context_scope(isolate->GetCurrentContext());
		Debug::ProcessDebugMessages();
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(ScriptsJavascript, Scripts) {
	RawCallback("execJS",        &ScriptsJavascript::exec,          TJS_STATICMEMBER);
	RawCallback("execStorageJS", &ScriptsJavascript::execStorage,   TJS_STATICMEMBER);
	RawCallback("processDebugJS", &ScriptsJavascript::processDebug, TJS_STATICMEMBER);
};

//---------------------------------------------------------------------------

/**
 * 登録処理前
 */
static void PreRegistCallback()
{
	// Copyright 表示
	TVPAddImportantLog(ttstr(copyright));

	isolate = Isolate::New();
	isolate->Enter();

	HandleScope handle_scope(isolate);

	// グローバルテンプレートの準備
	Local<ObjectTemplate> globalTemplate = ObjectTemplate::New(isolate);
	
	TJSInstance::init(isolate, globalTemplate);
	TJSObject::init(isolate);

	// コンテキスト生成
	Local<Context> mainContext = Context::New(isolate, 0, globalTemplate);
	Context::Scope context_scope(mainContext);
	
	// グローバルオブジェクトの準備
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		// 吉里吉里のグローバルに Javascript のグローバルを登録する
		{
			tTJSVariant result = toVariant(isolate, mainContext->Global());
			global->PropSet(TJS_MEMBERENSURE, JAVASCRIPT_GLOBAL, NULL, &result, global);
		}
		// Javascript の グローバルに吉里吉里の グローバルを登録する
		mainContext->Global()->Set(String::NewFromTwoByte(isolate, KIRIKIRI_GLOBAL), toJSValue(isolate, tTJSVariant(global, global)));
		global->Release();
	}
}

/**
 * 開放処理後
 */
static void PostUnregistCallback()
{
	if (isolate) {
		TJSObject::done(isolate);
		isolate->Dispose();
		isolate = NULL;
	}
	V8::Dispose();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
