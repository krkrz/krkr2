//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "System" class interface
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "tjsMessage.h"
#include "SystemIntf.h"
#include "SysInitIntf.h"
#include "MsgIntf.h"
#include "GraphicsLoaderIntf.h"
#include "EventIntf.h"
#include "LayerIntf.h"
#include "Random.h"


//---------------------------------------------------------------------------
// tTJSNC_System
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_System::ClassID = -1;
tTJSNC_System::tTJSNC_System() : inherited(TJS_W("System"))
{
	// registration of native members

	TJS_BEGIN_NATIVE_MEMBERS(System)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*TJS class name*/System)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/System)
//----------------------------------------------------------------------

//-- methods

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/terminate)
{
	TVPTerminateAsync();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/terminate)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/exit)
{
	// this method does not return

	TVPTerminateSync();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/exit)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/inputString)
{
	if(numparams < 3) return TJS_E_BADPARAMCOUNT;

	ttstr value = *param[2];
	bool b = TVPInputQuery(*param[0], *param[1], value);

	if(result)
	{
		if(b)
			*result = value;
		else
			result->Clear();
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/inputString)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/addContinuousHandler)
{
	// add function to continus handler list

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	tTJSVariantClosure clo = param[0]->AsObjectClosureNoAddRef();

	TVPAddContinuousHandler(clo);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/addContinuousHandler)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/removeContinuousHandler)
{
	// remove function from continuous handler list

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	tTJSVariantClosure clo = param[0]->AsObjectClosureNoAddRef();

	TVPRemoveContinuousHandler(clo);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/removeContinuousHandler)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/toActualColor)
{
	// convert color codes to 0xRRGGBB format.

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		tjs_uint32 color = (tjs_int)(*param[0]);
		color = TVPToActualColor(color);
		*result = (tjs_int)color;
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/toActualColor)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/clearGraphicCache)
{
	// clear graphic cache
	TVPClearGraphicCache();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/clearGraphicCache)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/touchImages)
{
	// try to cache graphics

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	std::vector<ttstr> storages;
	tTJSVariantClosure array = param[0]->AsObjectClosureNoAddRef();

	tjs_int count = 0;
	while(true)
	{
		tTJSVariant val;
		if(TJS_FAILED(array.Object->PropGetByNum(0, count, &val, array.ObjThis)))
			break;
		if(val.Type() == tvtVoid) break;
		storages.push_back(ttstr(val));
		count++;
	}

	tjs_int limit = 0;
	tjs_uint64 timeout = 0;

	if(numparams >= 2 && param[1]->Type() != tvtVoid) limit = *param[1];
	if(numparams >= 3 && param[2]->Type() != tvtVoid) timeout = (tjs_int64)*param[2];

	TVPTouchImages(storages, limit, timeout);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/touchImages)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/createUUID)
{
	// create UUID
	// return UUID string in form of "43abda37-c597-4646-a279-c27a1373af90"

	tjs_uint8 uuid[16];

	TVPGetRandomBits128(uuid);

	uuid[8] &= 0x3f;
	uuid[8] |= 0x80; // override clock_seq

	uuid[6] &= 0x0f;
	uuid[6] |= 0x40; // override version

	tjs_char buf[40];
	TJS_sprintf(buf,
TJS_W("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
		uuid[ 0], uuid[ 1], uuid[ 2], uuid[ 3],
		uuid[ 4], uuid[ 5], uuid[ 6], uuid[ 7],
		uuid[ 8], uuid[ 9], uuid[10], uuid[11],
		uuid[12], uuid[13], uuid[14], uuid[15]);

	if(result) *result = tTJSVariant(buf);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/createUUID)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/assignMessage)
{
	// assign system message

	if(numparams < 2) return TJS_E_BADPARAMCOUNT;

	ttstr id(*param[0]);
	ttstr msg(*param[1]);

	bool res = TJSAssignMessage(id.c_str(), msg.c_str());

	if(result) *result = tTJSVariant((tjs_int)res);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/assignMessage)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/doCompact)
{
	// compact memory usage

	tjs_int level = TVP_COMPACT_LEVEL_MAX;

	if(numparams >= 1 && param[0]->Type() != tvtVoid)
		level = (tjs_int)*param[0];

	TVPDeliverCompactEvent(level);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/doCompact)
//----------------------------------------------------------------------

//--properties

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(versionString)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPGetVersionString();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(versionString)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(versionInformation)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPGetVersionInformation();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(versionInformation)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(eventDisabled)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPGetSystemEventDisabledState();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TVPSetSystemEventDisabledState(param->operator bool());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(eventDisabled)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(graphicCacheLimit)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = (tjs_int)TVPGetGraphicCacheLimit();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TVPSetGraphicCacheLimit((tjs_int)*param);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(graphicCacheLimit)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(platformName)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPGetPlatformName();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(platformName)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(osName)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = TVPGetOSName();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(osName)
//----------------------------------------------------------------------

//----------------------------------------------------------------------

	TJS_END_NATIVE_MEMBERS


	// register default "exceptionHandler" member
	tTJSVariant val((iTJSDispatch2*)NULL, (iTJSDispatch2*)NULL);
	PropSet(TJS_MEMBERENSURE, TJS_W("exceptionHandler"), NULL, &val, this);
}
//---------------------------------------------------------------------------
tTJSNativeInstance * tTJSNC_System::CreateNativeInstance()
{
	// this class cannot create an instance
	TVPThrowExceptionMessage(TVPCannotCreateInstance);

	return NULL;
}
//---------------------------------------------------------------------------




