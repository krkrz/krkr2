//---------------------------------------------------------------------------
// krflashmain.cpp ( part of KRFLASH.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------
/*
	We separated this module because VCL's ActiveX support is too large (in size)
	to link with the main module.

	This requires Macromedia Flash Player plug-in for Internet Explorer.
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop


#include "..\krmovie.h"
#include "FlashContainerFormUnit.h"
#include "krflashmain.h"
#include <evcode.h>

//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPFlashOverlay
//---------------------------------------------------------------------------
tTVPFlashOverlay::tTVPFlashOverlay(const wchar_t *flashfile, HWND callbackwindow)
{
	RefCount = 1;
	CallbackWindow = callbackwindow;
	OwnerWindow = NULL;
	FileName = new wchar_t[wcslen(flashfile) + 1];
	wcscpy(FileName, flashfile);
	Visible = false;
	Shutdown = false;
	CompleteSent = false;
	Rect.left = 0; Rect.top = 0; Rect.right = 0; Rect.bottom = 0;
	RectSet = false;
	Form = NULL;
}
//---------------------------------------------------------------------------
tTVPFlashOverlay::~tTVPFlashOverlay()
{
	Shutdown = true;
	if(FileName) delete [] FileName;
	if(Form) delete Form;
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::AddRef()
{
	RefCount ++;
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::Release()
{
	if(RefCount == 1)
	{
		delete this;
	}
	else
	{
		RefCount--;
	}
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::SetMessageDrainWindow(HWND wnd)
{
	// nothing to do, because flash support does not drain messages.
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::SetWindow(HWND wnd)
{
	if(Shutdown) return NULL;
	OwnerWindow = wnd;
	try
	{
		ResetForm();
	}
	catch(...)
	{
		return L"Cannot create window instance (Flash player ActiveX is not installed ?)";
	}
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::SetRect(RECT *rect)
{
	if(Shutdown) return NULL;
	if(Rect.right-Rect.left == rect->right-rect->left &&
		Rect.bottom-Rect.top == rect->bottom-rect->top)
	{
		Rect = *rect;
		if(Form) Form->SetBounds(Rect.left, Rect.top, Rect.right - Rect.left,
					Rect.bottom - Rect.top);
	}
	else
	{
		Rect = *rect;
		RectSet = true;
		try
		{
			ResetForm();
		}
		catch(...)
		{
			return L"Cannot create window instance (Flash player ActiveX is not installed ?)";
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::SetVisible(bool b)
{
 	if(Shutdown) return NULL;
	Visible = true;
	if(Form) Form->SetFlashVisible(b); else return L"Owner window not specified";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::Play()
{
	if(Shutdown) return NULL;
	if(Form) Form->Play(); else return L"Owner window not specified";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::Stop()
{
	if(Shutdown) return NULL;
	if(Form) Form->Stop(); else return L"Owner window not specified";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::Pause()
{
	if(Shutdown) return NULL;
	if(Form) Form->Pause(); else return L"Owner window not specified";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPFlashOverlay::SetPosition(unsigned __int64 tick)
{
	if(Shutdown) return NULL;
	return L"Currently not supported";
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPFlashOverlay::GetPosition(unsigned __int64 *tick)
{
	if(Shutdown) return NULL;
	return L"Currently not supported";
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPFlashOverlay::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return NULL;
	if(Form) *status = Form->GetStatus(); else return L"Owner window not specified";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPFlashOverlay::GetEvent(long *evcode, long *param1,
			long *param2, bool *got)
{
	if(Shutdown) return NULL;
	*got = false; // not implemented
	return NULL;
}
//---------------------------------------------------------------------------
void tTVPFlashOverlay::SendCommand(wchar_t *command, wchar_t *arg)
{
	::SendMessage(CallbackWindow, WM_CALLBACKCMD, (WPARAM)command, (LPARAM)arg);
}
//---------------------------------------------------------------------------
void tTVPFlashOverlay::ResetForm()
{
	if(Form) delete Form;
	if(!(OwnerWindow && RectSet)) return;
	Form = new TFlashContainerForm(Application, this, OwnerWindow, Rect);
	Form->SetMovie(FileName);
	Form->Visible = Visible;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// GetVideoOverlayObject
//---------------------------------------------------------------------------
extern "C" const wchar_t* _export __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPFlashOverlay(streamname, callbackwin);
	return NULL;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// GetAPIVersion
//---------------------------------------------------------------------------
extern "C" void _export __stdcall GetAPIVersion(DWORD *ver)
{
	*ver = TVP_KRMOVIE_VER;
}
//---------------------------------------------------------------------------


