#include <windows.h>
#include "tp_stub.h"
#include "sqtjsobj.h"

// sqwrapper.cpp より
extern void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
extern SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);
extern SQRESULT ERROR_KRKR(HSQUIRRELVM v, tjs_error error);

// sqfunc.cpp より
extern SQRESULT ERROR_CREATE(HSQUIRRELVM v);
extern SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v);
void registerInherit(const SQChar *typeName, const SQChar *parentName);
void registerTypeTag(const SQChar *typeName, SQUserPointer tag);
SQUserPointer getInstance(HSQUIRRELVM v, int idx, const SQChar *typeName);

// Main.cpp より
tjs_int getArrayCount(iTJSDispatch2 *array);
void getArrayString(iTJSDispatch2 *array, int idx, ttstr &store);

// 型情報
static const SQChar *typeName = _SC("TJSObject"); ///< 型名
static const SQUserPointer TJSOBJTYPETAG = (SQUserPointer)_SC("TJSOBJTYPETAG"); ///< squirrel型タグ

/**
 * 辞書データからクラス登録用のスクリプトを生成するためのオブジェクト
 */
class ScriptCreater : public tTJSDispatch /** EnumMembers 用 */
{
protected:
	// スクリプト記録用
	ttstr script;

	// ファンクションのコードを生成
	void registFunction(const tjs_char *functionName) {
		script += "function ";
		script += functionName;
		script += "(...){local args=[];for(local i=0;i<vargc;i++){args.append(vargv[i]);};return tjsInvoker(\"";
		script += functionName;
		script += "\", args);}";
	}

	// プロパティのコードを生成
	void registProperty(const tjs_char *propertyName) {
		script += L"function set";
		script += toupper(*propertyName);
		script += (propertyName + 1);
		script += L"(arg){tjsSetter(\"";
		script += propertyName;
		script += L"\", arg);}";
		script += L"function get";
		script += toupper(*propertyName);
		script += (propertyName + 1);
		script += L"(){return tjsGetter(\"";
		script += propertyName;
		script += L"\");}";
	}

	// ファンクションかどうか
	static bool isFunction(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Function", member));
	}

	// プロパティかどうか
	static bool isProperty(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Property", member));
	}
	
public:

	// コンストラクタ
	ScriptCreater(const tjs_char *className) {
		script += "function constructor(...){local args=[];for(local i=0;i<vargc;i++){args.append(vargv[i]);};return tjsConstructor(\"";
		script += className;
		script += "\", args);}";
	};

	// データの取得
	const tjs_char *getData() {
		return script.c_str();
	}

	// データの長さの取得
	int getLength() {
		return script.GetLen();
	}

	// EnumMember用繰り返し実行部
	// param[0] メンバ名
	// param[1] フラグ
	// param[2] メンバの値
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
			if (/**(int)param[1] != TJS_HIDDENMEMBER &&*/ param[2]->Type() == tvtObject) {
				const tjs_char *name = param[0]->GetString();
				iTJSDispatch2 *member = param[2]->AsObjectNoAddRef();
				if (isFunction(member)) {
					registFunction(name);
				} else if (isProperty(member)) {
					registProperty(name);
				}
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

// -----------------------------------------------------------------------

// 初期化用
void
TJSObject::init()
{
	registerInherit(typeName, SQOBJECTNAME);
	registerTypeTag(typeName, TJSOBJTYPETAG);
}

/**
 * クラスの登録
 * @param HSQUIRRELVM v
 * @param className 登録クラス名
 * @param tjsClassInfo クラス定義情報(TJSのクラス名のリスト)
 */
void
TJSObject::registerClass(HSQUIRRELVM v, const tjs_char *className, tTJSVariant *tjsClassInfo)
{
	ttstr tjsClassName;
	iTJSDispatch2 *classArray;
	
	if (tjsClassInfo == NULL) {
		classArray = NULL;
		tjsClassName = className;
	} else if (tjsClassInfo->Type() == tvtString) {
		classArray = NULL;
		tjsClassName = tjsClassInfo->GetString();
	} else {
		classArray = tjsClassInfo->AsObjectNoAddRef();
		getArrayString(classArray, 0, tjsClassName);
	}
	
	// クラス生成用スクリプトを生成する
	ScriptCreater script(tjsClassName.c_str());
	tTJSVariantClosure closure(&script);
	
	// 親クラスのメソッドを登録
	if (classArray) {
		int count = getArrayCount(classArray);
		for (int i=count-1;i>0;i--) {
			ttstr className;
			getArrayString(classArray, i, className);
			{			
				tTJSVariant classObj;
				TVPExecuteExpression(className, &classObj);
				iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
				if (!dispatch || TJS_FAILED(dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch))) {
					TVPThrowExceptionMessage(L"not tjs class");
				}
				dispatch->EnumMembers(TJS_IGNOREPROP, &closure, dispatch);
			}
		}
	}
	
	// 指定クラスのメソッドを登録
	{
		tTJSVariant classObj;
		TVPExecuteExpression(tjsClassName, &classObj);
		iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
		if (!dispatch || TJS_FAILED(dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch))) {
			TVPThrowExceptionMessage(L"not tjs class");
		}
		dispatch->EnumMembers(TJS_IGNOREPROP, &closure, dispatch);
	}
	
