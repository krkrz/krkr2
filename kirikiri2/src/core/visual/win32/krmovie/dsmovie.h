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

#ifdef _DEBUG
	DWORD		m_dwROTReg;	//!< 値はかぶらないように適切な物にすること
	bool		m_RegisteredROT;
#endif

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

	HRESULT __stdcall AddToROT( DWORD ROTreg );
	void __stdcall RemoveFromROT( DWORD ROTreg );

#if 0
// LayerDrawでのみ必要？
	CComPtr<IPin>			m_SourceOutPin;
	CComPtr<IPin>			m_SpliterInPin;
#endif


public:
	tTVPDSMovie();
	virtual ~tTVPDSMovie();

	virtual const wchar_t* __stdcall BuildGraph( HWND callbackwin, IStream *stream,
	const wchar_t * streamname, const wchar_t *type, unsigned __int64 size ) = 0;

	virtual void __stdcall AddRef();
	virtual void __stdcall Release();

	virtual void __stdcall ReleaseAll();

	virtual const wchar_t* __stdcall SetWindow(HWND window);
	virtual const wchar_t* __stdcall SetMessageDrainWindow(HWND window);
	virtual const wchar_t* __stdcall SetRect(RECT *rect);
	virtual const wchar_t* __stdcall SetVisible(bool b);

	virtual const wchar_t* __stdcall Play();
	virtual const wchar_t* __stdcall Stop();
	virtual const wchar_t* __stdcall Pause();
	virtual const wchar_t* __stdcall Rewind();

	virtual const wchar_t* __stdcall SetPosition(unsigned __int64 tick);
	virtual const wchar_t* __stdcall GetPosition(unsigned __int64 *tick);
	virtual const wchar_t* __stdcall GetStatus(tTVPVideoStatus *status);
	virtual const wchar_t* __stdcall GetEvent(long *evcode, long *param1, long *param2, bool *got);
	virtual const wchar_t* __stdcall FreeEventParams(long evcode, long param1, long param2);

	virtual const wchar_t* __stdcall SetFrame( int f );
	virtual const wchar_t* __stdcall GetFrame( int *f );
	virtual const wchar_t* __stdcall GetFPS( double *f );
	virtual const wchar_t* __stdcall GetNumberOfFrame( int *f );
	virtual const wchar_t* __stdcall GetTotalTime( __int64 *t );

	virtual const wchar_t* __stdcall GetVideoSize( long *width, long *height );
	virtual const wchar_t* __stdcall GetFrontBuffer( BYTE **buff );
	virtual const wchar_t* __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size );
};

#endif
