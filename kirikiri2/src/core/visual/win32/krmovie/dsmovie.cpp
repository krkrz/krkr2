//---------------------------------------------------------------------------
// ( part of KRMOVIE.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.


	Modified by T.Imoto <http://www.kaede-software.com>
*/

#include "dsmovie.h"
#include "CIStream.h"

#ifdef _DEBUG
#include "DShowException.h"
#endif

tTVPDSMovie::tTVPDSMovie()
{
#ifdef _DEBUG
	m_dwROTReg = 0xfedcba98;
	m_RegisteredROT = false;
#endif

	CoInitialize(NULL);
	OwnerWindow = NULL;
	Visible = false;
	Rect.left = 0; Rect.top = 0; Rect.right = 320; Rect.bottom = 240;
	RefCount = 1;
	Shutdown = false;
}
tTVPDSMovie::~tTVPDSMovie()
{
	Shutdown = true;
	ReleaseAll();
	CoUninitialize();
}
//----------------------------------------------------------------------------
//! @brief	  	参照カウンタのインクリメント
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::AddRef()
{
	RefCount++;
}
//----------------------------------------------------------------------------
//! @brief	  	参照カウンタのデクリメント。1ならdelet。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Release()
{
	if(RefCount == 1)
		delete this;
	else
		RefCount--;
}
//----------------------------------------------------------------------------
//! @brief	  	インターフェイスを解放する
//!
//! デバック時、ROTにまだ登録されている場合は、ここで登録を解除する。@n
//! しかし、本来はこのクラスを継承したクラスで事前に登録解除をコールした方がよい
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::ReleaseAll()
{
#ifdef _DEBUG
	if( m_RegisteredROT )	// 登録がまだ解除されていない時はここで解除
		RemoveFromROT( m_dwROTReg );
#endif

	if( m_MediaControl.p != NULL )
	{
		m_MediaControl->Stop();
		m_MediaControl.Release();
	}
	if( m_MediaPosition.p != NULL )
		m_MediaPosition.Release();

	if( m_MediaSeeking.p != NULL )
		m_MediaSeeking.Release();

	if( m_MediaEventEx.p != NULL )
		m_MediaEventEx.Release();

	if( m_BasicVideo.p != NULL )
		m_BasicVideo.Release();

	if( m_GraphBuilder.p != NULL )
		m_GraphBuilder.Release();

	if( m_Proxy )
	{
		delete m_Proxy;
		m_Proxy = NULL;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを再生する
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Play()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Run()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Run.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを停止する
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Stop()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Stop()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Stop.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを一時停止する
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Pause()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Pause()) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaControl::Pause.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	現在のムービー時間を設定する
//! @param 		tick	設定する現在の時間
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetPosition( unsigned __int64 tick )
{
	HRESULT	hr;
	if(Shutdown) return NULL;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	requestTime = (LONGLONG)(tick * 10000);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame , NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		LONGLONG	requestFrame = (LONGLONG)(((tick / 1000.0) / AvgTimePerFrame) + 0.5);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame , NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else
	{
		return L"Not supported time format.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	現在のムービー時間を取得する
//! @param 		tick	現在の時間を返す変数
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetPosition( unsigned __int64 *tick )
{
	if(Shutdown) return NULL;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		return L"Failed to call IMediaSeeking::GetCurrentPosition.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	curTime = (Current + 5000) / 10000; // 一応、四捨五入しておく
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		LONGLONG	curTime = (LONGLONG)(Current * AvgTimePerFrame * 1000.0);
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	現在のムービーの状態を取得する
//! @param 		status	現在の状態を返す変数
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return NULL;
	HRESULT hr;
	OAFilterState state;
	hr = Controller()->GetState(50, &state);
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
//----------------------------------------------------------------------------
//! @brief	  	A sample has been delivered. Copy it to the texture.
//! @param 		evcode	イベントコード
//! @param 		param1	パラメータ1。内容はイベントコードにより異なる。
//! @param 		param2	パラメータ2。内容はイベントコードにより異なる。
//! @param 		got		取得の正否
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetEvent( long *evcode, long *param1, long *param2, bool *got )
{
	if(Shutdown) return NULL;
	HRESULT hr;
	*got = false;
	hr = Event()->GetEvent(evcode, param1, param2, 0);
	if(SUCCEEDED(hr)) *got = true;
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	イベントを解放する
//! 
//! GetEventでイベントを得て、処理した後、このメソッドによってイベントを解放すること
//! @param 		evcode	解放するイベントコード
//! @param 		param1	解放するパラメータ1。内容はイベントコードにより異なる。
//! @param 		param2	解放するパラメータ2。内容はイベントコードにより異なる。
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::FreeEventParams(long evcode, long param1, long param2)
{
	if(Shutdown) return NULL;

	Event()->FreeEventParams(evcode, param1, param2);
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ムービーを最初の位置まで巻き戻す
//! @return		エラーメッセージ
//! @note		IMediaPositionは非推奨のようだが、サンプルでは使用されていたので、
//! 			同じままにしておく。
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::Rewind()
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	if( FAILED(hr = Position()->put_CurrentPosition(0)) )
	{
#ifdef _DEBUG
		OutputDebugString( DShowException(hr).what() );
#endif
		return L"Failed to call IMediaPosition::put_CurrentPosition.";
	}
	return NULL;
}
//! このメソッドによって設定された位置は、指定したフレームと完全に一致するわけではない。
//! フレームは、指定したフレームに最も近いキーフレームの位置に設定される。
const wchar_t* __stdcall tTVPDSMovie::SetFrame( int f )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		LONGLONG	requestTime = (LONGLONG)(AvgTimePerFrame * 10000000.0 * f);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame , NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		LONGLONG	requestFrame = f;
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame , NULL, AM_SEEKING_NoPositioning )) )
		{
			return L"Failed to call IMediaSeeking::SetPositions.";
		}
	}
	else
	{
		return L"Not supported time format.";
	}
	return NULL;
}
const wchar_t* __stdcall tTVPDSMovie::GetFrame( int *f )
{
	if(Shutdown) return NULL;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		return L"Failed to call IMediaSeeking::GetCurrentPosition.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		double	currentTime = Current / 10000000.0;
		*f = (int)(currentTime / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)Current;
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
const wchar_t* __stdcall tTVPDSMovie::GetFPS( double *f )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	REFTIME	AvgTimePerFrame;
	if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
	{
		return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
	}
	*f = 1.0 / AvgTimePerFrame;
	return NULL;
}
const wchar_t* __stdcall tTVPDSMovie::GetNumberOfFrame( int *f )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		return L"Failed to call IMediaSeeking::GetDuration.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		double	totalSec = totalTime / 10000000.0;
		*f = (int)(totalSec / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)totalTime;
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
// milli-secondにしておく？
const wchar_t* __stdcall tTVPDSMovie::GetTotalTime( __int64 *t )
{
	if(Shutdown) return NULL;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		return L"Failed to call IMediaSeeking::GetDuration.";
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		return L"Failed to call IMediaSeeking::GetTimeFormat.";
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		*t = (__int64)(totalTime / 10000.0 );
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			return L"Failed to call IBasicVideo::get_AvgTimePerFrame.";
		}
		// フレームから秒へ、秒からmsecへ
		*t = (__int64)((totalTime * AvgTimePerFrame) * 1000.0 );
	}
	else
	{
		return L"Not supported time format.";
	}

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの画像サイズを取得する
//! @param		width : 幅
//! @param		height : 高さ
//! @return		Always NULL
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetVideoSize( long *width, long *height )
{
	if( width != NULL )
		Video()->get_SourceWidth( width );

	if( height != NULL )
		Video()->get_SourceHeight( height );

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	buffにNULLを設定する。
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::GetFrontBuffer( BYTE **buff )
{
	*buff = NULL;
	return NULL;
}
const wchar_t* __stdcall tTVPDSMovie::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetWindow( HWND window )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetMessageDrainWindow( HWND window )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetRect( RECT *rect )
{
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//! @return		Always NULL.
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSMovie::SetVisible( bool b )
{
	return NULL;
}
#ifdef _DEBUG
//----------------------------------------------------------------------------
//! @brief	  	ROT ( Running Object Table )にグラフを登録する。
//!
//! Running Object Table functions: Used to debug. By registering the graph
//! in the running object table, GraphEdit is able to connect to the running
//! graph. This code should be removed before the application is shipped in
//! order to avoid third parties from spying on your graph.
//! @param		ROTreg : 登録I.D.。なんか、それっぽいのを渡せばいいんでないの。@n
//! 				サンプルではdwROTReg = 0xfedcba98というのを渡している。
//! @return		正否
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::AddToROT( DWORD ROTreg )
{
	IUnknown			*pUnkGraph = m_GraphBuilder;
	IMoniker			*pmk;
	IRunningObjectTable	*pirot;

	if( FAILED(GetRunningObjectTable(0, &pirot)) )
		return E_FAIL;

	WCHAR	wsz[256];
	wsprintfW(wsz, L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pmk);
	if( SUCCEEDED(hr) )
	{
		hr = pirot->Register( 0, pUnkGraph, pmk, &ROTreg );
		m_RegisteredROT = true;
		pmk->Release();
	}
	pirot->Release();
	return hr;
}

//----------------------------------------------------------------------------
//! @brief	  	ROT ( Running Object Table )からグラフの登録を解除する。
//! @param		ROTreg : AddToROTで渡したのと同じ物を渡す。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::RemoveFromROT( DWORD ROTreg )
{
	IRunningObjectTable *pirot;
	if (SUCCEEDED( GetRunningObjectTable(0, &pirot) ))
	{
		pirot->Revoke( ROTreg );
		m_RegisteredROT = false;
		pirot->Release();
	}
}
#endif	// _DEBUG
//----------------------------------------------------------------------------
//! @brief	  	1フレームの平均表示時間を取得します
//! @param		pAvgTimePerFrame : 1フレームの平均表示時間
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame )
{
	return Video()->get_AvgTimePerFrame( pAvgTimePerFrame );
}
const wchar_t* tTVPDSMovie::ParseVideoType( CMediaType &mt, const wchar_t *type )
{
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
	else if (wcsicmp(type, L".avi") == 0)
		mt.subtype = MEDIASUBTYPE_Avi;
	else if (wcsicmp(type, L".mov") == 0)
		mt.subtype = MEDIASUBTYPE_QTMovie;
	else
		return L"Unknown video format extension."; // unknown format
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	MPEG1 用のグラフを手動で構築する
//! @param		pRdr : グラフに参加しているレンダーフィルタ
//! @param		pSrc : グラフに参加しているソースフィルタ
//! @param		useSound : サウンドが使用されるかどうか
//! @return		エラー文字列
//----------------------------------------------------------------------------
const wchar_t* tTVPDSMovie::BuildMPEGGraph( IBaseFilter *pRdr, IBaseFilter *pSrc, bool useSound )
{
	HRESULT	hr;
	// Connect to MPEG 1 splitter filter
	CComPtr<IBaseFilter>	pMPEG1Splitter;	// for MPEG 1 splitter filter
	if( FAILED(hr = pMPEG1Splitter.CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER)) )
		return L"Failed to create MPEG 1 splitter filter object.";
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEG1Splitter, L"MPEG-I Stream Splitter")) )
		return L"Failed to call IFilterGraph::AddFilter.";
	if( FAILED(hr = ConnectFilters( pSrc, pMPEG1Splitter )) )
		return L"Failed to call ConnectFilters.";

	// Connect to MPEG 1 video codec filter
	CComPtr<IBaseFilter>	pMPEGVideoCodec;	// for MPEG 1 video codec filter
	if( FAILED(hr = pMPEGVideoCodec.CoCreateInstance(CLSID_CMpegVideoCodec, NULL, CLSCTX_INPROC_SERVER)) )
		return L"Failed to create MPEG 1 video codec filter object.";
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGVideoCodec, L"MPEG Video Decoder")) )
		return L"Failed to call IFilterGraph::AddFilter.";
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGVideoCodec )) )
		return L"Failed to call ConnectFilters.";

	// Connect to render filter
	if( FAILED(hr = ConnectFilters( pMPEGVideoCodec, pRdr )) )
		return L"Failed to call ConnectFilters.";

	if( useSound )
	{	// Connect to DDS render filter
		CComPtr<IBaseFilter>	pDDSRenderer;	// for sound renderer filter
		if( FAILED(hr = pDDSRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER)) )
			return L"Failed to create sound render filter object.";
		if( FAILED(hr = GraphBuilder()->AddFilter(pDDSRenderer, L"Sound Renderer")) )
			return L"Failed to call IFilterGraph::AddFilter.";
		if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pDDSRenderer ) ) )
			return L"Failed to call ConnectFilters.";
	}
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	2つのフィルターを接続する
//! @param		pFilterUpstream : アップストリームフィルタ
//! @param		pFilterDownstream : ダウンストリームフィルタ
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::ConnectFilters( IBaseFilter* pFilterUpstream, IBaseFilter* pFilterDownstream )
{
	HRESULT			hr = E_FAIL;
	CComPtr<IPin>	pIPinUpstream;
	PIN_INFO		PinInfoUpstream;
	PIN_INFO		PinInfoDownstream;

	// validate passed in filters
	ASSERT(pFilterUpstream);
	ASSERT(pFilterDownstream);

	// grab upstream filter's enumerator
	CComPtr<IEnumPins> pIEnumPinsUpstream;
	if( FAILED(hr = pFilterUpstream->EnumPins(&pIEnumPinsUpstream)) )
		throw L"Failed to call IBaseFilter::EnumPins.";

	// iterate through upstream filter's pins
	while( pIEnumPinsUpstream->Next (1, &pIPinUpstream, 0) == S_OK )
	{
		if( FAILED(hr = pIPinUpstream->QueryPinInfo(&PinInfoUpstream)) )
			throw L"Failed to call IPin::QueryPinInfo.";

		CComPtr<IPin>	 pPinDown;
		pIPinUpstream->ConnectedTo( &pPinDown );

		// bail if pins are connected
		// otherwise check direction and connect
		if( (PINDIR_OUTPUT == PinInfoUpstream.dir) && (pPinDown == NULL) )
		{
			// grab downstream filter's enumerator
			CComPtr<IEnumPins>	pIEnumPinsDownstream;
			hr = pFilterDownstream->EnumPins (&pIEnumPinsDownstream);

			// iterate through downstream filter's pins
			CComPtr<IPin>	pIPinDownstream;
			while( pIEnumPinsDownstream->Next (1, &pIPinDownstream, 0) == S_OK )
			{
				// make sure it is an input pin
				if( SUCCEEDED(hr = pIPinDownstream->QueryPinInfo(&PinInfoDownstream)) )
				{
					CComPtr<IPin>	 pPinUp;
					pIPinDownstream->ConnectedTo( &pPinUp );
					if( (PINDIR_INPUT == PinInfoDownstream.dir) && (pPinUp == NULL) )
					{
						if( SUCCEEDED(m_GraphBuilder->Connect( pIPinUpstream, pIPinDownstream)) )
						{
							PinInfoDownstream.pFilter->Release();
							PinInfoUpstream.pFilter->Release();
							return S_OK;
						}
					}
				}

				PinInfoDownstream.pFilter->Release();
				pIPinDownstream = NULL;
			} // while next downstream filter pin

			//We are now back into the upstream pin loop
		} // if output pin

		pIPinUpstream = NULL;
		PinInfoUpstream.pFilter->Release();
	} // while next upstream filter pin

	return E_FAIL;
}
