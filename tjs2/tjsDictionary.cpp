//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Dictionary class implementation
//---------------------------------------------------------------------------

#include "tjsCommHead.h"

#include "tjsDictionary.h"
#include "tjsArray.h"

namespace TJS
{
//---------------------------------------------------------------------------
static tjs_int32 ClassID_Dictionary;
//---------------------------------------------------------------------------
// tTJSDictionaryClass : tTJSDictionary class
//---------------------------------------------------------------------------
tjs_uint32 tTJSDictionaryClass::ClassID = (tjs_uint32)-1;
tTJSDictionaryClass::tTJSDictionaryClass() :
	tTJSNativeClass(TJS_W("Dictionary"))
{
	// TJS class constructor

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Dictionary)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/* var. name */_this,
	/* var. type */tTJSDictionaryNI, /* TJS class name */ Dictionary)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_CONSTRUCTOR_DECL(/*TJS class name*/Dictionary)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/load)
{
	TJS_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tTJSDictionaryNI);
	if(!ni->IsValid()) return TJS_E_INVALIDOBJECT;

	// TODO: implement Dictionary.load()
	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func. name*/load)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func.name*/save)
{
	TJS_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tTJSDictionaryNI);
	if(!ni->IsValid()) return TJS_E_INVALIDOBJECT;

	// TODO: implement Dictionary.save();
	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/save)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func.name*/saveStruct)
{
	// Structured output for flie;
	// the content can be interpret as an expression to re-construct the object.

	TJS_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tTJSDictionaryNI);
	if(!ni->IsValid()) return TJS_E_INVALIDOBJECT;

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	ttstr name(*param[0]);
	ttstr mode;
	if(numparams >= 2 && param[1]->Type() != tvtVoid) mode = *param[1];

	iTJSTextWriteStream * stream = TJSCreateTextStreamForWrite(name, mode);
	try
	{
		std::vector<iTJSDispatch2 *> stack;
		stack.push_back(objthis);
		tTJSStringAppender string;
		ni->SaveStructuredData(stack, string, TJS_W(""));
		stream->Write(ttstr(string.GetData(), string.GetLen()));
	}
	catch(...)
	{
		stream->Destruct();
		throw;
	}
	stream->Destruct();

	if(result) *result = tTJSVariant(objthis, objthis);

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/saveStruct)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func.name*/assign)
{
	TJS_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tTJSDictionaryNI);
	if(!ni->IsValid()) return TJS_E_INVALIDOBJECT;

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	bool clear = true;
	if(numparams >= 2 && param[1]->Type() != tvtVoid)
		clear = (tjs_int)*param[1];

	tTJSVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.ObjThis)
		ni->Assign(clo.ObjThis, clear);
	else if(clo.Object)
		ni->Assign(clo.Object, clear);
	else TJS_eTJSError(TJSNullAccess);

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/assign)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/* func.name */assignStruct)
{
	TJS_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tTJSDictionaryNI);
	if(!ni->IsValid()) return TJS_E_INVALIDOBJECT;

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	std::vector<iTJSDispatch2 *> stack;

	tTJSVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.ObjThis)
		ni->AssignStructure(clo.ObjThis, stack);
	else if(clo.Object)
		ni->AssignStructure(clo.Object, stack);
	else TJS_eTJSError(TJSNullAccess);

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/* func.name */assignStruct)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func.name*/clear)
{
	TJS_GET_NATIVE_INSTANCE(/* var. name */ni, /* var. type */tTJSDictionaryNI);
	if(!ni->IsValid()) return TJS_E_INVALIDOBJECT;

	ni->Clear();

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL(/*func.name*/clear)
//----------------------------------------------------------------------

	ClassID_Dictionary = TJS_NCM_CLASSID;
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
tTJSDictionaryClass::~tTJSDictionaryClass()
{
}
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSDictionaryClass::CreateNativeInstance()
{
	return new tTJSDictionaryNI();
}
//---------------------------------------------------------------------------
iTJSDispatch2 *tTJSDictionaryClass::CreateBaseTJSObject()
{
	return new tTJSDictionaryObject();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSDictionaryNI
//---------------------------------------------------------------------------
tTJSDictionaryNI::tTJSDictionaryNI()
{
	Owner = NULL;
}
//---------------------------------------------------------------------------
tTJSDictionaryNI::~tTJSDictionaryNI()
{
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTJSDictionaryNI::Construct(tjs_int numparams,
	tTJSVariant **param, iTJSDispatch2 *tjsobj)
{
	// called from TJS constructor
	if(numparams != 0) return TJS_E_BADPARAMCOUNT;
	Owner = static_cast<tTJSCustomObject*>(tjsobj);
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSDictionaryNI::Invalidate() // Invalidate override
{
	// put here something on invalidation
	Owner = NULL;
	inherited::Invalidate();
}
//---------------------------------------------------------------------------
void tTJSDictionaryNI::Assign(iTJSDispatch2 * dsp, bool clear)
{
	// copy members from "dsp" to "Owner"

	// determin dsp's object type
	tTJSArrayNI *arrayni = NULL;
	if(dsp && TJS_SUCCEEDED(dsp->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
		TJSGetArrayClassID(), (iTJSNativeInstance**)&arrayni)) )
	{
		// convert from array
		if(clear) Owner->Clear();

		tTJSArrayNI::tArrayItemIterator i;
		for(i = arrayni->Items.begin(); i != arrayni->Items.end(); i++)
		{
			tTJSVariantString *name = i->AsStringNoAddRef();
			i++;
			if(arrayni->Items.end() == i) break;
			Owner->PropSetByVS(TJS_MEMBERENSURE|TJS_IGNOREPROP, name,
				&(*i),
				Owner);
		}
	}
	else
	{
		// otherwise
		if(clear) Owner->Clear();
		tAssignCallback callback;
		callback.Owner = Owner;

		dsp->EnumMembers(TJS_IGNOREPROP, &tTJSVariantClosure(&callback, NULL), dsp);

	}
}
//---------------------------------------------------------------------------
void tTJSDictionaryNI::Clear()
{
	Owner->Clear();
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSDictionaryNI::tAssignCallback::FuncCall(tjs_uint32 flag,
	const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	// called from iTJSDispatch2::EnumMembers
	// (tTJSDictionaryNI::Assign calls iTJSDispatch2::EnumMembers)
	if(numparams < 3) return TJS_E_BADPARAMCOUNT;

	// hidden members are not copied
	tjs_uint32 flags = (tjs_int)*param[1];
	if(flags & TJS_HIDDENMEMBER)
	{
		if(result) *result = (tjs_int)1;
		return TJS_S_OK;
	}

	Owner->PropSetByVS(TJS_MEMBERENSURE|TJS_IGNOREPROP|flags,
		param[0]->AsStringNoAddRef(), param[2], Owner);

	if(result) *result = (tjs_int)1;
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void tTJSDictionaryNI::SaveStructuredData(std::vector<iTJSDispatch2 *> &stack,
	tTJSStringAppender & string, const ttstr &indentstr)
{
#ifdef TJS_TEXT_OUT_CRLF
	string += TJS_W("%[\r\n");
#else
	string += TJS_W("%[\n");
#endif
	ttstr indentstr2 = indentstr + TJS_W(" ");

	tSaveStructCallback callback;
	callback.Stack = &stack;
	callback.String = &string;
	callback.IndentStr = &indentstr2;
	callback.First = true;

	Owner->EnumMembers(TJS_IGNOREPROP, &tTJSVariantClosure(&callback, NULL), Owner);

#ifdef TJS_TEXT_OUT_CRLF
	if(!callback.First) string += TJS_W("\r\n");
#else
	if(!callback.First) string += TJS_W("\n");
#endif
	string += indentstr;
	string += TJS_W("]");
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTJSDictionaryNI::tSaveStructCallback::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result, tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 *objthis)
{
	// called indirectly from tTJSDictionaryNI::SaveStructuredData

	if(numparams < 3) return TJS_E_BADPARAMCOUNT;

	// hidden members are not processed
	tjs_uint32 flags = (tjs_int)*param[1];
	if(flags & TJS_HIDDENMEMBER)
	{
		if(result) *result = (tjs_int)1;
		return TJS_S_OK;
	}

#ifdef TJS_TEXT_OUT_CRLF
	if(!First) *String += TJS_W(",\r\n");
#else
	if(!First) *String += TJS_W(",\n");
#endif

	First = false;

	*String += *IndentStr;

	*String += TJS_W("\"");
	*String += ttstr(*param[0]).EscapeC();
	*String += TJS_W("\" => ");

	tTJSVariantType type = param[2]->Type();
	if(type == tvtObject)
	{
		// object
		tTJSVariantClosure clo = param[2]->AsObjectClosureNoAddRef();
		tTJSArrayNI::SaveStructuredDataForObject(clo.SelectObjectNoAddRef(),
			*Stack, *String, *IndentStr);
	}
	else
	{
		*String += TJSVariantToExpressionString(*param[2]);
	}

	if(result) *result = (tjs_int)1;
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void tTJSDictionaryNI::AssignStructure(iTJSDispatch2 * dsp,
	std::vector<iTJSDispatch2 *> &stack)
{
	// assign structured data from dsp
	tTJSArrayNI *dicni = NULL;
	if(TJS_SUCCEEDED(dsp->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
		ClassID_Dictionary, (iTJSNativeInstance**)&dicni)) )
	{
		// copy from dictionary
		stack.push_back(dsp);
		try
		{
			Owner->Clear();

			tAssignStructCallback callback;
			callback.Dest = Owner;
			callback.Stack = &stack;

			dsp->EnumMembers(TJS_IGNOREPROP, &tTJSVariantClosure(&callback, NULL), dsp);
		}
		catch(...)
		{
			stack.pop_back();
			throw;
		}
		stack.pop_back();
	}
	else
	{
		TJS_eTJSError(TJSSpecifyDicOrArray);
	}

}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTJSDictionaryNI::tAssignStructCallback::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result, tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 *objthis)
{
	// called indirectly from tTJSDictionaryNI::AssignStructure or
	// tTJSArrayNI::AssignStructure

	if(numparams < 3) return TJS_E_BADPARAMCOUNT;

	// hidden members are not processed
	tjs_uint32 flags = (tjs_int)*param[1];
	if(flags & TJS_HIDDENMEMBER)
	{
		if(result) *result = (tjs_int)1;
		return TJS_S_OK;
	}

	tTJSVariant &value = *param[2];

	tTJSVariantType type = value.Type();
	if(type == tvtObject)
	{
		// object

		iTJSDispatch2 *dsp = value.AsObjectNoAddRef();
		// determin dsp's object type

		tTJSVariant val;

		tTJSDictionaryNI *dicni = NULL;
		tTJSArrayNI *arrayni = NULL;

		if(dsp && TJS_SUCCEEDED(dsp->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
			TJSGetDictionaryClassID(), (iTJSNativeInstance**)&dicni)) )
		{
			// dictionary
			bool objrec = false;
			std::vector<iTJSDispatch2 *>::iterator i;
			for(i = Stack->begin(); i != Stack->end(); i++)
			{
				if(*i == dsp)
				{
					// object recursion detected
					objrec = true;
					break;
				}
			}
			if(objrec)
			{
				val.SetObject(NULL); // becomes null
			}
			else
			{
				iTJSDispatch2 * newobj = TJSCreateDictionaryObject();
				val.SetObject(newobj, newobj);
				newobj->Release();
				tTJSDictionaryNI * newni = NULL;
				if(TJS_SUCCEEDED(newobj->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
					TJSGetDictionaryClassID(), (iTJSNativeInstance**)&newni)) )
				{
					newni->AssignStructure(dsp, *Stack);
				}
			}
		}
		else if(dsp && TJS_SUCCEEDED(dsp->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
			TJSGetArrayClassID(), (iTJSNativeInstance**)&arrayni)) )
		{
			// array
			bool objrec = false;
			std::vector<iTJSDispatch2 *>::iterator i;
			for(i = Stack->begin(); i != Stack->end(); i++)
			{
				if(*i == dsp)
				{
					// object recursion detected
					objrec = true;
					break;
				}
			}
			if(objrec)
			{
				val.SetObject(NULL); // becomes null
			}
			else
			{
				iTJSDispatch2 * newobj = TJSCreateArrayObject();
				val.SetObject(newobj, newobj);
				newobj->Release();
				tTJSArrayNI * newni = NULL;
				if(TJS_SUCCEEDED(newobj->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
					TJSGetArrayClassID(), (iTJSNativeInstance**)&newni)) )
				{
					newni->AssignStructure(dsp, *Stack);
				}
			}
		}
		else
		{
			// other object types
			val = value;
		}

		Dest->PropSetByVS(TJS_MEMBERENSURE|TJS_IGNOREPROP, param[0]->AsStringNoAddRef(), &val, Dest);
	}
	else
	{
		// other types
		Dest->PropSetByVS(TJS_MEMBERENSURE|TJS_IGNOREPROP, param[0]->AsStringNoAddRef(), &value, Dest);
	}

	if(result) *result = (tjs_int)1;
	return TJS_S_OK;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSDictionaryObject
//---------------------------------------------------------------------------
tTJSDictionaryObject::tTJSDictionaryObject() : tTJSCustomObject()
{
	CallFinalize = false;
}
//---------------------------------------------------------------------------
tTJSDictionaryObject::~tTJSDictionaryObject()
{
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSDictionaryObject::FuncCall(tjs_uint32 flag, const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::FuncCall(flag, membername, hint, result, numparams,
		param, objthis);
//	if(hr == TJS_E_MEMBERNOTFOUND)
//		return TJS_E_INVALIDTYPE; // call operation for void
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSDictionaryObject::PropGet(tjs_uint32 flag, const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result, iTJSDispatch2 *objthis)
{
	tjs_error hr;
	hr = inherited::PropGet(flag, membername, hint, result, objthis);
	if(hr == TJS_E_MEMBERNOTFOUND && !(flag & TJS_MEMBERMUSTEXIST))
	{
		if(result) result->Clear(); // returns void
		return TJS_S_OK;
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSDictionaryObject::CreateNew(tjs_uint32 flag, const tjs_char * membername,
		tjs_uint32 *hint,
		iTJSDispatch2 **result, tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::CreateNew(flag, membername, hint, result, numparams,
		param, objthis);
	if(hr == TJS_E_MEMBERNOTFOUND && !(flag & TJS_MEMBERMUSTEXIST))
		return TJS_E_INVALIDTYPE; // call operation for void
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD 
	tTJSDictionaryObject::Operation(tjs_uint32 flag, const tjs_char *membername,
		tjs_uint32 *hint,
		tTJSVariant *result, const tTJSVariant *param, iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::Operation(flag, membername, hint, result, param, objthis);
	if(hr == TJS_E_MEMBERNOTFOUND && !(flag & TJS_MEMBERMUSTEXIST))
	{
		// value not found -> create a value, do the operation once more
		static tTJSVariant VoidVal;
		hr = inherited::PropSet(TJS_MEMBERENSURE, membername, hint, &VoidVal, objthis);
		if(TJS_FAILED(hr)) return hr;
		hr = inherited::Operation(flag, membername, hint, result, param, objthis);
	}
	return hr;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSGetDictionaryClassID
//---------------------------------------------------------------------------
tjs_int32 TJSGetDictionaryClassID()
{
	return ClassID_Dictionary;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSCreateDictionaryObject
//---------------------------------------------------------------------------
iTJSDispatch2 * TJSCreateDictionaryObject(iTJSDispatch2 **classout)
{
	// create a Dictionary object
	struct tHolder
	{
		iTJSDispatch2 * Obj;
		tHolder() { Obj = new tTJSDictionaryClass(); }
		~tHolder() { Obj->Release(); }
	} static dictionaryclass;

	if(classout) *classout = dictionaryclass.Obj, dictionaryclass.Obj->AddRef();

	tTJSDictionaryObject *dictionaryobj;
	(dictionaryclass.Obj)->CreateNew(0, NULL,  NULL,
		(iTJSDispatch2**)&dictionaryobj, 0, NULL, dictionaryclass.Obj);
	return dictionaryobj;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace TJS









