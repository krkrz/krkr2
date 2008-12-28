#ifndef __SQOBJECT_H__
#define __SQOBJECT_H__

#include <squirrel.h>
#include <vector>

class MyObject;
class MyThread;

/**
 * squirrel オブジェクト保持用クラス
 */
class SQObjectInfo {

protected:
	HSQUIRRELVM v; // オブジェクトの属していたVM
	HSQOBJECT obj; // オブジェクト参照情報

public:
	// 内容消去
	void clear();

	// スタックから取得
	void getStack(HSQUIRRELVM v, int idx);

	// コンストラクタ
	SQObjectInfo();

	// コンストラクタ
	SQObjectInfo(HSQUIRRELVM v, int idx);

	// コピーコンストラクタ
	SQObjectInfo(const SQObjectInfo &orig);

	// 代入
	SQObjectInfo & operator=(const SQObjectInfo &orig);
	
	// デストラクタ
	virtual ~SQObjectInfo();

	// スレッドか？
	bool isThread() const;

	// 同じスレッドか？
	bool isSameThread(const HSQUIRRELVM v) const;

	// スレッドを取得
	operator HSQUIRRELVM() const;
	
	// インスタンスユーザポインタを取得
	SQUserPointer getInstanceUserPointer(const SQUserPointer tag);

	// オブジェクトをPUSH
	void push(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// delegate 処理用
	// ---------------------------------------------------

	// delegate として機能するかどうか
	bool isDelegate();

	// bindenv させるかどうか
	bool isBindDelegate();

	// ---------------------------------------------------
	// オブジェクト取得
	// ---------------------------------------------------

	// インスタンスユーザポインタを取得
	MyThread *getMyThread();

	// インスタンスユーザポインタを取得
	MyObject *getMyObject();
	
	// ---------------------------------------------------
	// wait処理用メソッド
	// ---------------------------------------------------

	// MyObjectか？
	bool isMyObject();

	bool isString() const;

	bool isSameString(const SQChar *str) const;

	bool isNumeric() const;
};

/**
 * オブジェクト用
 */
class MyObject {

protected:
	// このオブジェクトを待ってるスレッドの一覧
	std::vector<MyThread*> _waitThreadList;
	// delegate
	SQObjectInfo delegate;

public:
	/**
	 * オブジェクト待ちの登録
	 * @param thread スレッド
	 */
	void addWait(MyThread *thread);

	/**
	 * オブジェクト待ちの解除
	 * @param thread スレッド
	 */
	void removeWait(MyThread *thread);

protected:

	/**
	 * コンストラクタ
	 */
	MyObject();

	/**
	 * デストラクタ
	 */
	virtual ~MyObject();

	/**
	 * このオブジェクトを待っている１スレッドの待ちを解除
	 */
	void _notify();
	
	/**
	 * このオブジェクトを待っている全スレッドの待ちを解除
	 */
	void _notifyAll();

	// ------------------------------------------------------------------

	/**
	 * @return オブジェクト情報オブジェクト
	 */
	static MyObject *getObject(HSQUIRRELVM v, int idx);

	/**
	 * オブジェクトのリリーサ
	 */
	static SQInteger release(SQUserPointer up, SQInteger size);

	static SQInteger ERROR_CREATE(HSQUIRRELVM v);
	
	/**
	 * オブジェクトのコンストラクタ
	 */
	static SQInteger constructor(HSQUIRRELVM v);

	static SQInteger ERROR_NOMEMBER(HSQUIRRELVM v);

	static bool isClosure(SQObjectType type);
	
	/**
	 * プロパティから値を取得
	 * @param name プロパティ名
	 * @return プロパティ値
	 */
	static SQInteger _get(HSQUIRRELVM v);

	/**
	 * プロパティに値を設定
	 * @param name プロパティ名
	 * @param value プロパティ値
	 */
	static SQInteger _set(HSQUIRRELVM v);

	/**
	 * setプロパティの存在確認
	 * @param name プロパティ名
	 * @return setプロパティが存在したら true
	 */
	static SQInteger hasSetProp(HSQUIRRELVM v);
	
	static SQInteger ERROR_BADINSTANCE(HSQUIRRELVM v);

	/**
	 * 委譲の設定
	 */
	static SQInteger setDelegate(HSQUIRRELVM v);
	
	/**
	 * 単一スレッドへのオブジェクト待ちの終了通知用
	 */
	static SQInteger notify(HSQUIRRELVM v);

	/**
	 * 全スレッドへのオブジェクト待ちの終了通知
	 */
	static SQInteger notifyAll(HSQUIRRELVM v);

public:
	/**
	 * クラスの登録
	 * @param v squirrel VM
	 */
	static void registClass(HSQUIRRELVM v);
};

#endif
