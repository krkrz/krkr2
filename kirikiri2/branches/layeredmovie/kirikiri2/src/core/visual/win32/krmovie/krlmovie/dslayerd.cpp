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
#include "IBufferRenderer.h"

tTVPDSLayerVideo::tTVPDSLayerVideo()
{
//	m_Bitmap[0] = NULL;
//	m_Bitmap[1] = NULL;

	m_BmpBits[0] = NULL;
	m_BmpBits[1] = NULL;

	m_VideoWidth = 0;
	m_VideoHeight = 0;
}
tTVPDSLayerVideo::~tTVPDSLayerVideo()
{
//	if( m_Bitmap[0] != NULL )
//		delete m_Bitmap[0];

//	if( m_Bitmap[1] != NULL )
//		delete m_Bitmap[1];

	m_BmpBits[0] = NULL;
	m_BmpBits[1] = NULL;
}

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
			throw L"Unknown video format extension."; // unknown format


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
		if( FAILED(hr = pBRender.CoCreateInstance(CLSID_BufferRenderer, NULL, CLSCTX_INPROC_SERVER)) )
			 throw L"Failed to create buffer render filter object.";

		// add fliter
		if( FAILED(hr = GraphBuilder()->AddFilter( pBRender, NULL)) )
			throw L"Failed to call IFilterGraph::AddFilter.";
		if( FAILED(hr = GraphBuilder()->AddFilter( m_Reader, NULL)) )
			throw L"Failed to call IFilterGraph::AddFilter.";

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
			if( FAILED(hr = ConnectFilters( pSpliter, pDDSRenderer ) ) )
				throw L"Failed to call ConnectFilters.";
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
	
		if( FAILED(hr = Video()->get_SourceHeight( &m_VideoHeight )) )
			throw L"Failed to call IBasicVideo::get_SourceHeight.";
		if( FAILED(hr = Video()->get_SourceWidth( &m_VideoWidth )) )
			throw L"Failed to call IBasicVideo::get_SourceWidth.";
//		AllocateVideoBuffer( m_VideoWidth, m_VideoHeight );

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

HRESULT tTVPDSLayerVideo::ConnectFilters( IBaseFilter* pFilterUpstream, IBaseFilter* pFilterDownstream )
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

	tTVPDSMovie::ReleaseAll();
}

const wchar_t* __stdcall tTVPDSLayerVideo::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
#if 0
	long width, height;
	long			size;
	size = width * height * 4;
	m_Bitmap[0] = new tTVPBaseBitmap( width, height, 32 );
	m_Bitmap[1] = new tTVPBaseBitmap( width, height, 32 );

	m_BmpBits[0] = m_Bitmap[0]->GetBitmap()->GetScanLine( m_Bitmap[0]->GetBitmap()->GetHeight()-1 );	// 先頭のポインタを得る この値は別に保持していた方がよいかも。
	m_BmpBits[1] = m_Bitmap[1]->GetBitmap()->GetScanLine( m_Bitmap[1]->GetBitmap()->GetHeight()-1 );
#endif

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

const wchar_t* __stdcall tTVPDSLayerVideo::GetFrontBuffer( BYTE **buff )
{
	long		size;
	if( FAILED(BufferAccess()->GetFrontBuffer( buff, &size )) )
		return L"Failed to call IBufferAccess::GetFrontBuffer.";
	return NULL;
}
const wchar_t* __stdcall tTVPDSLayerVideo::GetVideoSize( long *width, long *height )
{
	if( width )
		*width = m_VideoWidth;
	if( height )
		*height = m_VideoHeight;
	return NULL;
}
#if 0
const wchar_t* __stdcall tTVPDSLayerVideo::Update( class tTJSNI_BaseLayer *l1, class tTJSNI_BaseLayer *l2 )
{
	long		size;
	BYTE		*buff;

	if( l1 == NULL && l2 == NULL )	// nothing to do.
		return NULL;

	if( l1 != NULL )
	{
		if( l1->GetImageWidth() != m_VideoWidth || l1->GetImageHeight() != m_VideoHeight )
			l1->SetImageSize( m_VideoWidth, m_VideoHeight );
	}
	if( l2 != NULL )
	{
		if( l2->GetImageWidth() != m_VideoWidth || l2->GetImageHeight() != m_VideoHeight )
			l2->SetImageSize( m_VideoWidth, m_VideoHeight );
	}

	BufferAccess()->GetFrontBuffer( &buff, &size )
	if( buff == m_BmpBits[0] )
	{
		if( l1 )
			l1->AssignMainImage( Bitmap[0] );
		if( l2 )
			l2->AssignMainImage( Bitmap[0] );
	}
	else	// 0じゃなかったら、1とみなす。
	{
		if( l1 )
			l1->AssignMainImage( Bitmap[1] );
		if( l2 )
			l2->AssignMainImage( Bitmap[1] );
	}
	return NULL;
}
#endif