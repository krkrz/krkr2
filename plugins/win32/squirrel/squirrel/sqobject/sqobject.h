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
