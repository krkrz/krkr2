//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Text Editor
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "PadIntf.h"
#include "PadImpl.h"

//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNI_BasePad::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *dsp)
{
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD
tTJSNI_BasePad::Invalidate()
{
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_Pad::ClassID = -1;
//---------------------------------------------------------------------------
tTJSNC_Pad::tTJSNC_Pad(): inherited(TJS_W("Pad"))
{
	// registration of native members

	TJS_BEGIN_NATIVE_MEMBERS(Pad) // constructor
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_Pad,
	/*TJS class name*/Pad)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Pad)
//----------------------------------------------------------------------

//-- methods

//----------------------------------------------------------------------
//----------------------------------------------------------------------

//-- events

//----------------------------------------------------------------------
//----------------------------------------------------------------------

//--properties

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(text)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		*result = _this->GetText();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		_this->SetText(*param);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(text)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(fileName)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		*result = _this->GetFileName();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		_this->SetFileName(*param);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(fileName)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(color)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		*result = (tjs_int)_this->GetColor();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		_this->SetColor((tjs_uint32)(tjs_int)*param);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(color)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(visible)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		*result = (tjs_int)_this->GetVisible();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		_this->SetVisible(param->operator bool());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(visible)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(title)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		*result = _this->GetTitle();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Pad);
		_this->SetTitle(*param);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(title)
//----------------------------------------------------------------------

	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


