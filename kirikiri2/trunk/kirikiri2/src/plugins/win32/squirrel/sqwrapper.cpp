#include <windows.h>
#include "tp_stub.h"
#include "sqplus.h"

void store(HSQUIRRELVM v, iTJSDispatch2 *dispatch);
void store(HSQUIRRELVM v, tTJSVariant &variant);
void store(tTJSVariant &result, SquirrelObject &variant);
void store(tTJSVariant &result, StackHandler &stack, int idx=1);
void store(tTJSVariant &result, HSQUIRRELVM v, int idx=-1);



/**
 * 開放処理
 */
static SQInteger
tjsDispatchRelease(SQUserPointer up, SQInteger size)
{
	iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
	dispatch->Release();
	return 1;
}

/**
 * プロパティの設定
 */
static SQInteger
set(HSQUIRRELVM v)
{
	StackHandler stack(v);
	SQUserPointer up = stack.GetUserData(1);
	if (up) {
		iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
		tTJSVariant result;
		store(result, stack, 3);
		dispatch->PropSet(TJS_MEMBERENSURE, stack.GetString(2), NULL, &result, dispatch);
	}
	return 0;
}

/**
 * プロパティの取得
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
 * プロパティの削除
 */
static SQInteger
delslot(HSQUIRRELVM v)
{
	StackHandler stack(v);
	SQUserPointer up = stack.GetUserData(1);
	if (up) {
		iTJSDispatch2 *dispatch	= *((iTJSDispatch2**)up);
		dispatch->DeleteMember(0, stack.GetString(2), NULL, dispatch);
	}
	return 0;
}

/**
 * メソッドまたはコンストラクタの呼び出し
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
			store(*args[i], stack, i + 3);
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

		sq_pushstring(v, L"_set", -1);
		sq_newclosure(v, set, 0);
		sq_createslot(v, -3);

		sq_pushstring(v, L"_get", -1);
		sq_newclosure(v, get, 0);
		sq_createslot(v, -3);

		sq_pushstring(v, L"_newslot", -1);
		sq_newclosure(v, set, 0);
		sq_createslot(v, -3);
		
		sq_pushstring(v, L"_delslot", -1);
		sq_newclosure(v, delslot, 0);
		sq_createslot(v, -3);

		sq_pushstring(v, L"_call", -1);
		sq_newclosure(v, call, 0);
		sq_createslot(v, -3);
		
		sq_setdelegate(v, -2);

	} else {
		sq_pushnull(v);
	}
}

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
	case tvtOctet:
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

void
store(tTJSVariant &result, StackHandler &stack, int idx)
{
	result.Clear();
	switch (stack.GetType(idx)) {
	case OT_NULL:
		break;
	case OT_INTEGER:
		result = stack.GetInt(idx);
		break;
	case OT_FLOAT:
		result = (double)stack.GetFloat(idx);
		break;
	case OT_BOOL:
		result = stack.GetBool(idx) ? "1" : "0";
		break;
	case OT_STRING:
		result = stack.GetString(idx);
		break;
	case OT_USERDATA:
		{
			SQUserPointer up = stack.GetUserData(idx);
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
		break;
	}
}

void
store(tTJSVariant &result, HSQUIRRELVM v, int idx)
{
	store(result, StackHandler(v), idx);
}

void
store(tTJSVariant &result, SquirrelObject &variant)
{
	StackHandler stack(SquirrelVM::GetVMPtr());
	stack.Return(variant);
	store(result, stack);
}

// -------------------------------------------------------

void registglobal(HSQUIRRELVM v, const SQChar *name, iTJSDispatch2 *dispatch)
{
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	store(v, dispatch);
	sq_createslot(v, -3); 
    sq_pop(v, 1);
}
