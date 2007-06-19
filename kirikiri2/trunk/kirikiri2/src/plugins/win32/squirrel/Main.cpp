#include "ncbind/ncbind.hpp"
#include "sqplus.h"

// squirrel 上での TJS2のグローバル空間の参照名
#define KIRIKIRI_GLOBAL L"krkr"
// TJS2 上での squirrel のグローバル空間の参照名
#define SQUIRREL_GLOBAL L"sqglobal"

// コピーライト表記
static const char *copyright =
"\n------ Squirrel Copyright START ------\n"
"Copyright (c) 2003-2007 Alberto Demichelis\n"
"------ Squirrel Copyright END ------";

/**
 * ログ出力用 for squirrel
 */
static void PrintFunc(HSQUIRRELVM v, const SQChar* format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, sizeof msg, format, args);
	TVPAddLog(msg);
	va_end(args);
}

//---------------------------------------------------------------------------
// squirrel -> TJS2 ブリッジ用
//---------------------------------------------------------------------------

extern void store(tTJSVariant &result, SquirrelObject &variant);
extern void store(tTJSVariant &result, HSQUIRRELVM v, int idx=-1);
extern void registglobal(HSQUIRRELVM v, const SQChar *name, iTJSDispatch2 *dispatch);
extern void unregistglobal(HSQUIRRELVM v, const SQChar *name);

//---------------------------------------------------------------------------

#include "../json/Writer.hpp"

static void
quoteString(const tjs_char *str, IWriter *writer)
{
	if (str) {
		writer->write((tjs_char)'"');
		const tjs_char *p = str;
		int ch;
		while ((ch = *p++)) {
			if (ch == '"') {
				writer->write(L"\\\"");
			} else {
				writer->write((tjs_char)ch);
			}
		}
		writer->write((tjs_char)'"');
	} else {
		writer->write(L"null");
	}
}

static void getVariantString(tTJSVariant &var, IWriter *writer);

/**
 * 辞書の内容表示用の呼び出しロジック
 */
