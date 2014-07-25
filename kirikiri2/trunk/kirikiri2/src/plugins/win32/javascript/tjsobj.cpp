#include "tjsobj.h"
extern Local<Value> toJSValue(Isolate *, const tTJSVariant &variant);
extern tTJSVariant toVariant(Isolate *, Handle<Value> value);

/**
 * Javascriptに対してエラー通知
 */
Handle<Value>
ERROR_KRKR(Isolate *isolate, tjs_error error)
{
	switch (error) {
	case TJS_E_MEMBERNOTFOUND:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "member not found"));
	case TJS_E_NOTIMPL:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "not implemented"));
	case TJS_E_INVALIDPARAM:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "invalid param"));
	case TJS_E_BADPARAMCOUNT:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "bad param count"));
	case TJS_E_INVALIDTYPE:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "invalid type"));
	case TJS_E_INVALIDOBJECT:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "invalid object"));
	case TJS_E_ACCESSDENYED:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "access denyed"));
	case TJS_E_NATIVECLASSCRASH:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "navive class crash"));
	default:
		return isolate->ThrowException(String::NewFromUtf8(isolate, "failed"));
	}
}

Handle<Value>
ERROR_BADINSTANCE(Isolate *isolate)
{
	return isolate->ThrowException(String::NewFromUtf8(isolate, "bad instance"));
}

//----------------------------------------------------------------------------
// tTJSVariantをオブジェクトとして保持するための機構
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
	HandleScope handle_scope(isolate);
	objectTemplate.Reset();
}

// コンストラクタ
TJSObject::TJSObject(Isolate *isolate, Handle<Object> obj, const tTJSVariant &variant) : TJSBase(variant)
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
	HandleScope handle_scope(info.GetIsolate());
	String::Value propName(property);
	if (wcscmp(*propName, TJSINSTANCENAME) == 0) {
		return;
	}
	tTJSVariant self;
	if (getVariant(self, info.This())) {
		tjs_error error;
		tTJSVariant result;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropGet(0, *propName, NULL, &result, NULL))) {
			info.GetReturnValue().Set(toJSValue(info.GetIsolate(), result));
		} else {
			info.GetReturnValue().Set(ERROR_KRKR(info.GetIsolate(), error));
		}
		return;
	}
	info.GetReturnValue().Set(ERROR_BADINSTANCE(info.GetIsolate()));
}

// プロパティの設定
void
TJSObject::setter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<Value>& info)
{
	HandleScope handle_scope(info.GetIsolate());
	tTJSVariant self;
	if (getVariant(self, info.This())) {
		String::Value propName(property);
		tTJSVariant param = toVariant(info.GetIsolate(), value);
		tjs_error error;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropSet(TJS_MEMBERENSURE, *propName, NULL, &param, NULL))) {
		} else {
			info.GetReturnValue().Set(ERROR_KRKR(info.GetIsolate(), error));
		}
		return;
	}
	info.GetReturnValue().Set(ERROR_BADINSTANCE(info.GetIsolate()));
}

// メソッドの呼び出し
void
TJSObject::caller(const FunctionCallbackInfo<Value>& args)
{
	HandleScope handle_scope(args.GetIsolate());
	tTJSVariant self;
	if (getVariant(self, args.This())) {
		Handle<Value> ret;
		
		// 引数変換
		tjs_int argc = args.Length();
		tTJSVariant **argv = new tTJSVariant*[argc];
		for (tjs_int i=0;i<argc;i++) {
			argv[i] = new tTJSVariant();
			*argv[i] = toVariant(args.GetIsolate(), args[i]);
		}

		if (self.AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Class", NULL) == TJS_S_TRUE) {
			// クラスオブジェクトならコンストラクタ呼び出し
			iTJSDispatch2 *instance = NULL;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().CreateNew(0, NULL, NULL, &instance, argc, argv, NULL))) {
				ret = toJSValue(args.GetIsolate(), tTJSVariant(instance, instance));
				instance->Release();
			} else {
				ret = ERROR_KRKR(args.GetIsolate(), error);
			}
		} else {
			// メソッド呼び出し
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().FuncCall(0, NULL, NULL, &result, argc, argv, NULL))) {
				ret = toJSValue(args.GetIsolate(), result);
			} else {
				ret = ERROR_KRKR(args.GetIsolate(), error);
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
	args.GetReturnValue().Set(ERROR_BADINSTANCE(args.GetIsolate()));
}

// tTJSVariant をオブジェクト化
Local<Object>
TJSObject::toJSObject(Isolate *isolate, const tTJSVariant &variant)
{
	HandleScope handle_scope(isolate);
	Handle<ObjectTemplate> templ = Local<ObjectTemplate>::New(isolate, objectTemplate);
	Local<Object> obj = templ->NewInstance();
	new TJSObject(isolate, obj, variant);
	return obj;
}
