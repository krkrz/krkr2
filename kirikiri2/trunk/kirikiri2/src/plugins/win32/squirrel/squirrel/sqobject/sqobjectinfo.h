/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */

#ifndef __SQOBJECTINFO_H__
#define __SQOBJECTINFO_H__

#include <stdio.h>
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
	void getStack(HSQUIRRELVM v, SQInteger idx);

    // スタックから弱参照として取得
	void getStackWeak(HSQUIRRELVM v, SQInteger idx);
  
	// コンストラクタ
	ObjectInfo();

	// コンストラクタ
	ObjectInfo(HSQUIRRELVM v, SQInteger idx);

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

	/// 複製を登録(失敗したらNULL)
	void pushClone(HSQUIRRELVM v) const;
	
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
	// 配列・辞書処理用
	// ---------------------------------------------------

	/// 配列として初期化
	void initArray(SQInteger size=0);
	
	/// @return 配列なら true
	bool isArray() const { return sq_isarray(obj); }

	/// 配列として初期化
	void initTable();

	/// @return 配列なら true
	bool isTable() const { return sq_istable(obj); }
	
	/// 配列に値を追加
	void append(HSQUIRRELVM v, SQInteger idx);

	/// 配列に配列を追加
	void appendArray(ObjectInfo &array);
	
	/// 配列に値を追加
	template<typename T>
	void append(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		sq_arrayappend(gv, -2);
		sq_pop(gv,1);
	}

	/// 配列に値を挿入
	template<typename T>
	void insert(SQInteger index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		sq_arrayinsert(gv, -2, index);
		sq_pop(gv,1);
	}

	/// 配列の値を削除
	void remove(SQInteger index) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		sq_arrayremove(gv, -1, index);
		sq_pop(gv,1);
	}
	
	/// 配列/辞書に値を格納
	template<typename K, typename T>
	void set(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		sq_set(gv, -3);
		sq_pop(gv,1);
	}

	/// 辞書に値を新規格納
	template<typename K, typename T>
	void create(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		sq_createslot(gv, -3);
		sq_pop(gv,1);
	}

	/// 辞書から値を削除
	template<typename K>
	void deleteslot(K key) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		sq_deleteslot(gv, -2, SQFalse);
		sq_pop(gv,1);
	}
	
	/// 配列/辞書の値を取得
	template<typename K>
	ObjectInfo get(K key) {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		ObjectInfo ret(gv, -1);
		sq_pop(gv,1);
		return ret;
	}

	/// 配列/辞書の値を取得。みつからなければnull
	template<typename K, typename T>
	SQRESULT get(K key, T *value) {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		SQRESULT ret = getValue(gv, value);
		sq_pop(gv,1);
		return ret;
	}
	
	/// 配列の値をpush。見つからなければnull
	template<typename T>
	void pushData(HSQUIRRELVM v, T key) const {
		push(v);
		pushValue(v, key);
		if (SQ_FAILED(sq_get(v, -2))) {
			sq_pushnull(v);
		}
		sq_remove(v,-2);
	}
	
	/// 配列・辞書の中身をクリア
	void clearData() {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		sq_clear(gv,-1);
		sq_pop(gv,1);
	}

	/// @return 配列の長さ
	SQInteger len() const;

	/**
	 * 配列の内容を全部PUSH
	 * @param v squirrelVM
	 * @return push した数
	 */
	SQInteger pushArray(HSQUIRRELVM v) const;

	// ---------------------------------------------------
	// 関数処理用
	// ---------------------------------------------------

	/// @return 配列なら true
	bool isClosure() const { return sq_isclosure(obj) || sq_isnativeclosure(obj); }

	// 呼び出し処理
	SQRESULT call(ObjectInfo *self=NULL);

	template<typename T1> SQRESULT call(T1 p1, ObjectInfo *self=NULL) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		SQRESULT ret = sq_call(gv, 2, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}

	template<typename T1, typename T2> SQRESULT call(T1 p1, T2 p2, ObjectInfo *self=NULL) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		pushValue(gv, p2);
		SQRESULT ret = sq_call(gv, 3, SQFalse, SQTrue);
		sq_pop(gv, 1);
		return ret;
	}
	
	template<typename R> SQRESULT callResult(R* r, ObjectInfo *self=NULL) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		if (SQ_SUCCEEDED(ret = sq_call(gv, 1, SQTrue, SQTrue))) {
			ret = getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1> SQRESULT callResult(R* r, T1 p1, ObjectInfo *self=NULL) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 2, SQTrue, SQTrue))) {
			ret = getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}
	template<typename R, typename T1, typename T2> SQRESULT callResult(R* r, T1 p1, T2 p2, ObjectInfo *self=NULL) {
		SQRESULT ret;
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		if (self) {
			self->push(gv);
		} else {
			sq_pushroottable(gv); // root
		}
		pushValue(gv, p1);
		pushValue(gv, p2);
		if (SQ_SUCCEEDED(ret = sq_call(gv, 3, SQTrue, SQTrue))) {
			ret = getValue(gv, r);
			sq_pop(gv, 1);
		}
		sq_pop(gv, 1);
		return ret;
	}

	/**
	 * 自己オブジェクトメソッド呼び出し（引数無し)
	 * @param methodName メソッド名
	 */
	SQRESULT callMethod(const SQChar *methodName) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(this);
			}
		}
		return SQ_ERROR;
	}

	/**
	 * 自己オブジェクトメソッド呼び出し（引数1つ)
	 * @param methodName メソッド名
	 * @param p1 引数
	 */
	template<typename T1> SQRESULT callMethod(const SQChar *methodName, T1 p1) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(p1, this);
			}
		}
		return SQ_ERROR;
	}
	
	/**
	 * 自己オブジェクトメソッド呼び出し（引数2つ)
	 * @param methodName メソッド名
	 * @param p1 引数
	 * @param p2 引数2
	 */
	template<typename T1, typename T2> SQRESULT callMethod(const SQChar *methodName, T1 p1, T2 p2) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(p1, p2, this);
			}
		}
		return SQ_ERROR;
	}
	
	/**
	 * 返値有り自己オブジェクトメソッド呼び出し（引数無し)
	 * @param r 帰り値ポインタ
	 * @param methodName メソッド名
	 */
	template<typename R> SQRESULT callMethodResult(R* r, const SQChar *methodName) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(r, this);
			}
		}
		return SQ_ERROR;
	}

	/**
	 * 返値あり自己オブジェクトメソッド呼び出し（引数1つ)
	 * @param r 帰り値ポインタ
	 * @param methodName メソッド名
	 * @param p1 引数
	 */
	template<typename R, typename T1> SQRESULT callMethodResult(R* r, const SQChar *methodName, T1 p1) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(r, p1, this);
			}
		}
		return SQ_ERROR;
	}
	
	/**
	 * 返値有り自己オブジェクトメソッド呼び出し（引数2つ)
	 * @param r 帰り値ポインタ
	 * @param methodName メソッド名
	 * @param p1 引数
	 * @param p2 引数2
	 */
	template<typename R, typename T1, typename T2> SQRESULT callMethodResult(R* r, const SQChar *methodName, T1 p1, T2 p2) {
		if (!isNull()) {
			ObjectInfo method = get(methodName);
			if (method.isClosure()) {
				return method.call(r, p1, p2, this);
			}
		}
		return SQ_ERROR;
	}

	// ---------------------------------------------------
	// クラス処理用
	// ---------------------------------------------------

	// クラスオブジェクトか
	bool isClass() const;

