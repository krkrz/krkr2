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
	MemberRegister(HSQUIRRELVM v, const tjs_char *className) : v(v), className(className) {
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
				bool staticMember = (param[1]->AsInteger() & TJS_STATICMEMBER) != 0;
				iTJSDispatch2 *member = param[2]->AsObjectNoAddRef();
				if (isFunction(member)) {
					registFunction(name, staticMember);
				} else if (isProperty(member)) {
					registProperty(name, staticMember);
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
	void registFunction(const tjs_char *functionName, bool staticMember) {
		sq_pushstring(v, functionName, -1);
		sq_pushstring(v, functionName, -1);
		if (staticMember) {
			sq_pushstring(v, className, -1);
			sq_newclosure(v, TJSObject::tjsStaticInvoker, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsInvoker, 1);
		}
		sq_createslot(v, -3);
	}

	// プロパティ登録
	void registProperty(const tjs_char *propertyName, bool staticMember) {

		ttstr name = L"set";
		name += toupper(*propertyName);
		name += (propertyName + 1);
		sq_pushstring(v, name.c_str(), -1);
		sq_pushstring(v, propertyName, -1);
		if (staticMember) {
			sq_pushstring(v, className, -1);
			sq_newclosure(v, TJSObject::tjsStaticSetter, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsSetter, 1);
		}
		sq_createslot(v, -3);

		name = L"get";
		name += toupper(*propertyName);
		name += (propertyName + 1);
		sq_pushstring(v, name.c_str(), -1);
		sq_pushstring(v, propertyName, -1);
		if (staticMember) {
			sq_pushstring(v, className, -1);
			sq_newclosure(v, TJSObject::tjsStaticGetter, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsGetter, 1);
		}
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
	const tjs_char *className;
};

// -----------------------------------------------------------------------

sqobject::ObjectInfo TJSObject::classMap;

// 初期化用
void
TJSObject::init(HSQUIRRELVM vm)
{
	classMap.initTable();
	registerInherit(typeName, SQOBJECTNAME);
	registerTypeTag(typeName, TJSOBJTYPETAG);
	
	sq_pushroottable(vm);
	sq_pushstring(vm, _SC("createClass"), -1);
	sq_newclosure(vm, createClass, 0);
	sq_createslot(vm, -3);
	sq_pop(vm, 1);
}

// 初期化用
void
TJSObject::done()
{
	classMap.clear();
}

/**
 * クラスの登録
 * @param HSQUIRRELVM v
 */
SQRESULT
TJSObject::createClass(HSQUIRRELVM v)
{
	int top = sq_gettop(v);
	if (top < 2) {
		return sq_throwerror(v, _SC("invalid param"));
	}
	
	// クラスを生成
	sq_pushstring(v, SQOBJECTNAME, -1);
	sq_get(v,-3);
	sq_newclass(v, true); // 継承する
	sq_settypetag(v, -1, TJSOBJTYPETAG);

	// メンバ登録
	const tjs_char *tjsClassName = NULL;
	for (int i=top;i>1;i--) {
		if (SQ_SUCCEEDED(sq_getstring(v, i, &tjsClassName))) {
			tTJSVariant classObj;
			TVPExecuteExpression(tjsClassName, &classObj);
			iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
			if (dispatch && TJS_SUCCEEDED(dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch))) {
				MemberRegister r(v, tjsClassName);
				tTJSVariantClosure closure(&r);
				dispatch->EnumMembers(TJS_IGNOREPROP, &closure, dispatch);
			}
		}
	}

	if (tjsClassName) {
		// コンストラクタ登録
		sq_pushstring(v, _SC("constructor"), -1);\
		sq_pushstring(v, tjsClassName, -1);
		sq_newclosure(v, tjsConstructor, 1);\
		sq_createslot(v, -3);
		// クラス情報を記録
		sqobject::ObjectInfo classObj(v, -1);
		classMap.create(tjsClassName, classObj);
	}
	
	return 1;
}

/**
 * squirrel から吉里吉里オブジェクトを取得
 */
iTJSDispatch2 *
TJSObject::getDispatch(HSQUIRRELVM v, int idx)
{
	TJSObject *obj = (TJSObject*)::getInstance(v, idx, typeName);
	return obj ? obj->dispatch : NULL;
}

/**
 * 吉里吉里オブジェクトを squirrel に登録。
 * 登録済みクラスの場合はそのクラスのインスタンスとしてくみ上げる
 * @return 登録成功
 */
bool
TJSObject::pushDispatch(HSQUIRRELVM v, iTJSDispatch2 *dispatch)
{
	// インスタンスからクラス名を取得
	tTJSVariant val;
	if (TJS_SUCCEEDED(dispatch->ClassInstanceInfo(TJS_CII_GET, 0, &val))) {
		const tjs_char *className = val.GetString();
		// 合致するクラス名が登録済みか？
		sqobject::ObjectInfo info = classMap.get(className);
		if (info.isClass()) {
			// クラスを push
			info.push(v);
			if (SQ_SUCCEEDED(sq_createinstance(v, -1))) {
				// インスタンスを生成
				TJSObject *obj = new TJSObject(v, dispatch);
				if (SQ_SUCCEEDED(sq_setinstanceup(v, -1, obj))) {
					sq_setreleasehook(v, -1, release);
					sq_remove(v, -2);
					return true;
				} else {
					sq_pop(v,1);
					obj->dispatch = NULL;
					delete obj;
				}
			}
			sq_pop(v,1);
		}
	}
	return false;
}

// -----------------------------------------------------------------------

// コンストラクタ
TJSObject::TJSObject(HSQUIRRELVM v) : dispatch(NULL)
{
	initSelf(v);
}

// コンストラクタ。値を返す場合用
TJSObject::TJSObject(HSQUIRRELVM v, iTJSDispatch2 *dispatch) : dispatch(dispatch)
{
	initSelf(v, -1);
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
 * 引数2～ 引数
 * 自由変数1 クラス名
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
		TVPExecuteExpression(sqobject::getString(v,-1), &classObj);
		iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
		if (dispatch) {
			// 引数変換
			int argc = sq_gettop(v) - 2;
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

		// 引数変換
		int argc = sq_gettop(v) - 2;
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
		if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, sqobject::getString(v,-1), NULL, &result, argc, args, dispatch))) {
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
		if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v, -1), NULL, &result, dispatch))) {
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
		if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, sqobject::getString(v, -1), NULL, &result, dispatch))) {
			return SQ_OK;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * TJSオブジェクト用のメソッド
 * 引数1 オブジェクト
 * 引数2～ 引数
 * 自由変数1 クラス名
 * 自由変数2 メンバ名
 */
SQRESULT
TJSObject::tjsStaticInvoker(HSQUIRRELVM v)
{
	tTJSVariant classObj;
	TVPExecuteExpression(sqobject::getString(v,-2), &classObj);
	iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
	if (dispatch) {
		// 引数変換
		int argc = sq_gettop(v) - 3;
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
		if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, sqobject::getString(v,-1), NULL, &result, argc, args, dispatch))) {
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
 * 自由変数1 クラス名
 * 自由変数2 プロパティ名
 */
SQRESULT
TJSObject::tjsStaticGetter(HSQUIRRELVM v)
{
	tTJSVariant classObj;
	TVPExecuteExpression(sqobject::getString(v,-2), &classObj);
	iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
	if (dispatch) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v, -1), NULL, &result, dispatch))) {
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
 * 自由変数1 クラス名
 * 自由変数2 プロパティ名
 */
SQRESULT
TJSObject::tjsStaticSetter(HSQUIRRELVM v)
{
	tTJSVariant classObj;
	TVPExecuteExpression(sqobject::getString(v,-2), &classObj);
	iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
	if (dispatch) {
		tTJSVariant result;
		sq_getvariant(v, 2, &result);
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropSet(TJS_MEMBERENSURE, sqobject::getString(v, -1), NULL, &result, dispatch))) {
			return SQ_OK;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}
