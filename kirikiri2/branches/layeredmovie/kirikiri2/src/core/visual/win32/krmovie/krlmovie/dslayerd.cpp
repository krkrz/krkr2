/****************************************************************************/
/*! @file
@brief DirectShowを利用したムービーのレイヤー描画再生

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/25
@note
			2004/08/25	T.Imoto		
*****************************************************************************/


#include "dslayerd.h"
#include "CIStream.h"

#ifdef _DEBUG
#include "DShowException.h"
#endif
#include "BufferRenderer.h"

//----------------------------------------------------------------------------
//! @brief	  	m_BmpBitsにNULLを設定する
//----------------------------------------------------------------------------
tTVPDSLayerVideo::tTVPDSLayerVideo()
{
	m_BmpBits[0] = NULL;
	m_BmpBits[1] = NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	m_BmpBitsにNULLを設定する
//----------------------------------------------------------------------------
tTVPDSLayerVideo::~tTVPDSLayerVideo()
{
	m_BmpBits[0] = NULL;
	m_BmpBits[1] = NULL;
	ReleaseAll();
}
//----------------------------------------------------------------------------
//! @brief	  	フィルタグラフの構築
//! @param 		callbackwin : メッセージを送信するウィンドウ
//! @param 		stream : 読み込み元ストリーム
//! @param 		streamname : ストリームの名前
//! @param 		type : メディアタイプ(拡張子)
//! @param 		size : メディアサイズ
//! @return		エラーメッセージ。NULLの場合エラーなし
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSLayerVideo::BuildGraph( HWND callbackwin, IStream *stream,
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

		// Create the Buffer Renderer object
		CComPtr<IBaseFilter>	pBRender;	// for buffer renderer filter
		TBufferRenderer			*pCBR;
		pCBR = new TBufferRenderer( NAME("Buffer Renderer"), NULL, &hr );
		if( FAILED(hr) )
			throw L"Failed to create buffer renderer object.";
		pBRender = pCBR;

		// add fliter
		if( FAILED(hr = GraphBuilder()->AddFilter( pBRender, L"Buffer Renderer")) )
			throw L"Failed to call IFilterGraph::AddFilter.";
		if( FAILED(hr = GraphBuilder()->AddFilter( m_Reader, L"Stream Reader")) )
			throw L"Failed to call IFilterGraph::AddFilter.";

		if( mt.subtype == MEDIASUBTYPE_Avi || mt.subtype == MEDIASUBTYPE_QTMovie )
		{
			CComPtr<IPin>			pRdrPinIn;
			CComPtr<IPin>			pSrcPinOut;
			if( FAILED(hr = pBRender->FindPin( L"In", &pRdrPinIn )) )
				throw L"Failed to call IBaseFilter::FindPin.";
			pSrcPinOut = m_Reader->GetPin(0);
			if( FAILED(hr = GraphBuilder()->Connect( pSrcPinOut, pRdrPinIn )) )
				throw L"Failed to call IGraphBuilder::Connect.";
	
			CComPtr<IPin>			pSpliterPinIn;
			if( FAILED(hr = pSrcPinOut->ConnectedTo( &pSpliterPinIn )) )
				throw L"Failed to call IPin::ConnectedTo.";
	
			{	// Connect to DDS render filter
				CComPtr<IBaseFilter>	pDDSRenderer;	// for sound renderer filter
				if( FAILED(hr = pDDSRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER)) )
					 throw L"Failed to create sound render filter object.";
				if( FAILED(hr = GraphBuilder()->AddFilter(pDDSRenderer, L"Sound Renderer")) )
					throw L"Failed to call IFilterGraph::AddFilter.";
	
				CComPtr<IBaseFilter>	pSpliter;
				PIN_INFO	pinInfo;
				if( FAILED(hr = pSpliterPinIn->QueryPinInfo( &pinInfo )) )
					throw L"Failed to call IPin::QueryPinInfo.";
				pSpliter = pinInfo.pFilter;
				pinInfo.pFilter->Release();
				// AVI, QTにAudioが含まれていない場合、次の接続は失敗すると思われる。
				if( FAILED(hr = ConnectFilters( pSpliter, pDDSRenderer ) ) )
					throw L"Failed to call ConnectFilters.";
			}
		}
		else
		{
			if( mt.subtype == MEDIASUBTYPE_MPEG1Video )
			{	// Not use audio
				if( (errmsg = BuildMPEGGraph( pBRender, m_Reader, false )) != NULL )
					throw errmsg;
			}
			else
			{
				if( (errmsg = BuildMPEGGraph( pBRender, m_Reader, true )) != NULL )
					throw errmsg;
			}
		}

#if 0	// 吉里吉里のBitmapは上下逆の形式らしいので、再接続は必要ない
		{	// Reconnect buffer render filter
			// get decoder output pin
			CComPtr<IPin>			pDecoderPinOut;
			if( FAILED(hr = pRdrPinIn->ConnectedTo( &pDecoderPinOut )) )
				throw L"Failed to call IPin::ConnectedTo.";

			// get connection media type
			CMediaType	mt;
			if( FAILED(hr = pRdrPinIn->ConnectionMediaType( &mt )) )
				throw L"Failed to call IPin::ConnectionMediaType.";

			// dissconnect pins
			if( FAILED(hr = pDecoderPinOut->Disconnect()) )
				throw L"Failed to call IPin::ConnectionMediaType.";
			if( FAILED(hr = pRdrPinIn->Disconnect()) )
				throw L"Failed to call IPin::ConnectionMediaType.";

			if( IsEqualGUID( mt.FormatType(), FORMAT_VideoInfo) )
			{	// reverse vertical line
				VIDEOINFOHEADER	*pVideoInfo;
				pVideoInfo = reinterpret_cast<VIDEOINFOHEADER*>(mt.Format());
				if( pVideoInfo->bmiHeader.biHeight > 0 )
					pVideoInfo->bmiHeader.biHeight *= -1;
			}

			if( FAILED(hr = GraphBuilder()->ConnectDirect( pDecoderPinOut, pRdrPinIn, &mt )) )
				throw L"Failed to call IGraphBuilder::ConnectDirect.";
		}
#endif

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

		if( FAILED(hr = pBRender->QueryInterface( &m_BuffAccess )) )
			 throw L"Failed to query IRendererBufferAccess.";
		if( FAILED(hr = pBRender->QueryInterface( &m_BuffVideo )) )
			 throw L"Failed to query IRendererBufferVideo.";

//		if( FAILED(hr = MediaSeeking()->SetTimeFormat( &TIME_FORMAT_FRAME )) )
//			throw L"Failed to call IMediaSeeking::SetTimeFormat.";

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

	CoUninitialize();	// ここでこれを呼ぶとまずそうな気がするけど、大丈夫なのかなぁ
	return NULL;
}

