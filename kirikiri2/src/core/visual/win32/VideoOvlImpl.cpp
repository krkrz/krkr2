//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Video Overlay support implementation
//---------------------------------------------------------------------------


#include "tjsCommHead.h"
#pragma hdrstop

#include "MsgIntf.h"
#include "VideoOvlImpl.h"
#include "SysInitIntf.h"
#include "StorageImpl.h"
#include "krmovie.h"
#include <evcode.h>
typedef void (__stdcall *tGetAPIVersion)(DWORD *version);
typedef const wchar_t*  (__stdcall *tGetVideoOverlayObject)(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out);

//---------------------------------------------------------------------------
class tTVPVideoModule
{
	tTVPPluginHolder *Holder;
	HMODULE Handle;
	tGetAPIVersion procGetAPIVersion;
	tGetVideoOverlayObject procGetVideoOverlayObject;

public:
	tTVPVideoModule(const ttstr & name);
	~tTVPVideoModule();

	void GetAPIVersion(DWORD *version) { procGetAPIVersion(version); }
	const wchar_t * GetVideoOverlayObject(HWND callbackwin, IStream *stream,
		const wchar_t * streamname, const wchar_t *type, unsigned __int64 size,
		iTVPVideoOverlay **out)
	{
		return procGetVideoOverlayObject(callbackwin, stream, streamname, type, size, out);
	}
};
static tTVPVideoModule *TVPMovieVideoModule = NULL;
static tTVPVideoModule *TVPFlashVideoModule = NULL;
static void TVPUnloadKrMovie();
//---------------------------------------------------------------------------
tTVPVideoModule::tTVPVideoModule(const ttstr &name)
{
	Holder = new tTVPPluginHolder(name);
	Handle = LoadLibrary(Holder->GetLocalName().AsAnsiString().c_str());
	if(!Handle)
	{
		delete Holder;
		TVPThrowExceptionMessage(TVPCannotLoadKrMovieDLL);
	}

	try
	{
		procGetVideoOverlayObject = (tGetVideoOverlayObject)
			GetProcAddress(Handle, "GetVideoOverlayObject");
		procGetAPIVersion = (tGetAPIVersion)
			GetProcAddress(Handle, "GetAPIVersion");
		if(!procGetVideoOverlayObject || !procGetAPIVersion)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
		DWORD version;
		procGetAPIVersion(&version);
		if(version != TVP_KRMOVIE_VER)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
	}
	catch(...)
	{
		FreeLibrary(Handle);
		delete Holder;
		throw;
	}
}
//---------------------------------------------------------------------------
tTVPVideoModule::~tTVPVideoModule()
{
	FreeLibrary(Handle);
	delete Holder;
}
//---------------------------------------------------------------------------
static tTVPVideoModule * TVPGetMovieVideoModule()
{
	if(TVPMovieVideoModule == NULL)
		TVPMovieVideoModule = new tTVPVideoModule("krmovie.dll");

	return TVPMovieVideoModule;
}
//---------------------------------------------------------------------------
static tTVPVideoModule * TVPGetFlashVideoModule()
{
	if(TVPFlashVideoModule == NULL)
		TVPFlashVideoModule = new tTVPVideoModule("krflash.dll");

	return TVPFlashVideoModule;
}
//---------------------------------------------------------------------------
/*
//---------------------------------------------------------------------------
static void TVPLoadKrMovie(bool flash)
{
	if(TVPKrMovieHandle) TVPUnloadKrMovie();

	TVPFlashDllLoaded = flash;
	TVPKrMovieHolder = new tTVPPluginHolder(TJS_W("krmovie.dll"));
	TVPKrMovieHandle =
		LoadLibrary(TVPKrMovieHolder->GetLocalName().AsAnsiString().c_str());
	if(!TVPKrMovieHandle)
	{
		delete TVPKrMovieHolder;
		TVPKrMovieHolder = NULL;
		TVPThrowExceptionMessage(TVPCannotLoadKrMovieDLL);
	}

	try
	{
		TVPGetVideoOverlayObject = (tGetVideoOverlayObject)
			GetProcAddress(TVPKrMovieHandle, "GetVideoOverlayObject");
		TVPGetAPIVersion = (tGetAPIVersion)
			GetProcAddress(TVPKrMovieHandle, "GetAPIVersion");
		if(!TVPGetVideoOverlayObject || !TVPGetAPIVersion)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
		DWORD version;
		TVPGetAPIVersion(&version);
		if(version != TVP_KRMOVIE_VER)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
	}
	catch(...)
	{
		FreeLibrary(TVPKrMovieHandle), TVPKrMovieHandle = NULL;
		delete TVPKrMovieHolder;
		TVPKrMovieHolder = NULL;
		throw;
	}
}
//---------------------------------------------------------------------------
static void TVPUnloadKrMovie()
{
	if(!TVPKrMovieHandle) return;

	FreeLibrary(TVPKrMovieHandle);
	TVPKrMovieHandle = NULL;

	delete TVPKrMovieHolder;
	TVPKrMovieHolder = NULL;
}
*/
//---------------------------------------------------------------------------
static std::vector<tTJSNI_VideoOverlay *> TVPVideoOverlayVector;
//---------------------------------------------------------------------------
static void TVPAddVideOverlay(tTJSNI_VideoOverlay *ovl)
{
	TVPVideoOverlayVector.push_back(ovl);
}
//---------------------------------------------------------------------------
static void TVPRemoveVideoOverlay(tTJSNI_VideoOverlay *ovl)
{
	std::vector<tTJSNI_VideoOverlay*>::iterator i;
	i = std::find(TVPVideoOverlayVector.begin(), TVPVideoOverlayVector.end(), ovl);
	if(i != TVPVideoOverlayVector.end())
		TVPVideoOverlayVector.erase(i);
}
//---------------------------------------------------------------------------
static void TVPShutdownVideoOverlay()
{
	// shutdown all overlay object and release krmovie.dll / krflash.dll
	std::vector<tTJSNI_VideoOverlay*>::iterator i;
	for(i = TVPVideoOverlayVector.begin(); i != TVPVideoOverlayVector.end(); i++)
	{
		(*i)->Shutdown();
	}

	if(TVPMovieVideoModule) delete TVPMovieVideoModule, TVPMovieVideoModule = NULL;
	if(TVPFlashVideoModule) delete TVPFlashVideoModule, TVPFlashVideoModule = NULL;
}
static tTVPAtExit TVPShutdownVideoOverlayAtExit
	(TVP_ATEXIT_PRI_PREPARE, TVPShutdownVideoOverlay);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSNI_VideoOverlay
