//---------------------------------------------------------------------------
// krmovie.cpp ( part of KRMOVIE.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.
*/

//---------------------------------------------------------------------------

#include <objidl.h>
#include <streams.h>
#include "asyncio.h"
#include "asyncrdr.h"


#include "..\krmovie.h"

//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// CIStreamProxy : IStream to CAsyncStream proxy
//---------------------------------------------------------------------------
class CIStreamProxy : public CAsyncStream
{
public:
    CIStreamProxy(IStream *refstream, unsigned __int64 size) :
        m_RefStream(refstream),
        m_Size(size)
    {
		m_RefStream->AddRef();
    }

	~CIStreamProxy()
	{
		m_RefStream->Release();
	}

	HRESULT SetPointer(LONGLONG llPos)
    {
		ULARGE_INTEGER result;
		LARGE_INTEGER to;
		to.QuadPart = llPos;
		HRESULT hr = m_RefStream->Seek(to, STREAM_SEEK_SET, &result); 
		if(SUCCEEDED(hr) && llPos != (LONGLONG)result.QuadPart) return E_FAIL;
		return hr;
    }
    
	HRESULT Read(PBYTE pbBuffer,
                 DWORD dwBytesToRead,
                 BOOL bAlign,
                 LPDWORD pdwBytesRead)
    {
        CAutoLock lck(&m_csLock);

		ULONG  bytesread;
		HRESULT hr = m_RefStream->Read(pbBuffer, dwBytesToRead, &bytesread);
		*pdwBytesRead = bytesread;
		return hr;
    }

    LONGLONG Size(LONGLONG *pSizeAvailable)
    {
        *pSizeAvailable = m_Size;
        return m_Size;
    }
    
	DWORD Alignment()
    {
        return 1;
    }
    
	void Lock()
    {
        m_csLock.Lock();
    }
    
	void Unlock()
    {
        m_csLock.Unlock();
    }

private:
    CCritSec		 m_csLock;
	IStream	 *		 m_RefStream;
	unsigned __int64 m_Size;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// CIStreamReader 
//---------------------------------------------------------------------------
class CIStreamReader : public CAsyncReader
{
public:
    STDMETHODIMP Register()
    {
        return S_OK;
    }
    STDMETHODIMP Unregister()
    {
        return S_OK;
    }
    CIStreamReader(CIStreamProxy *pStream, CMediaType *pmt, HRESULT *phr) :
        CAsyncReader(NAME("IStream Reader"), NULL, pStream, phr)
    {
        m_mt = *pmt;
    }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTVPVideoOverlayInterfaces
//---------------------------------------------------------------------------
struct tTVPVideoOverlayInterfaces
{
	CIStreamProxy *Proxy;
	CIStreamReader *Reader;
	IFilterGraph *FilterGraph;
	IGraphBuilder *GraphBuilder;
	IMediaControl *MediaControl;
	IMediaEventEx *MediaEventEx;
	IVideoWindow  *VideoWindow;
	IBasicAudio   *BasicAudio;
	IBasicVideo   *BasicVideo;
	IMediaSeeking *MediaSeeking;

#define RELEASE_AND_NULL(x) if(x) x->Release(), x = NULL;
	void ReleaseAll()
	{
		RELEASE_AND_NULL(Reader);
		RELEASE_AND_NULL(MediaEventEx);
		RELEASE_AND_NULL(MediaSeeking);
		RELEASE_AND_NULL(MediaControl);
		RELEASE_AND_NULL(BasicAudio);
		RELEASE_AND_NULL(BasicVideo);
		RELEASE_AND_NULL(VideoWindow);
		RELEASE_AND_NULL(GraphBuilder);
		RELEASE_AND_NULL(FilterGraph);
		if(Proxy) delete Proxy, Proxy = NULL;
	}
#undef RELEASE_AND_NULL
};


//---------------------------------------------------------------------------
// tTVPVideoOverlay
//---------------------------------------------------------------------------
class tTVPVideoOverlay : public iTVPVideoOverlay
{
	ULONG RefCount;
	tTVPVideoOverlayInterfaces Intfs;
	HWND OwnerWindow;
	bool Visible;
	bool Shutdown;
	RECT Rect;

public:
	tTVPVideoOverlay(const tTVPVideoOverlayInterfaces &intfs);
	~tTVPVideoOverlay();

