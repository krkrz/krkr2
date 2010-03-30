/**
 * 一般的な sqobject 実装
 * 
 * Object, Thread の登録処理の実装例です。
 * 継承情報は単純リスト管理してます
 */
#include "sqfunc.h"

#include <string.h>
#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

SQRESULT ERROR_CREATE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("can't create native instance"));
}

SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad instance"));
}

SQRESULT ERROR_BADMETHOD(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad method"));
}

sqobject::ObjectInfo typeTagListMap;
sqobject::ObjectInfo typeMap;

/**
 * 継承関係を登録する
 * @param typeName 型名
 * @param parentName 親の型名
 */
void
registerInherit(const SQChar *typeName, const SQChar *parentName)
{
	typeMap.create(typeName, parentName);
}

/**
 * 親の型名を返す
 * @param typeName 型名
 * @return 親の型名
 */
const SQChar *
getParentName(const SQChar *typeName)
{
	const SQChar *parent = NULL;
	typeMap.get(typeName, &parent);
	return parent;
}

/**
 * オブジェクトのタグを登録する。親オブジェクトにも再帰的に登録する。
 * @param typeName 型名
 * @parma tag タグ
 */
void
registerTypeTag(const SQChar *typeName, SQUserPointer tag)
{
	// タグを登録
	sqobject::ObjectInfo list = typeTagListMap.get(typeName);
	if (!list.isArray()) {
		list.initArray();
		typeTagListMap.create(typeName, list);
	}
	list.append(tag);

	// 親クラスにも登録
	const SQChar *pname = getParentName(typeName);
	if (pname) {
		registerTypeTag(pname, tag);
	}
}

/**
 * 該当オブジェクトのネイティブインスタンスを取得。登録されてるタグリストを使う
 * @param v squirrelVM
 * @param idx スタックインデックス
 * @param typeName 型名
 * @return 指定された型のネイティブインスタンス。みつからない場合は NULL
 */
SQUserPointer
getInstance(HSQUIRRELVM v, SQInteger idx, const SQChar *typeName)
{
	sqobject::ObjectInfo list = typeTagListMap.get(typeName);
	if (list.isArray()) {
		SQInteger max = list.len();
		for (SQInteger i=0;i<max;i++) {
			SQUserPointer tag;
			list.get(i, &tag);
			SQUserPointer up;
			if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, tag))) {
				return up;
			}
		}
	}
	return NULL;
}

namespace sqobject {

// typetag 全ソースでユニークなアドレスにする必要がある
const SQUserPointer OBJECTTYPETAG = (SQUserPointer)"OBJECTTYPETAG";
const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

// クラス情報定義

DECLARE_CLASSNAME(Object, SQOBJECT);
DECLARE_CLASSNAME(Thread, SQTHREAD);

// global vm
HSQUIRRELVM vm;

/// vm 初期化
HSQUIRRELVM init() {
	vm = sq_open(1024);
	sq_pushroottable(vm);
	sqstd_register_mathlib(vm);
	sqstd_register_stringlib(vm);
	sqstd_seterrorhandlers(vm);
	sq_pop(vm,1);
	typeMap.initTable();
	typeTagListMap.initTable();
	return vm;
}

/// 情報保持用グローバルVMの取得
HSQUIRRELVM getGlobalVM()
{
	return vm;
}

/// vm 終了
void done()
{
	// ルートテーブルをクリア
	sq_pushroottable(vm);
	sq_clear(vm,-1);
	sq_pop(vm,1);
	typeTagListMap.clearData();
	typeTagListMap.clear();
	typeMap.clearData();
	typeMap.clear();
	sq_close(vm);
}

// Thread のインスタンスユーザポインタを取得
Thread *
ObjectInfo::getThread()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Thread *ret = NULL;
	ret = (Thread*)::getInstance(gv, -1, GetTypeName(ret));
	sq_pop(gv,1);
	return ret;
}

// Object のインスタンスユーザポインタを取得
Object *
ObjectInfo::getObject()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Object *ret = NULL;
	ret = (Object*)::getInstance(gv, -1, GetTypeName(ret));
	sq_pop(gv,1);
	return ret;
}

// ------------------------------------------------------------------
// クラス登録用マクロ
// ------------------------------------------------------------------

#ifndef USE_SQOBJECT_TEMPLATE

static SQRESULT Object_notify(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		instance->notify();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_notifyAll(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		instance->notifyAll();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_hasSetProp(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->hasSetProp(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_setDelegate(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->setDelegate(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_getDelegate(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->getDelegate(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_get(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->_get(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Object_set(HSQUIRRELVM v)
{
	Object *instance = SQTemplate<Object,BaseClass>::getInstance(v);
	if (instance) {
		return instance->_set(v);
	}
	return ERROR_BADINSTANCE(v);
}

#endif

/**
 * クラスの登録
 * @param v squirrel VM
 */
void
Object::registerClass()
{
	HSQUIRRELVM v = getGlobalVM();
	sq_pushroottable(v); // root

	SQCLASS(Object,BaseClass,OBJECTTYPETAG);
	SQVCONSTRUCTOR();
	
#ifdef USE_SQOBJECT_TEMPLATE
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
	cls.RegisterV(&Object::_get, _SC("get"));
	cls.RegisterV(&Object::_set, _SC("set"));
#else
	SQNFUNC(Object,notify);
	SQNFUNC(Object,notifyAll);
	SQNFUNC(Object,hasSetProp);
	SQNFUNC(Object,setDelegate);
	SQNFUNC(Object,getDelegate);
	cls.Register(Object_get, _SC("_get"));
	cls.Register(Object_set, _SC("_set"));
	SQNFUNC(Object,get);
	SQNFUNC(Object,set);
#endif
	sq_createslot(v, -3); // 生成したクラスを登録
	sq_pop(v,1); // root
};

#ifndef USE_SQOBJECT_TEMPLATE

static SQRESULT Thread_exec(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->exec(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_exit(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->exit(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_stop(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		instance->stop();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_run(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		instance->run();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_getCurrentTick(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		sq_pushinteger(v,instance->getCurrentTick());
		return 1;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_getStatus(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		sq_pushinteger(v,instance->getStatus());
		return 1;
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_getExitCode(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->getExitCode(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_wait(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		return instance->wait(v);
	}
	return ERROR_BADINSTANCE(v);
}

static SQRESULT Thread_cancelWait(HSQUIRRELVM v)
{
	Thread *instance = SQTemplate<Thread,Object>::getInstance(v);
	if (instance) {
		instance->cancelWait();
		return SQ_OK;
	}
	return ERROR_BADINSTANCE(v);
}

#endif

/**
 * クラスの登録
 * @param v squirrel VM
 */
void
Thread::registerClass()
{
	HSQUIRRELVM v = getGlobalVM();
	sq_pushroottable(v); // root

	SQCLASS(Thread, Object, THREADTYPETAG);
	SQVCONSTRUCTOR();

#ifdef USE_SQOBJECT_TEMPLATE
	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);
#else
	SQNFUNC(Thread,exec);
	SQNFUNC(Thread,exit);
	SQNFUNC(Thread,stop);
	SQNFUNC(Thread,run);
	SQNFUNC(Thread,getCurrentTick);
	SQNFUNC(Thread,getStatus);
	SQNFUNC(Thread,getExitCode);
	SQNFUNC(Thread,wait);
	SQNFUNC(Thread,cancelWait);
#endif
	
	sq_createslot(v, -3);
	sq_pop(v, 1); // root
};

}

