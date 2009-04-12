/**
 * SQPLUS 版の sqobject 実装
 *
 * sqplus を使った Object, Thread 登録処理の実装例です。
 * sqplus の機能をつかって継承を処理しています。
 */
#include "sqobjectinfo.h"
#include "sqobject.h"
#include "sqthread.h"
#include <sqplus.h>

using namespace SqPlus;
using namespace sqobject;
DECLARE_INSTANCE_TYPE(Object);
DECLARE_INSTANCE_TYPE(Thread);

namespace sqobject {

// global vm
HSQUIRRELVM vm;

/// vm 初期化
HSQUIRRELVM init() {
	vm = sq_open(1024);
	SquirrelVM::InitNoRef(vm);
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
	if (sq_isinstance(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		Thread *ret = SqPlus::GetInstance<Thread,false>(gv, -1);
		sq_pop(gv,1);
		return ret;
	}
	return NULL;
}

// Object のインスタンスユーザポインタを取得
Object *
ObjectInfo::getObject()
{
	if (sq_isinstance(obj)) {
		HSQUIRRELVM gv = getGlobalVM();
		sq_pushobject(gv, obj);
		Object *ret = SqPlus::GetInstance<Object,false>(gv, -1);
		sq_pop(gv, 1);
		return ret;
	}
	return NULL;
}

/**
 * 汎用コンストラクタ
 * @param v squirrel VM
 */
template <typename T>
struct GeneralConstructor {
	static SQRESULT release(SQUserPointer up, SQInteger size) {
		if (up) { delete (T*)up; }
		return SQ_OK;
	}
	static SQRESULT constructor(HSQUIRRELVM v) {
		return SqPlus::PostConstruct<T>(v, new T(v), release);
	}
};

// ------------------------------------------------------------------
// クラス登録用マクロ
// ------------------------------------------------------------------

#define SQCONSTRUCT(Class) cls.staticFuncVarArgs(GeneralConstructor<Class>::constructor, _SC("constructor"))
#define SQFUNC(Class, Name) cls.func(&Class::Name,_SC(#Name))
#define SQVFUNC(Class, Name) cls.funcVarArgs(&Class::Name,_SC(#Name))

/**
 * Object クラスの登録
 */
void
Object::registerClass()
{
	SqPlus::SQClassDef<Object> cls(SQOBJECTNAME);
	SQCONSTRUCT(Object);
	SQFUNC(Object,notify);
	SQFUNC(Object,notifyAll);
	SQVFUNC(Object,hasSetProp);
	SQVFUNC(Object,setDelegate);
	SQVFUNC(Object,getDelegate);
	SQVFUNC(Object,_get);
	SQVFUNC(Object,_set);
};

/**
 * Thread クラスの登録
 */
void
Thread::registerClass()
{
	SqPlus::SQClassDef<Thread,Object> cls(SQTHREADNAME);
	SQCONSTRUCT(Thread);
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
