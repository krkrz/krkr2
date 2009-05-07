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
 * メンバ登録処理用
 */
class MemberRegister : public tTJSDispatch /** EnumMembers 用 */
{

public:
	// コンストラクタ
	MemberRegister(HSQUIRRELVM v) : v(v) {
	};

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

protected:
	// ファンクション登録
	void registFunction(const tjs_char *functionName) {
		sq_pushstring(v, functionName, -1);\
		sq_pushstring(v, functionName, -1);
		sq_newclosure(v, TJSObject::tjsInvoker, 1);\
		sq_createslot(v, -3);
	}

	// プロパティ登録
	void registProperty(const tjs_char *propertyName) {
		ttstr name = L"set";
		name += toupper(*propertyName);
		name += (propertyName + 1);
		sq_pushstring(v, name.c_str(), -1);\
		sq_pushstring(v, propertyName, -1);
		sq_newclosure(v, TJSObject::tjsSetter, 1);\
		sq_createslot(v, -3);
		
		name = L"get";
		name += toupper(*propertyName);
		name += (propertyName + 1);
		sq_pushstring(v, name.c_str(), -1);\
		sq_pushstring(v, propertyName, -1);
		sq_newclosure(v, TJSObject::tjsGetter, 1);\
		sq_createslot(v, -3);
	}

	// ファンクションかどうか
	static bool isFunction(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Function", member));
	}

	// プロパティかどうか
	static bool isProperty(iTJSDispatch2 *member) {
		return TJS_SUCCEEDED(member->IsInstanceOf(0, NULL, NULL, L"Property", member));
	}
private:
	HSQUIRRELVM v;
	
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
	
	sq_pushroottable(v); // root
	sq_pushstring(v, className, -1);
	sq_pushstring(v, SQOBJECTNAME, -1);
	sq_get(v,-3);
	sq_newclass(v, true); // 継承する
	sq_settypetag(v, -1, TJSOBJTYPETAG);
	
	// コンストラクタ登録
	sq_pushstring(v, _SC("constructor"), -1);\
	sq_pushstring(v, className, -1);
	sq_newclosure(v, tjsConstructor, 1);\
	sq_createslot(v, -3);

	// クラス生成用スクリプトを生成する
	MemberRegister r(v);
	tTJSVariantClosure closure(&r);
	
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
		dispatch->Invalidate(0, NULL, NULL, dispatch);
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
		
		int top = sq_gettop(v);
		
		// クラスを生成する
		tTJSVariant classObj;
		TVPExecuteExpression(sqobject::getString(v,top), &classObj);
		iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
		if (dispatch) {
			// 引数変換
			int argc = top - 2;
			tTJSVariant **args = NULL;
			if (argc > 0) {
				args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
					sq_getvariant(v, 2+i, args[i]);
				}
			}
			tjs_error error;
			if (TJS_SUCCEEDED(error = dispatch->CreateNew(0, NULL, NULL, &self->dispatch, argc, args, dispatch))) {
				result = SQ_OK;
			} else {
				result = ERROR_KRKR(v, error);
			}
			// 引数破棄
			if (args) {
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			}
		} else {
			result = ERROR_CREATE(v);
		}
	}
	return result;
}

/**
 * TJSオブジェクト用のメソッド
 * 引数1 オブジェクト
 * 引数2～ 引数
 * 自由変数1 メンバ名
 */
SQRESULT
TJSObject::tjsInvoker(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {

		int top = sq_gettop(v);

		// 引数変換
		int argc = top - 2;
		tTJSVariant **args = NULL;
		if (argc > 0) {
			args = new tTJSVariant*[argc];
			for (int i=0;i<argc;i++) {
				args[i] = new tTJSVariant();
				sq_getvariant(v, 2+i, args[i]);
			}
		}
		
		// メソッド呼び出し
		int ret = 0;
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, sqobject::getString(v,top), NULL, &result, argc, args, dispatch))) {
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
		if (args) {
			for (int i=0;i<argc;i++) {
				delete args[i];
			}
			delete[] args;
		}
		return result;
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJSオブジェクト用のプロパティゲッター
 * 引数1 オブジェクト
 * 自由変数1 プロパティ名
 */
SQRESULT
TJSObject::tjsGetter(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v, sq_gettop(v)), NULL, &result, dispatch))) {
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
 * 引数2 設定値
 * 自由変数1 プロパティ名
 */
SQRESULT
TJSObject::tjsSetter(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = getDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		sq_getvariant(v, 2, &result);
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, sqobject::getString(v, sq_gettop(v)), NULL, &result, dispatch))) {
			return SQ_OK;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}

