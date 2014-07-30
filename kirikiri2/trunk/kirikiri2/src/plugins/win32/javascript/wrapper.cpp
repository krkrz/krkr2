/**
 * Javascript ←→ 吉里吉里ブリッジ処理
 * 吉里吉里のオブジェクトは XXXX として管理する
 */

#include "tjsobj.h"
#include "tjsinstance.h"

// 値の格納・取得用
Local<Value> toJSValue(Isolate *isolate, const tTJSVariant &variant);
tTJSVariant toVariant(Isolate *isolate, Local<Value> &value);

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
	iTJSDispatch2Wrapper(Isolate *isolate, Local<Object> &obj) : isolate(isolate) {
		this->obj.Reset(isolate,obj);
	}
	
	/**
	 * デストラクタ
	 */
	~iTJSDispatch2Wrapper() {
		obj.Reset();
	}

	/**
	 * 保持してる値を返す
	 */
	Local<Object> getObject() {
		return Local<Object>::New(isolate, obj);
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
		HandleScope handle_scope(isolate);
		return TJSInstance::createMethod(isolate, getObject(), membername, result, numparams, param);
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
		HandleScope handle_scope(isolate);
		return TJSInstance::callMethod(isolate, getObject(), membername, result, numparams, param, objthis);
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
		HandleScope handle_scope(isolate);
		return TJSInstance::getProp(isolate, getObject(), membername, result);
	}

	// プロパティ設定
	tjs_error TJS_INTF_METHOD PropSet(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis) {
		HandleScope handle_scope(isolate);
		return TJSInstance::setProp(isolate, getObject(), membername, param);
	}

	// メンバ削除
	tjs_error TJS_INTF_METHOD DeleteMember(
		tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis) {
		HandleScope handle_scope(isolate);
		return TJSInstance::remove(isolate, getObject(), membername);
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
	Isolate *isolate;
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
toJSValue(Isolate *isolate, const tTJSVariant &variant)
{
	switch (variant.Type()) {
	case tvtVoid:
		return Undefined(isolate);
	case tvtObject:
		{
			iTJSDispatch2 *obj = variant.AsObjectNoAddRef();
			if (obj == NULL) {
				// NULLの処理
				return Null(isolate);
			} else if (obj->IsInstanceOf(0, NULL, NULL, JSOBJECTCLASS, obj) == TJS_S_TRUE) {
				// Javascript ラッピングオブジェクトの場合
				iTJSDispatch2Wrapper *wobj = (iTJSDispatch2Wrapper*)obj;
				return wobj->getObject();
			} else {
				Local<Object> result;
				if (TJSInstance::getJSObject(result, variant)) {
					// 登録済みインスタンスの場合
					return result;
				}
				// 単純ラッピング
				return TJSObject::toJSObject(isolate, variant);
			}
		}
		break;
	case tvtString:
		{
			const tjs_char *str = variant.GetString();
			return String::NewFromTwoByte(isolate, str ? str : L"");
		}
	case tvtOctet:
		return Null(isolate);
	case tvtInteger:
	case tvtReal:
		return Number::New(isolate, (tTVReal)variant);
	}
	return Undefined(isolate);
}

tTJSVariant
toVariant(Isolate *isolate, Local<Object> &object, Local<Object> &context)
{
	tTJSVariant result;
	iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(isolate, object);
	iTJSDispatch2 *tjsctx = new iTJSDispatch2Wrapper(isolate, context);
	if (tjsobj && tjsctx) {
		result = tTJSVariant(tjsobj, tjsctx);
		tjsobj->Release();
		tjsctx->Release();
	} else {
		if (tjsobj) { tjsobj->Release(); };
		if (tjsctx) { tjsctx->Release(); };
	}
	return result;
}

tTJSVariant
toVariant(Isolate *isolate, Local<Object> &object)
{
	tTJSVariant result;
	iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(isolate, object);
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
toVariant(Isolate *isolate, Local<Value> &value)
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
	} else if (value->IsFunction() || value->IsArray() || value->IsDate()) {
		// 単純ラッピング
		result = toVariant(isolate, value->ToObject());
	} else if (value->IsObject()) {
		HandleScope handle_scope(isolate);
		Local<Object> obj = value->ToObject();
		if (!TJSBase::getVariant(isolate, result, obj)) {
			// 単純ラッピング
			result = toVariant(isolate, obj);
		}
	} else if (value->IsBoolean()) {
		result = value->BooleanValue();
	} else if (value->IsNumber()) {
		result = value->NumberValue();
	} else if (value->IsInt32()) {
		result = value->Int32Value();
	} else if (value->IsUint32()) {
		result = (tTVInteger)value->Uint32Value();
	}
	// value->IsUndefined()
	// value->IsExternal()
	return result;
}
