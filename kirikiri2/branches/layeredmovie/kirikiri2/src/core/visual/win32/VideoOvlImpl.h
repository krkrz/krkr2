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
enum tTVPVideoOverlayMode {
	vomOverlay,		// Overlay
	vomLayerDraw,	// Draw Layer
};
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
//	void PlaySequence();	初めは追加しない
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
#if 0
	void SetTimePosition( tjs_int64 p );
	tjs_int64 GetTimePosition();

	void SetLoop( bool b );
	bool GetLoop() const { return Loop; }

	void SetFrame( tjs_int f );
	tjs_int GetFrame();

	tjs_real GetFPS();
	tjs_int GetNumberOfFrame();
	tjs_int64 GetTotalTime();

	void SetLayer1( tTJSNI_BaseLayer *l );
	tTJSNI_BaseLayer *GetLayer1() { return Layer1; }
	void SetLayer2( tTJSNI_BaseLayer *l );
	tTJSNI_BaseLayer *GetLayer2() { return Layer2; }

	void SetMode( tTVPVideoOverlayMode m );
	tTVPVideoOverlayMode GetMode() { return Mode; }
#endif
// End:		Add:	T.Imoto

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