protected:
	// ---------------------------------------------------
	// スタック処理用
	// ---------------------------------------------------

	// 値の push
	static void pushValue(HSQUIRRELVM v) {}
	static void pushValue(HSQUIRRELVM v, SQInteger value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, SQFloat value) { sq_pushfloat(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { sq_pushstring(v,value,-1); }
	static void pushValue(HSQUIRRELVM v, SQUserPointer value) { sq_pushuserpointer(v,value); }
	static void pushValue(HSQUIRRELVM v, ObjectInfo &obj) { obj.push(v); }
	
	// 値の取得
	static SQRESULT getValue(HSQUIRRELVM v, void *value) { return SQ_OK; }
	static SQRESULT getValue(HSQUIRRELVM v, SQInteger *value) { return sq_getinteger(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQFloat *value) { return sq_getfloat(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, const SQChar **value) { return sq_getstring(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQUserPointer *value) { return sq_getuserpointer(v, -1, value); }
	
private:
	HSQOBJECT obj; // オブジェクト参照情報

	// -------------------------------------------------------------
	// スレッド系操作用
	// -------------------------------------------------------------

public:
	// Object
	void addWait(ObjectInfo &thread);
	void removeWait(ObjectInfo &thread);

	// Thread
	bool notifyTrigger(const SQChar *name);
	bool notifyObject(ObjectInfo &target);
};

};

#endif
