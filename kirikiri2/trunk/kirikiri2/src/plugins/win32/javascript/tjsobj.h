#ifndef __TJSOBJ_H__
#define __TJSOBJ_H__

#include "tjsbase.h"

/**
 * 吉里吉里用オブジェクトを単純保持するJavaScript用クラス情報
 */
class TJSObject : public TJSBase {

public:
	// 初期化用
	static void init(Isolate *isolate);
	static void done(Isolate *isolate);

	// オブジェクト生成
	static Local<Object> toJSObject(Isolate *isolate, const tTJSVariant &variant);

private:
	// オブジェクト定義
	static Persistent<ObjectTemplate> objectTemplate;

	template<class T>
	static void release(const WeakCallbackData<T,TJSObject>& data) {
		delete data.GetParameter();
	}
	
	// アクセス用メソッド
	static void getter(Local<String> property, const PropertyCallbackInfo<Value>& info);
	static void setter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<Value>& info);
	static void caller(const FunctionCallbackInfo<Value>& info);

	// 格納情報コンストラクタ
	TJSObject(Isolate *isolate, Local<Object> &obj, const tTJSVariant &variant);
};

#endif