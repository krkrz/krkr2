//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Video Overlay support interface
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "MsgIntf.h"
#include "WindowIntf.h"
#include "VideoOvlIntf.h"

//---------------------------------------------------------------------------
// tTJSNI_BaseVideoOverlay
//---------------------------------------------------------------------------
tTJSNI_BaseVideoOverlay::tTJSNI_BaseVideoOverlay()
{
	ActionOwner.Object = ActionOwner.ObjThis = NULL;
	Status = ssUnload;
	Owner = NULL;
	CanDeliverEvents = true;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNI_BaseVideoOverlay::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = inherited::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	tTJSVariantClosure clo = param[0]->AsObjectClosureNoAddRef();
	if(clo.Object == NULL) TVPThrowExceptionMessage(TVPSpecifyWindow);
	tTJSNI_Window *win = NULL;
	if(TJS_FAILED(clo.Object->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
		tTJSNC_Window::ClassID, (iTJSNativeInstance**)&win)))
		TVPThrowExceptionMessage(TVPSpecifyWindow);
	if(!win) TVPThrowExceptionMessage(TVPSpecifyWindow);
	Window = win;

	Window->RegisterVideoOverlayObject(this);

	ActionOwner = param[0]->AsObjectClosure();
	Owner = tjs_obj;

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_BaseVideoOverlay::Invalidate()
{
	Owner = NULL;
	if(Window) Window->UnregisterVideoOverlayObject(this);
	ActionOwner.Release();

	inherited::Invalidate();
}
//---------------------------------------------------------------------------
ttstr tTJSNI_BaseVideoOverlay::GetStatusString() const
{
	static ttstr unload(TJS_W("unload"));
	static ttstr play(TJS_W("play"));
	static ttstr stop(TJS_W("stop"));
	static ttstr unknown(TJS_W("unknown"));

	switch(Status)
	{
	case ssUnload:	return unload;
	case ssPlay:	return play;
	case ssStop:	return stop;
	default:		return unknown;
	}
}
//---------------------------------------------------------------------------
void tTJSNI_BaseVideoOverlay::SetStatus(tTVPSoundStatus s)
{
	// this function may call the onStatusChanged event immediately
	
	if(Status != s)
	{
		Status = s;

		if(Owner)
		{
			// Cancel Previous un-delivered Events
			TVPCancelSourceEvents(Owner);

			// fire
			if(CanDeliverEvents)
			{
				// fire onStatusChanged event
				tTJSVariant param(GetStatusString());
				static ttstr eventname(TJS_W("onStatusChanged"));
				TVPPostEvent(Owner, Owner, eventname, 0, TVP_EPT_IMMEDIATE,
					1, &param);
			}
		}
	}
}
//---------------------------------------------------------------------------
void tTJSNI_BaseVideoOverlay::SetStatusAsync(tTVPSoundStatus s)
{
	// this function posts the onStatusChanged event

	if(Status != s)
	{
		Status = s;

		if(Owner)
		{
			// Cancel Previous un-delivered Events
			TVPCancelSourceEvents(Owner);

			// fire
			if(CanDeliverEvents)
			{
				// fire onStatusChanged event
				tTJSVariant param(GetStatusString());
				static ttstr eventname(TJS_W("onStatusChanged"));
				TVPPostEvent(Owner, Owner, eventname, 0, TVP_EPT_POST,
					1, &param);
			}
		}
	}
}
//---------------------------------------------------------------------------
void tTJSNI_BaseVideoOverlay::FireCallbackCommand(const ttstr & command,
	const ttstr & argument)
{
	// fire call back command event.
	// this is always synchrinized event.
	if(Owner)
	{
		// fire
		if(CanDeliverEvents)
		{
			// fire onStatusChanged event
			tTJSVariant param[2] = {command, argument};
			static ttstr eventname(TJS_W("onCallbackCommand"));
			TVPPostEvent(Owner, Owner, eventname, 0, TVP_EPT_IMMEDIATE,
				2, param);
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_VideoOverlay : TJS VideoOverlay class
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_VideoOverlay::ClassID = -1;
tTJSNC_VideoOverlay::tTJSNC_VideoOverlay()  :
	tTJSNativeClass(TJS_W("VideoOverlay"))
{
	// registration of native members

	TJS_BEGIN_NATIVE_MEMBERS(VideoOverlay) // constructor
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this,
	/*var.type*/tTJSNI_VideoOverlay,
	/*TJS class name*/VideoOverlay)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/VideoOverlay)
//----------------------------------------------------------------------

//-- methods

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/open)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	_this->Open(*param[0]);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/open)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/play)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	_this->Play();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/play)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/stop)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	_this->Stop();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/stop)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/close)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	_this->Close();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/close)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setPos) // not SetPosition
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	if(numparams < 2) return TJS_E_BADPARAMCOUNT;
	_this->SetPosition(*param[0], *param[1]);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/setPos)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setSize)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	if(numparams < 2) return TJS_E_BADPARAMCOUNT;
	_this->SetSize(*param[0], *param[1]);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/setSize)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setBounds)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	if(numparams < 4) return TJS_E_BADPARAMCOUNT;
	tTVPRect r;
	r.left = *param[0];
	r.top = *param[1];
	r.right = r.left + (tjs_int)*param[2];
	r.bottom = r.top + (tjs_int)*param[3];
	_this->SetBounds(r);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/setBounds)
//----------------------------------------------------------------------

//-- events

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/onStatusChanged)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	tTJSVariantClosure obj = _this->GetActionOwnerNoAddRef();
	if(obj.Object)
	{
		TVP_ACTION_INVOKE_BEGIN(1, "onStatusChanged", objthis);
		TVP_ACTION_INVOKE_MEMBER("status");
		TVP_ACTION_INVOKE_END(obj);
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/onStatusChanged)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/onCallbackCommand)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_VideoOverlay);

	tTJSVariantClosure obj = _this->GetActionOwnerNoAddRef();
	if(obj.Object)
	{
		TVP_ACTION_INVOKE_BEGIN(2, "onCallbackCommand", objthis);
		TVP_ACTION_INVOKE_MEMBER("command");
		TVP_ACTION_INVOKE_MEMBER("arg");
		TVP_ACTION_INVOKE_END(obj);
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/onCallbackCommand)
//----------------------------------------------------------------------

//-- properties

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(position)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		// not yet implemented

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		// not yet implemented

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(position)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(left)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		*result = _this->GetLeft();

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		_this->SetLeft(*param);

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(left)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(top)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		*result = _this->GetTop();

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		_this->SetTop(*param);

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(top)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(width)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		*result = _this->GetWidth();

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		_this->SetWidth(*param);

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(width)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(height)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		*result = _this->GetHeight();

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		_this->SetHeight(*param);

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(height)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(visible)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		*result = _this->GetVisible();

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_VideoOverlay);

		_this->SetVisible(param->operator bool());

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(visible)
//----------------------------------------------------------------------

	TJS_END_NATIVE_MEMBERS

//----------------------------------------------------------------------


}

