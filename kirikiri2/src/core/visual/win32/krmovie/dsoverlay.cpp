//---------------------------------------------------------------------------
// ( part of KRMOVIE.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.


	Modified by T.Imoto <http://www.kaede-software.com> 2004
*/

#include "dsoverlay.h"
#include "CIStream.h"

#ifdef _DEBUG
#include "DShowException.h"
#endif

//----------------------------------------------------------------------------
//! @brief	  	何もしない
//----------------------------------------------------------------------------
tTVPDSVideoOverlay::tTVPDSVideoOverlay()
{}
//----------------------------------------------------------------------------
//! @brief	  	何もしない
//----------------------------------------------------------------------------
tTVPDSVideoOverlay::~tTVPDSVideoOverlay()
{}
//----------------------------------------------------------------------------
//! @brief	  	フィルタグラフの構築
//! @param 		callbackwin : メッセージを送信するウィンドウ
//! @param 		stream : 読み込み元ストリーム
//! @param 		streamname : ストリームの名前
//! @param 		type : メディアタイプ(拡張子)
//! @param 		size : メディアサイズ
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSVideoOverlay::BuildGraph( HWND callbackwin, IStream *stream,
	const wchar_t * streamname, const wchar_t *type, unsigned __int64 size )
{
	const wchar_t	*errmsg = NULL;
	HRESULT			hr;

	CoInitialize(NULL);

	// detect CMediaType from stream's extension ('type')
	try {
		CMediaType mt;
		mt.majortype = MEDIATYPE_Stream;
		if( (errmsg = ParseVideoType( mt, type )) != NULL )
			throw errmsg;

		// create proxy filter
		m_Proxy = new CIStreamProxy( stream, size );
		hr = S_OK;
		m_Reader = new CIStreamReader( m_Proxy, &mt, &hr );

		if( FAILED(hr) || m_Reader == NULL )
			throw L"Failed to create proxy filter object.";

		m_Reader->AddRef();

		// create IFilterGraph instance
		if( FAILED(hr = m_GraphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)) )
			throw L"Failed to create FilterGraph.";

		// add fliter
		if( FAILED(hr = GraphBuilder()->AddFilter( m_Reader, NULL)) )
			throw L"Failed to call IFilterGraph::AddFilter.";

		if( mt.subtype == MEDIASUBTYPE_Avi || mt.subtype == MEDIASUBTYPE_QTMovie )
		{
			// render output pin
			if( FAILED(hr = GraphBuilder()->Render(m_Reader->GetPin(0))) )
				throw L"Failed to call IGraphBuilder::Render.";
		}
		else
		{
			CComPtr<IBaseFilter>	pVRender;	// for video renderer filter
			if( FAILED(hr = pVRender.CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER)) )
				return L"Failed to create video renderer filter object.";
			if( FAILED(hr = GraphBuilder()->AddFilter(pVRender, L"Video Renderer")) )
				return L"Failed to call IFilterGraph::AddFilter.";

			if( mt.subtype == MEDIASUBTYPE_MPEG1Video )
			{	// Not use audio
				if( (errmsg = BuildMPEGGraph( pVRender, m_Reader, false )) != NULL )
					throw errmsg;
			}
			else
			{
				if( (errmsg = BuildMPEGGraph( pVRender, m_Reader, true )) != NULL )
					throw errmsg;
			}
		}

		// query each interfaces
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaControl )) )
			throw L"Failed to query IMediaControl";
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaPosition )) )
			throw L"Failed to query IMediaPosition";
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaSeeking )) )
			throw L"Failed to query IMediaSeeking";
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaEventEx )) )
			throw L"Failed to query IMediaEventEx";

		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_BasicVideo )) )
			throw L"Failed to query IBasicVideo";

		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_VideoWindow )) )
			throw L"Failed to query IVideoWindow";
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_BasicAudio )) )
			throw L"Failed to query IBasicAudio";

		// check whether the stream has video 
		if(!m_VideoWindow || !m_BasicVideo )
			throw L"The stream has no video components.";

		{
			long visible;
			if(FAILED(VideoWindow()->get_Visible(&visible)))
				throw L"The stream has no video components or has unsupported video format.";
		}
	
		// disable AutoShow
		VideoWindow()->put_AutoShow(OAFALSE);

		// set notify event
		if(callbackwin)
		{
			if(FAILED(Event()->SetNotifyWindow((OAHWND)callbackwin, WM_GRAPHNOTIFY, (long)(this))))
				throw L"Failed to set IMediaEventEx::SetNotifyWindow.";
		}
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
		ReleaseAll();
		CoUninitialize();
		return errmsg;
	}

	CoUninitialize();
	return NULL;
}