	sq_pushroottable(v); // root
	sq_pushstring(v, className, -1);
	sq_pushstring(v, SQOBJECTNAME, -1);
	sq_get(v,-3);
	sq_newclass(v, true); // 継承する
	sq_settypetag(v, -1, TJSOBJTYPETAG);
	
#define REGISTER_METHOD(func) \
	sq_pushstring(v, _SC(#func), -1);\
	sq_newclosure(v, func, 0);\
	sq_createslot(v, -3);
	
	// TJS用処理メソッドを登録
	REGISTER_METHOD(tjsConstructor);
	REGISTER_METHOD(tjsInvoker);
	REGISTER_METHOD(tjsSetter);
	REGISTER_METHOD(tjsGetter);
	
	// クラスのコンテキストで生成スクリプトを実行
	if (SQ_SUCCEEDED(sq_compilebuffer(v, script.getData(), script.getLength(), className, SQTrue))) {
		sq_push(v, -2); // class
		if (SQ_FAILED(sq_call(v, 1, SQFalse, SQTrue))) {
			sq_getlasterror(v);
			const SQChar *str;
			sq_getstring(v, -1, &str);
			SQPRINT(v, str);
			sq_pop(v, 1);
		}
		sq_pop(v, 1);
	} else {
		sq_getlasterror(v);
		const SQChar *str;
		sq_getstring(v, -1, &str);
		SQPRINT(v, str);
		sq_pop(v, 1);
	}
	
	sq_createslot(v, -3);
	sq_pop(v, 1); // root
}

iTJSDispatch2 *
TJSObject::getDispatch(HSQUIRRELVM v, int idx)
{
	TJSObject *obj = (TJSObject*)::getInstance(v, idx, typeName);
	return obj ? obj->dispatch : NULL;
}

// -----------------------------------------------------------------------

// コンストラクタ
TJSObject::TJSObject(HSQUIRRELVM v) : dispatch(NULL)
{
	initSelf(v);
}

// デストラクタ
TJSObject::~TJSObject()
{
	if (dispatch) {
		dispatch->Release();
	}
}

/**
 * オブジェクトのリリーサ
 */
SQRESULT
TJSObject::release(SQUserPointer up, SQInteger size)
{
	TJSObject *self = (TJSObject*)up;
	delete self;
	return SQ_OK;
}

/**
 * TJSオブジェクトのコンストラクタ
 * 引数1 オブジェクト
 * 引数2 クラス名
 * 引数3 引数
 */
SQRESULT
TJSObject::tjsConstructor(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	TJSObject *self = new TJSObject(v);
	if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
		sq_setreleasehook(v, 1, release);
	} else {
		delete self;
	}
	if (SQ_SUCCEEDED(result)) {
		// クラスを生成する
		tTJSVariant classObj;
		TVPExecuteExpression(sqobject::getString(v,2), &classObj);
		iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
		if (dispatch) {
			// 引数変換
			int argc = sq_getsize(v,3);
			tTJSVariant **args = new tTJSVariant*[argc];
			for (int i=0;i<argc;i++) {
				args[i] = new tTJSVariant();
				sq_pushinteger(v, i);
				if (SQ_SUCCEEDED(sq_get(v, 3))) {
					sq_getvariant(v, -1, args[i]);
					sq_pop(v, 1);
				}
			}
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->CreateNew(0, NULL, NULL, &self->dispatch, argc, args, dispatch))) {
				result = SQ_OK;
			} else {
				result = ERROR_KRKR(v, error);
			}
			// 引数破棄
			for (int i=0;i<argc;i++) {
				delete args[i];
			}
			delete[] args;
		} else {
			result = ERROR_CREATE(v);
		}
	}
	return result;
}

/**
 * TJSオブジェクト用のメソッド
 * 引数1 オブジェクト
 * 引数2 メンバ名
 * 引数3 引数の配列
 */
SQRESULT
TJSObject::tjsInvoker(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		// 引数変換
		int argc = sq_getsize(v,3);
		tTJSVariant **args = new tTJSVariant*[argc];
		for (int i=0;i<argc;i++) {
			args[i] = new tTJSVariant();
			sq_pushinteger(v, i);
			if (SQ_SUCCEEDED(sq_get(v, 3))) {
				sq_getvariant(v, -1, args[i]);
				sq_pop(v, 1);
			}
		}
		
		// メソッド呼び出し
		int ret = 0;
		tTJSVariant result;
		tjs_error error;
		const tjs_char *methodName = sqobject::getString(v,2);
		if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, sqobject::getString(v,2), NULL, &result, argc, args, dispatch))) {
			if (result.Type() != tvtVoid) {
				sq_pushvariant(v, result);
				result = 1;
			} else {
				result = 0;
			}
		} else {
			result = ERROR_KRKR(v, error);
		}
		
		// 引数破棄
		for (int i=0;i<argc;i++) {
			delete args[i];
		}
		delete[] args;
		return result;
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJSオブジェクト用のプロパティゲッター
 * 引数1 オブジェクト
 * 引数2 プロパティ名
 */
SQRESULT
TJSObject::tjsGetter(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v,2), NULL, &result, dispatch))) {
			sq_pushvariant(v, result);
			return 1;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJSオブジェクト用のプロパティセッター
 * 引数1 オブジェクト
 * 引数2 プロパティ名
 * 引数3 設定値
 */
SQRESULT
TJSObject::tjsSetter(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		sq_getvariant(v, 3, &result);
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, sqobject::getString(v, 2), NULL, &result, dispatch))) {
			return SQ_OK;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}

