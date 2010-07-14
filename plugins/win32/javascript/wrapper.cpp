/**
 * Javascript ←→ 吉里吉里ブリッジ処理
 * 吉里吉里のオブジェクトは XXXX として管理する
 */

#include <windows.h>
#include "tp_stub.h"
#include <v8.h>
#include "tjsobj.h"

using namespace v8;

extern Persistent<Context> mainContext;

/**
 * 吉里吉里に対して例外通知
 */
void
JSEXCEPTION(TryCatch *try_catch)
{
	String::Value exception(try_catch->Exception());
	TVPThrowExceptionMessage(*exception);
}

// 値の格納・取得用
Local<Value> toJSValue(const tTJSVariant &variant);
tTJSVariant toVariant(Handle<Value> value);

#define JSOBJECTCLASS L"JavascriptObject"

/**
 * Javascript object 用 iTJSDispatch2 ラッパー
 */
class iTJSDispatch2Wrapper : public tTJSDispatch
{
public:
	/**
	 * コンストラクタ
	 * @param obj IDispatch
	 */
	iTJSDispatch2Wrapper(Handle<Object> obj) {
		this->obj = Persistent<Object>::New(obj);
	}
	
	/**
	 * デストラクタ
	 */
	~iTJSDispatch2Wrapper() {
		obj.Dispose();
	}

	/**
	 * 保持してる値を返す
	 */
	void getValue(Handle<Value> &result) {
		result = obj;
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
		if (!obj->IsFunction()) {
			return TJS_E_NOTIMPL;
		}

		HandleScope handle_scope;
		Context::Scope context_scope(mainContext);
		TryCatch try_catch;
		
		// 関数抽出
		Local<Function> func = Local<Function>::Cast(obj->ToObject());
		// 引数
		Handle<Value> *argv = new Handle<Value>[numparams];
		for (int i=0;i<numparams;i++) {
			argv[i] = toJSValue(*param[i]);
		}
		Handle<Object> ret = func->NewInstance(numparams, argv);
		delete argv;
		
		if (ret.IsEmpty()) {
			JSEXCEPTION(&try_catch);
		} else {
			if (result) {
				*result = toVariant(ret);
			}
		}
		return TJS_S_OK;
	}

	// メソッド呼び出し
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		) {

		HandleScope handle_scope;
		Context::Scope context_scope(mainContext);
		TryCatch try_catch;

		if (membername == NULL) {
			return callMethod(obj->ToObject(), result, numparams, param);
		} else {
			return callMethod(obj->Get(String::New(membername)), result, numparams, param);
		}
	}

	// プロパティ取得
	tjs_error TJS_INTF_METHOD PropGet(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis) {
		if (!membername) {
			return TJS_E_NOTIMPL;
		}

		HandleScope handle_scope;
		Context::Scope context_scope(mainContext);
		TryCatch try_catch;

		Local<Value> ret = obj->Get(String::New(membername));
		if (ret.IsEmpty()) {
			return TJS_E_MEMBERNOTFOUND;
		} else {
			if (result) {
				*result = toVariant(ret);
			}
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
		if (!membername) {
			return TJS_E_NOTIMPL;
		}

		HandleScope handle_scope;
		Context::Scope context_scope(mainContext);
		TryCatch try_catch;

		if (obj->Set(String::New(membername), toJSValue(*param))) {
			return TJS_S_OK;
		}
		return TJS_E_MEMBERNOTFOUND;
	}

	tjs_error TJS_INTF_METHOD IsInstanceOf(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		const tjs_char * classname,
		iTJSDispatch2 *objthis
		) {
		if (membername == NULL && wcscmp(classname, JSOBJECTCLASS) == 0) {
			return TJS_S_TRUE;
		}
		return TJS_S_FALSE;
	}

protected:
	/// 内部保持用
	Persistent<Object> obj;

	// メソッド呼び出し共通処理
	static tjs_error callMethod(Local<Value> method, tTJSVariant *result, tjs_int numparams, tTJSVariant **param) {
		if (!method->IsFunction()) {
			return TJS_E_NOTIMPL;
		}
		
		HandleScope handle_scope;
		Context::Scope context_scope(mainContext);
		TryCatch try_catch;
		
		// 関数抽出
		Local<Function> func = Local<Function>::Cast(method);
		// 引数
		Handle<Value> *argv = new Handle<Value>[numparams];
		for (int i=0;i<numparams;i++) {
			argv[i] = toJSValue(*param[i]);
		}
		Handle<Value> ret = func->Call(mainContext->Global(), numparams, argv);
		delete argv;
		
		if (ret.IsEmpty()) {
			JSEXCEPTION(&try_catch);
		} else {
			if (result) {
				*result = toVariant(ret);
			}
		}
		return TJS_S_OK;
	}
};

