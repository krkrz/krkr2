#ifndef SQPLUSFUNC_H
#define SQPLUSFUNC_H

#include "sqobjectinfo.h"
using namespace SqPlus;

/**
 * 各種処理用テンプレート
 */
template <typename T>
struct SQTemplate {
  static SQRESULT release(SQUserPointer up, SQInteger size) {
	T* self = (T*)up;
	if (self){ 
	  self->destructor();
	  delete self;
	}
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
  static SQRESULT factory(HSQUIRRELVM v) {
	T *data = NULL;
	SQRESULT ret;
	if (SQ_SUCCEEDED(ret = T::factory(v, &data))) {
	  data->initSelf(v);
	  SqPlus::PostConstruct<T>(v, data, release);
	  return SQ_OK;
	} else {
	  return ret;
	}
  }

  static SQRESULT noconstructor(HSQUIRRELVM v) {
	return sq_throwerror(v, "can't create instance");
  }
  static SQRESULT destructor(HSQUIRRELVM v) {
	  return SQ_OK;
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

template <typename T>
SQRESULT pushotherobj(HSQUIRRELVM v, const SQChar *className, T *obj) {
	if (obj && CreateNativeClassInstance(v, className, obj, ReleaseClassPtr<T>::release)) {
		return 1;
	} else {
		return sq_throwerror(v, "can't create instance");
	}
}


// ------------------------------------------------------------------
// クラス登録用マクロ
// ------------------------------------------------------------------

// ----------------------------------------------
// クラス定義用マクロ
// クラスの名前参照のためあらかじめ DECLARE_INSTANCE_TYPE_* で型情報を定義しておく必要があります

// クラス定義(通常用)
#define SQCLASS_NOCONSTRUCTOR(Class, Name)\
 SQClassDefNoConstructor<Class> cls(TypeInfo<Class>().typeName);\
 cls.staticFuncVarArgs(SQTemplate<Class>::noconstructor, _SC("constructor"))

// クラス定義 任意継承
#define SQCLASSEX(Class,Parent,Name)\
 SQClassDefNoConstructor<Class,Parent> cls(TypeInfo<Class>().typeName, TypeInfo<Parent>().typeName);\
 cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"))

#define SQCLASSEX_VCONSTRUCTOR(Class,Parent,Name)\
 SQClassDefNoConstructor<Class,Parent> cls(TypeInfo<Class>().typeName, TypeInfo<Parent>().typeName);\
 cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"))

#define SQCLASSEX_FACTORY(Class,Parent,Name)\
 SQClassDefNoConstructor<Class,Parent> cls(TypeInfo<Class>().typeName, TypeInfo<Parent>().typeName);\
 cls.staticFuncVarArgs(SQTemplate<Class>::factory, _SC("constructor"))

#define SQCLASSEX_NOCONSTRUCTOR(Class,Parent,Name)\
 SQClassDefNoConstructor<Class,Parent> cls(TypeInfo<Class>().typeName, TypeInfo<Parent>().typeName);\
 cls.staticFuncVarArgs(SQTemplate<Class>::noconstructor, _SC("constructor"))

// Object継承クラス
#define SQCLASSOBJ(Class,Name)\
 SQClassDefNoConstructor<Class,Object> cls(TypeInfo<Class>().typeName, SQOBJECTNAME);\
 cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"))

#define SQCLASSOBJ_VCONSTRUCTOR(Class,Name)\
 SQClassDefNoConstructor<Class,Object> cls(TypeInfo<Class>().typeName, SQOBJECTNAME);\
 cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"))

#define SQCLASSOBJ_FACTORY(Class,Name)\
 SQClassDefNoConstructor<Class,Object> cls(TypeInfo<Class>().typeName, SQOBJECTNAME);\
 cls.staticFuncVarArgs(SQTemplate<Class>::factory, _SC("constructor"))

#define SQCLASSOBJ_NOCONSTRUCTOR(Class,Name)\
 SQClassDefNoConstructor<Class,Object> cls(TypeInfo<Class>().typeName, SQOBJECTNAME);\
 cls.staticFuncVarArgs(SQTemplate<Class>::noconstructor, _SC("constructor"))


// ----------------------------------------------
// ファンクション登録用マクロ

#define SQFUNC(Class, Name) cls.func(&Class::Name,_SC(#Name))
#define SQFUNCNAME(Class, Method, Name) cls.func(&Class::Method,_SC(#Name))
#define SQVFUNC(Class, Name) cls.funcVarArgs(&Class::Name,_SC(#Name))
#define SQVFUNCNAME(Class, Method, Name) cls.funcVarArgs(&Class::Method,_SC(#Name))
#define SQSFUNC(Class, Name) cls.staticFunc(&Class::Name,_SC(#Name))
#define SQSFUNCNAME(Class, Method, Name) cls.staticFunc(&Class::Method,_SC(#Name))
#define SQSVFUNC(Class, Name) cls.staticFuncVarArgs(&Class::Name,_SC(#Name))
#define SQSVFUNCNAME(Class, Method, Name) cls.staticFuncVarArgs(&Class::Method,_SC(#Name))

#endif
