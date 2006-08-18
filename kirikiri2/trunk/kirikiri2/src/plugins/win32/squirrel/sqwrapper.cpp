#include <windows.h>
#include "tp_stub.h"
#include "sqplus.h"

void store(HSQUIRRELVM v, iTJSDispatch2 *dispatch);
void store(HSQUIRRELVM v, tTJSVariant &variant);
void store(tTJSVariant &result, HSQOBJECT &obj);
void store(tTJSVariant &result, HSQUIRRELVM v, int idx=-1);
void store(tTJSVariant &result, SquirrelObject &obj);

/**
 * IDispatch 用 iTJSDispatch2 ラッパー
 */
class iTJSDispatch2Wrapper : public tTJSDispatch
{
protected:
	/// 内部保持用
	HSQOBJECT obj;

public:
	/**
	 * コンストラクタ
	 * @param obj IDispatch
	 */
	iTJSDispatch2Wrapper(HSQOBJECT obj) : obj(obj) {
		sq_addref(SquirrelVM::GetVMPtr(), &obj);
	}
	
	/**
	 * デストラクタ
	 */
	~iTJSDispatch2Wrapper() {
		if(SquirrelVM::GetVMPtr()) {
			sq_release(SquirrelVM::GetVMPtr(), &obj);
		}
	}

public:

	tjs_error TJS_INTF_METHOD CreateNew(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		)
	{
		if (membername) {
			return TJS_E_MEMBERNOTFOUND;
		}
		if (obj._type == OT_CLASS) {
			int ret = S_FALSE;
			sq_pushobject(SquirrelVM::GetVMPtr(), obj);
			// this 相当部分
			sq_pushroottable(SquirrelVM::GetVMPtr());
			// パラメータ群
			for (int i=0;i<numparams;i++) {
				store(SquirrelVM::GetVMPtr(), *param[i]);
			}
			if (result) {
				// 帰り値がある場合
				if (SQ_SUCCEEDED(sq_call(SquirrelVM::GetVMPtr(), numparams + 1, SQTrue, SQFalse))) {
					ret = S_OK;
					{
						HSQOBJECT x;
						sq_resetobject(&x);
						sq_getstackobj(SquirrelVM::GetVMPtr(),-1,&x);
						*result = new iTJSDispatch2Wrapper(x);
					}
					sq_pop(SquirrelVM::GetVMPtr(), 1);
				}
			} else {
				if (SQ_SUCCEEDED(sq_call(SquirrelVM::GetVMPtr(), numparams + 1, SQTrue, SQFalse))) {
					ret = S_OK;
				}
			}
			sq_pop(SquirrelVM::GetVMPtr(), 1);
			return ret;
		}
		return TJS_E_NOTIMPL;
	}

	
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
			if (obj._type == OT_CLOSURE ||
				obj._type == OT_NATIVECLOSURE ||
				obj._type == OT_GENERATOR) {
				int ret = S_FALSE;
				sq_pushobject(SquirrelVM::GetVMPtr(), obj);
				// this 相当部分
				sq_pushroottable(SquirrelVM::GetVMPtr());
				// パラメータ群
				for (int i=0;i<numparams;i++) {
					store(SquirrelVM::GetVMPtr(), *param[i]);
				}
				if (result) {
					// 帰り値がある場合
					if (SQ_SUCCEEDED(sq_call(SquirrelVM::GetVMPtr(), numparams + 1, SQTrue, SQFalse))) {
						ret = S_OK;
						store(*result, SquirrelVM::GetVMPtr());
						sq_pop(SquirrelVM::GetVMPtr(), 1);
					}
				} else {
					if (SQ_SUCCEEDED(sq_call(SquirrelVM::GetVMPtr(), numparams + 1, SQTrue, SQFalse))) {
						ret = S_OK;
					}
				}
				sq_pop(SquirrelVM::GetVMPtr(), 1);
				return ret;
			}
			return TJS_E_NOTIMPL;
		} else {
			int ret = S_FALSE;
			sq_pushobject(SquirrelVM::GetVMPtr(), obj);
			sq_pushstring(SquirrelVM::GetVMPtr(), membername,-1);
			if(SQ_SUCCEEDED(sq_get(SquirrelVM::GetVMPtr(),-2))) {
				// this
				sq_pushobject(SquirrelVM::GetVMPtr(), obj);
				// パラメータ群
				for (int i=0;i<numparams;i++) {
					store(SquirrelVM::GetVMPtr(), *param[i]);
				}
				if (result) {
					// 帰り値がある場合
					if (SQ_SUCCEEDED(sq_call(SquirrelVM::GetVMPtr(), numparams + 1, SQTrue, SQFalse))) {
						ret = S_OK;
						store(*result, SquirrelVM::GetVMPtr());
						sq_pop(SquirrelVM::GetVMPtr(), 1);
					}
				} else {
					if (SQ_SUCCEEDED(sq_call(SquirrelVM::GetVMPtr(), numparams + 1, SQTrue, SQFalse))) {
						ret = S_OK;
					}
				}
			}
			sq_pop(SquirrelVM::GetVMPtr(), 1);
			return ret;
		}
	}

	tjs_error TJS_INTF_METHOD PropGet(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis
) {
		// プロパティはない
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		int ret = S_FALSE;
		sq_pushobject(SquirrelVM::GetVMPtr(), obj);
		sq_pushstring(SquirrelVM::GetVMPtr(), membername,-1);
		if(SQ_SUCCEEDED(sq_get(SquirrelVM::GetVMPtr(),-2))) {
			ret = S_OK;
			if (result) {
				store(*result, SquirrelVM::GetVMPtr());
			}	
			sq_pop(SquirrelVM::GetVMPtr(),1);
		}
		sq_pop(SquirrelVM::GetVMPtr(),1);
		return ret;
	}

	tjs_error TJS_INTF_METHOD PropSet(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis
		) {
		// プロパティはない
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		int ret = S_FALSE;
		sq_pushobject(SquirrelVM::GetVMPtr(), obj);
		sq_pushstring(SquirrelVM::GetVMPtr(), membername,-1);
		store(SquirrelVM::GetVMPtr(), (tTJSVariant&)*param);
		if ((flag & TJS_MEMBERENSURE)) {
			if(SQ_SUCCEEDED(sq_newslot(SquirrelVM::GetVMPtr(),-3, SQFalse))) {
				ret = S_OK;
			}
		} else {
			if(SQ_SUCCEEDED(sq_set(SquirrelVM::GetVMPtr(),-3))) {
				ret = S_OK;
			}
		}
		sq_pop(SquirrelVM::GetVMPtr(),1);
		return ret;
	}
};