//----------------------------------------------------------------------------
// 変換用
//----------------------------------------------------------------------------

/**
 * tTJSVariant を squirrel の空間に投入する
 * @param result javascrpt value
 * @param variant tTJSVariant
 */
Local<Value>
toJSValue(const tTJSVariant &variant)
{
	switch (variant.Type()) {
	case tvtVoid:
		return *Undefined();
	case tvtObject:
		{
			iTJSDispatch2 *obj = variant.AsObjectNoAddRef();
			if (obj == NULL) {
				// NULLの処理
				return *Null();
//			} else if (TJSInstance::getJSValue(result, variant)) {
			} else {
				// オブジェクトラッピング処理失敗の場合
				if (obj->IsInstanceOf(0, NULL, NULL, JSOBJECTCLASS, obj) == TJS_S_TRUE) {
					// Javascript ラッピングオブジェクトの場合
					iTJSDispatch2Wrapper *wobj = (iTJSDispatch2Wrapper*)obj;
					Local<Value> result;
					wobj->getValue(result);
					return result;
				} else {
					return TJSObject::toJSObject(variant);
				}
			}
		}
		break;
	case tvtString:
		return String::New(variant.GetString(), -1);
	case tvtOctet:
		return *Null();
	case tvtInteger:
	case tvtReal:
		return Number::New((tTVReal)variant);
	}
	return *Undefined();
}

tTJSVariant
toVariant(Handle<Object> object)
{
	tTJSVariant result;
	iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(object);
	if (tjsobj) {
		result = tTJSVariant(tjsobj, tjsobj);
		tjsobj->Release();
	}
	return result;
}

/**
 * javascript値を tTJSVariant に変換する
 * @param value Javascript値
 * @return tTJSVariant
 */
tTJSVariant
toVariant(Handle<Value> value)
{
	tTJSVariant result;
	if (value->IsNull()) {
		result = (iTJSDispatch2*)0;
	} else if (value->IsTrue()) {
		result = true;
	} else if (value->IsFalse()) {
		result = false;
	} else if (value->IsString()) {
		String::Value str(value);
		result = *str;
	} else if (value->IsFunction()) {
		result = toVariant(value->ToObject());
	} else if (value->IsArray()) {
		result = toVariant(value->ToObject());
	} else if (value->IsObject()) {
		Local<Object> obj = value->ToObject();
		if (!TJSObject::getVariant(result, obj) /**&& !TJSInstance::getVariant(result,obj)*/) {
			result = toVariant(obj);
		}
	} else if (value->IsBoolean()) {
		result = value->BooleanValue();
	} else if (value->IsNumber()) {
		result = value->NumberValue();
	} else if (value->IsInt32()) {
		result = value->Int32Value();
	} else if (value->IsUint32()) {
		result = (tTVInteger)value->Uint32Value();
	} else if (value->IsDate()) {
		result = (iTJSDispatch2*)0;
	}
	// value->IsUndefined()
	// value->IsExternal()
	return result;
}