class DictMemberDispCaller : public tTJSDispatch /** EnumMembers 用 */
{
protected:
	IWriter *writer;
	bool first;
public:
	DictMemberDispCaller(IWriter *writer) : writer(writer) { first = true; };
	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
												tjs_uint32 flag,			// calling flag
												const tjs_char * membername,// member name ( NULL for a default member )
												tjs_uint32 *hint,			// hint for the member name (in/out)
												tTJSVariant *result,		// result
												tjs_int numparams,			// number of parameters
												tTJSVariant **param,		// parameters
												iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			if ((int)param[1] != TJS_HIDDENMEMBER) {
				if (first) {
					first = false;
				} else {
					writer->write((tjs_char)',');
					writer->newline();
				}
				writer->write((tjs_char)'[');
				quoteString(param[0]->GetString(), writer);
				writer->write(L"]=");
				getVariantString(*param[2], writer);
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

static void getDictString(iTJSDispatch2 *dict, IWriter *writer)
{
	writer->write((tjs_char)'{');
	writer->addIndent();
	DictMemberDispCaller caller(writer);
	tTJSVariantClosure closure(&caller);
	dict->EnumMembers(TJS_IGNOREPROP, &closure, dict);
	writer->delIndent();
	writer->write((tjs_char)'}');
}

// Array クラスメンバ
static iTJSDispatch2 *ArrayCountProp   = NULL;   // Array.count

static void getArrayString(iTJSDispatch2 *array, IWriter *writer)
{
	writer->write((tjs_char)'[');
	writer->addIndent();
	tjs_int64 count = 0;
	{
		tTJSVariant result;
		if (TJS_SUCCEEDED(ArrayCountProp->PropGet(0, NULL, NULL, &result, array))) {
			count = result.AsInteger();
		}
	}
	for (int i=0; i<count; i++) {
		if (i != 0) {
			writer->write((tjs_char)',');
			writer->newline();
		}
		tTJSVariant result;
		if (array->PropGetByNum(TJS_IGNOREPROP, i, &result, array) == TJS_S_OK) {
			getVariantString(result, writer);
		}
	}
	writer->delIndent();
	writer->write((tjs_char)']');
}

static void
getVariantString(tTJSVariant &var, IWriter *writer)
{
	switch(var.Type()) {

	case tvtVoid:
		writer->write(L"null");
		break;
		
	case tvtObject:
		{
			iTJSDispatch2 *obj = var.AsObjectNoAddRef();
			if (obj == NULL) {
				writer->write(L"null");
			} else if (obj->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Array",obj) == TJS_S_TRUE) {
				getArrayString(obj, writer);
			} else {
				getDictString(obj, writer);
			}
		}
		break;
		
	case tvtString:
		quoteString(var.GetString(), writer);
		break;

	case tvtInteger:
		writer->write((tTVInteger)var);
		break;

	case tvtReal:
		writer->write((tTVReal)var);
		break;

	default:
		writer->write(L"null");
		break;
	};
}

//---------------------------------------------------------------------------

/**
 * Scripts クラスへの Squirrel 実行メソッドの追加
 */
class ScriptsSquirrel {

public:
	ScriptsSquirrel(){};

	/**
	 * squirrel スクリプトの実行
	 * @param script スクリプト
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD exec(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
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

	/**
	 * squirrel スクリプトのファイルからの実行
	 * @param filename ファイル名
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD execStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {

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

	/**
	 * squirrel 形式でのオブジェクトの保存
	 * @param filename ファイル名
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD save(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		IFileWriter writer(param[0]->GetString());
		writer.write(L"return ");
		getVariantString(*param[1], &writer);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD toString(tTJSVariant *result,
											  tjs_int numparams,
											  tTJSVariant **param,
											  iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (result) {
			IStringWriter writer;
			getVariantString(*param[0], &writer);
			*result = writer.buf;
		}
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(ScriptsSquirrel, Scripts) {
	RawCallback("execSQ",        &ScriptsSquirrel::exec,        TJS_STATICMEMBER);
	RawCallback("execStorageSQ", &ScriptsSquirrel::execStorage, TJS_STATICMEMBER);
	RawCallback("saveSQ",        &ScriptsSquirrel::save,        TJS_STATICMEMBER);
	RawCallback("toSQString",    &ScriptsSquirrel::toString,    TJS_STATICMEMBER);
};

/**
 * Squirrel 用 Continuous ハンドラクラス
 * 直接 squirrel のメソッドを呼び出す Continuous ハンドラを生成する
 */
class SQContinuous : public tTVPContinuousEventCallbackIntf {

protected:
	// 内部オブジェクト参照保持用
	HSQOBJECT obj;

public:
	/**
	 * コンストラクタ
	 * @param name 名称
	 */
	SQContinuous(const tjs_char *name) {
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_resetobject(&obj);          // ハンドルを初期化
		sq_pushroottable(v);
		sq_pushstring(v, name, -1);
		sq_get(v, -2);                   // ルートテーブルから関数を取得
		sq_getstackobj(v, -1, &obj); // 位置-1からオブジェクトハンドルを得る
		sq_addref(v, &obj);              // オブジェクトへの参照を追加
		sq_pop(v, 2);
	}

	/**
	 * デストラクタ
	 */
	~SQContinuous() {
		stop();
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_release(v, &obj);  
	}

	/**
	 * 呼び出し開始
	 */
	void start() {
		stop();
		TVPAddContinuousEventHook(this);
	}

	/**
	 * 呼び出し停止
	 */
	void stop() {
		TVPRemoveContinuousEventHook(this);
	}

	/**
	 * Continuous コールバック
	 * 吉里吉里が暇なときに常に呼ばれる
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_pushobject(v, obj);
		sq_pushroottable(v);
		sq_pushinteger(v, (SQInteger)tick); // 切り捨て御免
		if (!SQ_SUCCEEDED(sq_call(v, 2, SQFalse, SQFalse))) {
			stop();
		}
		sq_pop(SquirrelVM::GetVMPtr(), 1);
	}
};

NCB_REGISTER_CLASS(SQContinuous) {
	NCB_CONSTRUCTOR((const tjs_char *));
	NCB_METHOD(start);
	NCB_METHOD(stop);
};

//---------------------------------------------------------------------------

/**
 * 登録処理前
 */
static void PreRegistCallback()
{
	// Copyright 表示
	TVPAddImportantLog(ttstr(copyright));

	// squirrel 初期化
	SquirrelVM::Init();
	// squirrel printf 登録
	sq_setprintfunc(SquirrelVM::GetVMPtr(), PrintFunc);
}

/**
 * 登録処理後
 */
static void PostRegistCallback()
{
	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		// 吉里吉里のグローバルに Squirrel のグローバルを登録する
		{
			tTJSVariant result = tTJSVariant();
			HSQUIRRELVM v = SquirrelVM::GetVMPtr();
			sq_pushroottable(v);
			store(result, v, -1);
			sq_pop(v, 1);
			global->PropSet(TJS_MEMBERENSURE, SQUIRREL_GLOBAL, NULL, &result, global);
		}
		// Squirrel の グローバルに吉里吉里の グローバルを登録する
		registglobal(SquirrelVM::GetVMPtr(), KIRIKIRI_GLOBAL, global);
		global->Release();
	}

	// Array.count を取得
	{
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Array"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		tTJSVariant val;
		if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
										 TJS_W("count"),
										 NULL,
										 &val,
										 dispatch))) {
			TVPThrowExceptionMessage(L"can't get Array.count");
		}
		ArrayCountProp = val.AsObject();
	}
}

/**
 * 開放処理前
 */
static void PreUnregistCallback()
{
	if (ArrayCountProp) {
		ArrayCountProp->Release();
		ArrayCountProp = NULL;
	}
	
	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global)	{
		// Squirrel の グローバルから吉里吉里のグローバルを削除
		unregistglobal(SquirrelVM::GetVMPtr(), KIRIKIRI_GLOBAL);
		// squirrel の global への登録を解除
		global->DeleteMember(0, SQUIRREL_GLOBAL, NULL, global);
		global->Release();
	}
}

/**
 * 開放処理後
 */
static void PostUnregistCallback()
{
	// squirrel 終了
	SquirrelVM::Shutdown();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
