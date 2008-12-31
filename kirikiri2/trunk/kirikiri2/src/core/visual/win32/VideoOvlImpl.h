//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
	tTVPVideoOverlayMode	Mode;	//!< Mode�̓��I�ȕύX�͏o���Ȃ��Bopen�O�ɃZ�b�g���Ă�������
	bool	Loop;

	class tTVPBaseBitmap	*Bitmap[2];	//!< Layer�`��p�o�b�t�@�pBitmap
	BYTE			*BmpBits[2];

	bool	IsPrepare;			//!< ����[�h���ǂ���

	int		SegLoopStartFrame;	//!< �Z�O�����g���[�v�J�n�t���[��
	int		SegLoopEndFrame;	//!< �Z�O�����g���[�v�I���t���[��

	//! �C�x���g���ݒ肳�ꂽ���A���݃t���[���̕�i��ł������ǂ����B
	//! �C�x���g���ݒ肳��Ă���t���[�����O�Ɍ��݃t���[�����ړ��������A���̃t���O�͉��������B
	bool	IsEventPast;
	int		EventFrame;		//!< �C�x���g�𔭐�������t���[��
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
	void CancelSegmentLoop() { SegLoopStartFrame = -1; SegLoopEndFrame = -1; }
	void SetPeriodEvent( int eventFrame );

	void SetStopFrame( tjs_int f );
	void SetDefaultStopFrame();
	tjs_int GetStopFrame();
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

	tjs_real GetPlayRate();
	void SetPlayRate(tjs_real r);

	tjs_int GetSegmentLoopStartFrame() { return SegLoopStartFrame; }
	tjs_int GetSegmentLoopEndFrame() { return SegLoopEndFrame; }
	tjs_int GetPeriodEventFrame() { return EventFrame; }

	tjs_int GetAudioBalance();
	void SetAudioBalance(tjs_int b);
	tjs_int GetAudioVolume();
	void SetAudioVolume(tjs_int v);

	tjs_uint GetNumberOfAudioStream();
	void SelectAudioStream(tjs_uint n);
	tjs_int GetEnabledAudioStream();
	void DisableAudioStream();

	tjs_uint GetNumberOfVideoStream();
	void SelectVideoStream(tjs_uint n);
	tjs_int GetEnabledVideoStream();
	void SetMixingLayer( tTJSNI_BaseLayer *l );
	void ResetMixingBitmap();

	void SetMixingMovieAlpha( tjs_real a );
	tjs_real GetMixingMovieAlpha();
	void SetMixingMovieBGColor( tjs_uint col );
	tjs_uint GetMixingMovieBGColor();

// End:		Add:	T.Imoto

	tjs_int GetOriginalWidth();
	tjs_int GetOriginalHeight();

	void ResetOverlayParams();
	void SetRectOffset(tjs_int ofsx, tjs_int ofsy);
	void DetachVideoOverlay();

private:
	void __fastcall WndProc(Messages::TMessage &Msg);
		// UtilWindow's window procedure
	void ClearWndProcMessages(); // clear WndProc's message queue

};
//---------------------------------------------------------------------------

#endif
