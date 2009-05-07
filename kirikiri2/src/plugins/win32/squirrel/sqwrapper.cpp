/**
 * Squirrel ←→ 吉里吉里ブリッジ処理
 * 吉里吉里のオブジェクトは UserData として管理する
 */

#include <windows.h>
#include "tp_stub.h"
#include <squirrel.h>
#include "sqtjsobj.h"

// sqfunc.cpp
extern SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v);

// 外部参照

SQRESULT ERROR_KRKR(HSQUIRRELVM v, tjs_error error) {
	switch (error) {
	case TJS_E_MEMBERNOTFOUND:
		return sq_throwerror(v, _SC("member not found"));
	case TJS_E_NOTIMPL:
		return sq_throwerror(v, _SC("not implemented"));
	case TJS_E_INVALIDPARAM:
		return sq_throwerror(v, _SC("invalid param"));
	case TJS_E_BADPARAMCOUNT:
		return sq_throwerror(v, _SC("bad param count"));
	case TJS_E_INVALIDTYPE:
		return sq_throwerror(v, _SC("invalid type"));
	case TJS_E_INVALIDOBJECT:
		return sq_throwerror(v, _SC("invalid object"));
	case TJS_E_ACCESSDENYED:
		return sq_throwerror(v, _SC("access denyed"));
	case TJS_E_NATIVECLASSCRASH:
		return sq_throwerror(v, _SC("navive class crash"));
	default:
		return sq_throwerror(v, _SC("failed"));
	}
}

// 格納・取得用
void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);

/**
 * エラー処理
 */
void
SQEXCEPTION(HSQUIRRELVM v)
{
	sq_getlasterror(v);
	const SQChar *str;
	sq_getstring(v, -1, &str);
	ttstr error = str;
	sq_pop(v, 1);
	TVPThrowExceptionMessage(error.c_str());
}

/**
 * HSQOBJECT 用 iTJSDispatch2 ラッパー
 */
class iTJSDispatch2Wrapper : public tTJSDispatch
{
protected:
	/// 内部保持用
	HSQUIRRELVM v;
	HSQOBJECT obj;

public:
	/**
	 * コンストラクタ
	 * @param obj IDispatch
	 */
	iTJSDispatch2Wrapper(HSQUIRRELVM v, int idx) : v(v) {
		sq_resetobject(&obj);
		sq_getstackobj(v,idx,&obj);
		sq_addref(v, &obj);
	}
	
	/**
	 * デストラクタ
	 */
	~iTJSDispatch2Wrapper() {
		if(v) {
			sq_release(v, &obj);
		}
	}

public:
	// オブジェクト生成
	tjs_error TJS_INTF_METHOD CreateNew(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis) {
		if (membername) {
			return TJS_E_MEMBERNOTFOUND;
		}
		if (obj._type != OT_CLASS) {
			return TJS_E_NOTIMPL;
		}
		int ret = S_FALSE;
		sq_pushobject(v, obj);
		sq_pushroottable(v);			// this 相当部分
		for (int i=0;i<numparams;i++) {	// パラメータ群
			sq_pushvariant(v, *param[i]);
		}
		if (SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
			if (result) {
				tTJSVariant var;
				sq_getvariant(v, -1, &var);
				sq_pop(v, 1); // newobj
				*result = var;
			}
			sq_pop(v, 1); // obj
		} else {
			sq_pop(v, 1); // obj
			SQEXCEPTION(v);
		}
		return TJS_S_OK;
	}

