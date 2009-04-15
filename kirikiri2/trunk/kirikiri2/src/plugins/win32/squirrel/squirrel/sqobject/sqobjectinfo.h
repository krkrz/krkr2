/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */

#ifndef __SQOBJECTINFO_H__
#define __SQOBJECTINFO_H__

#include <squirrel.h>

namespace sqobject {

/// 初期化
HSQUIRRELVM init();
/// 終了
extern void done();
/// 情報保持用グローバルVMの取得
extern HSQUIRRELVM getGlobalVM();

class Object;
class Thread;
	
// ---------------------------------------------------------
// ObjectInfo
// ---------------------------------------------------------

/**
 * squirrel オブジェクト保持用クラス
 * 弱参照も保持可能
 */
class ObjectInfo {

public:
	/// 比較
	bool operator ==(const ObjectInfo& o);

	// 内容消去
	void clear();

	// スタックから取得
	void getStack(HSQUIRRELVM v, int idx);

    // スタックから弱参照として取得
	void getStackWeak(HSQUIRRELVM v, int idx);
  
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

	// null か？
	bool isNull() const;
	
	// 同じスレッドか？
	bool isSameThread(const HSQUIRRELVM v) const;

	// スレッドを取得
	operator HSQUIRRELVM() const;
	
	// オブジェクトをPUSH
	void push(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// delegate 処理用
	// ---------------------------------------------------

	// delegate として機能するかどうか
	bool isDelegate() const;

	// bindenv させるかどうか
	bool isBindDelegate() const;

	// ---------------------------------------------------
	// データ取得
	// ---------------------------------------------------

	const SQChar *getString();
	
	// ---------------------------------------------------
	// オブジェクト取得
	// ---------------------------------------------------

	// Thread のインスタンスユーザポインタを取得
	Thread *getThread();
	
	// Object のインスタンスユーザポインタを取得
	Object *getObject();
	
	// ---------------------------------------------------
	// wait処理用メソッド
	// ---------------------------------------------------

	bool isSameString(const SQChar *str) const;

	// ---------------------------------------------------
	// 配列処理用
	// ---------------------------------------------------

	/// 配列として初期化
	void initArray(int size=0);

	/// @return 配列なら true
	bool isArray() const { return sq_isarray(obj); }

	/// 配列に値を追加
	void append(HSQUIRRELVM v, int idx);
	
	/// 配列に値を追加
	template<typename T>
	void append(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		pushValue(gv, value);
		sq_arrayappend(gv, -2);
		sq_pop(gv,1);
	}

	/// 配列に値を挿入
	template<typename T>
	void insert(int index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		pushValue(gv, value);
		sq_arrayinsert(gv, -2, index);
		sq_pop(gv,1);
	}

	/// 配列に値を格納
	template<typename T>
	void set(int index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushinteger(gv, index);
		pushValue(gv, value);
		sq_set(gv, -3);
		sq_pop(gv,1);
	}

	/// @return 配列の長さ
	int len() const;

	/**
	 * 配列の内容を全部PUSH
	 * @param v squirrelVM
	 * @return push した数
	 */
	int pushArray(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// 関数処理用
	// ---------------------------------------------------

	/// @return 配列なら true
	bool isClosure() const { return sq_isclosure(obj) || sq_isnativeclosure(obj); }

	// 呼び出し処理
	SQRESULT call();

	template<typename T1> SQRESULT call(T1 p1) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		SQRESULT ret = sq_call(gv, 2, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}

	template<typename T1, typename T2> SQRESULT call(T1 p1, T2 p2) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		pushValue(gv, p2);
		SQRESULT ret = sq_call(gv, 3, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}
	
	template<typename R> SQRESULT call(R*) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		if (SQ_SUCCEEDED(ret = sq_call(gv, 1, SQTrue, SQTrue))) {
			getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1> SQRESULT call(R* r, T1 p1) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 2, SQTrue, SQTrue))) {
			getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1, typename T2> SQRESULT call(R* r, T1 p1, T2 p2) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_pushroottable(gv); // root
		pushValue(gv, p1);
		pushValue(gv, p2);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 3, SQTrue, SQTrue))) {
			getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	
protected:
	// ---------------------------------------------------
	// スタック処理用
	// ---------------------------------------------------

	// 値の push
	static void pushValue(HSQUIRRELVM v) {}
	static void pushValue(HSQUIRRELVM v, int value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { sq_pushstring(v,value,-1); }
	
	// 値の取得
	static void getValue(HSQUIRRELVM v, void *value) {}
	static void getValue(HSQUIRRELVM v, int *value) { sq_getinteger(v, -1, value); }
	static void getValue(HSQUIRRELVM v, const SQChar **value) { sq_getstring(v, -1, value); }
	
private:
	HSQOBJECT obj; // オブジェクト参照情報
};

};

#endif
