//---------------------------------------------------------------------------
// ( part of KRMOVIE.DLL )
// (c)2001-2005, W.Dee <dee@kikyou.info>
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

#include "DShowException.h"

tTVPDSMovie::tTVPDSMovie()
{
	m_dwROTReg = 0xfedcba98;
	m_RegisteredROT = false;

	m_Proxy = NULL;
	m_Reader = NULL;

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
//! @brief	  	参照カウンタのデクリメント。1ならdelete。
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
	if( m_RegisteredROT )	// 登録がまだ解除されていない時はここで解除
		RemoveFromROT( m_dwROTReg );

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
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Play()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Run()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Run.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを停止する
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Stop()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Stop()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Stop.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを一時停止する
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Pause()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Pause()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Pause.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	現在のムービー時間を設定する
//! @param 		tick : 設定する現在の時間
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetPosition( unsigned __int64 tick )
{
	HRESULT	hr;
	if(Shutdown) return;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetPosition).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	requestTime = (LONGLONG)(tick * 10000);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetPosition).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::SetPosition).", hr);
		}
		LONGLONG	requestFrame = (LONGLONG)(((tick / 1000.0) / AvgTimePerFrame) + 0.5);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetPosition).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	現在のムービー時間を取得する
//! @param 		tick : 現在の時間を返す変数
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetPosition( unsigned __int64 *tick )
{
	if(Shutdown) return;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetCurrentPosition (in tTVPDSMovie::GetPosition).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetPosition).", hr);
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
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetPosition).", hr);
		}
		LONGLONG	curTime = (LONGLONG)(Current * AvgTimePerFrame * 1000.0);
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	現在のムービーの状態を取得する
//! @param 		status : 現在の状態を返す変数
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return;
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
}
//----------------------------------------------------------------------------
//! @brief	  	A sample has been delivered. Copy it to the texture.
//! @param 		evcode : イベントコード
//! @param 		param1 : パラメータ1。内容はイベントコードにより異なる。
//! @param 		param2 : パラメータ2。内容はイベントコードにより異なる。
//! @param 		got : 取得の正否
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetEvent( long *evcode, long *param1, long *param2, bool *got )
{
	if(Shutdown) return;
	HRESULT hr;
	*got = false;
	hr = Event()->GetEvent(evcode, param1, param2, 0);
	if(SUCCEEDED(hr)) *got = true;
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	イベントを解放する
//! 
//! GetEventでイベントを得て、処理した後、このメソッドによってイベントを解放すること
//! @param 		evcode : 解放するイベントコード
//! @param 		param1 : 解放するパラメータ1。内容はイベントコードにより異なる。
//! @param 		param2 : 解放するパラメータ2。内容はイベントコードにより異なる。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::FreeEventParams(long evcode, long param1, long param2)
{
	if(Shutdown) return;

	Event()->FreeEventParams(evcode, param1, param2);
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	ムービーを最初の位置まで巻き戻す
//! @note		IMediaPositionは非推奨のようだが、サンプルでは使用されていたので、
//! 			同じままにしておく。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Rewind()
{
	if(Shutdown) return;

	HRESULT	hr;
	if( FAILED(hr = Position()->put_CurrentPosition(0)) )
	{
		ThrowDShowException(L"Failed to call IMediaPosition::put_CurrentPosition(0).", hr);
	}
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	指定されたフレームへ移動する
//! 
//! このメソッドによって設定された位置は、指定したフレームと完全に一致するわけではない。
//! フレームは、指定したフレームに最も近いキーフレームの位置に設定される。
//! @param		f : 移動するフレーム
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetFrame( int f )
{
	if(Shutdown) return;

	HRESULT	hr;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::SetFrame).", hr);
		}
		LONGLONG	requestTime = (LONGLONG)(AvgTimePerFrame * 10000000.0 * f);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetFrame).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		LONGLONG	requestFrame = f;
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetFrame).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	現在のフレームを取得する
//! @param		f : 現在のフレームを入れる変数へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetFrame( int *f )
{
	if(Shutdown) return;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetCurrentPosition (in tTVPDSMovie::GetFrame).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetFrame).", hr);
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
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}