	// オブジェクト機能呼び出し
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		) {
		if (membername == NULL) {
			if (!(obj._type == OT_CLOSURE ||
				  obj._type == OT_NATIVECLOSURE ||
				  obj._type == OT_GENERATOR)) {
				return TJS_E_NOTIMPL;
			}
			sq_pushobject(v, obj);
			sq_pushroottable(v);
			for (int i=0;i<numparams;i++) {	// パラメータ群
				sq_pushvariant(v, *param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
				if (result) {
					sq_getvariant(v, -1, result);
					sq_pop(v, 1);
				}
				sq_pop(v, 1);
			} else {
				sq_pop(v, 1);
				SQEXCEPTION(v);
			}
			return TJS_S_OK;
		} else {
			sq_pushobject(v, obj);
			sq_pushstring(v, membername,-1);
			if (SQ_SUCCEEDED(sq_get(v,-2))) {
				sq_pushobject(v, obj); // this
				for (int i=0;i<numparams;i++) {	// パラメータ群
					sq_pushvariant(v, *param[i]);
				}
				// 帰り値がある場合
				if (SQ_SUCCEEDED(sq_call(v, numparams + 1, result ? SQTrue:SQFalse, SQTrue))) {
					if (result) {
						sq_getvariant(v, -1, result);
						sq_pop(v, 1);
					}
					sq_pop(v, 2);
				} else {
					sq_pop(v, 2);
					SQEXCEPTION(v);
				}
			} else {
				sq_pop(v, 1);
				return TJS_E_MEMBERNOTFOUND;
			}
			return TJS_S_OK;
		}
	}

	// プロパティ取得
	tjs_error TJS_INTF_METHOD PropGet(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis) {
		// プロパティはない
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		sq_pushobject(v, obj);
		sq_pushstring(v, membername,-1);
		if (SQ_SUCCEEDED(sq_get(v,-2))) {
			if (result) {
				sq_getvariant(v, -1, result);
			}
			sq_pop(v,2);
		} else {
			sq_pop(v,1);
			return TJS_E_MEMBERNOTFOUND;
		}
		return TJS_S_OK;
	}

	// プロパティ設定
	tjs_error TJS_INTF_METHOD PropSet(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis) {
		// プロパティはない
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		sq_pushobject(v, obj);
		sq_pushstring(v, membername,-1);
		sq_pushvariant(v, (tTJSVariant&)*param);
		if ((flag & TJS_MEMBERENSURE)) {
			if (SQ_SUCCEEDED(sq_newslot(v,-3, SQFalse))) {
				sq_pop(v,1);
			} else {
				sq_pop(v,1);
				SQEXCEPTION(v);
			}
		} else {
			if (SQ_SUCCEEDED(sq_set(v,-3))) {
				sq_pop(v,1);
			} else {
				sq_pop(v,1);
				SQEXCEPTION(v);
			}
		}
		return TJS_S_OK;
	}
};


//----------------------------------------------------------------------------
// iTJSDispatch2 を UserData として保持するための機構
//----------------------------------------------------------------------------

/**
 * iTJSDispatch2 用オブジェクト開放処理
 */
static SQRESULT
tjsDispatchRelease(SQUserPointer up, SQInteger size)
{
	iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
	dispatch->Release();
	return 1;
}

static const SQUserPointer TJSTYPETAG = (SQUserPointer)"TJSTYPETAG";

// iTJSDispatch2* 取得用
static iTJSDispatch2* GetDispatch(HSQUIRRELVM v, int idx)
{
	SQUserPointer otag;
	SQUserPointer up;
	if (SQ_SUCCEEDED(sq_getuserdata(v,idx,&up,&otag)) && otag == TJSTYPETAG) {
		return *((iTJSDispatch2**)up);
	}
	return NULL;
}

/**
 * iTJSDispatch2 用プロパティの取得
 * @param v squirrel VM
 */
static SQRESULT
get(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch = GetDispatch(v, 1);
	if (dispatch) {
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->PropGet(0, sqobject::getString(v, 2), NULL, &result, dispatch))) {
			sq_pushvariant(v, result);
			return 1;
		} else {
			return ERROR_KRKR(v, error);
		}
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * iTJSDispatch2 用プロパティの設定
 * @param v squirrel VM
 */
