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

#ifndef __DSMOVIE_H_
#define __DSMOVIE_H_

#include <assert.h>
#include <atlbase.h>
#include <streams.h>
#include "../krmovie.h"

class CIStreamProxy;
class CIStreamReader;
//----------------------------------------------------------------------------
//! @brief DirectShowクラス
//!
//! レイヤー描画、オーバーレイの基底クラス
//----------------------------------------------------------------------------
class tTVPDSMovie : public iTVPVideoOverlay
{
protected:
	ULONG		RefCount;
	HWND		OwnerWindow;
	bool		Visible;
	bool		Shutdown;
	RECT		Rect;

	DWORD		m_dwROTReg;	//!< 値はかぶらないように適切な物にすること
	bool		m_RegisteredROT;

	CIStreamProxy			*m_Proxy;
	CIStreamReader			*m_Reader;

	CComPtr<IGraphBuilder>	m_GraphBuilder;		//!< Graph Builder
	CComPtr<IMediaControl>	m_MediaControl;		//!< Media Control
	CComPtr<IMediaPosition>	m_MediaPosition;	//!< Media Postion
	CComPtr<IMediaSeeking >	m_MediaSeeking;		//!< Media Seeking
	CComPtr<IMediaEventEx>	m_MediaEventEx;		//!< Media Event
	CComPtr<IBasicVideo>	m_BasicVideo;		//!< Basic Video

	//----------------------------------------------------------------------------
	//! @brief	  	IMediaSeekingを取得する
	//! @return		IMediaSeekingインターフェイス
	//----------------------------------------------------------------------------
	IMediaSeeking *MediaSeeking()
	{
		assert( m_MediaSeeking.p );
		return m_MediaSeeking;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IMediaPositionを取得する
	//! @return		IMediaPositionインターフェイス
	//----------------------------------------------------------------------------
	IMediaPosition *Position()
	{
		assert( m_MediaPosition.p );
		return m_MediaPosition;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IMediaControlを取得する
	//! @return		IMediaControlインターフェイス
	//----------------------------------------------------------------------------
	IMediaControl *Controller()
	{ 
		assert( m_MediaControl.p );
		return m_MediaControl;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IMediaEventExを取得する
	//! @return		IMediaEventExインターフェイス
	//----------------------------------------------------------------------------
	IMediaEventEx *Event()
	{
		assert( m_MediaEventEx.p );
		return m_MediaEventEx;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IGraphBuilderを取得する
	//! @return		IGraphBuilderインターフェイス
	//----------------------------------------------------------------------------
	IGraphBuilder *GraphBuilder()
	{
		assert( m_GraphBuilder.p );
		return m_GraphBuilder;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IBasicVideoを取得する
	//! @return		IBasicVideoインターフェイス
	//----------------------------------------------------------------------------
	IBasicVideo *Video()
	{
		assert( m_BasicVideo.p );
		return m_BasicVideo;
	}

	HRESULT ConnectFilters( IBaseFilter* pFilterUpstream, IBaseFilter* pFilterDownstream );
	void BuildMPEGGraph( IBaseFilter *pRdr, IBaseFilter *pSrc );
	void ParseVideoType( CMediaType &mt, const wchar_t *type );

	HRESULT __stdcall AddToROT( DWORD ROTreg );
	void __stdcall RemoveFromROT( DWORD ROTreg );

public:
	tTVPDSMovie();
	virtual ~tTVPDSMovie();

	virtual void __stdcall BuildGraph( HWND callbackwin, IStream *stream,
		const wchar_t * streamname, const wchar_t *type, unsigned __int64 size ) = 0;

	virtual void __stdcall AddRef();
	virtual void __stdcall Release();

	virtual void __stdcall ReleaseAll();

	virtual void __stdcall SetWindow(HWND window);
	virtual void __stdcall SetMessageDrainWindow(HWND window);
	virtual void __stdcall SetRect(RECT *rect);
	virtual void __stdcall SetVisible(bool b);

	virtual void __stdcall Play();
	virtual void __stdcall Stop();
	virtual void __stdcall Pause();
	virtual void __stdcall Rewind();

	virtual void __stdcall SetPosition(unsigned __int64 tick);
	virtual void __stdcall GetPosition(unsigned __int64 *tick);
	virtual void __stdcall GetStatus(tTVPVideoStatus *status);
	virtual void __stdcall GetEvent(long *evcode, long *param1, long *param2, bool *got);
	virtual void __stdcall FreeEventParams(long evcode, long param1, long param2);

	virtual void __stdcall SetFrame( int f );
	virtual void __stdcall GetFrame( int *f );
	virtual void __stdcall GetFPS( double *f );
	virtual void __stdcall GetNumberOfFrame( int *f );
	virtual void __stdcall GetTotalTime( __int64 *t );

	virtual void __stdcall GetFrontBuffer( BYTE **buff );
	virtual void __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size );

	virtual void __stdcall GetVideoSize( long *width, long *height );
	virtual HRESULT __stdcall GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame );
};

#endif
