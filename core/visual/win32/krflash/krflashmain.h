//---------------------------------------------------------------------------
// krflashmain.h ( part of KRFLASH.DLL )
// (c)2001-2005, W.Dee <dee@kikyou.info> and contributors
//---------------------------------------------------------------------------
#ifndef krflashmainh
#define krflashmainh

#include "..\krmovie.h"

//---------------------------------------------------------------------------
// tTVPFlashOverlay
//---------------------------------------------------------------------------
class TFlashContainerForm;
class tTVPFlashOverlay : public iTVPVideoOverlay
{
	ULONG RefCount;
	HWND OwnerWindow;
	HWND CallbackWindow;
	wchar_t *FileName;
	bool Visible;
	bool Shutdown;
	RECT Rect;
	TFlashContainerForm *Form;

	bool CompleteSent;

public:
	tTVPFlashOverlay(const wchar_t *flashfile, HWND callbackwindow);
	~tTVPFlashOverlay();

	void __stdcall AddRef();
	void __stdcall Release();

	void __stdcall SetWindow(HWND window);
	void __stdcall SetMessageDrainWindow(HWND window);
	void __stdcall SetRect(RECT *rect);
	void __stdcall SetVisible(bool b);
	void __stdcall Play();
	void __stdcall Stop();
	void __stdcall Pause();
	void __stdcall SetPosition(unsigned __int64 tick);
	void __stdcall GetPosition(unsigned __int64 *tick);
	void __stdcall GetStatus(tTVPVideoStatus *status);
	void __stdcall GetEvent(long *evcode, long *param1,
			long *param2, bool *got);
// Start:	Add:	T.Imoto
	void __stdcall FreeEventParams(long evcode, long param1, long param2);

	void __stdcall Rewind();
	void __stdcall SetFrame( int f );
	void __stdcall GetFrame( int *f );
	void __stdcall GetFPS( double *f );
	void __stdcall GetNumberOfFrame( int *f );
	void __stdcall GetTotalTime( __int64 *t );

	void __stdcall GetVideoSize( long *width, long *height );
	void __stdcall GetFrontBuffer( BYTE **buff );
	void __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size );

	virtual void __stdcall SetStopFrame( int frame );
	virtual void __stdcall GetStopFrame( int *frame );
	virtual void __stdcall SetDefaultStopFrame();
// End:	Add:	T.Imoto


	void SendCommand(wchar_t *command, wchar_t *arg);
private:
	void ResetForm();
};
//---------------------------------------------------------------------------
#endif
