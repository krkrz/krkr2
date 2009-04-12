/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include <string.h>

namespace sqobject {

// 内容消去
void
ObjectInfo::clear()
{
	HSQUIRRELVM gv = getGlobalVM();
	sq_release(gv,&obj);
	sq_resetobject(&obj);
}

// スタックから取得
void
ObjectInfo::getStack(HSQUIRRELVM v, int idx)
{
	clear();
	HSQUIRRELVM gv = getGlobalVM();
	sq_move(gv, v, idx);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}

// スタックから弱参照として取得
void
ObjectInfo::getStackWeak(HSQUIRRELVM v, int idx)
{
	clear();
	HSQUIRRELVM gv = getGlobalVM();
	sq_weakref(v, idx);
	sq_move(gv, v, -1);
	sq_pop(v, 1);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}


// コンストラクタ
ObjectInfo::ObjectInfo() {
	sq_resetobject(&obj);
}

// コンストラクタ
ObjectInfo::ObjectInfo(HSQUIRRELVM v, int idx)
{
	sq_resetobject(&obj);
	HSQUIRRELVM gv = getGlobalVM();
	sq_move(gv, v, idx);
	sq_getstackobj(gv, -1, &obj);
	sq_addref(gv, &obj);
	sq_pop(gv, 1);
}

// コピーコンストラクタ
ObjectInfo::ObjectInfo(const ObjectInfo &orig)
{
	HSQUIRRELVM gv = getGlobalVM();
	sq_resetobject(&obj);
	obj = orig.obj;
	sq_addref(gv, &obj);
}

// 代入
ObjectInfo& ObjectInfo::operator =(const ObjectInfo &orig)
{
	HSQUIRRELVM gv = getGlobalVM();
	clear();
	obj = orig.obj;
	sq_addref(gv, &obj);
	return *this;
}

// デストラクタ
ObjectInfo::~ObjectInfo()
{
	clear();
}

// nullか？
bool
ObjectInfo::isNull() const
{
	if (sq_isweakref(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		sq_getweakrefval(gv, -1);
		bool ret = sq_gettype(gv, -1) == OT_NULL;
		sq_pop(gv, 2);
		return ret;
	}
	return sq_isnull(obj);
}

// 同じスレッドか？
bool
ObjectInfo::isSameThread(const HSQUIRRELVM v) const
{
	return sq_isthread(obj) && obj._unVal.pThread == v;
}

// スレッドを取得
ObjectInfo::operator HSQUIRRELVM() const
{
	HSQUIRRELVM vm = sq_isthread(obj) ? obj._unVal.pThread : NULL;
	return vm;
}
	
// オブジェクトをPUSH
void
ObjectInfo::push(HSQUIRRELVM v) const
{
    if (sq_isweakref(obj)) {
		sq_pushobject(v, obj);
		sq_getweakrefval(v, -1);
		sq_remove(v, -2);
	} else {
		sq_pushobject(v, obj);
	}
}

// ---------------------------------------------------
// delegate 処理用
// ---------------------------------------------------

// delegate として機能するかどうか
bool
ObjectInfo::isDelegate()
{
	return (sq_isinstance(obj) || sq_istable(obj));
}

// bindenv させるかどうか
bool
ObjectInfo::isBindDelegate()
{
	return (sq_isinstance(obj));
}

// ---------------------------------------------------
// データ取得
// ---------------------------------------------------

const SQChar *
ObjectInfo::getString()
{
	if (sq_isstring(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		const SQChar *mystr;
		sq_pushobject(gv, obj);
		sq_getstring(gv, -1, &mystr);
		sq_pop(gv, 1);
		return mystr;
	}
	return NULL;
}

// ---------------------------------------------------
// wait処理用メソッド
// ---------------------------------------------------

bool
ObjectInfo::isSameString(const SQChar *str) const
{
	if (str && sq_isstring(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		const SQChar *mystr;
		sq_pushobject(gv, obj);
		sq_getstring(gv, -1, &mystr);
		sq_pop(gv, 1);
		return mystr && scstrcmp(str, mystr) == 0;
	}
	return false;
}

};
