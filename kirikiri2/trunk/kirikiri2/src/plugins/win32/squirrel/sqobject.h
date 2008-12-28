#ifndef __SQOBJECT_H__
#define __SQOBJECT_H__

#include <squirrel.h>
#include <vector>

// パラメータ参照用
extern int getInt(HSQUIRRELVM v, int idx, int defValue);
extern const SQChar *getString(HSQUIRRELVM v, int idx);

// エラー処理用
extern SQInteger ERROR_CREATE(HSQUIRRELVM v);
extern SQInteger ERROR_NOMEMBER(HSQUIRRELVM v);
extern SQInteger ERROR_BADINSTANCE(HSQUIRRELVM v);

// 型定義
extern const SQUserPointer OBJTYPETAG;
extern const SQUserPointer THREADTYPETAG;

#include <tchar.h>
#if _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

extern void getSetterName(tstring &store, const SQChar *name);
extern void getGetterName(tstring &store, const SQChar *name);

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

	bool isString() const;

	bool isSameString(const SQChar *str) const;

	bool isNumeric() const;
};

/**
 * オブジェクト用
 */
class MyObject {

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

public:
	/**
	 * @return オブジェクト情報オブジェクト
	 */
	static MyObject *getObject(HSQUIRRELVM v, int idx);

protected:
	/**
	 * オブジェクトのリリーサ
	 */
	static SQInteger release(SQUserPointer up, SQInteger size);

	/**
	 * オブジェクトのコンストラクタ
	 */
	static SQInteger constructor(HSQUIRRELVM v);

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

// 型名
#define OBJECTNAME _SC("Object")
#define THREADNAME _SC("Thread")

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