//---------------------------------------------------------------------------
tTJSNI_VideoOverlay::tTJSNI_VideoOverlay()
{
	VideoOverlay = NULL;
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = 320;
	Rect.bottom = 240;
	Visible = false;
	OwnerWindow = NULL;
	LocalTempStorageHolder = NULL;

	UtilWindow = AllocateHWnd(WndProc);
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNI_VideoOverlay::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = inherited::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_VideoOverlay::Invalidate()
{
	inherited::Invalidate();

	Close();

	if(UtilWindow) DeallocateHWnd(UtilWindow);
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Open(const ttstr &_name)
{
	// open

	// first, close
	Close();


	// check window
	if(!Window) TVPThrowExceptionMessage(TVPWindowAlreadyMissing);

	// open target storage
	ttstr name(_name);
	ttstr param;

	const tjs_char * param_pos;
	int param_pos_ind;
	param_pos = TJS_strchr(name.c_str(), TJS_W('?'));
	param_pos_ind = param_pos - name.c_str();
	if(param_pos != NULL)
	{
		param = param_pos;
		name = ttstr(name, param_pos_ind);
	}

	IStream *istream = NULL;
	long size;
	bool flash;
	ttstr ext = TVPExtractStorageExt(name).c_str();
	ext.ToLowerCase();

	tTVPVideoModule *mod = NULL;
	if(ext == TJS_W(".swf"))
	{
		// shockwave flash movie
		flash = true;

		// load krflash.dll
		mod = TVPGetFlashVideoModule();

		// prepare local storage
		if(LocalTempStorageHolder)
			delete LocalTempStorageHolder, LocalTempStorageHolder = NULL;

		// find local name
		ttstr placed = TVPSearchPlacedPath(name);

		// open and hold
		LocalTempStorageHolder =
			new tTVPLocalTempStorageHolder(placed);
	}
	else
	{
		flash = false;

		// load krmovie.dll
		mod = TVPGetMovieVideoModule();

		// prepate IStream
		tTJSBinaryStream *stream0 = NULL;
		try
		{
			stream0 = TVPCreateStream(name);
			size = (long)stream0->GetSize();
		}
		catch(...)
		{
			if(stream0) delete stream0;
			throw;
		}

		istream = new tTVPIStreamAdapter(stream0);
	}

	// 'istream' is an IStream instance at this point

	// create video overlay object
	try
	{
		const wchar_t *message;
		if(flash)
		{
			message = mod->GetVideoOverlayObject(UtilWindow,
				NULL, (LocalTempStorageHolder->GetLocalName() + param).c_str(),
				ext.c_str(), 0, &VideoOverlay);
		}
		else
		{
			message = mod->GetVideoOverlayObject(UtilWindow,
				istream, name.c_str(), ext.c_str(),
				size, &VideoOverlay);
		}
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);

		// set Window handle
		tjs_int ofsx, ofsy;
		message = VideoOverlay->SetWindow(Window->GetWindowHandle(ofsx, ofsy));
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);

		VideoOverlay->SetMessageDrainWindow(Window->GetSurfaceWindowHandle());
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);

		// set Rectangle
		RECT r = {Rect.left + ofsx, Rect.top + ofsy,
			Rect.right + ofsx, Rect.bottom + ofsy};
		message = VideoOverlay->SetRect(&r);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);

		// set Visible
		message = VideoOverlay->SetVisible(Visible);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
	}
	catch(...)
	{
		if(istream) istream->Release();
		Close();
		throw;
	}
	if(istream) istream->Release();

	// set Status
	ClearWndProcMessages();
	SetStatus(ssStop);
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Close()
{
	// close
	// release VideoOverlay object
	if(VideoOverlay)
	{
		VideoOverlay->Release(), VideoOverlay = NULL;
		tjs_int ofsx, ofsy;
		::SetFocus(Window->GetWindowHandle(ofsx, ofsy));
	}
	if(LocalTempStorageHolder)
		delete LocalTempStorageHolder, LocalTempStorageHolder = NULL;
	ClearWndProcMessages();
	SetStatus(ssUnload);
//	TVPUnloadKrMovie();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Shutdown()
{
	// shutdown the system
	// this functions closes the overlay object, but must not fire any events.
	bool c = CanDeliverEvents;
	ClearWndProcMessages();
	SetStatus(ssUnload);
	try
	{
		if(VideoOverlay) VideoOverlay->Release(), VideoOverlay = NULL;
	}
	catch(...)
	{
		CanDeliverEvents = c;
		throw;
	}
	CanDeliverEvents = c;
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Disconnect()
{
	// disconnect the object
	Shutdown();

	Window = NULL;
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Play()
{
	// start playing
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->Play();
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
		ClearWndProcMessages();
		SetStatus(ssPlay);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Stop()
{
	// stop playing
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->Stop();
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
		ClearWndProcMessages();
		SetStatus(ssStop);
	}
}
//---------------------------------------------------------------------------
// Start:	Add:	T.Imoto
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Pause()
{
	// pause playing
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->Pause();
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
		ClearWndProcMessages();
		SetStatus(ssPause);
	}
}
void tTJSNI_VideoOverlay::Rewind()
{
	// rewind playing
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->Rewind();
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
		ClearWndProcMessages();
	}
}
//---------------------------------------------------------------------------
// End:		Add:	T.Imoto
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetRectangleToVideoOverlay()
{
	// set Rectangle to video overlay
	if(VideoOverlay && OwnerWindow)
	{
		const wchar_t *message;
		RECT r = {Rect.left, Rect.top, Rect.right, Rect.bottom};
		message = VideoOverlay->SetRect(&r);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetPosition(tjs_int left, tjs_int top)
{
	Rect.set_offsets(left, top);
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetSize(tjs_int width, tjs_int height)
{
	Rect.set_size(width, height);
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetBounds(const tTVPRect & rect)
{
	Rect = rect;
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetLeft(tjs_int l)
{
	Rect.set_offsets(l, Rect.top);
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetTop(tjs_int t)
{
	Rect.set_offsets(Rect.left, t);
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetWidth(tjs_int w)
{
	Rect.right = Rect.left + w;
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetHeight(tjs_int h)
{
	Rect.bottom = Rect.top + h;
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetVisible(bool b)
{
	Visible = b;
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->SetVisible(Visible);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetWindowHandle(HWND wnd)
{
	OwnerWindow = wnd;
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->SetWindow(wnd);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetMessageDrainWindow(HWND wnd)
{
	if(VideoOverlay)
	{
		const wchar_t *message;
		message = VideoOverlay->SetMessageDrainWindow(wnd);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetRectOffset(tjs_int ofsx, tjs_int ofsy)
{
	if(VideoOverlay)
	{
		const wchar_t *message;
		RECT r = {Rect.left + ofsx, Rect.top + ofsy,
			Rect.right + ofsx, Rect.bottom + ofsy};
		message = VideoOverlay->SetRect(&r);
		if(message) TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, message);
	}
}
//---------------------------------------------------------------------------
void __fastcall tTJSNI_VideoOverlay::WndProc(Messages::TMessage &Msg)
{
	// UtilWindow's message procedure
	if(VideoOverlay)
	{
		if(Msg.Msg == WM_GRAPHNOTIFY)
		{
			long evcode, p1, p2;
			bool got;
			VideoOverlay->GetEvent(&evcode, &p1, &p2, &got);
			if(got)
			{
				if(Status == ssPlay && evcode == EC_COMPLETE) 
					SetStatusAsync(ssStop); // All data has been rendered
			}
			return;
		}
		else if(Msg.Msg == WM_CALLBACKCMD)
		{
			// wparam : command
			// lparam : argument
			FireCallbackCommand((tjs_char*)Msg.WParam, (tjs_char*)Msg.LParam);
			return;
		}
	}

	Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::ClearWndProcMessages()
{
	// clear WndProc's message queue
	MSG msg;
	while(PeekMessage(&msg, UtilWindow, WM_GRAPHNOTIFY, WM_GRAPHNOTIFY+2, PM_REMOVE))
	{
		if(VideoOverlay)
		{
			long evcode, p1, p2;
			bool got;
			VideoOverlay->GetEvent(&evcode, &p1, &p2, &got); // dummy call
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_VideoOverlay::CreateNativeInstance : returns proper instance object
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_VideoOverlay::CreateNativeInstance()
{
	return new tTJSNI_VideoOverlay();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCreateNativeClass_VideoOverlay
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_VideoOverlay()
{
	return new tTJSNC_VideoOverlay();
}
//---------------------------------------------------------------------------


