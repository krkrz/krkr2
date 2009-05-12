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

// 型情報
static const SQChar *typeName = _SC("TJSObject"); ///< 型名
static const SQUserPointer TJSOBJTYPETAG = (SQUserPointer)_SC("TJSOBJTYPETAG"); ///< squirrel型タグ
static const SQChar *tjsClassAttrName = _SC("tjsClass");

/**
 * メンバ登録処理用
 */
class MemberRegister : public tTJSDispatch /** EnumMembers 用 */
{

public:
	// コンストラクタ
	MemberRegister(HSQUIRRELVM v, tTJSVariant &tjsClassObj) : v(v), tjsClassObj(tjsClassObj) {
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
				tTVInteger flag = param[1]->AsInteger();
				bool staticMember = (flag & TJS_STATICMEMBER) != 0;
				iTJSDispatch2 *member = param[2]->AsObjectNoAddRef();
				if (member) {
					if (TJS_SUCCEEDED(member->IsInstanceOf(0,NULL,NULL,L"Function",NULL))) {
						registFunction(name, staticMember);
					} else if (TJS_SUCCEEDED(member->IsInstanceOf(0,NULL,NULL,L"Property",NULL))) {
						registProperty(name, staticMember);
					}
				} else {
					registNull(name);
				}
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}

protected:

	// null 登録
	void registNull(const tjs_char *functionName) {
		sq_pushstring(v, functionName, -1);
		sq_pushnull(v);
		sq_createslot(v, -3);
	}

	// ファンクション登録
	void registFunction(const tjs_char *functionName, bool staticMember) {
		sq_pushstring(v, functionName, -1);
		sq_pushstring(v, functionName, -1);
		if (staticMember) {
			sq_pushvariant(v, tjsClassObj);
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
			sq_pushvariant(v, tjsClassObj);
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
			sq_pushvariant(v, tjsClassObj);
			sq_newclosure(v, TJSObject::tjsStaticGetter, 2);
		} else {
			sq_newclosure(v, TJSObject::tjsGetter, 1);
		}
		sq_createslot(v, -3);
	}
	
private:
	HSQUIRRELVM v;
	tTJSVariant &tjsClassObj;
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
	tTJSVariant tjsClassObj;
	for (int i=top;i>1;i--) {
		if ((tjsClassName = sqobject::getString(v,i))) {
			TVPExecuteExpression(tjsClassName, &tjsClassObj);
			if (tjsClassObj.Type() == tvtObject &&
				TJS_SUCCEEDED(tjsClassObj.AsObjectClosureNoAddRef().IsInstanceOf(0,NULL,NULL,L"Class",NULL))) {
				MemberRegister r(v, tjsClassObj);
				tTJSVariantClosure closure(&r);
				tjsClassObj.AsObjectClosureNoAddRef().EnumMembers(TJS_IGNOREPROP, &closure, NULL);
			}
		}
	}

	if (tjsClassName) {
		// コンストラクタ登録
		sq_pushstring(v, _SC("constructor"), -1);
		sq_pushvariant(v, tjsClassObj);
		sq_newclosure(v, tjsConstructor, 1);
		sq_createslot(v, -3);
		// クラス属性に tjsクラスを登録
		sq_pushnull(v);
		sq_newtable(v);
		if (SQ_SUCCEEDED(sq_setattributes(v,-3))) {
			sq_pop(v,1);
			sq_pushnull(v);
			if (SQ_SUCCEEDED(sq_getattributes(v, -2))) {
				top = sq_gettop(v);
				sq_pushstring(v, tjsClassAttrName, -1);
				sq_pushvariant(v, tjsClassObj);
				top = sq_gettop(v);
				if (SQ_SUCCEEDED(sq_createslot(v, -3))) {
					sq_pop(v,1);
				} else {
					sq_pop(v,2);
				}
			} else {
				// XXX
				sq_pop(v,1);
			}
		} else {
			// XXX
			sq_pop(v,2);
		}
		// 作成したクラス情報を tjsクラス名とあわせて記録
		classMap.create(tjsClassName, sqobject::ObjectInfo(v,-1));
	}

	return 1;
}

/**
 * squirrel から吉里吉里オブジェクトを取得
 */
bool
TJSObject::getVariant(HSQUIRRELVM v, int idx, tTJSVariant *variant)
{
	if (sq_gettype(v, idx) == OT_CLASS) {
		if (idx < 0) {
			idx = sq_gettop(v) + 1 + idx;
		}
		bool ret = false;
		// クラス属性からオブジェクト情報を引き出す
		sq_pushnull(v);
		if (SQ_SUCCEEDED(sq_getattributes(v, idx))) {
			sq_pushstring(v, tjsClassAttrName, -1);
			if (SQ_SUCCEEDED(sq_get(v,-2))) {
				if (SQ_SUCCEEDED(sq_getvariant(v,-1, variant))) {
					ret = true;
				}
				sq_pop(v,1);
			}
			sq_pop(v,1);
		} else {
			// XXX
			sq_pop(v,1);
		}
		return ret;
	} else if (sq_gettype(v, idx) == OT_INSTANCE) {
		TJSObject *obj = (TJSObject*)::getInstance(v, idx, typeName);
		if (obj) {
			*variant = obj->instance;
			return true;
		}
	}
	return false;
}

/**
 * 吉里吉里オブジェクトを squirrel に登録。
 * 登録済みクラスの場合はそのクラスのインスタンスとしてくみ上げる
 * @return 登録成功
 */
