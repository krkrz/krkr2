//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Support for C++ native class/method/property definitions
//---------------------------------------------------------------------------
#ifndef tjsNativeH
#define tjsNativeH

#include "tjsObject.h"

namespace TJS
{
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(tjs_int32, TJSRegisterNativeClass, (const tjs_char *name));
TJS_EXP_FUNC_DEF(tjs_int32, TJSFindNativeClassID, (const tjs_char *name));
TJS_EXP_FUNC_DEF(const tjs_char *, TJSFindNativeClassName, (tjs_int32 id));
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tTJSNativeInstanceType
//---------------------------------------------------------------------------
enum tTJSNativeInstanceType
{
	nitClass,
	nitMethod,
	nitProperty
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNativeInstance
//---------------------------------------------------------------------------
class tTJSNativeInstance : public iTJSNativeInstance
{
public:
	virtual tjs_error TJS_INTF_METHOD Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj) {return TJS_S_OK;}
	virtual void TJS_INTF_METHOD Invalidate() {;}
	virtual void TJS_INTF_METHOD Destruct() { delete this; }
	virtual ~tTJSNativeInstance() {;};
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNativeClassMethod
//---------------------------------------------------------------------------
class tTJSNativeClassMethod : public tTJSDispatch
{
	typedef tTJSDispatch inherited;
public:

	tTJSNativeClassMethod();
	~tTJSNativeClassMethod();

	tjs_error TJS_INTF_METHOD
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tjs_char *classname,
		iTJSDispatch2 *objthis);
	tjs_error  TJS_INTF_METHOD
		FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis);
	virtual tjs_error Process(tTJSVariant *result,
			tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis) = 0;
};
//---------------------------------------------------------------------------
// tTJSNativeClassConstructor
//---------------------------------------------------------------------------
class tTJSNativeClassConstructor : public tTJSNativeClassMethod
{
	typedef tTJSNativeClassMethod inherited;
public:
	tjs_error  TJS_INTF_METHOD
		FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis);
};
//---------------------------------------------------------------------------
// tTJSNativeClassProperty
//---------------------------------------------------------------------------
class tTJSNativeClassProperty : public tTJSDispatch
{
	typedef tTJSDispatch inherited;
public:

	tTJSNativeClassProperty();
	~tTJSNativeClassProperty();

	tjs_error TJS_INTF_METHOD
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tjs_char *classname,
		iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis);

	virtual tjs_error Get(tTJSVariant *result, iTJSDispatch2 *objthis) = 0;
	virtual tjs_error Set(const tTJSVariant *param, iTJSDispatch2 *objthis) = 0;

	tjs_error DenyGet(tjs_uint32 flag,
		const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,
		iTJSDispatch2 *objthis);
	tjs_error DenySet(tjs_uint32 flag,
		const tjs_char *membername, tjs_uint32 *hint, const tTJSVariant *param,
		iTJSDispatch2 *objthis);
};
//---------------------------------------------------------------------------
// tTJSNativeClass
//---------------------------------------------------------------------------
class tTJSNativeClass : public tTJSCustomObject
{
	typedef tTJSCustomObject inherited;

public:
	tTJSNativeClass(const ttstr &name);
	~tTJSNativeClass();

	void RegisterNCM(const tjs_char *name,
		iTJSDispatch2 *dsp,
		const tjs_char *classname,
		tTJSNativeInstanceType type,
		tjs_uint32 flags = 0);

protected:
	tjs_int32 _ClassID;
	ttstr ClassName;
	void Finalize(void);

	virtual iTJSNativeInstance *CreateNativeInstance()  {return NULL;}

	virtual iTJSDispatch2 *CreateBaseTJSObject();

private:
	std::vector<iTJSDispatch2 *> Items;
	std::vector<ttstr> ItemNames;
	std::vector<tjs_uint32> ItemFlags;
	void ClearItems();

public:
	tjs_error TJS_INTF_METHOD
	FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
			tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis);
	tjs_error TJS_INTF_METHOD
	CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		 iTJSDispatch2 **result,
			tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis);
	tjs_error TJS_INTF_METHOD
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		 const tjs_char *classname,
		iTJSDispatch2 *objthis);


	const ttstr & GetClassName() const { return ClassName; }
};
//---------------------------------------------------------------------------
// following macros are to be written in the constructor of child class
// to define native methods/properties.


#define TJS_GET_NATIVE_INSTANCE(varname, typename) \
		if(!objthis) return TJS_E_NATIVECLASSCRASH; \
		typename *varname; \
		{ \
			tjs_error hr; \
			hr = objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE, \
					TJS_NCM_CLASSID, (iTJSNativeInstance**)&varname); \
			if(TJS_FAILED(hr)) return TJS_E_NATIVECLASSCRASH; \
		}

#define TJS_GET_NATIVE_INSTANCE_OUTER(classname, varname, typename) \
		if(!objthis) return TJS_E_NATIVECLASSCRASH; \
		typename *varname; \
		{ \
			tjs_error hr; \
			hr = objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE, \
					classname::TJS_NCM_CLASSID, (iTJSNativeInstance**)&varname); \
			if(TJS_FAILED(hr)) return TJS_E_NATIVECLASSCRASH; \
		}