//----------------------------------------------------------------------------
//! @brief	  	インターフェイスを解放する
//----------------------------------------------------------------------------
void __stdcall tTVPDSLayerVideo::ReleaseAll()
{
#ifdef _DEBUG
	if( m_RegisteredROT )
		RemoveFromROT( m_dwROTReg );
#endif

	if( m_MediaControl.p != NULL )
	{
		m_MediaControl->Stop();
	}

	if( m_BuffAccess.p )
		m_BuffAccess.Release();

	if( m_BuffVideo.p )
		m_BuffVideo.Release();

	tTVPDSMovie::ReleaseAll();
}
//----------------------------------------------------------------------------
//! @brief	  	描画するバッファを設定する
//! @param		buff1 : バッファ1
//! @param		buff2 : バッファ2
//! @param		size : バッファのサイズ
//! @return		エラー文字列
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSLayerVideo::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
	if( buff1 == NULL || buff2 == NULL )
		return L"SetVideoBuffer Parameter Error";

	m_BmpBits[0] = buff1;
	m_BmpBits[1] = buff2;
	if( FAILED(BufferAccess()->SetFrontBuffer( m_BmpBits[0], &size )) )
		return L"Failed to call IBufferAccess::SetFrontBuffer.";
	if( FAILED(BufferAccess()->SetBackBuffer( m_BmpBits[1], &size )) )
		return L"Failed to call IBufferAccess::SetBackBuffer.";
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	フロントバッファを取得する
//! @param		buff : バッファ
//! @return		エラー文字列
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSLayerVideo::GetFrontBuffer( BYTE **buff )
{
	long		size;
	if( FAILED(BufferAccess()->GetFrontBuffer( buff, &size )) )
		return L"Failed to call IBufferAccess::GetFrontBuffer.";
	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの画像サイズを取得する
//! @param		width : 幅
//! @param		height : 高さ
//! @return		Always NULL
//----------------------------------------------------------------------------
const wchar_t* __stdcall tTVPDSLayerVideo::GetVideoSize( long *width, long *height )
{
	if( width != NULL )
		BufferVideo()->get_VideoWidth( width );

	if( height != NULL )
		BufferVideo()->get_VideoHeight( height );

	return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	1フレームの平均表示時間を取得します
//! @param		pAvgTimePerFrame : 1フレームの平均表示時間
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSLayerVideo::GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame )
{
	return BufferVideo()->get_AvgTimePerFrame( pAvgTimePerFrame );
}
