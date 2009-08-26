/**
 * SQPLUS 版の sqobject 実装
 *
 * sqplus を使った Object, Thread 登録処理の実装例です。
 * sqplus の機能をつかって継承を処理しています。
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include "sqplusfunc.h"

#include <sqstdstring.h>
#include <sqstdmath.h>
#include <sqstdaux.h>

using namespace SqPlus;
using namespace sqobject;
DECLARE_INSTANCE_TYPE_NAME_RELEASE(Object, SQOBJECT);
DECLARE_INSTANCE_TYPE_NAME_RELEASE(Thread, SQTHREAD);

namespace sqobject {

// global vm
HSQUIRRELVM vm;

/// vm 初期化
HSQUIRRELVM init() {
	vm = sq_open(1024);
	SquirrelVM::InitNoRef(vm);
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
	SquirrelVM::ClearRootTable();
	SquirrelVM::Shutdown();
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
	Thread *ret = SqPlus::GetInstance<Thread,false>(gv, -1);
	sq_pop(gv,1);
	return ret;
}

// Object のインスタンスユーザポインタを取得
Object *
ObjectInfo::getObject()
{
	HSQUIRRELVM gv = getGlobalVM();
	push(gv);
	Object *ret = SqPlus::GetInstance<Object,false>(gv, -1);
	sq_pop(gv, 1);
	return ret;
}

/**
 * Object クラスの登録
 */
void
Object::registerClass()
{
	SQCLASS(Object);
	SQVCONSTRUCTOR(Object);
	SQDESTRUCTOR(Object);
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
	// _get / _set で登録したものは後から参照できないので、
	// 継承先で使うために別名で登録しておく
	cls.funcVarArgs(&Object::_get,_SC("get"));
	cls.funcVarArgs(&Object::_set,_SC("set"));
};

/**
 * Thread クラスの登録
 */
void
Thread::registerClass()
{
	SQCLASSEX(Thread, Object);
	SQVCONSTRUCTOR(Thread);
	SQDESTRUCTOR(Thread);
	SQVFUNC(Thread,exec);
	SQVFUNC(Thread,exit);
	SQFUNC(Thread,stop);
	SQFUNC(Thread,run);
	SQFUNC(Thread,getCurrentTick);
	SQFUNC(Thread,getStatus);
	SQVFUNC(Thread,getExitCode);
	SQVFUNC(Thread,wait);
	SQFUNC(Thread,cancelWait);
};

}