//----------------------------------------------------------------------------
// iTJSDispatch2 を UserData として保持するための機構
//----------------------------------------------------------------------------

/**
 * iTJSDispatch2 用オブジェクト開放処理
 */
static SQInteger
tjsDispatchRelease(SQUserPointer up, SQInteger size)
{
	iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
	dispatch->Release();
	return 1;
}

/**
 * iTJSDispatch2 用プロパティの取得
 * @param v squirrel VM
 */
static SQInteger
get(HSQUIRRELVM v)
{
	StackHandler stack(v);
	SQUserPointer up = stack.GetUserData(1);
	if (up) {
		iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
		tTJSVariant result;
		if (SUCCEEDED(dispatch->PropGet(0, stack.GetString(2), NULL, &result, dispatch))) {
			store(v, result);
			return 1;
		}
	}
	return 0;
}

/**
 * iTJSDispatch2 用プロパティの設定
 * @param v squirrel VM
 */
static SQInteger
set(HSQUIRRELVM v)
{
	StackHandler stack(v);
	SQUserPointer up = stack.GetUserData(1);
	if (up) {
		iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
		tTJSVariant result;
		store(result, stack.GetObjectHandle(3));
		dispatch->PropSet(TJS_MEMBERENSURE, stack.GetString(2), NULL, &result, dispatch);
	}
	return 0;
}

/**
 * iTJSDispatch2 用メソッドまたはコンストラクタの呼び出し
 * @param v squirrel VM
 */