#define TJS_BEGIN_NATIVE_MEMBERS(classname) \
	{ \
		static const tjs_char *__classname = TJS_W(#classname); \
		static tjs_int32 TJS_NCM_CLASSID = \
			TJSRegisterNativeClass(__classname); \
		_ClassID = ClassID = TJS_NCM_CLASSID;

#define TJS_BEGIN_NATIVE_METHOD_DECL(name) \
		class NCM_##name : public tTJSNativeClassMethod { \
			virtual tjs_error \
			Process( tTJSVariant *result, \
				tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis) { \

#define TJS_END_NATIVE_METHOD_DECL_INT \
			} \
		};

#define TJS_END_NATIVE_METHOD_DECL(name) \
		TJS_END_NATIVE_METHOD_DECL_INT \
		RegisterNCM(TJS_W(#name), new NCM_##name(), __classname, nitMethod);

#define TJS_END_NATIVE_HIDDEN_METHOD_DECL(name) \
		TJS_END_NATIVE_METHOD_DECL_INT \
		RegisterNCM(TJS_W(#name), new NCM_##name(), __classname, nitMethod, TJS_HIDDENMEMBER);

#define TJS_END_NATIVE_STATIC_METHOD_DECL(name) \
		TJS_END_NATIVE_METHOD_DECL_INT \
		RegisterNCM(TJS_W(#name), new NCM_##name(), __classname, nitMethod, TJS_STATICMEMBER);

#define TJS_END_NATIVE_METHOD_DECL_OUTER(object, name) \
		TJS_END_NATIVE_METHOD_DECL_INT \
		(object)->RegisterNCM(TJS_W(#name), new NCM_##name(), (object)->GetClassName().c_str(), nitMethod);

#define TJS_DECL_EMPTY_FINALIZE_METHOD \
	TJS_BEGIN_NATIVE_METHOD_DECL(finalize) \
	{ return TJS_S_OK; } \
	TJS_END_NATIVE_METHOD_DECL(finalize)

#define TJS_NATIVE_CONSTRUCTOR_CALL_NATIVE_CONSTRUCTOR(varname, typename) \
				typename *varname; \
				{ \
					tjs_error hr; \
					hr = objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE, \
						TJS_NCM_CLASSID, \
						(iTJSNativeInstance**)&varname); \
					if(TJS_FAILED(hr)) return TJS_E_NATIVECLASSCRASH; \
					if(!varname) return TJS_E_NATIVECLASSCRASH; \
					hr = varname->Construct(numparams, param, objthis); \
					if(TJS_FAILED(hr)) return hr; \
				}

#define TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(classname) \
		class NCM_##classname : public tTJSNativeClassConstructor { \
			virtual tjs_error \
			Process(tTJSVariant *result, \
			tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis) { \

#define TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(varname, typename, classname) \
	TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(classname) \
	TJS_NATIVE_CONSTRUCTOR_CALL_NATIVE_CONSTRUCTOR(varname, typename)

#define TJS_END_NATIVE_CONSTRUCTOR_DECL(classname) \
	TJS_END_NATIVE_METHOD_DECL(classname)

#define TJS_END_NATIVE_STATIC_CONSTRUCTOR_DECL(classname) \
	TJS_END_NATIVE_STATIC_METHOD_DECL(classname)

#define TJS_BEGIN_NATIVE_PROP_DECL(name) \
		class NCM_##name : public tTJSNativeClassProperty

#define TJS_END_NATIVE_PROP_DECL(name) \
		;RegisterNCM(TJS_W(#name), new NCM_##name(), __classname, nitProperty);

#define TJS_END_NATIVE_PROP_DECL_OUTER(object, name) \
		;(object)->RegisterNCM(TJS_W(#name), new NCM_##name(), (object)->GetClassName().c_str(), nitProperty);

#define TJS_END_NATIVE_STATIC_PROP_DECL(name) \
		;RegisterNCM(TJS_W(#name), new NCM_##name(), __classname, nitProperty, TJS_STATICMEMBER);

#define TJS_BEGIN_NATIVE_PROP_GETTER \
		tjs_error Get(tTJSVariant *result, iTJSDispatch2 *objthis) { \

#define TJS_END_NATIVE_PROP_GETTER \
		}

#define TJS_DENY_NATIVE_PROP_GETTER \
		tjs_error TJS_INTF_METHOD \
		PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, \
		tTJSVariant *result, \
		iTJSDispatch2 *objthis) { \
			return DenyGet(flag, membername, hint, result, objthis); \
		} \
		tjs_error Get(tTJSVariant *result, iTJSDispatch2 *objthis) \
		{ return TJS_S_OK; }

#define TJS_BEGIN_NATIVE_PROP_SETTER \
		tjs_error Set(const tTJSVariant *param, iTJSDispatch2 *objthis) { \

#define TJS_END_NATIVE_PROP_SETTER \
		}

#define TJS_DENY_NATIVE_PROP_SETTER \
		tjs_error TJS_INTF_METHOD  \
		PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint, \
		const tTJSVariant *param, \
		iTJSDispatch2 *objthis) { \
			return DenySet(flag, membername, hint, param, objthis); \
		} \
		tjs_error Set(const tTJSVariant *param, iTJSDispatch2 *objthis) \
		{ return TJS_S_OK; }

#define TJS_END_NATIVE_MEMBERS \
	}

#define TJS_PARAM_EXIST(num) (numparams>(num) ? param[num]->Type()!=tvtVoid : false)

//---------------------------------------------------------------------------
// tTJSNativeFunction
//---------------------------------------------------------------------------
// base class used for native function ( for non-class-method )
class tTJSNativeFunction : public tTJSDispatch
{
	typedef tTJSDispatch inherited;
public:
	tTJSNativeFunction(const tjs_char *name = NULL);
		// 'name' is just to be used as a label for debugging
	~tTJSNativeFunction();

	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		const tjs_char *classname,
		iTJSDispatch2 *objthis);

protected:
	tjs_error virtual Process(tTJSVariant *result, tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *objthis) = 0;
		// override this instead of FuncCall
};
//---------------------------------------------------------------------------
} // namespace TJS

#endif
