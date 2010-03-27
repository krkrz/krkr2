/**
 * sqrat 版の sqobject 実装
 *
 * sqrat を使った Object, Thread 登録処理の実装例です。
 * sqrat の機能をつかって継承を処理しています。
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include "sqratfunc.h"

#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

using namespace sqobject;

namespace sqobject {

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
	sq_close(vm);
}

// ---------------------------------------------------
// オブジェクト取得
// ---------------------------------------------------

// Thread のインスタンスユーザポインタを取得
Thread *
ObjectInfo::getThread()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Thread *ret = Sqrat::Var<Thread*>(gv, -1).value;
	sq_pop(gv,1);
	return ret;
}

// Object のインスタンスユーザポインタを取得
Object *
ObjectInfo::getObject()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Object *ret = Sqrat::Var<Object*>(gv, -1).value;
	sq_pop(gv, 1);
	return ret;
}

// デストラクタ登録用
static SQRESULT destructor(HSQUIRRELVM v) {
	return SQ_OK;
}

/**
 * Object クラスの登録
 */
void
Object::registerClass()
{
	Sqrat::Class<Object, sqobject::VMConstructor<Object>> cls(vm);
	cls.SquirrelFunc(_SC("destructor"), ::destructor);
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	// sqrat の set/get を上書きして sqobject 機能と整合をとる
	sqobject::OverrideSetGet<Object>::Func(vm);
	Sqrat::RootTable(vm).Bind(SQOBJECTNAME, cls);
};

/**
 * Thread クラスの登録
 */
void
Thread::registerClass()
{
	Sqrat::DerivedClass<Thread, Object, sqobject::VMConstructor<Thread>> cls(vm);
	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);
	sqobject::OverrideSetGet<Thread>::Func(vm);
	Sqrat::RootTable(vm).Bind(SQTHREADNAME, cls);
};

}
