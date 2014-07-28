#ifndef __TJSINSTANCE_H__
#define __TJSINSTANCE_H__

#include "tjsbase.h"

/**
 * 吉里吉里インスタンスを保持するJavaScript用クラス情報
 * メンバ情報をプロトタイプに登録します
 */
class TJSInstance : public TJSBase, iTJSNativeInstance {

public:
	// 初期化用
	static void init(Isolate *isolate, Local<ObjectTemplate> &globalTemplate);

	/**
	 * 吉里吉里オブジェクトを Javascrip オブジェクトに変換
	 * @param result 格納先
	 * @param variant 変換元
	 * @return 変換成功したら true
	 */
	static bool getJSObject(Local<Object> &result, const tTJSVariant &variant);

	// メソッド呼び出し用
	static tjs_error getProp(Isolate *isolate, Local<Object> &obj, const tjs_char *membername, tTJSVariant *result);
	static tjs_error setProp(Isolate *isolate, Local<Object> &obj, const tjs_char *membername, const tTJSVariant *param);
	static tjs_error remove(Isolate *isolate, Local<Object> &obj, const tjs_char *membername);
	static tjs_error createMethod(Isolate *isolate, Local<Object> &obj, const tjs_char *membername, iTJSDispatch2 **result, tjs_int numparams, tTJSVariant **param);
	static tjs_error callMethod(Isolate *isolate, Local<Object> &obj, const tjs_char *membername, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
	
	// ---------------------------------------------------------------

	/**
	 * call 処理用の口
	 * TJSインスタンスからJavascriptインスタンスのメソッドを直接呼び出す
	 */
	static tjs_error call(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
						  tTJSVariant *result,
						  tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
	
	/**
	 * missing 処理用の口
	 * TJSインスタンスにメンバが存在しなかった場合は Javascriptインスタンスを参照する
	 */
	static tjs_error missing(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
							 tTJSVariant *result,
							 tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	// ---------------------------------------------------------------
	
	/**
	 * TJSオブジェクト用のメソッド
	 * @param args 引数
	 * @return 結果
	 */
	static void tjsInvoker(const FunctionCallbackInfo<Value>& args);

	/**
	 * TJSオブジェクト用のプロパティゲッター
	 */
	static void tjsGetter(Local<String> property, const PropertyCallbackInfo<Value>& info);
	
	/**
	 * TJSオブジェクト用のプロパティセッター
	 */
	static void tjsSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

	/**
	 * コンストラクタ
	 * @param obj 自己オブジェクト
	 * @param instance バインド対象のTJSオブジェクト
	 */
	TJSInstance(Isolate *isolate, Local<Object> &obj, const tTJSVariant &instance);
	
private:

	/**
	 * 吉里吉里クラスから Javascript クラスを生成
	 * @param args 引数
	 * @return 結果
	 */
	static void createTJSClass(const FunctionCallbackInfo<Value>& args);

	/**
	 * 破棄処理
	 */
	void invalidate();

	template<class T>
	static void release(const WeakCallbackData<T,TJSInstance>& data) {
		TJSInstance *self = data.GetParameter();
		if (self) {
			self->invalidate();
		}
	}
	
	/**
	 * TJSオブジェクトの有効確認
	 * @param args 引数
	 * @return 結果
	 */
	static void tjsIsValid(const FunctionCallbackInfo<Value>& args);

	/**
	 * TJSオブジェクトに対するオーバライド処理
	 * @param args 引数
	 * @return 結果
	 */
	static void tjsOverride(const FunctionCallbackInfo<Value>& args);
	
	/**
	 * TJSオブジェクトのコンストラクタ
	 * @param args 引数
	 * @return 結果
	 */
	static void tjsConstructor(const FunctionCallbackInfo<Value>& args);

	// NativeClass ID
	static int classId;
	
	// 自己参照用
	Persistent<Object> self;
	Isolate *isolate;

public:
	// NativeInstance 対応用メンバ
	virtual tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	virtual void TJS_INTF_METHOD Invalidate();
	virtual void TJS_INTF_METHOD Destruct();

	Local<Object> getObject() {
		return Local<Object>::New(isolate, self);
	}
};

#endif
