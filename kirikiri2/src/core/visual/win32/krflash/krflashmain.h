//---------------------------------------------------------------------------
// krflashmain.h ( part of KRFLASH.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------
#ifndef krflashmainh
#define krflashmainh

#include "..\voMode.h"
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
	bool RectSet;
	TFlashContainerForm *Form;

	bool CompleteSent;

public:
	tTVPFlashOverlay(const wchar_t *flashfile, HWND callbackwindow);
	~tTVPFlashOverlay();

	void __stdcall AddRef();
	void __stdcall Release();

	const wchar_t* __stdcall SetWindow(HWND window);
	const wchar_t* __stdcall SetMessageDrainWindow(HWND window);
	const wchar_t* __stdcall SetRect(RECT *rect);
	const wchar_t* __stdcall SetVisible(bool b);
	const wchar_t* __stdcall Play();
	const wchar_t* __stdcall Stop();
	const wchar_t* __stdcall Pause();
	const wchar_t* __stdcall SetPosition(unsigned __int64 tick);
	const wchar_t* __stdcall GetPosition(unsigned __int64 *tick);
	const wchar_t* __stdcall GetStatus(tTVPVideoStatus *status);
	const wchar_t* __stdcall GetEvent(long *evcode, long *param1,
			long *param2, bool *got);
// Start:	Add:	T.Imoto
	const wchar_t* __stdcall FreeEventParams(long evcode, long param1, long param2);
	const wchar_t* __stdcall BuildGraph( HWND callbackwin, IStream *stream,
	const wchar_t * streamname, const wchar_t *type, unsigned __int64 size );

	const wchar_t* __stdcall Rewind();
	const wchar_t* __stdcall SetFrame( int f );
	const wchar_t* __stdcall GetFrame( int *f );
	const wchar_t* __stdcall GetFPS( double *f );
	const wchar_t* __stdcall GetNumberOfFrame( int *f );
	const wchar_t* __stdcall GetTotalTime( __int64 *t );

	const wchar_t* __stdcall GetVideoSize( long *width, long *height );
	const wchar_t* __stdcall GetFrontBuffer( BYTE **buff );
	const wchar_t* __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size );
// End:	Add:	T.Imoto


	void SendCommand(wchar_t *command, wchar_t *arg);
private:
	void ResetForm();
};
//---------------------------------------------------------------------------
#endif
