//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// krmovie.dll ( kirikiri movie playback support DLL ) interface
//---------------------------------------------------------------------------


#ifndef __KRMOVIE_H__
#define __KRMOVIE_H__

#define TVP_KRMOVIE_VER   0x00010007


//---------------------------------------------------------------------------
enum tTVPVideoStatus { vsStopped, vsPlaying, vsPaused, vsProcessing };
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// iTVPVideoOverlay
//---------------------------------------------------------------------------
class iTVPVideoOverlay // this is not a COM object
{
public:
	virtual void __stdcall AddRef() = 0;
	virtual void __stdcall Release() = 0;

	virtual void __stdcall SetWindow(HWND window) = 0;
	virtual void __stdcall SetMessageDrainWindow(HWND window) = 0;
	virtual void __stdcall SetRect(RECT *rect) = 0;
	virtual void __stdcall SetVisible(bool b) = 0;
	virtual void __stdcall Play() = 0;
	virtual void __stdcall Stop() = 0;
	virtual void __stdcall Pause() = 0;
	virtual void __stdcall SetPosition(unsigned __int64 tick) = 0;
	virtual void __stdcall GetPosition(unsigned __int64 *tick) = 0;
	virtual void __stdcall GetStatus(tTVPVideoStatus *status) = 0;
	virtual void __stdcall GetEvent(long *evcode, long *param1,
			long *param2, bool *got) = 0;

// Start:	Add:	T.Imoto
	virtual void __stdcall FreeEventParams(long evcode, long param1, long param2) = 0;

	virtual void __stdcall Rewind() = 0;
	virtual void __stdcall SetFrame( int f ) = 0;
	virtual void __stdcall GetFrame( int *f ) = 0;
	virtual void __stdcall GetFPS( double *f ) = 0;
	virtual void __stdcall GetNumberOfFrame( int *f ) = 0;
	virtual void __stdcall GetTotalTime( __int64 *t ) = 0;
	
	virtual void __stdcall GetVideoSize( long *width, long *height ) = 0;
	virtual void __stdcall GetFrontBuffer( BYTE **buff ) = 0;
	virtual void __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size ) = 0;

	virtual void __stdcall SetStopFrame( int frame ) = 0;
	virtual void __stdcall GetStopFrame( int *frame ) = 0;
	virtual void __stdcall SetDefaultStopFrame() = 0;
// End:	Add:	T.Imoto
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
typedef void (__stdcall *tGetAPIVersion)(DWORD *version);
typedef void  (__stdcall *tGetVideoOverlayObject)(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define WM_GRAPHNOTIFY  (WM_USER+15)
#define WM_CALLBACKCMD  (WM_USER+16)
// Start:	Add:	T.Imoto
#define EC_UPDATE		(EC_USER+1)
// End:		Add:	T.Imoto
//---------------------------------------------------------------------------


#endif


