#ifndef __SQTJS_OBJ_H_
#define __SQTJS_OBJ_H_

#include <sqobject.h>

/**
 * 吉里吉里オブジェクトを保持するクラス
 */
class TJSObject : public sqobject::Object {

public:
	/**
	 * 初期化用
	 */
	static void init(HSQUIRRELVM vm);

	/**
	 * 廃棄用
	 */
	static void done();

	// ---------------------------------------------------------------
	
	/**
	 * スタックからの吉里吉里オブジェクトの取得
	 * @param v squirrelVM
	 * @param idx インデックス
	 * @return 吉里吉里ディスパッチャ
	 */
	static iTJSDispatch2 *getDispatch(HSQUIRRELVM v, int idx);

	/**
	 * スタックへの吉里吉里オブジェクトの登録
	 * @return 登録成功したら true
	 */
	static bool pushDispatch(HSQUIRRELVM v, iTJSDispatch2 *dispatch);

	// ---------------------------------------------------------------
	
	/**
	 * 吉里吉里クラスから squirrel クラスを生成
	 * @param v squirrelVM
	 */
	static SQRESULT createClass(HSQUIRRELVM v);

	// ---------------------------------------------------------------
	
	/**
	 * TJSオブジェクト用のメソッド
	 * 引数1 オブジェクト
	 * 引数2～配列
	 * 自由変数1 メンバ名
	 */
	static SQRESULT tjsInvoker(HSQUIRRELVM v);

	/**
	 * TJSオブジェクト用のプロパティゲッター
	 * 引数1 オブジェクト
	 * 自由変数1 プロパティ名
	 */
	static SQRESULT tjsGetter(HSQUIRRELVM v);

	/**
	 * TJSオブジェクト用のプロパティセッター
	 * 引数1 オブジェクト
	 * 引数2 設定値
	 * 自由変数1 プロパティ名
	 */
	static SQRESULT tjsSetter(HSQUIRRELVM v);

	/**
	 * TJSオブジェクト用のメソッド
	 * 引数1 オブジェクト
	 * 引数2～配列
	 * 自由変数1 クラス名
	 * 自由変数2 メンバ名
	 */
	static SQRESULT tjsStaticInvoker(HSQUIRRELVM v);

	/**
	 * TJSオブジェクト用のプロパティゲッター
	 * 引数1 オブジェクト
	 * 自由変数1 クラス名
	 * 自由変数2 プロパティ名
	 */
	static SQRESULT tjsStaticGetter(HSQUIRRELVM v);

	/**
	 * TJSオブジェクト用のプロパティセッター
	 * 引数1 オブジェクト
	 * 引数2 設定値
	 * 自由変数1 クラス名
	 * 自由変数2 プロパティ名
	 */
	static SQRESULT tjsStaticSetter(HSQUIRRELVM v);
	
protected:
	// コンストラクタ
	TJSObject(HSQUIRRELVM v);

	// コンストラクタ
	// インスタンスを返す場合用
	TJSObject(HSQUIRRELVM v, iTJSDispatch2 *dispatch);
	
	// デストラクタ
	~TJSObject();
	
	/**
	 * オブジェクトのリリーサ
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size);

	/**
	 * TJSオブジェクトのコンストラクタ
	 * TJSオブジェクトのコンストラクタ
	 * 引数1 オブジェクト
	 * 引数2～ 引数
	 * 自由変数1 クラス名
	 */
	static SQRESULT tjsConstructor(HSQUIRRELVM v);

private:
	// 処理対象オブジェクト
	iTJSDispatch2 *dispatch;

	// 登録されたクラスの名前マップ
	static sqobject::ObjectInfo classMap;
};

#endif
