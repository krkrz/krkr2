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


#include "FlashContainerFormUnit.h"
#include "krflashmain.h"
#include <evcode.h>
#include "tp_stub.h"

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
void __stdcall tTVPFlashOverlay::SetMessageDrainWindow(HWND wnd)
{
	// nothing to do, because flash support does not drain messages.
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::SetWindow(HWND wnd)
{
	if(Shutdown) return;
	OwnerWindow = wnd;
	try
	{
		ResetForm();
	}
	catch(Exception &e)
	{
		TVPThrowExceptionMessage(
			(ttstr(L"Cannot create window instance (Flash player ActiveX is not installed ?) / ") +
				e.Message.c_str()).c_str());
	}
	catch(...)
	{
		TVPThrowExceptionMessage(L"Cannot create window instance (Flash player ActiveX is not installed ?)");
	}
	return;
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::SetRect(RECT *rect)
{
	if(Shutdown) return;
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
		catch(Exception &e)
		{
			TVPThrowExceptionMessage(
				(ttstr(L"Cannot create window instance (Flash player ActiveX is not installed ?) / ") +
					e.Message.c_str()).c_str());
		}
		catch(...)
		{
			TVPThrowExceptionMessage(L"Cannot create window instance (Flash player ActiveX is not installed ?)");
		}
	}
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::SetVisible(bool b)
{
 	if(Shutdown) return;
	Visible = true;
	if(Form)
		Form->SetFlashVisible(b);
	else
		TVPThrowExceptionMessage(L"Owner window not specified");
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::Play()
{
	if(Shutdown) return;
	if(Form)
		Form->Play();
	else
		TVPThrowExceptionMessage(L"Owner window not specified");
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::Stop()
{
	if(Shutdown) return;
	if(Form)
		Form->Stop();
	else
		TVPThrowExceptionMessage(L"Owner window not specified");
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::Pause()
{
	if(Shutdown) return;
	if(Form)
		Form->Pause();
	else
		TVPThrowExceptionMessage(L"Owner window not specified");
}
//---------------------------------------------------------------------------
void __stdcall  tTVPFlashOverlay::SetPosition(unsigned __int64 tick)
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"SetPosition: Currently not supported");
}
//---------------------------------------------------------------------------
void __stdcall  tTVPFlashOverlay::GetPosition(unsigned __int64 *tick)
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetPosition: Currently not supported");
}
//---------------------------------------------------------------------------
void __stdcall  tTVPFlashOverlay::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return;
	if(Form)
		*status = Form->GetStatus();
	else
		TVPThrowExceptionMessage(L"Owner window not specified");
}
//---------------------------------------------------------------------------
void __stdcall tTVPFlashOverlay::GetEvent(long *evcode, long *param1,
			long *param2, bool *got)
{
	if(Shutdown) return;
	*got = false; // not implemented
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
// Start:	Add:	T.Imoto
void __stdcall tTVPFlashOverlay::FreeEventParams(long evcode, long param1, long param2)
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"FreeEventParams: Currently not supported");
}
void __stdcall tTVPFlashOverlay::Rewind()
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"Rewind: Currently not supported");
}
void __stdcall tTVPFlashOverlay::SetFrame( int f )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"SetFrame: Currently not supported");
}
void __stdcall tTVPFlashOverlay::GetFrame( int *f )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetFrame: Currently not supported");
}
void __stdcall tTVPFlashOverlay::GetFPS( double *f )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetFPS: Currently not supported");
}
void __stdcall tTVPFlashOverlay::GetNumberOfFrame( int *f )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetNumberOfFrame: Currently not supported");
}
void __stdcall tTVPFlashOverlay::GetTotalTime( __int64 *t )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetTotalTime: Currently not supported");
}
void __stdcall tTVPFlashOverlay::GetVideoSize( long *width, long *height )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetVideoSize: Currently not supported");
}
void __stdcall tTVPFlashOverlay::GetFrontBuffer( BYTE **buff )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"GetFrontBuffer: Currently not supported");
}
void __stdcall tTVPFlashOverlay::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
	if(Shutdown) return;
	TVPThrowExceptionMessage(L"SetVideoBuffer: Currently not supported");
}
// End:	Add:	T.Imoto



//---------------------------------------------------------------------------
// GetVideoOverlayObject
//---------------------------------------------------------------------------
extern "C" void _export __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPFlashOverlay(streamname, callbackwin);
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



//---------------------------------------------------------------------------
// V2Link : Initialize TVP plugin interface
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	TVPInitImportStub(exporter);

	return S_OK;
}
//---------------------------------------------------------------------------
// V2Unlink : Uninitialize TVP plugin interface
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------