//----------------------------------------------------------------------------
//! @brief	  	インターフェイスを解放する
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::ReleaseAll()
{
#ifdef _DEBUG
	if( m_RegisteredROT )
		RemoveFromROT( m_dwROTReg );
#endif

	if( m_MediaControl.p != NULL )
	{
		m_MediaControl->Stop();
	}

	if( m_VideoWindow.p )
		m_VideoWindow.Release();

	if( m_BasicAudio.p )
		m_BasicAudio.Release();

	tTVPDSMovie::ReleaseAll();
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオ ウィンドウを所有する親ウィンドウを設定する。 
//! 
//! ビデオ ウィンドウを所有する親ウィンドウを設定し、表示矩形も同時に設定する。
//! @param 		window	親ウィンドウ
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSVideoOverlay::SetWindow( HWND window )
{
	if(Shutdown) return NULL;

	HRESULT hr;
	if(OwnerWindow != window)
	{
		if(window == NULL)
		{
			if( FAILED(hr = VideoWindow()->put_Visible(OAFALSE)) )
			{
#ifdef _DEBUG
				OutputDebugString( DShowException(hr).what() );
#endif
				return L"Failed to call IVideoWindow::put_Visible(OAFALSE).";
			}
		}
		if( FAILED(hr = VideoWindow()->put_Owner((OAHWND)window)) )
		{
#ifdef _DEBUG
			OutputDebugString( DShowException(hr).what() );
#endif
			return L"Failed to call IVideoWindow::put_Owner.";
		}
		if(window != NULL)
		{
			if( FAILED(hr = VideoWindow()->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)) )
			{
#ifdef _DEBUG
				OutputDebugString( DShowException(hr).what() );
#endif
				return L"Failed to call IVideoWindow::put_WindowStyle.";
			}
			if( FAILED(hr = VideoWindow()->put_Visible(Visible?OATRUE:OAFALSE)) )
			{
#ifdef _DEBUG
				OutputDebugString( DShowException(hr).what() );
#endif
				return L"Failed to call IVideoWindow::put_Visible.";
			}
			if( FAILED(hr = VideoWindow()->SetWindowPosition(Rect.left, Rect.top,
				Rect.right-Rect.left, Rect.bottom-Rect.top)) )
			{
#ifdef _DEBUG
				OutputDebugString( DShowException(hr).what() );
#endif
				return L"Failed to call IVideoWindow::SetWindowPosition.";
			}
		}
		OwnerWindow = window;
	}
	
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオ ウィンドウのメッセージの送信先ウィンドウを指定する。
//! @param 		window	送信先ウィンドウ
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSVideoOverlay::SetMessageDrainWindow( HWND window )
{
	HRESULT hr;
	if( FAILED(hr = VideoWindow()->put_MessageDrain((OAHWND)window)) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IVideoWindow::put_MessageDrain.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	表示矩形を設定する
//! @param 		rect	表示矩形
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSVideoOverlay::SetRect( RECT *rect )
{
	if(Shutdown) return NULL;
	HRESULT hr;
	Rect = *rect;
	if( FAILED(hr = VideoWindow()->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top)) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IVideoWindow::SetWindowPosition.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	表示/非表示を設定する
//! @param 		b	表示/非表示
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSVideoOverlay::SetVisible( bool b )
{
	if(Shutdown) return NULL;
	HRESULT hr;
	if (OwnerWindow )
	{
		if( FAILED(hr = VideoWindow()->put_Visible(b?OATRUE:OAFALSE)) )
		{
#ifdef _DEBUG
			OutputDebugString( DShowException(hr).what() );
#endif
			return L"Failed to call IVideoWindow::put_Visible.";
		}
	}
	Visible = b;
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを再生する
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSVideoOverlay::Play()
{
	if( Shutdown ) return NULL;

	HRESULT hr;
	if( FAILED(hr = VideoWindow()->SetWindowPosition( Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top )) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IVideoWindow::SetWindowPosition.";
	}

	if( FAILED(hr = Controller()->Run()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Run.";
	}

	if( FAILED(hr = VideoWindow()->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top)) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IVideoWindow::SetWindowPosition.";
	}

	return NULL;
}
