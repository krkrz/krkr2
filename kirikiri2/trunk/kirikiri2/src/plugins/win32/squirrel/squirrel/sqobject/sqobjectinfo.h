/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */

#ifndef __SQOBJECTINFO_H__
#define __SQOBJECTINFO_H__

#include <stdio.h>
#include <squirrel.h>
#include <string>

#ifdef USESQPLUS
#include <sqplus.h>
#else
#ifdef USESQRAT
#include <sqrat.h>
#else
#include "sqfunc.h"
#endif
#endif

namespace sqobject {

class Object;

/// 初期化
HSQUIRRELVM init();
/// 終了
extern void done();
/// 情報保持用グローバルVMの取得
extern HSQUIRRELVM getGlobalVM();

// ---------------------------------------------------------
// ObjectInfo
// ---------------------------------------------------------

/**
 * squirrel オブジェクト保持用クラス
 * 弱参照も保持可能
 */
class ObjectInfo {

public:
	
	// roottable の取得
	static ObjectInfo getRoot();
	static ObjectInfo createArray(SQInteger size=0);
	static ObjectInfo createTable();

	/// 比較
	bool operator ==(const ObjectInfo& o);
	bool operator !=(const ObjectInfo& o);
	bool operator <(const ObjectInfo& o);
	bool operator <=(const ObjectInfo& o);
	bool operator >(const ObjectInfo& o);
	bool operator >=(const ObjectInfo& o);

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

	// 任意型のコンストラクタ
	template<typename T>
	ObjectInfo(T value) {
		setValue(value);
	}

	// 任意型の代入
	template<typename T>
	ObjectInfo & operator=(T value) {
		setValue(value);
		return *this;
	}

	// 任意型へのキャスト
	// 取得できなかった場合はクリア値になる
	template<typename T>
	operator T() const
	{
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		T value;
		if (SQ_FAILED(getValue(gv, &value))) {
			clearValue(&value);
		}
		sq_pop(gv, 1);
		return value;
	}
	
	// 値の設定
	template<typename T>
	void setValue(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		pushValue(gv, value);
		getStack(gv, -1);
		sq_pop(gv, 1);
	}
	
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
	SQRESULT append(HSQUIRRELVM v, SQInteger idx);

	/// 配列に配列を追加
	SQRESULT appendArray(ObjectInfo &array);
	
	/// 配列に値を追加
	template<typename T>
	SQRESULT append(T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		SQRESULT ret = sq_arrayappend(gv, -2);
		sq_pop(gv,1);
		return ret;
	}

	/// 配列に値を挿入
	template<typename T>
	SQRESULT insert(SQInteger index, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, value);
		SQRESULT ret = sq_arrayinsert(gv, -2, index);
		sq_pop(gv,1);
		return ret;
	}

