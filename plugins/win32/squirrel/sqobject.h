#ifndef __SQOBJECT_H__
#define __SQOBJECT_H__

// 型名
#ifndef SQOBJECTNAME
#define SQOBJECTNAME _SC("Object")
#endif

#ifndef SQTHREADNAME
#define SQTHREADNAME _SC("Thread")
#endif

#include <squirrel.h>
#include <vector>
#include <string>

/**
 * オブジェクト/スレッド処理の初期化
 * @param v squirrel VM
 */
extern void sqobject_init(HSQUIRRELVM v);

/**
 * オブジェクト/スレッド処理メイン部分
 * @param tick tick値
 */
extern int sqobject_main(int tick);

/**
 * オブジェクト/スレッド処理終了処理
 */
extern void sqobject_done();

namespace sqobject {

typedef std::basic_string<SQChar> tstring;

// エラー処理用
extern SQRESULT ERROR_CREATE(HSQUIRRELVM v);
extern SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v);

// setter/getter 名前決定
extern void getSetterName(tstring &store, const SQChar *name);
extern void getGetterName(tstring &store, const SQChar *name);

class Object; // オブジェクトクラス
class Thread; // スレッドクラス

/**
 * squirrel オブジェクト保持用クラス
 */
class ObjectInfo {

protected:
	HSQUIRRELVM v; // オブジェクトの属していたVM
	HSQOBJECT obj; // オブジェクト参照情報

public:
	// 内容消去
	void clear();

	// スタックから取得
	void getStack(HSQUIRRELVM v, int idx);

	// コンストラクタ
	ObjectInfo();

	// コンストラクタ
	ObjectInfo(HSQUIRRELVM v, int idx);

	// コピーコンストラクタ
	ObjectInfo(const ObjectInfo &orig);

	// 代入
	ObjectInfo & operator=(const ObjectInfo &orig);
	
	// デストラクタ
	virtual ~ObjectInfo();

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
	Thread *getThread();

	// インスタンスユーザポインタを取得
	Object *getObject();
	
	// ---------------------------------------------------
	// wait処理用メソッド
	// ---------------------------------------------------

	bool isString() const;

	bool isSameString(const SQChar *str) const;

	bool isNumeric() const;
};

/**
 * オブジェクト用
 */
class Object {

	// ------------------------------
	// 継承情報
	// ------------------------------
protected:
	static std::vector<SQUserPointer>tags;
public:
	static void pushTag(const SQUserPointer tag) {
		tags.push_back(tag);
	}

protected:
	// このオブジェクトを待ってるスレッドの一覧
	std::vector<Thread*> _waitThreadList;
	// delegate
	ObjectInfo delegate;

public:
	/**
	 * オブジェクト待ちの登録
	 * @param thread スレッド
	 */
	void addWait(Thread *thread);

	/**
	 * オブジェクト待ちの解除
	 * @param thread スレッド
	 */
	void removeWait(Thread *thread);

protected:

	/**
	 * コンストラクタ
	 */
	Object();

	/**
	 * デストラクタ
	 */
	virtual ~Object();

	/**
	 * このオブジェクトを待っている１スレッドの待ちを解除
	 */
	void _notify();
	
	/**
	 * このオブジェクトを待っている全スレッドの待ちを解除
	 */
	void _notifyAll();

	// ------------------------------------------------------------------

public:
	/**
	 * @return オブジェクト情報オブジェクト
	 */
	static Object *getObject(HSQUIRRELVM v, int idx);

protected:
	/**
	 * オブジェクトのリリーサ
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size);

	/**
	 * オブジェクトのコンストラクタ
	 */
	static SQRESULT constructor(HSQUIRRELVM v);

	static bool isClosure(SQObjectType type);
	
	/**
	 * プロパティから値を取得
	 * @param name プロパティ名
	 * @return プロパティ値
	 */
	static SQRESULT _get(HSQUIRRELVM v);

	/**
	 * プロパティに値を設定
	 * @param name プロパティ名
	 * @param value プロパティ値
	 */
	static SQRESULT _set(HSQUIRRELVM v);

	/**
	 * setプロパティの存在確認
	 * @param name プロパティ名
	 * @return setプロパティが存在したら true
	 */
	static SQRESULT hasSetProp(HSQUIRRELVM v);
	
	/**
	 * 委譲の設定
	 */
	static SQRESULT setDelegate(HSQUIRRELVM v);

	/**
	 * 委譲の取得
	 */
	static SQRESULT getDelegate(HSQUIRRELVM v);
	
	/**
	 * 単一スレッドへのオブジェクト待ちの終了通知用
	 */
	static SQRESULT notify(HSQUIRRELVM v);

	/**
	 * 全スレッドへのオブジェクト待ちの終了通知
	 */
	static SQRESULT notifyAll(HSQUIRRELVM v);

public:
	/**
	 * クラスの登録
	 * @param v squirrel VM
	 */
	static void registClass(HSQUIRRELVM v);
};

};// namespace

// メソッド登録
#define REGISTMETHOD(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, name, 0);\
	sq_createslot(v, -3);

// メソッド登録（名前つき）
#define REGISTMETHODNAME(name, method) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, method, 0);\
	sq_createslot(v, -3);

// メソッド登録
#define REGISTENUM(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_pushinteger(v, name);\
	sq_createslot(v, -3);

#endif
