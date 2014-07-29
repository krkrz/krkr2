#include "tjsobj.h"
extern Local<Value> toJSValue(Isolate *, const tTJSVariant &variant);
extern tTJSVariant toVariant(Isolate *, Local<Value> &value);

/**
 * Javascriptに対してエラー通知
 */
Local<Value>
ERROR_KRKR(Isolate *isolate, tjs_error error)
{
	EscapableHandleScope handle_scope(isolate);
	Local<Value> ret;
	switch (error) {
	case TJS_E_MEMBERNOTFOUND:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "member not found"));
		break;
	case TJS_E_NOTIMPL:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "not implemented"));
		break;
	case TJS_E_INVALIDPARAM:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "invalid param"));
		break;
	case TJS_E_BADPARAMCOUNT:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "bad param count"));
		break;
	case TJS_E_INVALIDTYPE:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "invalid type"));
		break;
	case TJS_E_INVALIDOBJECT:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "invalid object"));
		break;
	case TJS_E_ACCESSDENYED:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "access denyed"));
		break;
	case TJS_E_NATIVECLASSCRASH:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "navive class crash"));
		break;
	default:
		ret = isolate->ThrowException(String::NewFromUtf8(isolate, "failed"));
		break;
	}
	return handle_scope.Escape(ret);
}

Local<Value>
ERROR_BADINSTANCE(Isolate *isolate)
{
	EscapableHandleScope handle_scope(isolate);
	return handle_scope.Escape(isolate->ThrowException(String::NewFromUtf8(isolate, "bad instance")));
}

//----------------------------------------------------------------------------
// tTJSVariantをJSオブジェクトとして保持するための機構
//----------------------------------------------------------------------------

Persistent<ObjectTemplate> TJSObject::objectTemplate;

// オブジェクト定義初期化
void
TJSObject::init(Isolate *isolate)
{
	HandleScope handle_scope(isolate);
	Local<ObjectTemplate> obj = ObjectTemplate::New(isolate);
	obj->SetNamedPropertyHandler(getter, setter);
	obj->SetCallAsFunctionHandler(caller);
	objectTemplate.Reset(isolate, obj);
}

// オブジェクト定義解放
void
TJSObject::done(Isolate *isolate)
{
	objectTemplate.Reset();
}

// コンストラクタ
TJSObject::TJSObject(Isolate *isolate, Local<Object> &obj, const tTJSVariant &variant) : TJSBase(variant)
{
	HandleScope handle_scope(isolate);
	wrap(isolate, obj);
	Persistent<Object> ref(isolate, obj);
	ref.SetWeak(this, release); 
}

// プロパティの取得
void
TJSObject::getter(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
	Isolate *isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	String::Value propName(property);
	if (wcscmp(*propName, TJSINSTANCENAME) == 0) {
		return;
	}
	tTJSVariant self;
	if (getVariant(isolate, self, info.This())) {
		tjs_error error;
		tTJSVariant result;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropGet(0, *propName, NULL, &result, NULL))) {
			info.GetReturnValue().Set(toJSValue(isolate, result));
		} else {
			info.GetReturnValue().Set(ERROR_KRKR(isolate, error));
		}
		return;
	}
	info.GetReturnValue().Set(ERROR_BADINSTANCE(isolate));
}

// プロパティの設定
void
TJSObject::setter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<Value>& info)
{
	Isolate *isolate = info.GetIsolate();
	HandleScope handle_scope(isolate);
	tTJSVariant self;
	if (getVariant(isolate, self, info.This())) {
		String::Value propName(property);
		tTJSVariant param = toVariant(isolate, value);
		tjs_error error;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropSet(TJS_MEMBERENSURE, *propName, NULL, &param, NULL))) {
		} else {
			info.GetReturnValue().Set(ERROR_KRKR(isolate, error));
		}
		return;
	}
	info.GetReturnValue().Set(ERROR_BADINSTANCE(isolate));
}

// メソッドの呼び出し
void
TJSObject::caller(const FunctionCallbackInfo<Value>& args)
{
	Isolate *isolate = args.GetIsolate();
	HandleScope handle_scope(isolate);
	tTJSVariant self;
	if (getVariant(isolate, self, args.This())) {
		Local<Value> ret;
		
		// 引数変換
		tjs_int argc = args.Length();
		tTJSVariant **argv = new tTJSVariant*[argc];
		for (tjs_int i=0;i<argc;i++) {
			argv[i] = new tTJSVariant();
			*argv[i] = toVariant(isolate, args[i]);
		}

		if (self.AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Class", NULL) == TJS_S_TRUE) {
			// クラスオブジェクトならコンストラクタ呼び出し
			iTJSDispatch2 *instance = NULL;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().CreateNew(0, NULL, NULL, &instance, argc, argv, NULL))) {
				ret = toJSValue(isolate, tTJSVariant(instance, instance));
				instance->Release();
			} else {
				ret = ERROR_KRKR(isolate, error);
			}
		} else {
			// メソッド呼び出し
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().FuncCall(0, NULL, NULL, &result, argc, argv, NULL))) {
				ret = toJSValue(isolate, result);
			} else {
				ret = ERROR_KRKR(isolate, error);
			}
		}

		// 引数解放
		if (argv) {
			for (int i=0;i<argc;i++) {
				delete argv[i];
			}
			delete[] argv;
		}
		
		args.GetReturnValue().Set(ret);
		return;
	}
	args.GetReturnValue().Set(ERROR_BADINSTANCE(isolate));
}

// tTJSVariant をオブジェクト化
Local<Object>
TJSObject::toJSObject(Isolate *isolate, const tTJSVariant &variant)
{
	EscapableHandleScope handle_scope(isolate);
	Local<ObjectTemplate> templ = Local<ObjectTemplate>::New(isolate, objectTemplate);
	Local<Object> obj = templ->NewInstance();
	new TJSObject(isolate, obj, variant);
	return handle_scope.Escape(obj);
}
