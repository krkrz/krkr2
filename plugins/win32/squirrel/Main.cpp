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

extern void store(HSQUIRRELVM v, tTJSVariant &variant);
extern void store(tTJSVariant &result, SquirrelObject &variant);
extern void store(tTJSVariant &result, HSQUIRRELVM v, int idx=-1);
extern void registglobal(HSQUIRRELVM v, const SQChar *name, iTJSDispatch2 *dispatch);
extern void unregistglobal(HSQUIRRELVM v, const SQChar *name);
extern void SQEXCEPTION(HSQUIRRELVM v);

//---------------------------------------------------------------------------

#include "../json/Writer.hpp"

// squirrel 予約語一覧
static const char *reservedKeys[] = 
{
	"break",
	"case",
	"catch",
	"class",
	"clone",
	"continue",
	"default",
	"delegate",
	"delete",
	"else",
	"extends",
	"for",
	"function",
	"if",
	"in",
	"local",
	"null",
	"resume",
	"return",
	"switch",
	"this",
	"throw",
	"try",
	"typeof" ,
	"while",
	"parent",
	"yield",
	"constructor",
	"vargc",
	"vargv" ,
	"instanceof",
	"true",
	"false",
	"static",
	NULL
};

#include <set>
using namespace std;

// 予約語一覧
set<ttstr> reserved;

// 予約語の登録
static void initReserved()
{
	const char **p = reservedKeys;
	while (*p != NULL) {
		reserved.insert(ttstr(*p));
		p++;
	}
}

// 予約語かどうか
static bool isReserved(const tjs_char *key)
{
	return reserved.find(ttstr(key)) != reserved.end();
}

static bool isal(int c) {
	return (c >= 'a' && c <= 'z' ||
			c >= 'A' && c <= 'Z' ||
			c == '_');
}


static bool isaln(int c) {
	return (c >= '0' && c <= '9' ||
			c >= 'a' && c <= 'z' ||
			c >= 'A' && c <= 'Z' ||
			c == '_');
}

static bool
isSimpleString(const tjs_char *str)
{
	const tjs_char *p = str;
	if (!isal(*p)) {
		return false;
	}
	while (*p != '\0') {
		if (!isaln(*p)) {
			return false;
		}
		p++;
	}
	return true;
}

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
			} else if (ch == '\\') {
				writer->write(L"\\\\");
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
//					writer->newline();
				}
				const tjs_char *name = param[0]->GetString();
				if (!isReserved(name) && isSimpleString(name)) {
					writer->write(name);
					writer->write((tjs_char)'=');
				} else {
					writer->write((tjs_char)'[');
					quoteString(name, writer);
					writer->write(L"]=");
				}
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
	//writer->addIndent();
	DictMemberDispCaller caller(writer);
	tTJSVariantClosure closure(&caller);
	dict->EnumMembers(TJS_IGNOREPROP, &closure, dict);
	//writer->delIndent();
	writer->write((tjs_char)'}');
}

// Array クラスメンバ
static iTJSDispatch2 *ArrayCountProp   = NULL;   // Array.count