bool
TJSObject::pushVariant(HSQUIRRELVM v, tTJSVariant &variant)
{
	// インスタンスからクラス名を取得
	tTJSVariant val;
	iTJSDispatch2 *dispatch = variant.AsObjectNoAddRef();
	if (TJS_SUCCEEDED(dispatch->ClassInstanceInfo(TJS_CII_GET, 0, &val))) {
		const tjs_char *className = val.GetString();
		// 合致するクラス名が登録済みか？
		sqobject::ObjectInfo info = classMap.get(className);
		if (info.isClass()) {
			// クラスを push
			info.push(v);
			if (SQ_SUCCEEDED(sq_createinstance(v, -1))) {
				// インスタンスを生成
				TJSObject *obj = new TJSObject(v, variant);
				if (SQ_SUCCEEDED(sq_setinstanceup(v, -1, obj))) {
					sq_setreleasehook(v, -1, release);
					sq_remove(v, -2);
					return true;
				} else {
					sq_pop(v,1);
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
TJSObject::TJSObject(HSQUIRRELVM v)
{
	initSelf(v);
}

// コンストラクタ。値を返す場合用
TJSObject::TJSObject(HSQUIRRELVM v, tTJSVariant &instance) : instance(instance)
{
	initSelf(v, -1);
}

// デストラクタ
TJSObject::~TJSObject()
{
}

/**
 * オブジェクトのリリーサ
 */
SQRESULT
TJSObject::release(SQUserPointer up, SQInteger size)
{
	TJSObject *self = (TJSObject*)up;
	if (self) {
		self->destructor();
		delete self;
	}
	return SQ_OK;
}

/**
 * TJSオブジェクトのコンストラクタ
 * 引数1 オブジェクト
 * 引数2～ 引数
 * 自由変数1 TJSクラスオブジェクト
 */
SQRESULT
TJSObject::tjsConstructor(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	TJSObject *self = new TJSObject(v);
	if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
		sq_setreleasehook(v, 1, release);
		// クラスを生成する
		tTJSVariant tjsClassObj;
		if (SQ_SUCCEEDED(sq_getvariant(v, -1, &tjsClassObj)) && tjsClassObj.Type() == tvtObject) {
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
			tTJSVariantClosure &cls = tjsClassObj.AsObjectClosureNoAddRef();
			tjs_error error;
			iTJSDispatch2 *instance;
			if (TJS_SUCCEEDED(error = cls.CreateNew(0, NULL, NULL, &instance, argc, args, NULL))) {
				self->instance = tTJSVariant(instance, instance);
				instance->Release();
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
			return result;
		}
	} else {
		delete self;
	}
	return ERROR_CREATE(v);
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
	tTJSVariant instance;
	if (getVariant(v,1,&instance) && instance.Type() == tvtObject) {
		
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
		if (TJS_SUCCEEDED(error = instance.AsObjectClosureNoAddRef().FuncCall(0, sqobject::getString(v,-1), NULL, &result, argc, args, NULL))) {
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
	tTJSVariant instance;
	if (getVariant(v,1,&instance) && instance.Type() == tvtObject) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = instance.AsObjectClosureNoAddRef().PropGet(0, sqobject::getString(v, -1), NULL, &result, NULL))) {
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
	tTJSVariant instance;
	if (getVariant(v,1,&instance) && instance.Type() == tvtObject) {
		tTJSVariant result;
		sq_getvariant(v, 2, &result);
		tjs_error error;
		if (TJS_SUCCEEDED(error = instance.AsObjectClosureNoAddRef().PropSet(TJS_MEMBERENSURE, sqobject::getString(v, -1), NULL, &result, NULL))) {
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
 * 自由変数1 クラスオブジェクト
 * 自由変数2 メンバ名
 */
SQRESULT
TJSObject::tjsStaticInvoker(HSQUIRRELVM v)
{
	tTJSVariant tjsClassObj;
	if (SQ_SUCCEEDED(sq_getvariant(v, -2, &tjsClassObj)) && tjsClassObj.Type() == tvtObject) {
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
		if (TJS_SUCCEEDED(error = tjsClassObj.AsObjectClosureNoAddRef().FuncCall(0, sqobject::getString(v,-1), NULL, &result, argc, args, NULL))) {
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
 * 自由変数1 クラスオブジェクト
 * 自由変数2 プロパティ名
 */
SQRESULT
TJSObject::tjsStaticGetter(HSQUIRRELVM v)
{
	tTJSVariant tjsClassObj;
	if (SQ_SUCCEEDED(sq_getvariant(v, -2, &tjsClassObj)) && tjsClassObj.Type() == tvtObject) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = tjsClassObj.AsObjectClosureNoAddRef().PropGet(0, sqobject::getString(v, -1), NULL, &result, NULL))) {
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
 * 自由変数1 クラスオブジェクト
 * 自由変数2 プロパティ名
 */
SQRESULT
TJSObject::tjsStaticSetter(HSQUIRRELVM v)
{
	tTJSVariant tjsClassObj;
	if (SQ_SUCCEEDED(sq_getvariant(v, -2, &tjsClassObj)) && tjsClassObj.Type() == tvtObject) {
		tTJSVariant result;
		sq_getvariant(v, 2, &result);
		tjs_error error;
		if (TJS_SUCCEEDED(error = tjsClassObj.AsObjectClosureNoAddRef().PropSet(TJS_MEMBERENSURE, sqobject::getString(v, -1), NULL, &result, NULL))) {
			return SQ_OK;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}
