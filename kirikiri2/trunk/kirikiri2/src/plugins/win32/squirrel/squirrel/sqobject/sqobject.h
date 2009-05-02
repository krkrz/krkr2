/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
#ifndef __SQOBJECT_H__
#define __SQOBJECT_H__

// 型名
#ifndef SQOBJECTNAME
#define SQOBJECTNAME _SC("Object")
#endif

#include "sqobjectinfo.h"

namespace sqobject {

const SQChar *getString(HSQUIRRELVM v, int idx);

/**
 * オブジェクト用
 */
class Object {

protected:
	// 自己参照
	ObjectInfo self;
	// このオブジェクトを待ってるスレッドの一覧
	ObjectInfo _waitThreadList;
	// delegate
	ObjectInfo delegate;

public:
	/**
	 * オブジェクト待ちの登録
	 * @param thread スレッド
	 */
	void _addWait(ObjectInfo &thread);
	
	/**
	 * オブジェクト待ちの解除
	 * @param thread スレッド
	 */
	void _removeWait(ObjectInfo &thread);
	
	/**
	 * コンストラクタ
	 */
	Object();
	
	/**
	 * コンストラクタ
	 */
	Object(HSQUIRRELVM v, int delegateIdx=2);

	/**
	 * デストラクタ
	 */
	virtual ~Object();

	/**
	 * 自己参照初期化用
	 * デフォルトコンストラクタで処理した場合は必ずこの処理をよぶこと
	 * @param v SQUIRREL vm
	 * @param idx 自分のオブジェクトがあるインデックス
	 */
	void initSelf(HSQUIRRELVM v, int idx=1);

public:
	
	// ------------------------------------------------------------------

	/**
	 * このオブジェクトを待っている１スレッドの待ちを解除
	 */
	void notify();
	
	/**
	 * このオブジェクトを待っている全スレッドの待ちを解除
	 */
	void notifyAll();
	
	/**
	 * プロパティから値を取得
	 * @param name プロパティ名
	 * @return プロパティ値
	 */
	SQRESULT _get(HSQUIRRELVM v);

	/**
	 * プロパティに値を設定
	 * @param name プロパティ名
	 * @param value プロパティ値
	 */
	SQRESULT _set(HSQUIRRELVM v);

	/**
	 * setプロパティの存在確認
	 * @param name プロパティ名
	 * @return setプロパティが存在したら true
	 */
	SQRESULT hasSetProp(HSQUIRRELVM v);
	
	/**
	 * 委譲の設定
	 */
	SQRESULT setDelegate(HSQUIRRELVM v);

	/**
	 * 委譲の取得
	 */
	SQRESULT getDelegate(HSQUIRRELVM v);

public:
	/**
	 * squirrel クラス登録
	 */
	static void registerClass();


protected:

	/**
	 * 自己オブジェクトイベント呼び出し（引数無し)
	 * @param eventName イベント名
	 */
	SQRESULT callEvent(const SQChar *eventName) {
		return self.callMethod(eventName);
	}

	/**
	 * 自己オブジェクトイベント呼び出し（引数1つ)
	 * @param eventName イベント名
	 * @param p1 引数
	 */
	template<typename T1> SQRESULT callEvent(const SQChar *eventName, T1 p1) {
		return self.callMethod(eventName, p1);
	}
	
	/**
	 * 自己オブジェクトイベント呼び出し（引数2つ)
	 * @param eventName イベント名
	 * @param p1 引数
	 * @param p2 引数2
	 */
	template<typename T1, typename T2> SQRESULT callEvent(const SQChar *eventName, T1 p1, T2 p2) {
		return self.callMethod(eventName, p1, p2);
	}
	
	/**
	 * 返値有り自己オブジェクトイベント呼び出し（引数無し)
	 * @param r 帰り値ポインタ
	 * @param eventName イベント名
	 */
	template<typename R> SQRESULT callEventResult(R* r, const SQChar *eventName) {
		return self.callMethodResult(r, eventName);
	}

	/**
	 * 返値あり自己オブジェクトイベント呼び出し（引数1つ)
	 * @param r 帰り値ポインタ
	 * @param eventName イベント名
	 * @param p1 引数
	 */
	template<typename R, typename T1> SQRESULT callEventResult(R* r, const SQChar *eventName, T1 p1) {
		return self.callMethodResult(r, eventName, p1);
	}
	
	/**
	 * 返値有り自己オブジェクトイベント呼び出し（引数2つ)
	 * @param r 帰り値ポインタ
	 * @param eventName イベント名
	 * @param p1 引数
	 * @param p2 引数2
	 */
	template<typename R, typename T1, typename T2> SQRESULT callEventResult(R* r, const SQChar *eventName, T1 p1, T2 p2) {
		return self.callMethodResult(r, eventName, p1, p2);
	}

};

};// namespace

// ログ出力用
#define SQPRINT(v,msg) {\
	SQPRINTFUNCTION print = sq_getprintfunc(v);\
	if (print) {\
		print(v,msg);\
	}\
}

#endif