static void getArrayString(iTJSDispatch2 *array, IWriter *writer)
{
	writer->write((tjs_char)'[');
	//writer->addIndent();
	tjs_int count = 0;
	{
		tTJSVariant result;
		if (TJS_SUCCEEDED(ArrayCountProp->PropGet(0, NULL, NULL, &result, array))) {
			count = result.AsInteger();
		}
	}
	for (tjs_int i=0; i<count; i++) {
		if (i != 0) {
			writer->write((tjs_char)',');
			writer->newline();
		}
		tTJSVariant result;
		if (array->PropGetByNum(TJS_IGNOREPROP, i, &result, array) == TJS_S_OK) {
			getVariantString(result, writer);
		}
	}
	//writer->delIndent();
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
	 * @param obj オブジェクト
	 * @param utf true なら UTF-8 で出力
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD save(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams < 2) return TJS_E_BADPARAMCOUNT;
		IFileWriter writer(param[0]->GetString(), numparams > 2 ? (int)*param[2] != 0: false);
		writer.write(L"return ");
		getVariantString(*param[1], &writer);
		return TJS_S_OK;
	}

	/**
	 * squirrel 形式で文字列化
	 * @param obj オブジェクト
	 * @return 実行結果
	 */
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

	/**
	 * squirrel の名前空間に登録
	 * @param name オブジェクト名
	 * @param obj オブジェクト
	 */
	static tjs_error TJS_INTF_METHOD regist(tTJSVariant *result,
											tjs_int numparams,
											tTJSVariant **param,
											iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (numparams > 1) {
			registglobal(SquirrelVM::GetVMPtr(),
						 param[0]->GetString(),
						 param[1]->AsObjectNoAddRef());
		} else {
			tTJSVariant var;
			TVPExecuteExpression(param[0]->GetString(), &var);
			registglobal(SquirrelVM::GetVMPtr(),
						 param[0]->GetString(),
						 var.AsObjectNoAddRef());
		}
		return TJS_S_OK;
	}

	/**
	 * squirrel の名前空間から削除
	 * @param name オブジェクト名
	 * @param obj オブジェクト
	 */
	static tjs_error TJS_INTF_METHOD unregist(tTJSVariant *result,
											  tjs_int numparams,
											  tTJSVariant **param,
											  iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		unregistglobal(SquirrelVM::GetVMPtr(),
					   param[0]->GetString());
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(ScriptsSquirrel, Scripts) {
	RawCallback("execSQ",        &ScriptsSquirrel::exec,        TJS_STATICMEMBER);
	RawCallback("execStorageSQ", &ScriptsSquirrel::execStorage, TJS_STATICMEMBER);
	RawCallback("saveSQ",        &ScriptsSquirrel::save,        TJS_STATICMEMBER);
	RawCallback("toSQString",    &ScriptsSquirrel::toString,    TJS_STATICMEMBER);
	RawCallback("registSQ",      &ScriptsSquirrel::regist,      TJS_STATICMEMBER);
	RawCallback("unregistSQ",    &ScriptsSquirrel::unregist,    TJS_STATICMEMBER);
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
		if (!SQ_SUCCEEDED(sq_get(v, -2))) { // ルートテーブルから関数を取得
			sq_pop(v, 1);
			ttstr error = "指定された関数は存在しません:";
			error += name;
			TVPThrowExceptionMessage(error.c_str());
		}
		sq_getstackobj(v, -1, &obj); // 位置-1からオブジェクトハンドルを得る
		sq_addref(v, &obj);          // オブジェクトへの参照を追加
		sq_pop(v, 2);
	}

	/**
	 * デストラクタ
	 */
	~SQContinuous() {
		stop();
		sq_release(SquirrelVM::GetVMPtr(), &obj);  
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
		if (!SQ_SUCCEEDED(sq_call(v, 2, SQFalse, SQTrue))) {
			stop();
			sq_pop(v, 1);
			SQEXCEPTION(v);
		}
		sq_pop(v, 1);
	}
};

NCB_REGISTER_CLASS(SQContinuous) {
	NCB_CONSTRUCTOR((const tjs_char *));
	NCB_METHOD(start);
	NCB_METHOD(stop);
};


/**
 * 直接 squirrel のグローバルファンクションを呼び出すことができるラッパー
 * 変換処理を介さずに処理できるので効率が良い
 */
class SQFunction {

protected:
	// 内部オブジェクト参照保持用
	HSQOBJECT obj;

public:
	/**
	 * コンストラクタ
	 * @param name 名称
	 */
	SQFunction(const tjs_char *name) {
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_resetobject(&obj);          // ハンドルを初期化
		sq_pushroottable(v);
		sq_pushstring(v, name, -1);
		if (!SQ_SUCCEEDED(sq_get(v, -2))) { // ルートテーブルから関数を取得
			sq_pop(v, 1);
			ttstr error = "指定された関数は存在しません:";
			error += name;
			TVPThrowExceptionMessage(error.c_str());
		}
		sq_getstackobj(v, -1, &obj); // 位置-1からオブジェクトハンドルを得る
		sq_addref(v, &obj);          // オブジェクトへの参照を追加
		sq_pop(v, 2);
	}

	/**
	 * デストラクタ
	 */
	~SQFunction() {
		sq_release(SquirrelVM::GetVMPtr(), &obj);  
	}

	/**
	 * ファンクションとして呼び出し
	 */
	static tjs_error TJS_INTF_METHOD call(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  SQFunction *objthis) {
		HSQUIRRELVM v = SquirrelVM::GetVMPtr();
		sq_pushobject(v, objthis->obj);
		sq_pushroottable(v); // this
		for (int i=0;i<numparams;i++) { // 引数
			store(v, *param[i]);
		}
		if (!SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
			sq_pop(v, 1);
			SQEXCEPTION(v);
		}
		if (result) {
			store(*result, v);
			sq_pop(v, 1);
		}
		sq_pop(v, 1);
		return TJS_S_OK;
	}
};

NCB_REGISTER_CLASS(SQFunction) {
	NCB_CONSTRUCTOR((const tjs_char *));
	RawCallback("call", &SQFunction::call, 0);
};

//---------------------------------------------------------------------------

/**
 * 登録処理前
 */
static void PreRegistCallback()
{
	// Copyright 表示
	TVPAddImportantLog(ttstr(copyright));

	// 予約語登録
	initReserved();

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