	void __stdcall AddRef();
	void __stdcall Release();

	const wchar_t* __stdcall SetWindow(HWND window);
	const wchar_t* __stdcall SetMessageDrainWindow(HWND window);
	const wchar_t* __stdcall SetRect(RECT *rect);
	const wchar_t* __stdcall SetVisible(bool b);
	const wchar_t* __stdcall Play();
	const wchar_t* __stdcall Stop();
	const wchar_t* __stdcall Pause();
	const wchar_t* __stdcall SetPosition(unsigned __int64 tick);
	const wchar_t* __stdcall GetPosition(unsigned __int64 *tick);
	const wchar_t* __stdcall GetStatus(tTVPVideoStatus *status);
	const wchar_t* __stdcall GetEvent(long *evcode, long *param1,
			long *param2, bool *got);
};
//---------------------------------------------------------------------------
tTVPVideoOverlay::tTVPVideoOverlay(const tTVPVideoOverlayInterfaces &intfs)
{
	CoInitialize(NULL);
	OwnerWindow = NULL;
	Visible = false;
	Rect.left = 0; Rect.top = 0; Rect.right = 320; Rect.bottom = 240;
	Intfs = intfs;
	RefCount = 1;
	Shutdown = false;
}
//---------------------------------------------------------------------------
tTVPVideoOverlay::~tTVPVideoOverlay()
{
	Shutdown = true;
	Intfs.MediaControl->Stop();
/*
	if(Intfs.VideoWindow && OwnerWindow)
	{
		Intfs.VideoWindow->put_Visible(OAFALSE);
		Intfs.VideoWindow->put_MessageDrain(NULL);
		Intfs.VideoWindow->put_Owner(NULL);
			// Microsoft document says that Intfs.VideoWindow->put_Owner(NULL) is needed,
			// but this conflicts with functonality of full-screened mode.
	}
*/
	Intfs.ReleaseAll();
	CoUninitialize();
}
//---------------------------------------------------------------------------
void __stdcall tTVPVideoOverlay::AddRef()
{
	RefCount ++;
}
//---------------------------------------------------------------------------
void __stdcall tTVPVideoOverlay::Release()
{
	if(RefCount == 1)
		delete this;
	else
		RefCount--;
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::SetWindow(HWND window)
{
	/*
		This sets owner window and re-position window displaying area.
	*/
	if(Shutdown) return NULL;

	HRESULT hr;
	if(OwnerWindow != window)
	{
		if(window == NULL)
		{
			hr = Intfs.VideoWindow->put_Visible(OAFALSE);
			if(FAILED(hr)) return L"Failed to call IVideoWindow::put_Visible(OAFALSE).";
		}

		hr = Intfs.VideoWindow->put_Owner((OAHWND)window);
		if(FAILED(hr)) return L"Failed to call IVideoWindow::put_Owner.";
		
		if(window != NULL)
		{
            hr = Intfs.VideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			if(FAILED(hr)) return L"Failed to call IVideoWindow::put_WindowStyle.";
			
			hr = Intfs.VideoWindow->put_Visible(Visible?OATRUE:OAFALSE);
			if(FAILED(hr)) return L"Failed to call IVideoWindow::put_Visible.";
					
			hr = Intfs.VideoWindow->SetWindowPosition(Rect.left, Rect.top,
				Rect.right-Rect.left, Rect.bottom-Rect.top);
			if(FAILED(hr)) return L"Failed to call IVideoWindow::SetWindowPosition.";
		}
		OwnerWindow = window;
	}
	
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPVideoOverlay::SetMessageDrainWindow(HWND window)
{
	HRESULT hr;
	hr = Intfs.VideoWindow->put_MessageDrain((OAHWND)window);
	if(FAILED(hr)) return L"Failed to call IVideoWindow::put_MessageDrain.";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPVideoOverlay::SetRect(RECT *rect)
{
	if(Shutdown) return NULL;
	HRESULT hr;
	Rect = *rect;
	hr = Intfs.VideoWindow->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top);
	if(FAILED(hr)) return L"Failed to call IVideoWindow::SetWindowPosition.";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPVideoOverlay::SetVisible(bool b)
{
	if(Shutdown) return NULL;
	HRESULT hr;
	if(OwnerWindow)
	{
		hr = Intfs.VideoWindow->put_Visible(b?OATRUE:OAFALSE);
		if(FAILED(hr)) return L"Failed to call IVideoWindow::put_Visible.";
	}
	Visible = b;
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::Play()
{
	if(Shutdown) return NULL;
	HRESULT hr;
	hr = Intfs.VideoWindow->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top);
	if(FAILED(hr)) return L"Failed to call IVideoWindow::SetWindowPosition.";
	hr = Intfs.MediaControl->Run();
	if(FAILED(hr)) return L"Failed to call IMediaControl::Run.";
	hr = Intfs.VideoWindow->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top);
	if(FAILED(hr)) return L"Failed to call IVideoWindow::SetWindowPosition.";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::Stop()
{
	if(Shutdown) return NULL;
	HRESULT hr;
	hr = Intfs.MediaControl->Stop();
	if(FAILED(hr)) return L"Failed to call IMediaControl::Stop.";
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::Pause()
{
	if(Shutdown) return NULL;
	return L"Currently not supported";
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::SetPosition(unsigned __int64 tick)
{
	if(Shutdown) return NULL;
	return L"Currently not supported";
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::GetPosition(unsigned __int64 *tick)
{
	if(Shutdown) return NULL;
	return L"Currently not supported";
}
//---------------------------------------------------------------------------
const wchar_t*  __stdcall  tTVPVideoOverlay::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return NULL;
	HRESULT hr;
	OAFilterState state;
	hr = Intfs.MediaControl->GetState(50, &state);
	if(hr == VFW_S_STATE_INTERMEDIATE)
		*status = vsProcessing;
	else if(state == State_Stopped)
		*status = vsStopped;
	else if(state == State_Running)
		*status = vsPlaying;
	else if(state == State_Paused)
		*status = vsPaused;
	return NULL;
}
//---------------------------------------------------------------------------
const wchar_t* __stdcall tTVPVideoOverlay::GetEvent(long *evcode, long *param1,
			long *param2, bool *got)
{
	if(Shutdown) return NULL;
	HRESULT hr;
	*got = false;
	hr = Intfs.MediaEventEx->GetEvent(evcode, param1, param2, 0);
	if(SUCCEEDED(hr)) *got = true;
	return NULL;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
const wchar_t*  CreateVideoOverlay(HWND callbackwin, IStream *stream, 
	const wchar_t * streamname, const wchar_t *type,
	unsigned __int64 size, iTVPVideoOverlay **out)
{
	tTVPVideoOverlayInterfaces intf;
	memset(&intf, 0, sizeof(intf));

	const wchar_t *errmsg = NULL;
	HRESULT hr;
	CoInitialize(NULL);


	// detect CMediaType from stream's extension ('type')

	try
	{
		CMediaType mt;
		mt.majortype = MEDIATYPE_Stream;

//		mt.subtype = MEDIASUBTYPE_None;

		// note: audio-less mpeg stream must have an extension of
		// ".mpv" .
		if      (wcsicmp(type, L".mpg") == 0)
			mt.subtype = MEDIASUBTYPE_MPEG1System;
		else if (wcsicmp(type, L".mpeg") == 0)
			mt.subtype = MEDIASUBTYPE_MPEG1System;
		else if (wcsicmp(type, L".mpv") == 0) 
			mt.subtype = MEDIASUBTYPE_MPEG1Video;
		else if (wcsicmp(type, L".dat") == 0)
			mt.subtype = MEDIASUBTYPE_MPEG1VideoCD;
//		else if (wcsicmp(type, L".asf") == 0)
//			mt.subtype = MEDIASUBTYPE_Asf;  // asf not supported
		else if (wcsicmp(type, L".avi") == 0)
			mt.subtype = MEDIASUBTYPE_Avi;
		else if (wcsicmp(type, L".mov") == 0)
			mt.subtype = MEDIASUBTYPE_QTMovie;
		else
			throw L"Unknown video format extension."; // unknown format


		// create proxy filter
		intf.Proxy = new CIStreamProxy(stream, size);
		hr = S_OK;
		intf.Reader = new CIStreamReader(intf.Proxy, &mt, &hr);

		if(FAILED(hr) || intf.Reader == NULL)
			throw L"Failed to create proxy filter object.";

		intf.Reader->AddRef();
		
		// create IFilterGraph instance
		hr = CoCreateInstance(CLSID_FilterGraph,
	                               NULL,
	                               CLSCTX_INPROC,
	                               IID_IFilterGraph,
	                               (void**) &intf.FilterGraph);
		if (FAILED(hr)) throw L"Failed to create FilterGraph.";

		// add fliter
		hr = intf.FilterGraph->AddFilter(intf.Reader, NULL);
		if (FAILED(hr)) throw L"Failed to call IFilterGraph::AddFilter.";

		// query IGraphBuilder
		hr = intf.FilterGraph->QueryInterface(IID_IGraphBuilder,
				(void **)&intf.GraphBuilder);
		if (FAILED(hr)) throw L"Failed to query IGraphBuilder.";

		// render output pin
		hr = intf.GraphBuilder->Render(intf.Reader->GetPin(0));
		if (FAILED(hr)) throw L"Failed to call IGraphBuilder::Render.";

		// release GraphBuilder
		intf.GraphBuilder->Release(); intf.GraphBuilder = NULL;

		// query each interfaces
       if(FAILED(intf.FilterGraph->QueryInterface(IID_IMediaControl, (void **)&intf.MediaControl)))
			throw L"Failed to query IMediaControl";
        if(FAILED(intf.FilterGraph->QueryInterface(IID_IMediaEventEx, (void **)&intf.MediaEventEx)))
			throw L"Failed to query IMediaEventEx";
        if(FAILED(intf.FilterGraph->QueryInterface(IID_IMediaSeeking, (void **)&intf.MediaSeeking)))
			throw L"Failed to query IMediaSeeking";
		
        if(FAILED(intf.FilterGraph->QueryInterface(IID_IVideoWindow, (void **)&intf.VideoWindow)))
			throw L"Failed to query IVideoWindow";
        if(FAILED(intf.FilterGraph->QueryInterface(IID_IBasicVideo, (void **)&intf.BasicVideo)))
			throw L"Failed to query IBasicVideo";

        if(FAILED(intf.FilterGraph->QueryInterface(IID_IBasicAudio, (void **)&intf.BasicAudio)))
			throw L"Failed to query IBasicAudio";

		// check whether the stream has video 
		if(!intf.VideoWindow || !intf.BasicVideo)
			throw L"The stream has no video components.";

		{
			long visible;
			if(FAILED(intf.VideoWindow->get_Visible(&visible)))
				throw L"The stream has no video components or has unsupported video format.";
		}
	
		// disable AutoShow
		intf.VideoWindow->put_AutoShow(OAFALSE);

		// set notify event
		if(callbackwin)
		{
			if(FAILED(intf.MediaEventEx->SetNotifyWindow((OAHWND)callbackwin, WM_GRAPHNOTIFY, 0)))
				throw L"Failed to set IMediaEventEx::SetNotifyWindow.";
		}

		// create video overlay object
		*out = new tTVPVideoOverlay(intf);
	}
	catch(const wchar_t * msg)
	{
		errmsg = msg;
	}
	catch(...)
	{
		errmsg = L"Unknown error";
	}

	if(errmsg)
	{
		intf.ReleaseAll();
		CoUninitialize();
		return errmsg;
	}

	CoUninitialize();
	return NULL;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// GetVideoOverlayObject
//---------------------------------------------------------------------------
const wchar_t* __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	return CreateVideoOverlay(callbackwin, stream, streamname, type,
			size, out);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// GetAPIVersion
//---------------------------------------------------------------------------
void __stdcall GetAPIVersion(DWORD *ver)
{
	*ver = TVP_KRMOVIE_VER;
}
//---------------------------------------------------------------------------


