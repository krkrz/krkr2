#ifndef SQPLUSFUNC_H
#define SQPLUSFUNC_H

#include <sqplus.h>

/**
 * 各種処理用テンプレート
 */
template <typename T>
struct SQTemplate {
  static SQRESULT release(SQUserPointer up, SQInteger size) {
	if (up) { delete (T*)up; }
	return SQ_OK;
  }
  static SQRESULT _constructor(HSQUIRRELVM v, T *data) {
	if (data) {
	  data->initSelf(v);
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return SQ_ERROR;
	}
  }
  static SQRESULT constructor(HSQUIRRELVM v) {
	T *data = new T();
	if (data) {
	  data->initSelf(v);
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return SQ_ERROR;
	}
  }
  static SQRESULT vconstructor(HSQUIRRELVM v) {
	T *data = new T(v);
	if (data) {
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return SQ_ERROR;
	}
  }
  static SQRESULT noconstructor(HSQUIRRELVM v) {
	return sq_throwerror(v, "can't create instance");
  }
};

/**
 * 該当する型のオブジェクトをスタックに格納する。
 * initSelfの呼び出しも併せて行う
 * @param name 型名
 * @param obj オブジェクト
 * @return SQRESULT
 */
template <typename T>
SQRESULT pushsqobj(HSQUIRRELVM v, const SQChar *className, T *obj) {
	if (obj && CreateNativeClassInstance(v, className, obj, SQTemplate<T>::release)) {
		obj->initSelf(v,-1);
		return 1;
	} else {
		return sq_throwerror(v, "can't create instance");
	}
}

/**
 * 該当する型のオブジェクトをスタックに格納する。生成失敗した場合はオブジェクトをdeleteする
 * initSelfの呼び出しも併せて行う
 * @param name 型名
 * @param obj オブジェクト
 * @return SQRESULT
 */
template <typename T>
SQRESULT pushnewsqobj(HSQUIRRELVM v, const SQChar *className, T *obj) {
	if (obj && CreateNativeClassInstance(v, className, obj, SQTemplate<T>::release)) {
		obj->initSelf(v,-1);
		return 1;
	} else {
		delete obj;
		return sq_throwerror(v, "can't create instance");
	}
}


// ------------------------------------------------------------------
// クラス登録用マクロ
// ------------------------------------------------------------------

#define SQCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"))
#define SQVCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"))
#define SQNOCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::noconstructor, _SC("constructor"))
#define SQFUNC(Class, Name) cls.func(&Class::Name,_SC(#Name))
#define SQVFUNC(Class, Name) cls.funcVarArgs(&Class::Name,_SC(#Name))
#define SQSFUNC(Class, Name) cls.staticFunc(&Class::Name,_SC(#Name))
#define SQSVFUNC(Class, Name) cls.staticFuncVarArgs(&Class::Name,_SC(#Name))
#define SQPUSH(v, obj) pushsqobj(v, GetTypeName(*obj), obj)
#define SQPUSHNEW(v, obj) pushnewsqobj(v, GetTypeName(*obj), obj)
#define SQSETINSTANCE(v, Class, param) SQTemplate<Class>::_constructor(v, new Class param)

#endif
