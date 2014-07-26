#include <stdio.h>
#include "ncbind/ncbind.hpp"
#include <vector>
#include <include/v8.h>
#include <include/v8-debug.h>
#include <include/libplatform/libplatform.h>

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

//---------------------------------------------------------------------------

/**
 * Scripts クラスへの Javascript 実行メソッドの追加
 */
class ScriptsJavascript {

public:
	/**
	 * 登録処理前
	 */
	static void PreRegisterCallback() {
		// Copyright 表示
		TVPAddImportantLog(ttstr(copyright));
		getInstance();
	}
	
	/**
	 * 開放処理後
	 */
	static void PostUnregisterCallback() {
		destroy();
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
		return getInstance()->_exec(NULL, param[0]->GetString(), result);
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
		return getInstance()->_exec(filename, data.c_str(), result);
	}

	// デバッガ駆動用
	static tjs_error TJS_INTF_METHOD processDebug(tTJSVariant *result,
												  tjs_int numparams,
												  tTJSVariant **param,
												  iTJSDispatch2 *objthis) {
		getInstance()->processDebug();
		return TJS_S_OK;
	}

	static ScriptsJavascript *getInstance() {
		if (!instance) {
			instance = new ScriptsJavascript();
		}
		return instance;
	}
	
	Local<Context> getContext() {
		return Local<Context>::New(isolate, mainContext);
	}
	
protected:

	/**
	 * コンストラクタ
	 */
	ScriptsJavascript(){
		v8::V8::InitializeICU();
		platform = v8::platform::CreateDefaultPlatform();
		v8::V8::InitializePlatform(platform);
		//v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
		
		isolate = Isolate::New();
		isolate->Enter();
		
		HandleScope handle_scope(isolate);
		
		// グローバルテンプレートの準備
		Local<ObjectTemplate> globalTemplate = ObjectTemplate::New(isolate);
		// グローバル関数に登録
		//globalTemplate->Set(String::NewFromUtf8(isolate, "log"), FunctionTemplate::New(isolate, LogCallback));
		
		TJSInstance::init(isolate, globalTemplate);
		TJSObject::init(isolate);
		
		// コンテキスト生成
		Local<Context> context = Context::New(isolate, 0, globalTemplate);
		// 記録しておく
		mainContext.Reset(isolate, context);

		Context::Scope context_scope(context);
		
		// グローバルオブジェクトの準備
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		if (global) {
			// 吉里吉里のグローバルに Javascript のグローバルを登録する
			{
				tTJSVariant result = toVariant(isolate, context->Global());
				global->PropSet(TJS_MEMBERENSURE, JAVASCRIPT_GLOBAL, NULL, &result, global);
			}
			// Javascript の グローバルに吉里吉里の グローバルを登録する
			context->Global()->Set(String::NewFromTwoByte(isolate, KIRIKIRI_GLOBAL), toJSValue(isolate, tTJSVariant(global, global)));
			global->Release();
		}
	};

	/**
	 * デストラクタ
	 */
	~ScriptsJavascript(){
		mainContext.Reset();
		TJSObject::done(isolate);
		isolate->Dispose();
		isolate = 0;
		V8::Dispose();
		v8::V8::ShutdownPlatform();
		delete platform;
		platform = 0;
	}
	
	/**
	 * javascript スクリプトの実行
	 * @param script スクリプト
	 * @param ... 引数
	 * @return 実行結果
	 */
	tjs_error _exec(const tjs_char *filename,
					const tjs_char *scriptText,
					tTJSVariant *result) {

		HandleScope handle_scope(isolate);
		Context::Scope context_scope(getContext());
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

	void processDebug() {
		HandleScope handle_scope(isolate);
		Context::Scope context_scope(getContext());
		Debug::ProcessDebugMessages();
	}

	static void destroy() {
		if (instance) {
			delete instance;
			instance = 0;
		}
	}

private:
	static ScriptsJavascript *instance;
	v8::Platform* platform;
	Isolate* isolate;
	Persistent<Context> mainContext;
};

ScriptsJavascript *ScriptsJavascript::instance = 0;

NCB_ATTACH_FUNCTION(execJS, Scripts, ScriptsJavascript::exec);
NCB_ATTACH_FUNCTION(execStorageJS, Scripts, ScriptsJavascript::execStorage);
NCB_ATTACH_FUNCTION(processDebugJS, Scripts, ScriptsJavascript::processDebug);

Local<Context> getContext() {
	return ScriptsJavascript::getInstance()->getContext();
}

//---------------------------------------------------------------------------

static void PreRegisterCallback()
{
	ScriptsJavascript::PreRegisterCallback();
}

/**
 * 開放処理前
 */
static void PostUnregisterCallback()
{
	ScriptsJavascript::PostUnregisterCallback();
}


NCB_PRE_REGIST_CALLBACK(PreRegisterCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregisterCallback);