//----------------------------------------------------------------------------
//! @brief	  	指定されたフレームで再生を停止させる
//! 
//! このメソッドによって設定された位置は、指定したフレームと完全に一致するわけではない。
//! フレームは、指定したフレームに最も近いキーフレームの位置に設定される。
//! @param		f : 再生を停止させるフレーム
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetStopFrame( int f )
{
	if(Shutdown) return;

	HRESULT	hr;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetStopFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::SetStopFrame).", hr);
		}
		LONGLONG	requestTime = (LONGLONG)(AvgTimePerFrame * 10000000.0 * f);
		if( FAILED(hr = MediaSeeking()->SetPositions( NULL, AM_SEEKING_NoPositioning, &requestTime, AM_SEEKING_AbsolutePositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetStopFrame).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		LONGLONG	requestFrame = f;
		if( FAILED(hr = MediaSeeking()->SetPositions( NULL, AM_SEEKING_NoPositioning, &requestFrame, AM_SEEKING_AbsolutePositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetStopFrame).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	現在の再生が停止するフレームを取得する
//! @param		f : 現在の再生が停止するフレームを入れる変数へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetStopFrame( int *f )
{
	if(Shutdown) return;

	HRESULT		hr;
	LONGLONG	Stop;
	if( FAILED(hr = MediaSeeking()->GetStopPosition( &Stop ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetStopPosition (in tTVPDSMovie::GetStopFrame).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetStopFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetStopFrame).", hr);
		}
		double	stopTime = Stop / 10000000.0;
		*f = (int)(stopTime / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)Stop;
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	再生を停止するフレームを初期状態に戻す。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetDefaultStopFrame()
{
	int		numOfFrame;
	GetNumberOfFrame( &numOfFrame );
	SetStopFrame( numOfFrame );
}
//----------------------------------------------------------------------------
//! @brief	  	FPSを取得する
//! @param		f : FPSを入れる変数へのポインタ
//! @return		エラーメッセージ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetFPS( double *f )
{
	if(Shutdown) return;

	HRESULT	hr;
	REFTIME	AvgTimePerFrame;
	if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
	{
		ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetFPS).", hr);
	}
	*f = 1.0 / AvgTimePerFrame;
}
//----------------------------------------------------------------------------
//! @brief	  	全フレーム数を取得する
//! @param		f : 全フレーム数を入れる変数へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetNumberOfFrame( int *f )
{
	if(Shutdown) return;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetDuration (in tTVPDSMovie::GetNumberOfFrame).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetNumberOfFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetNumberOfFrame).", hr);
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
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ムービーの長さ(msec)を取得する
//! @param		f : ムービーの長さを入れる変数へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetTotalTime( __int64 *t )
{
	if(Shutdown) return;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetDuration (in tTVPDSMovie::GetTotalTime).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetTotalTime).", hr);
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
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetTotalTime).", hr);
		}
		// フレームから秒へ、秒からmsecへ
		*t = (__int64)((totalTime * AvgTimePerFrame) * 1000.0 );
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの画像サイズを取得する
//! @param		width : 幅
//! @param		height : 高さ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetVideoSize( long *width, long *height )
{
	if( width != NULL )
		Video()->get_SourceWidth( width );

	if( height != NULL )
		Video()->get_SourceHeight( height );
}
//----------------------------------------------------------------------------
//! @brief	  	buffにNULLを設定する。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetFrontBuffer( BYTE **buff )
{
	*buff = NULL;
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetWindow( HWND window )
{
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetMessageDrainWindow( HWND window )
{
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetRect( RECT *rect )
{
}
//----------------------------------------------------------------------------
//! @brief	  	何もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetVisible( bool b )
{
}
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
//----------------------------------------------------------------------------
//! @brief	  	1フレームの平均表示時間を取得します
//! @param		pAvgTimePerFrame : 1フレームの平均表示時間
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame )
{
	return Video()->get_AvgTimePerFrame( pAvgTimePerFrame );
}
//----------------------------------------------------------------------------
//! @brief	  	拡張子からムービーのタイプを判別します
//! @param		mt : メディアタイプを返す変数への参照
//! @param		type : ムービーファイルの拡張子
//----------------------------------------------------------------------------
void tTVPDSMovie::ParseVideoType( CMediaType &mt, const wchar_t *type )
{
	// note: audio-less mpeg stream must have an extension of
	// ".mpv" .
	if      (wcsicmp(type, L".mpg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpeg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpv") == 0) 
//		mt.subtype = MEDIASUBTYPE_MPEG1Video;
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".dat") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1VideoCD;
	else if (wcsicmp(type, L".avi") == 0)
		mt.subtype = MEDIASUBTYPE_Avi;
	else if (wcsicmp(type, L".mov") == 0)
		mt.subtype = MEDIASUBTYPE_QTMovie;
	else if (wcsicmp(type, L".mp4") == 0)
		mt.subtype = MEDIASUBTYPE_QTMovie;
	else
		TVPThrowExceptionMessage(L"Unknown video format extension."); // unknown format
}

//----------------------------------------------------------------------------
//! @brief	  	メディアタイプの開放
//!				IEnumMediaTypesで取得したAM_MEDIA_TYPEは、このメソッドで削除すること
//! @param		pmt : IEnumMediaTypesで取得したAM_MEDIA_TYPE
//----------------------------------------------------------------------------
void tTVPDSMovie::UtilDeleteMediaType( AM_MEDIA_TYPE *pmt )
{
	// Allow NULL pointers for coding simplicity
	if( pmt == NULL )
		return;

	// Free media type's format data
	if (pmt->cbFormat != 0) 
	{
		CoTaskMemFree((PVOID)pmt->pbFormat);

		// Strictly unnecessary but tidier
		pmt->cbFormat = 0;
		pmt->pbFormat = NULL;
	}

	// Release interface
	if (pmt->pUnk != NULL) 
	{
		pmt->pUnk->Release();
		pmt->pUnk = NULL;
	}

	// Free media type
	CoTaskMemFree((PVOID)pmt);
}
//----------------------------------------------------------------------------
//! @brief	  	ピンに接続可能なメディアタイプをデバッグ出力に吐く
//! @param		pPin : 出力対象となるピン
//----------------------------------------------------------------------------
void tTVPDSMovie::DebugOutputPinMediaType( IPin *pPin )
{
	if( pPin == NULL ) return;
	CComPtr< IEnumMediaTypes > pMediaEnum;
	pPin->EnumMediaTypes(&pMediaEnum);
	if( pMediaEnum )
	{
		ULONG Fetched = 0;
		AM_MEDIA_TYPE *pMediaType;
		while( pMediaEnum->Next(1, &pMediaType, &Fetched) == S_OK )
		{
			if(Fetched)
			{
				if( pMediaType->majortype == MEDIATYPE_Stream )
					OutputDebugString("  MEDIATYPE_Stream : ");
				else if( pMediaType->majortype == MEDIATYPE_Audio )
					OutputDebugString("  MEDIATYPE_Audio : ");
				else if( pMediaType->majortype == MEDIATYPE_Video )
					OutputDebugString("  MEDIATYPE_Video : ");
				else
					OutputDebugString("  unknown MEDIATYPE : ");

				if( pMediaType->subtype == MEDIASUBTYPE_MPEG1System )
					OutputDebugString("  MEDIASUBTYPE_MPEG1System : ");
				else if( pMediaType->subtype == MEDIASUBTYPE_MPEG1VideoCD )
					OutputDebugString("  MEDIASUBTYPE_MPEG1VideoCD : ");
				else if( pMediaType->subtype == MEDIASUBTYPE_MPEG1Packet )
					OutputDebugString("  MEDIASUBTYPE_MPEG1Packet : ");
				else if( pMediaType->subtype == MEDIASUBTYPE_MPEG1Payload )
					OutputDebugString("  MEDIASUBTYPE_MPEG1Payload : ");
				else if( pMediaType->subtype == MEDIASUBTYPE_MPEG1Video )
					OutputDebugString("  MEDIASUBTYPE_MPEG1Video : ");
				else if( pMediaType->subtype == MEDIASUBTYPE_MPEG1Audio )
					OutputDebugString("  MEDIASUBTYPE_MPEG1Audio : ");
				else
					OutputDebugString("  unknown MEDIASUBTYPE\n");

				if( pMediaType->formattype == FORMAT_DvInfo )
					OutputDebugString("  FORMAT_DvInfo\n");
				else if( pMediaType->formattype == FORMAT_MPEGVideo )
					OutputDebugString("  FORMAT_MPEGVideo\n");
				else if( pMediaType->formattype == FORMAT_MPEG2Video )
					OutputDebugString("  FORMAT_MPEG2Video\n");
				else if( pMediaType->formattype == FORMAT_VideoInfo )
					OutputDebugString("  FORMAT_VideoInfo\n");
				else if( pMediaType->formattype == FORMAT_VideoInfo2 )
					OutputDebugString("  FORMAT_VideoInfo2\n");
				else if( pMediaType->formattype == FORMAT_WaveFormatEx )
					OutputDebugString("  FORMAT_WaveFormatEx\n");
				else if( pMediaType->formattype == GUID_NULL )
					OutputDebugString("  GUID_NULL\n");
				else
					OutputDebugString("  unknown FORMAT\n");

				UtilDeleteMediaType(pMediaType);
			}
		}
	}
}
//----------------------------------------------------------------------------
//! @brief	  	MPEG1 用のグラフを手動で構築する
//! @param		pRdr : グラフに参加しているレンダーフィルタ
//! @param		pSrc : グラフに参加しているソースフィルタ
//! @param		useSound : サウンドが使用されるかどうか
//----------------------------------------------------------------------------
void tTVPDSMovie::BuildMPEGGraph( IBaseFilter *pRdr, IBaseFilter *pSrc )
{
	HRESULT	hr;

	// Connect to MPEG 1 splitter filter
	CComPtr<IBaseFilter>	pMPEG1Splitter;	// for MPEG 1 splitter filter

	if( FAILED(hr = pMPEG1Splitter.CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create MPEG 1 splitter filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEG1Splitter, L"MPEG-I Stream Splitter")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pMPEG1Splitter, L\"MPEG-I Stream Splitter\").", hr);
	if( FAILED(hr = ConnectFilters( pSrc, pMPEG1Splitter )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pSrc, pMPEG1Splitter ).", hr);

	// Connect to MPEG 1 video codec filter
	CComPtr<IBaseFilter>	pMPEGVideoCodec;	// for MPEG 1 video codec filter
	if( FAILED(hr = pMPEGVideoCodec.CoCreateInstance(CLSID_CMpegVideoCodec, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create MPEG 1 video codec filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGVideoCodec, L"MPEG Video Decoder")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pMPEGVideoCodec, L\"MPEG Video Decoder\").", hr);
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGVideoCodec )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pMPEG1Splitter, pMPEGVideoCodec ).", hr);

	// Connect to render filter
	if( FAILED(hr = ConnectFilters( pMPEGVideoCodec, pRdr )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pMPEGVideoCodec, pRdr ).", hr);

	// Connect to MPEG audio codec filter
	CComPtr<IBaseFilter>	pMPEGAudioCodec;	// for MPEG audio codec filter
	if( FAILED(hr = pMPEGAudioCodec.CoCreateInstance(CLSID_CMpegAudioCodec, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create MPEG audio codec filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGAudioCodec, L"MPEG Audio Decoder")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pMPEGAudioCodec, L\"MPEG Audio Decoder\").", hr);
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGAudioCodec )) )
	{	// not have Audio.
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pMPEGAudioCodec)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pMPEGAudioCodec).", hr);
		return;
	}

	// Connect to DDS render filter
	CComPtr<IBaseFilter>	pDDSRenderer;	// for sound renderer filter
	if( FAILED(hr = pDDSRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create sound render filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pDDSRenderer, L"Sound Renderer")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pDDSRenderer, L\"Sound Renderer\").", hr);
	if( FAILED(hr = ConnectFilters( pMPEGAudioCodec, pDDSRenderer ) ) )
	{
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pMPEGAudioCodec)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pMPEGAudioCodec).", hr);
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pDDSRenderer)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pDDSRenderer).", hr);
	}

	return;
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
#if _DEBUG
	char debug[256];