static SQInteger
call(HSQUIRRELVM v)
{
	// param1 オブジェクト
	// param2 オリジナルオブジェクト
	// param3 ～ 本来の引数ぽ

	StackHandler stack(v);
	SQUserPointer up = stack.GetUserData(1);
	int ret = 0;
	if (up) {
		iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);

		// this を取得
		iTJSDispatch2 *thisobj = NULL;
		up = stack.GetUserData(2);
		if (up) {
			thisobj = *((iTJSDispatch2**)up);
		}
		
		int argc = stack.GetParamCount() - 2;
		
		// 引数変換
		tTJSVariant **args = new tTJSVariant*[argc];
		for (int i=0;i<argc;i++) {
			args[i] = new tTJSVariant();
			store(*args[i], stack.GetObjectHandle(i+3));
		}

		if (dispatch->IsInstanceOf(0, NULL, NULL, L"Class", dispatch) == TJS_S_TRUE) {
			// クラスオブジェクトの場合コンストラクタとみなす
			iTJSDispatch2 *instance = NULL;
			if (SUCCEEDED(dispatch->CreateNew(0, NULL, NULL, &instance, argc, args, thisobj))) {
				store(v, instance);
				ret = 1;
			}
		} else {
			// メソッド呼び出し
			tTJSVariant result;
			if (SUCCEEDED(dispatch->FuncCall(0, NULL, NULL, &result, argc, args, thisobj))) {
				if (result.Type() != tvtVoid) {
					store(v, result);
					ret = 1;
				}
			}
		}
			
		// 引数破棄
		for (int i=0;i<argc;i++) {
			delete args[i];
		}
		delete[] args;
	}
	return ret;
}

/**
 * iTJSDispatch2 を squirrel の空間に投入する
 * @param v squirrel VM
 * @param dispatch iTJSDispatch2
 */
void
store(HSQUIRRELVM v, iTJSDispatch2 *dispatch)
{
	if (dispatch) {
		dispatch->AddRef();
		SQUserPointer up = sq_newuserdata(v, sizeof(iTJSDispatch2*));
		*((iTJSDispatch2**)up) = dispatch;
		
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
		sq_newclosure(v, call, 0);
		sq_createslot(v, -3);
		
		sq_setdelegate(v, -2);

	} else {
		sq_pushnull(v);
	}
}

/**
 * tTJSVariant を squirrel の空間に投入する
 * @param v squirrel VM
 * @param variant tTJSVariant
 */
void
store(HSQUIRRELVM v, tTJSVariant &variant)
{
	switch (variant.Type()) {
	case tvtVoid:
		sq_pushnull(v);
		break;
	case tvtObject:
		store(v, variant.AsObjectNoAddRef());
		break;
	case tvtString:
		sq_pushstring(v, variant.GetString(), -1);
		break;
	case tvtOctet: // XXX
		sq_pushnull(v);
		break;
	case tvtInteger:
		sq_pushinteger(v, (int)(variant));
		break;
	case tvtReal:
		sq_pushfloat(v, (float)(double)(variant));
		break;
	}
}


/**
 * squirrel のオブジェクトを tTJSVariant に変換する(値渡し）
 * @param result 結果格納先
 * @param stack squirrel のスタックのラッパー
 * @param idx スタックのどの部分を取得するかのインデックス指定
 */
void
store(tTJSVariant &result, HSQOBJECT &obj)
{
	result.Clear();
	switch (obj._type) {
	case OT_NULL:
		break;
	case OT_INTEGER:
		result = sq_objtointeger(&obj);
		break;
	case OT_FLOAT:
		result = (double)sq_objtofloat(&obj);
		break;
	case OT_BOOL:
		result = sq_objtobool(&obj) ? 1 : 0;
		break;
	case OT_STRING:
		result = sq_objtostring(&obj);
		break;
	case OT_USERDATA:
		{
		// XXX 型判定がいるのではないか？
			void *up = sq_objtouserdata(&obj);
			if (up) {
				result = *((iTJSDispatch2**)up);
			}
		}
		break;
	case OT_TABLE:
	case OT_ARRAY:
	case OT_CLOSURE:
	case OT_NATIVECLOSURE:
	case OT_GENERATOR:
	case OT_USERPOINTER:
	case OT_THREAD:
	case OT_CLASS:
	case OT_INSTANCE:
	case OT_WEAKREF:
		// ラッピングが必要!
		{
			iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(obj);
			result = tjsobj;
			tjsobj->Release();
		}
		break;
	}
}

void
store(tTJSVariant &result, HSQUIRRELVM v, int idx)
{
	HSQOBJECT x;
	sq_resetobject(&x);
	sq_getstackobj(v,idx,&x);
	store(result, x);
}

void
store(tTJSVariant &result, SquirrelObject &obj)
{
	store(result, obj.GetObjectHandle());
}

// -------------------------------------------------------

/**
 * Squirrel の グローバル空間にに登録処理を行う
 */
void registglobal(HSQUIRRELVM v, const SQChar *name, iTJSDispatch2 *dispatch)
{
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	store(v, dispatch);
	sq_createslot(v, -3); 
	sq_pop(v, 1);
}
