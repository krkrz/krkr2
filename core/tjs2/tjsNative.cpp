//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Support for C++ native class/method/property definitions
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop


#include "tjsNative.h"
#include "tjsError.h"
#include "tjsGlobalStringMap.h"

namespace TJS
{
//---------------------------------------------------------------------------
// NativeClass registration
//---------------------------------------------------------------------------
static std::vector<ttstr > NativeClassNames;
//---------------------------------------------------------------------------
tjs_int32 TJSRegisterNativeClass(const tjs_char *name)
{
	for(tjs_uint i = 0; i<NativeClassNames.size(); i++)
	{
		if(NativeClassNames[i] == name) return i;
	}

	NativeClassNames.push_back(TJSMapGlobalStringMap(name));

	return NativeClassNames.size() -1;
}
//---------------------------------------------------------------------------
tjs_int32 TJSFindNativeClassID(const tjs_char *name)
{
	for(tjs_uint i = 0; i<NativeClassNames.size(); i++)
	{
		if(NativeClassNames[i] == name) return i;
	}

	return -1;
}
//---------------------------------------------------------------------------
const tjs_char * TJSFindNativeClassName(tjs_int32 id)
{
	if(id<0 || id>=(tjs_int32)NativeClassNames.size()) return NULL;
	return NativeClassNames[id].c_str();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSNativeClassMethod
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClassMethod::IsInstanceOf(tjs_uint32 flag,
	const tjs_char *membername,  tjs_uint32 *hint,
		const tjs_char *classname, iTJSDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!TJS_strcmp(classname, TJS_W("Function"))) return TJS_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNativeClassMethod::FuncCall(tjs_uint32 flag, const tjs_char * membername,
		tjs_uint32 *hint, tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis)
{
	if(membername) return tTJSDispatch::FuncCall(flag, membername, hint,
		result, numparams, param, objthis);
	if(!objthis) return TJS_E_NATIVECLASSCRASH;
	if(objthis->IsValid(0, NULL, NULL, objthis) != TJS_S_TRUE)
		return TJS_E_INVALIDOBJECT;
	if(result) result->Clear();
	tjs_error er;
	try
	{
		er = Process(result, numparams, param, objthis);
	}
	catch(...)
	{
		throw;
	}
	return er;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNativeClassConstructor
//---------------------------------------------------------------------------
tjs_error  TJS_INTF_METHOD
	tTJSNativeClassConstructor::FuncCall(tjs_uint32 flag,
	const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis)
{
	if(membername) return tTJSDispatch::FuncCall(flag, membername, hint,
		result, numparams, param, objthis);
	if(result) result->Clear();
	tjs_error er;
	try
	{
		er = Process(result, numparams, param, objthis);
	}
	catch(...)
	{
		throw;
	}
	return er;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNativeClassProperty
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClassProperty::IsInstanceOf(tjs_uint32 flag,
	const tjs_char *membername, tjs_uint32 *hint,
		const tjs_char *classname, iTJSDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!TJS_strcmp(classname, TJS_W("Property"))) return TJS_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClassProperty::PropGet(tjs_uint32 flag, const tjs_char * membername,
	tjs_uint32 *hint, tTJSVariant *result, iTJSDispatch2 *objthis)
{
	if(membername) return tTJSDispatch::PropGet(flag, membername, hint,
		result, objthis);
	if(!objthis) return TJS_E_NATIVECLASSCRASH;
	if(objthis->IsValid(0, NULL, NULL, objthis) != TJS_S_TRUE)
		return TJS_E_INVALIDOBJECT;
	if(!result) return TJS_E_FAIL;
// 	AddRef();
//	objthis->AddRef();
	tjs_error er;
	try
	{
		er = Get(result, objthis);
	}
	catch(...)
	{
//		Release();
//		objthis->Release();
		throw;
	}
//	Release();
//	objthis->Release();
	return er;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClassProperty::PropSet(tjs_uint32 flag, const tjs_char *membername,
	tjs_uint32 *hint, const tTJSVariant *param, iTJSDispatch2 *objthis)
{
	if(membername) return tTJSDispatch::PropSet(flag, membername, hint,
		param, objthis);
	if(!objthis) return TJS_E_NATIVECLASSCRASH;
	if(objthis->IsValid(0, NULL, NULL, objthis) != TJS_S_TRUE)
		return TJS_E_INVALIDOBJECT;
	if(!param) return TJS_E_FAIL;
//	AddRef();
//	objthis->AddRef();
	tjs_error er;
	try
	{
		er = Set(param, objthis);
	}
	catch(...)
	{
//		Release();
//		objthis->Release();
		throw;
	}
//	Release();
//	objthis->Release();
	return er;
}
//---------------------------------------------------------------------------
tjs_error tTJSNativeClassProperty::DenyGet(tjs_uint32 flag,
	const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,
	iTJSDispatch2 *objthis)
{
	if(membername) return tTJSDispatch::PropGet(flag, membername, hint,
		result, objthis);
	if(objthis->IsValid(0, NULL, NULL, objthis) != TJS_S_TRUE)
		return TJS_E_INVALIDOBJECT;
	return TJS_E_ACCESSDENYED;
}
//---------------------------------------------------------------------------
tjs_error tTJSNativeClassProperty::DenySet(tjs_uint32 flag,
	const tjs_char *membername, tjs_uint32 *hint, const tTJSVariant *param,
	iTJSDispatch2 *objthis)
{
	if(membername) return tTJSDispatch::PropSet(flag, membername, hint,
		param, objthis);
	if(objthis->IsValid(0, NULL, NULL, objthis) != TJS_S_TRUE)
		return TJS_E_INVALIDOBJECT;
	return TJS_E_ACCESSDENYED; 
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSNativeClass
//---------------------------------------------------------------------------
tTJSNativeClass::tTJSNativeClass(const ttstr &name)
{
	CallFinalize = false;
	ClassName = TJSMapGlobalStringMap(name);
}
//---------------------------------------------------------------------------
tTJSNativeClass::~tTJSNativeClass()
{
}
//---------------------------------------------------------------------------
void tTJSNativeClass::RegisterNCM(const tjs_char *name, iTJSDispatch2 *dsp,
	tjs_uint32 flags)
{
	// add to Items and ItemsNames
	ttstr tname = TJSMapGlobalStringMap(ttstr(name));
	ItemNames.push_back(tname);
	Items.push_back(dsp); // do not AddRef "dsp"
	ItemFlags.push_back(flags);


	// add to this
	tTJSVariant val;
	val = dsp;
	if(PropSetByVS((TJS_MEMBERENSURE | TJS_IGNOREPROP) | flags, tname.AsVariantStringNoAddRef(), &val, this)
		== TJS_E_NOTIMPL)
		PropSet((TJS_MEMBERENSURE | TJS_IGNOREPROP) | flags, tname.c_str(), NULL, &val, this);
}
//---------------------------------------------------------------------------
void tTJSNativeClass::Finalize(void)
{
	ClearItems();
	tTJSCustomObject::Finalize();
}
//---------------------------------------------------------------------------
iTJSDispatch2 * tTJSNativeClass::CreateBaseTJSObject()
{
	return new tTJSCustomObject;
}
//---------------------------------------------------------------------------
void tTJSNativeClass::ClearItems()
{
	tjs_uint i;
	for(i=0; i<Items.size(); i++) Items[i]->Release();
	Items.clear();
	ItemNames.clear();
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClass::FuncCall(tjs_uint32 flag, const tjs_char * membername,
	tjs_uint32 *hint,
	tTJSVariant *result, tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 *objthis)
{
	if(!GetValidity()) return TJS_E_INVALIDOBJECT;

	if(membername) return tTJSCustomObject::FuncCall(flag, membername, hint,
		result, numparams, param, objthis);

	tTJSVariant name(ClassName);
	objthis->ClassInstanceInfo(TJS_CII_ADD, 0, &name); // add class name

	// create base native object
	iTJSNativeInstance * nativeptr = CreateNativeInstance();

	// register native instance information to the object;
	// even if "nativeptr" is null
	objthis->NativeInstanceSupport(TJS_NIS_REGISTER, _ClassID, &nativeptr);

	// register members to "objthis" when this object is called by "FuncCall"
	tjs_uint i;
	for(i=0; i<Items.size(); i++)
	{
		if(!(ItemFlags[i] & TJS_STATICMEMBER))
		{
			tTJSVariant val(Items[i], objthis);
			if(objthis->PropSetByVS(TJS_MEMBERENSURE|TJS_IGNOREPROP|ItemFlags[i],
				ItemNames[i].AsVariantStringNoAddRef(), &val, objthis) == TJS_E_NOTIMPL)
				objthis->PropSet(TJS_MEMBERENSURE|TJS_IGNOREPROP|ItemFlags[i],
				ItemNames[i].c_str(), NULL, &val, objthis);
		}
	}

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClass::CreateNew(tjs_uint32 flag, const tjs_char * membername,
	tjs_uint32 *hint,
	iTJSDispatch2 **result, tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 *objthis)
{
	// CreateNew


	iTJSDispatch2 *dsp = CreateBaseTJSObject();

	tjs_error hr;
	try
	{

		hr = FuncCall(0, NULL, NULL, NULL, 0, NULL, dsp); // add member to dsp

		if(TJS_FAILED(hr)) return hr;

		hr = FuncCall(0, ClassName.c_str(), ClassName.GetHint(), NULL, numparams, param, dsp);
			// call the constructor
		if(hr == TJS_E_MEMBERNOTFOUND) hr = TJS_S_OK;
			// missing constructor is OK ( is this ugly ? )
	}
	catch(...)
	{
		dsp->Release();
		throw;
	}

	if(TJS_SUCCEEDED(hr)) *result = dsp;
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNativeClass::IsInstanceOf(tjs_uint32 flag,
	const tjs_char *membername, tjs_uint32 *hint, const tjs_char *classname,
		iTJSDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!TJS_strcmp(classname, TJS_W("Class"))) return TJS_S_TRUE;
		if(!TJS_strcmp(classname, ClassName.c_str())) return TJS_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTJSNativeFunction
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTJSNativeFunction::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if(membername)
	{
		return inherited::FuncCall(flag, membername, hint, result,
			numparams, param, objthis);
	}
	return Process(result, numparams, param, objthis);
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTJSNativeFunction::IsInstanceOf(
	tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	const tjs_char *classname, iTJSDispatch2 *objthis)
{
	if(membername == NULL)
	{
		if(!TJS_strcmp(classname, TJS_W("Function"))) return TJS_S_TRUE;
	}

	return inherited::IsInstanceOf(flag, membername, hint, classname, objthis);
}
//---------------------------------------------------------------------------
} // namespace TJS