#endif

	// validate passed in filters
	ASSERT(pFilterUpstream);
	ASSERT(pFilterDownstream);

	// grab upstream filter's enumerator
	CComPtr<IEnumPins> pIEnumPinsUpstream;
	if( FAILED(hr = pFilterUpstream->EnumPins(&pIEnumPinsUpstream)) )
		ThrowDShowException(L"Failed to call pFilterUpstream->EnumPins(&pIEnumPinsUpstream).", hr);

	// iterate through upstream filter's pins
	while( pIEnumPinsUpstream->Next (1, &pIPinUpstream, 0) == S_OK )
	{
		if( FAILED(hr = pIPinUpstream->QueryPinInfo(&PinInfoUpstream)) )
			ThrowDShowException(L"Failed to call pIPinUpstream->QueryPinInfo(&PinInfoUpstream).", hr);
#if _DEBUG
		sprintf(debug, "upstream: %ls\n", PinInfoUpstream.achName);
		OutputDebugString(debug);

		DebugOutputPinMediaType(pIPinUpstream);
#endif

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
#if _DEBUG
					sprintf(debug, "    downstream: %ls\n", PinInfoDownstream.achName);
					OutputDebugString(debug);

					DebugOutputPinMediaType(pIPinDownstream);
#endif
					CComPtr<IPin>	 pPinUp;
					pIPinDownstream->ConnectedTo( &pPinUp );
					if( (PINDIR_INPUT == PinInfoDownstream.dir) && (pPinUp == NULL) )
					{
						if( SUCCEEDED(hr = m_GraphBuilder->ConnectDirect( pIPinUpstream, pIPinDownstream, NULL)) )
						{
							PinInfoDownstream.pFilter->Release();
							PinInfoUpstream.pFilter->Release();
							return S_OK;
						}
#if _DEBUG
						else
							OutputDebugString( DShowException(hr).what() );
#endif
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
