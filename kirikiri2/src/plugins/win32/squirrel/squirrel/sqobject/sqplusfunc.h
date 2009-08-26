#ifndef SQPLUSFUNC_H
#define SQPLUSFUNC_H

#include <sqplus.h>

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

// ------------------------------------------------------------------
// クラス登録用マクロ
// ------------------------------------------------------------------

// ----------------------------------------------
// クラス定義用マクロ
// クラスの名前参照のためあらかじめ DECLARE_INSTANCE_TYPE_* で型情報を定義しておく必要があります

// クラス定義(通常用)
#define SQCLASS(Class) SQClassDefNoConstructor<Class> cls(TypeInfo<Class>().typeName)
// クラス定義 sqobject::Objectを継承（型名:Object)
#define SQCLASSOBJ(Class) SQClassDefNoConstructor<Class,Object> cls(TypeInfo<Class>().typeName, SQOBJECTNAME)
// クラス定義 任意継承
#define SQCLASSEX(Class,Parent) SQClassDefNoConstructor<Class,Parent> cls(TypeInfo<Class>().typeName, TypeInfo<Parent>().typeName)

// ----------------------------------------------
// コンストラクタ定義用マクロ

// sqobject::Object 系コンストラクタ・引数なし
#define SQCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"))
#define SQCONSTRUCTOR_(Class, TypeCheck) cls.staticFuncVarArgs(SQTemplate<Class>::constructor, _SC("constructor"), TypeCheck)

// sqobject::Object 系コンストラクタ・HSQUIRRELを引数
#define SQVCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"))
#define SQVCONSTRUCTOR_(Class,TypeCheck) cls.staticFuncVarArgs(SQTemplate<Class>::vconstructor, _SC("constructor"), TypeCheck)

// クラスに static SQRESULT constructor(HSQUIRRELVM) を持つ場合用
#define SQSCONSTRUCTOR(Class) cls.staticFuncVarArgs(Class::constructor, _SC("constructor"))
#define SQSCONSTRUCTOR_(Class,TypeCheck) cls.staticFuncVarArgs(Class::constructor, _SC("constructor"), TypeCheck)

// コンストラクタ無し
#define SQNOCONSTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::noconstructor, _SC("constructor"))

// 自前コンストラクタ
#define SQMYCONSTRUCTOR(constructor) cls.staticFuncVarArgs(constructor, _SC("constructor"))
#define SQMYCONSTRUCTOR_(constructor, TypeCheck) cls.staticFuncVarArgs(constructor, _SC("constructor"), TypeCheck)

// デストラクタ(空)
#define SQDESTRUCTOR(Class) cls.staticFuncVarArgs(SQTemplate<Class>::destructor, _SC("destructor"))

// ----------------------------------------------
// ファンクション登録用マクロ

#define SQFUNC(Class, Name) cls.func(&Class::Name,_SC(#Name))
#define SQFUNC_(Class, Name, TypeCheck) cls.func(&Class::Name,_SC(#Name), TypeCheck)
#define SQVFUNC(Class, Name) cls.funcVarArgs(&Class::Name,_SC(#Name))
#define SQVFUNC_(Class, Name, TypeCheck) cls.funcVarArgs(&Class::Name,_SC(#Name), TypeCheck)
#define SQSFUNC(Class, Name) cls.staticFunc(&Class::Name,_SC(#Name))
#define SQSFUNC_(Class, Name, TypeCheck) cls.staticFunc(&Class::Name,_SC(#Name), TypeCheck)
#define SQSVFUNC(Class, Name) cls.staticFuncVarArgs(&Class::Name,_SC(#Name))
#define SQSVFUNC_(Class, Name, TypeCheck) cls.staticFuncVarArgs(&Class::Name,_SC(#Name), TypeCheck)

#define SQPUSH(v, obj) pushsqobj(v, GetTypeName(*obj), obj)
#define SQPUSHNEW(v, obj) pushnewsqobj(v, GetTypeName(*obj), obj)
#define SQSETINSTANCE(v, Class, param) SQTemplate<Class>::_constructor(v, new Class param)

#endif
