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

protected:
	HSQOBJECT obj; // オブジェクト参照情報

public:
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
	bool isDelegate();

	// bindenv させるかどうか
	bool isBindDelegate();

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
};

};

#endif