static SQRESULT
set(HSQUIRRELVM v)
{
	iTJSDispatch2 *dispatch	= GetDispatch(v, 1);
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

/**
 * iTJSDispatch2 用コンストラクタの呼び出し
 * @param v squirrel VM
 */
static SQRESULT
callConstructor(HSQUIRRELVM v)
{
	// param1 オブジェクト
	// param2 オリジナルオブジェクト
	// param3 ～ 本来の引数ぽ

	iTJSDispatch2 *dispatch	= GetDispatch(v, 1);
	if (dispatch) {
		
		// this を取得
		iTJSDispatch2 *thisobj = GetDispatch(v, 2);

		int argc = sq_gettop(v) - 2;
		
		// 引数変換
		tTJSVariant **args = new tTJSVariant*[argc];
		for (int i=0;i<argc;i++) {
			args[i] = new tTJSVariant();
			sq_getvariant(v, i+3, args[i]);
		}

		int ret = 0;
		iTJSDispatch2 *instance = NULL;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->CreateNew(0, NULL, NULL, &instance, argc, args, thisobj))) {
			tTJSVariant var(instance, instance);
			sq_pushvariant(v, var);
			instance->Release();
			ret = 1;
		} else {
			ret = ERROR_KRKR(v, error);
		}
			
		// 引数破棄
		for (int i=0;i<argc;i++) {
			delete args[i];
		}
		delete[] args;

		return ret;
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * iTJSDispatch2 用メソッドの呼び出し
 * @param v squirrel VM
 */
static SQRESULT
callMethod(HSQUIRRELVM v)
{
	// param1 オブジェクト
	// param2 オリジナルオブジェクト
	// param3 ～ 本来の引数ぽ

	iTJSDispatch2 *dispatch	= GetDispatch(v, 1);
	if (dispatch) {

		// this を取得
		iTJSDispatch2 *thisobj = GetDispatch(v, 2);
		
		int argc = sq_gettop(v) - 2;
		
		// 引数変換
		tTJSVariant **args = new tTJSVariant*[argc];
		for (int i=0;i<argc;i++) {
			args[i] = new tTJSVariant();
			sq_getvariant(v, i+3, args[i]);
		}

		// メソッド呼び出し
		int ret = 0;
		tTJSVariant result;
		tjs_error error;
		if (TJS_SUCCEEDED(error = dispatch->FuncCall(0, NULL, NULL, &result, argc, args, thisobj))) {
			if (result.Type() != tvtVoid) {
				sq_pushvariant(v, result);
				ret = 1;
			} else {
				ret = 0;
			}
		} else {
			ret = ERROR_KRKR(v, error);
		}
			
		// 引数破棄
		for (int i=0;i<argc;i++) {
			delete args[i];
		}
		delete[] args;

		return ret;
	}
	return ERROR_BADINSTANCE(v);
}

/**
 * tTJSVariant を squirrel の空間に投入する
 * @param v squirrel VM
 * @param variant tTJSVariant
 */
void
sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant)
{
	switch (variant.Type()) {
	case tvtVoid:
		sq_pushnull(v);
		break;
	case tvtObject:
		{
			iTJSDispatch2 *dispatch = variant.AsObject();
			if (dispatch) {
				if (!TJSObject::pushDispatch(v, dispatch)) {
					// UserData 確保
					SQUserPointer up = sq_newuserdata(v, sizeof *dispatch);
					*((iTJSDispatch2**)up) = dispatch;
					
					// タグ登録
					sq_settypetag(v, -1, TJSTYPETAG);
					
					// 開放ロジックを追加
					sq_setreleasehook(v, -1, tjsDispatchRelease);
					
					// メソッド群を追加
					sq_newtable(v);
					
					sq_pushstring(v, L"_get", -1);
					sq_newclosure(v, get, 0);
					sq_createslot(v, -3);
					
					sq_pushstring(v, L"_set", -1);
					sq_newclosure(v, set, 0);
					sq_createslot(v, -3);
					
					sq_pushstring(v, L"_call", -1);
					if (dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch) == TJS_S_TRUE) {
						sq_newclosure(v, callConstructor, 0);
					} else {
						sq_newclosure(v, callMethod, 0);
					}
					sq_createslot(v, -3);
					
					sq_setdelegate(v, -2);
				}
			} else {
				sq_pushnull(v);
			}
		}
		break;
	case tvtString:
		sq_pushstring(v, variant.GetString(), -1);
		break;
	case tvtOctet: // XXX
		sq_pushnull(v);
		break;
	case tvtInteger:
		sq_pushinteger(v, (SQInteger)(variant));
		break;
	case tvtReal:
		sq_pushfloat(v, (SQFloat)(double)(variant));
		break;
	}
}

static void wrap(HSQUIRRELVM v, int idx, tTJSVariant *result)
{
	// ラッピング
	iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(v, idx);
	if (tjsobj) {
		*result = tTJSVariant(tjsobj, tjsobj);
		tjsobj->Release();
	}
}

/**
 * tTJSVariant を squirrel の空間から取得する
 * @param v squirrel VM
 * @param idx スタックのインデックス
 * @param result tTJSVariant を返す先
 */
SQRESULT
sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result)
{
	if (result) {
		switch (sq_gettype(v, idx)) {
		case OT_NULL: result->Clear(); break;
		case OT_INTEGER: { SQInteger i; sq_getinteger(v, idx, &i);	*result = (tjs_int64)i; } break;
		case OT_FLOAT:   { SQFloat f; sq_getfloat(v, idx, &f); 	    *result = (double)f; } break;
		case OT_BOOL:    { SQBool b; sq_getbool(v, idx, &b);        *result = b != SQFalse; } break;
		case OT_STRING:  { const SQChar *c; sq_getstring(v, idx, &c); *result = c; } break;
		case OT_USERDATA:
			{
				iTJSDispatch2 *dispatch = GetDispatch(v, idx);
				if (dispatch) {
					// 元々吉里吉里側から渡されたデータ
					*result = tTJSVariant(dispatch, dispatch);
				} else {
					wrap(v, idx, result);
				}
			}
			break;
		case OT_INSTANCE:
			{
				// TJSベースインスタンスだった場合
				iTJSDispatch2 *dispatch = TJSObject::getDispatch(v, idx);
				if (dispatch) {
					// TJSベースのインスタンス
					*result = tTJSVariant(dispatch, dispatch);
				} else {
					wrap(v, idx, result);
				}
				break;
			}
			// through down
		case OT_TABLE:
		case OT_ARRAY:
		case OT_CLOSURE:
		case OT_NATIVECLOSURE:
		case OT_GENERATOR:
		case OT_USERPOINTER:
		case OT_THREAD:
		case OT_CLASS:
		case OT_WEAKREF:
			wrap(v, idx, result);
			break;
		default:
			result->Clear();
		}
		return SQ_OK;
	}
	return SQ_ERROR;
}