	/// 配列の値を削除
	SQRESULT remove(SQInteger index) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		SQRESULT ret = sq_arrayremove(gv, -1, index);
		sq_pop(gv,1);
		return ret;
	}
	
	/// 配列/辞書に値を格納
	template<typename K, typename T>
	SQRESULT set(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		SQRESULT ret = sq_set(gv, -3);
		sq_pop(gv,1);
		return ret;
	}

	/// 辞書に値を新規格納
	template<typename K, typename T>
	SQRESULT create(K key, T value) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		pushValue(gv, value);
		SQRESULT ret = sq_newslot(gv, -3, SQFalse);
		sq_pop(gv,1);
		return ret;
	}

	/// 辞書から値を削除
	template<typename K>
	SQRESULT deleteslot(K key) {
		HSQUIRRELVM gv = getGlobalVM();
		push(gv);
		pushValue(gv, key);
		SQRESULT ret = sq_deleteslot(gv, -2, SQFalse);
		sq_pop(gv,1);
		return ret;
	}
	
	/// 配列/辞書の値を取得
	template<typename K>
	ObjectInfo get(K key) const {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		ObjectInfo ret(gv, -1);
		sq_pop(gv,1);
		return ret;
	}

	/**
	 * 配列参照用
	 */
	class ObjectInfoReference {

	public:
		// コンストラクタ
		ObjectInfoReference(ObjectInfo *obj, int key) : obj(obj), intKey(key), type(0) {};
		ObjectInfoReference(ObjectInfo *obj, const SQChar *key) : obj(obj), strKey(key), type(1) {};
		ObjectInfoReference(ObjectInfo *obj, std::basic_string<SQChar> &key) : obj(obj), strKey(key.c_str()), type(1) {};
		
		// 任意型の代入
		// 設定できなかった場合で辞書の場合は作成してしまう
		template<typename T>
		const T &operator=(const T &value) {
			if (obj->type() == OT_ARRAY) {
				if (type == 0) {
					// サイズが届いてないので null でうめる
					if (obj->len() <= intKey) {
						HSQUIRRELVM gv = getGlobalVM();
						obj->push(gv);
						while (obj->len() <= intKey) {
							sq_pushnull(gv);
							sq_arrayappend(gv, -2);
						}
						sq_pop(gv,1);
					}
					obj->set(intKey, value);
				}
			} else {
				SQRESULT ret;
				if (type == 0) {
					ret = obj->set(intKey, value);
				} else {
					ret = obj->set(strKey, value);
				}
				if (SQ_FAILED(ret)) {
					if (obj->type() == OT_TABLE || obj->type() == OT_CLASS) {
						if (type == 0) {
							obj->create(intKey, value);
						} else {
							obj->create(strKey, value);
						}
					}
				}
			}
			return value;
		}
		
		// 任意型へのキャスト
		// 変換できなかった場合は初期値(0)でのクリアになる
		template<typename T>
		operator T() const {
			T value;
			SQRESULT ret;
			if (type == 0) {
				ret = obj->get(intKey, &value);
			} else {
				ret = obj->get(strKey, &value);
			}
			if (SQ_FAILED(ret)) {
				clearValue(&value);
			}
			return value;
		}
		
	protected:
		ObjectInfo *obj;
		int intKey;
		const SQChar *strKey;
		int type;
	};
	

	/**
	 * 配列/辞書参照用リファレンスを取得
	 * 辞書アクセスした場合は
	 * ・代入失敗したら自動で作成
	 * ・取得失敗したら初期値(0)を返す
	 * 配列アクセスした場合は
	 * ・サイズが小さかったら自動的にそのサイズまで増やす(nullをうめる)
	 * ・取得失敗したら初期値(0)を返す
	 * という処理が自動的に行われます
	 */
	template<typename K>
	ObjectInfoReference operator[](K key) {
		return ObjectInfoReference(this, key);
	}

	/// 配列/辞書の値を取得。みつからなければnull
	template<typename K, typename T>
	SQRESULT get(K key, T *value) const {
		HSQUIRRELVM gv = getGlobalVM();
		pushData(gv, key);
		SQRESULT ret = getValue(gv, value);
		sq_pop(gv,1);
		return ret;
	}
	
	/// 配列/辞書の値をpush。見つからなければnull
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

	/// @return オブジェクトの型
	SQObjectType type() const {
		return obj._type;
	}
	
	/// @return 配列の長さ/辞書のサイズ/文字列の長さ
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
			ret = getResultValue(gv, r);
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
			ret = getResultValue(gv, r);
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
			ret = getResultValue(gv, r);
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
				return method.callResult(r, this);
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
				return method.callResult(r, p1, this);
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
				return method.callResult(r, p1, p2, this);
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

	// Objectを pushする
	// @return すでに squirrel 用に初期化済みでインスタンスをもっていて push できたら true
	static bool pushObject(HSQUIRRELVM v, Object *obj);
	
	// 値の push
	static void pushValue(HSQUIRRELVM v, bool value) { sq_pushbool(v,value ? SQTrue : SQFalse); }
	static void pushValue(HSQUIRRELVM v, SQBool value) { sq_pushbool(v,value); }
	static void pushValue(HSQUIRRELVM v, SQInteger value) { sq_pushinteger(v,value); }
	static void pushValue(HSQUIRRELVM v, SQFloat value) { sq_pushfloat(v,value); }
	static void pushValue(HSQUIRRELVM v, const SQChar *value) { if (value) {sq_pushstring(v,value,-1);} else { sq_pushnull(v);} }
	static void pushValue(HSQUIRRELVM v, SQUserPointer value) { if (value) {sq_pushuserpointer(v,value);} else { sq_pushnull(v);} }
	static void pushValue(HSQUIRRELVM v, ObjectInfo &obj) { obj.push(v); }
	static void pushValue(HSQUIRRELVM v, std::basic_string<SQChar> &value) { sq_pushstring(v,value.c_str(),value.length()); }
	
	// 値の取得
	static SQRESULT getValue(HSQUIRRELVM v, bool *value) { SQBool b;SQRESULT ret = sq_getbool(v, -1, &b); *value = b != SQFalse; return ret; }
	static SQRESULT getValue(HSQUIRRELVM v, SQBool *value) { return sq_getbool(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQInteger *value) { return sq_getinteger(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQFloat *value) { return sq_getfloat(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, const SQChar **value) { return sq_getstring(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, SQUserPointer *value) { return sq_getuserpointer(v, -1, value); }
	static SQRESULT getValue(HSQUIRRELVM v, ObjectInfo *value) { value->getStack(v,-1); return SQ_OK; }
	static SQRESULT getValue(HSQUIRRELVM v, std::basic_string<SQChar> *value) {const SQChar *str; SQRESULT ret; if (SQ_SUCCEEDED((ret = sq_getstring(v, -1, &str)))) { *value = str;} return ret;}

#ifdef USESQPLUS
	// 値の格納
	// 格納失敗したときはオブジェクトが削除されるので delete の必要はない
	// ※元が squirrel 側で生成されたオブジェクトだった場合は必ず成功する
	template<typename T>
	static void pushValue(HSQUIRRELVM v, T *value) {
		if (value) {
			if (pushObject(v, value) || SQ_SUCCEEDED(pushnewsqobj(v, GetTypeName(*value), value))) {
				return;
			}
		}
		sq_pushnull(v);
	}
	// 値の取得
	template<typename T>
	static SQRESULT getValue(HSQUIRRELVM v, T **value) {
		*value = SqPlus::GetInstance<T,false>(v, -1);
		return SQ_OK;
	}
#else
#ifdef USESQRAT
	// 値の格納 XXX処理検討中
	// 値の取得
	template<typename T>
	static SQRESULT getValue(HSQUIRRELVM v, T **value) {
		*value = Sqrat::Var<T*>(v, -1).value;
		return SQ_OK;
	}
#else
	// 値の格納 XXX処理検討中
	// 値の取得
	template<typename T>
	static SQRESULT getValue(HSQUIRRELVM v, T **value) {
		*value = (T*)::getInstance(v, -1, GetTypeName(*value));
		return SQ_OK;
	}
#endif
#endif
	
	// 値の強制初期化
	static void clearValue(bool *value) { *value = false; }
	static void clearValue(SQBool *value) { *value = SQFalse; }
	static void clearValue(SQInteger *value) { *value = 0; }
	static void clearValue(SQFloat *value) { *value = 0.0f; }
	static void clearValue(const SQChar **value) { *value = 0; }
	static void clearValue(SQUserPointer *value) { *value = 0; }
	static void clearValue(ObjectInfo *value) { value->clear(); }
	static void clearValue(std::basic_string<SQChar> *value) { *value = _SC(""); }

	// 値の強制初期化
	template<typename T>
	static void clearValue(T **value) {
		*value = 0;
	}
	
	// 値の取得：基本↑のコピペ。文字列は安全でない場合があるので排除する必要あり
	static SQRESULT getResultValue(HSQUIRRELVM v, bool *value) { SQBool b;SQRESULT ret = sq_getbool(v, -1, &b); *value = b != SQFalse; return ret; }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQBool *value) { return sq_getbool(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQInteger *value) { return sq_getinteger(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQFloat *value) { return sq_getfloat(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, SQUserPointer *value) { return sq_getuserpointer(v, -1, value); }
	static SQRESULT getResultValue(HSQUIRRELVM v, ObjectInfo *value) { value->getStack(v,-1); return SQ_OK; }
	static SQRESULT getResultValue(HSQUIRRELVM v, std::basic_string<SQChar> *value) {const SQChar *str; SQRESULT ret; if (SQ_SUCCEEDED((ret = sq_getstring(v, -1, &str)))) { *value = str;} return ret;}

	// 値の取得
	template<typename T>
	SQRESULT getResultValue(HSQUIRRELVM v, T **value) {
		return getValue(value);
	}
	
private:
	HSQOBJECT obj; // オブジェクト参照情報
};

/**
 * 引数処理用情報
 */
class StackInfo {

public:
	/**
	 * コンストラクタ
	 * @param vm VM
	 */
	StackInfo(HSQUIRRELVM vm) : vm(vm) {
		argc = sq_gettop(vm) - 1;
	}
	
	/**
	 * @return 引数の数
	 */
	int len() const {
		return argc;
	}

	/**
	 * @return self参照
	 */
	ObjectInfo getSelf() const {
		return ObjectInfo(vm, 1);
	}

	/**
	 * @param n 引数番号 0～
	 * @return 引数の型
	 */
	SQObjectType getType(int n) const {
		if (n < argc) {
			return sq_gettype(vm, n+2);
		}
		return OT_NULL;
	}
	
	/**
	 * @param n 引数番号 0～
	 * @return 引数の型
	 */
	ObjectInfo getArg(int n) const {
		ObjectInfo ret;
		if (n < argc) {
			ret.getStack(vm, n+2);
		}
		return ret;
	}

	/**
	 * 引数取得
	 * @param n 引数番号 0～
	 * @return 引数オブジェクト
	 */
	ObjectInfo operator[](int n) const {
		return getArg(n);
	}

	// 結果登録
	SQRESULT setReturn() const { return 0; }
	template<typename T>
	SQRESULT setReturn(T value) const {
		ObjectInfo::pushValue(vm, value);
		return 1;
	}

protected:
	HSQUIRRELVM vm; //< VM
	SQInteger argc; //< 引数の数
};

};

#endif
