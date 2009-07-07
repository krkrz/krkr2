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
	 * @param variant 格納先
	 * @return 格納成功したら true
	 */
	static bool getVariant(HSQUIRRELVM v, SQInteger idx, tTJSVariant *variant);

	/**
	 * スタックへの吉里吉里オブジェクトの登録
	 * @parma variant オブジェクト
	 * @return 登録成功したら true
	 */
	static bool pushVariant(HSQUIRRELVM v, tTJSVariant &variant);

	// ---------------------------------------------------------------
	
	/**
	 * 吉里吉里クラスから squirrel クラスを生成
	 * @param v squirrelVM
	 */
	static SQRESULT createTJSClass(HSQUIRRELVM v);

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
	TJSObject(HSQUIRRELVM v, tTJSVariant &instance);
	
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
	tTJSVariant instance;

	// 登録されたクラスの名前マップ
	static sqobject::ObjectInfo classMap;
};

#endif
