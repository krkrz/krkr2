//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Video Overlay support implementation
//---------------------------------------------------------------------------
#ifndef VideoOvlImplH
#define VideoOvlImplH
//---------------------------------------------------------------------------
#include "tjsNative.h"
#include "WindowIntf.h"

#include "VideoOvlIntf.h"
#include "StorageIntf.h"
#include "UtilStreams.h"

// Start:	Add:	T.Imoto
#include "voMode.h"
// End:		Add:	T.Imoto
//---------------------------------------------------------------------------
// tTJSNI_VideoOverlay : VideoOverlay Native Instance
//---------------------------------------------------------------------------
class iTVPVideoOverlay;
class tTJSNI_VideoOverlay : public tTJSNI_BaseVideoOverlay
{
	typedef tTJSNI_BaseVideoOverlay inherited;

	iTVPVideoOverlay *VideoOverlay;

	tTVPRect Rect;
	bool Visible;

	HWND OwnerWindow;

	HWND UtilWindow; // window which receives messages from video overlay object

	tTVPLocalTempStorageHolder *LocalTempStorageHolder;
// Start:	Add:	T.Imoto
	class tTJSNI_BaseLayer	*Layer1;
	class tTJSNI_BaseLayer	*Layer2;
	tTVPVideoOverlayMode	Mode;	//!< Modeの動的な変更は出来ない。open前にセットしておくこと
	bool	Loop;

	class tTVPBaseBitmap	*Bitmap[2];	//!< Layer描画用バッファ用Bitmap
	BYTE			*BmpBits[2];

	bool	IsPrepare;			//!< 準備モードかどうか

	int		SegLoopStartFrame;	//!< セグメントループ開始フレーム
	int		SegLoopEndFrame;	//!< セグメントループ終了フレーム

	//! イベントが設定された時、現在フレームの方が進んでいたかどうか。
	//! イベントが設定されているフレームより前に現在フレームが移動した時、このフラグは解除される。
	bool	IsEventPast;
	int		EventFrame;		//!< イベントを発生させるフレーム
// End:		Add:	T.Imoto

public:
	tTJSNI_VideoOverlay();
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();


public:
	void Open(const ttstr &name);
	void Close();
	void Shutdown();
	void Disconnect(); // tTJSNI_BaseVideoOverlay::Disconnect override

	void Play();
	void Stop();
// Start:	Add:	T.Imoto
	void Pause();
	void Rewind();
	void Prepare();

	void SetSegmentLoop( int comeFrame, int goFrame );
	void SetPeriodEvent( int eventFrame );
// End:		Add:	T.Imoto

public:
	void SetRectangleToVideoOverlay();

	void SetPosition(tjs_int left, tjs_int top);
	void SetSize(tjs_int width, tjs_int height);
	void SetBounds(const tTVPRect & rect);

	void SetLeft(tjs_int l);
	tjs_int GetLeft() const { return Rect.left; }
	void SetTop(tjs_int t);
	tjs_int GetTop() const { return Rect.top; }
	void SetWidth(tjs_int w);
	tjs_int GetWidth() const { return Rect.get_width(); }
	void SetHeight(tjs_int h);
	tjs_int GetHeight() const { return Rect.get_height(); }

	void SetVisible(bool b);
	bool GetVisible() const { return Visible; }

// Start:	Add:	T.Imoto
	void SetTimePosition( tjs_uint64 p );
	tjs_uint64 GetTimePosition();

	void SetFrame( tjs_int f );
	tjs_int GetFrame();

	tjs_real GetFPS();
	tjs_int GetNumberOfFrame();
	tjs_int64 GetTotalTime();

	void SetLoop( bool b );
	bool GetLoop() const { return Loop; }

	void SetLayer1( tTJSNI_BaseLayer *l );
	tTJSNI_BaseLayer *GetLayer1() { return Layer1; }
	void SetLayer2( tTJSNI_BaseLayer *l );
	tTJSNI_BaseLayer *GetLayer2() { return Layer2; }

	void SetMode( tTVPVideoOverlayMode m );
	tTVPVideoOverlayMode GetMode() { return Mode; }
#if 0
	SetSegmentLoop( int backFrame, int jumpFrame );
	CancelSegmentLoop();
	SetPeriodEvent(指定フレームでのイベント);
	CancelPeriodEvent(指定フレームでのイベント解除);
#endif
// End:		Add:	T.Imoto

	tjs_int GetOriginalWidth();
	tjs_int GetOriginalHeight();

	void SetWindowHandle(HWND wnd);
	void SetMessageDrainWindow(HWND wnd);
	void SetRectOffset(tjs_int ofsx, tjs_int ofsy);

private:
	void __fastcall WndProc(Messages::TMessage &Msg);
		// UtilWindow's window procedure
	void ClearWndProcMessages(); // clear WndProc's message queue

};
//---------------------------------------------------------------------------

#endif
