//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Video Overlay support implementation
//---------------------------------------------------------------------------


#include "tjsCommHead.h"
#pragma hdrstop

#include "MsgIntf.h"
#include "VideoOvlImpl.h"
// Start:	Add:	T.Imoto
#include "LayerIntf.h"
#include "LayerBitmapIntf.h"
// End:		Add:	T.Imoto
#include "SysInitIntf.h"
#include "StorageImpl.h"
#include "krmovie.h"
#include "PluginImpl.h"
#include <evcode.h>
#if 0
typedef void (__stdcall *tGetAPIVersion)(DWORD *version);
typedef const wchar_t*  (__stdcall *tGetVideoOverlayObject)(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out);
#endif
//---------------------------------------------------------------------------
class tTVPVideoModule
{
	tTVPPluginHolder *Holder;
	HMODULE Handle;
	tGetAPIVersion procGetAPIVersion;
	tGetVideoOverlayObject procGetVideoOverlayObject;
	tTVPV2LinkProc procV2Link;
	tTVPV2UnlinkProc procV2Unlink;

public:
	tTVPVideoModule(const ttstr & name);
	~tTVPVideoModule();

	void GetAPIVersion(DWORD *version) { procGetAPIVersion(version); }
	void GetVideoOverlayObject(HWND callbackwin, IStream *stream,
		const wchar_t * streamname, const wchar_t *type, unsigned __int64 size,
		iTVPVideoOverlay **out)
	{
		procGetVideoOverlayObject(callbackwin, stream, streamname, type, size, out);
	}
};
static tTVPVideoModule *TVPMovieVideoModule = NULL;
static tTVPVideoModule *TVPLayerMovieVideoModule = NULL;	// T.Imoto
static tTVPVideoModule *TVPFlashVideoModule = NULL;
static void TVPUnloadKrMovie();
//---------------------------------------------------------------------------
tTVPVideoModule::tTVPVideoModule(const ttstr &name)
{
	Holder = new tTVPPluginHolder(name);
	Handle = LoadLibrary(Holder->GetLocalName().AsAnsiString().c_str());
	if(!Handle)
	{
		delete Holder;
		TVPThrowExceptionMessage(TVPCannotLoadKrMovieDLL);
	}

	try
	{
		procGetVideoOverlayObject = (tGetVideoOverlayObject)
			GetProcAddress(Handle, "GetVideoOverlayObject");
		procGetAPIVersion = (tGetAPIVersion)
			GetProcAddress(Handle, "GetAPIVersion");
		procV2Link = (tTVPV2LinkProc)
			GetProcAddress(Handle, "V2Link");
		procV2Unlink = (tTVPV2UnlinkProc)
			GetProcAddress(Handle, "V2Unlink");
		if(!procGetVideoOverlayObject || !procGetAPIVersion ||
			!procV2Link || !procV2Unlink)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
		DWORD version;
		procGetAPIVersion(&version);
		if(version != TVP_KRMOVIE_VER)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
		procV2Link(TVPGetFunctionExporter()); // link functions used by tp_stub
	}
	catch(...)
	{
		FreeLibrary(Handle);
		delete Holder;
		throw;
	}
}
//---------------------------------------------------------------------------
tTVPVideoModule::~tTVPVideoModule()
{
	procV2Unlink();
	FreeLibrary(Handle);
	delete Holder;
}
//---------------------------------------------------------------------------
static tTVPVideoModule * TVPGetMovieVideoModule()
{
	if(TVPMovieVideoModule == NULL)
		TVPMovieVideoModule = new tTVPVideoModule("krmovie.dll");

	return TVPMovieVideoModule;
}
//---------------------------------------------------------------------------
// Start: Add: T.Imoto
static tTVPVideoModule * TVPGetLayerMovieVideoModule()
{
	if(TVPLayerMovieVideoModule == NULL)
		TVPLayerMovieVideoModule = new tTVPVideoModule("krlmovie.dll");

	return TVPLayerMovieVideoModule;
}
// End: Add: T.Imoto
//---------------------------------------------------------------------------
static tTVPVideoModule * TVPGetFlashVideoModule()
{
	if(TVPFlashVideoModule == NULL)
		TVPFlashVideoModule = new tTVPVideoModule("krflash.dll");

	return TVPFlashVideoModule;
}
//---------------------------------------------------------------------------
/*
//---------------------------------------------------------------------------
static void TVPLoadKrMovie(bool flash)
{
	if(TVPKrMovieHandle) TVPUnloadKrMovie();

	TVPFlashDllLoaded = flash;
	TVPKrMovieHolder = new tTVPPluginHolder(TJS_W("krmovie.dll"));
	TVPKrMovieHandle =
		LoadLibrary(TVPKrMovieHolder->GetLocalName().AsAnsiString().c_str());
	if(!TVPKrMovieHandle)
	{
		delete TVPKrMovieHolder;
		TVPKrMovieHolder = NULL;
		TVPThrowExceptionMessage(TVPCannotLoadKrMovieDLL);
	}

	try
	{
		TVPGetVideoOverlayObject = (tGetVideoOverlayObject)
			GetProcAddress(TVPKrMovieHandle, "GetVideoOverlayObject");
		TVPGetAPIVersion = (tGetAPIVersion)
			GetProcAddress(TVPKrMovieHandle, "GetAPIVersion");
		if(!TVPGetVideoOverlayObject || !TVPGetAPIVersion)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
		DWORD version;
		TVPGetAPIVersion(&version);
		if(version != TVP_KRMOVIE_VER)
			TVPThrowExceptionMessage(TVPInvalidKrMovieDLL);
	}
	catch(...)
	{
		FreeLibrary(TVPKrMovieHandle), TVPKrMovieHandle = NULL;
		delete TVPKrMovieHolder;
		TVPKrMovieHolder = NULL;
		throw;
	}
}
//---------------------------------------------------------------------------
static void TVPUnloadKrMovie()
{
	if(!TVPKrMovieHandle) return;

	FreeLibrary(TVPKrMovieHandle);
	TVPKrMovieHandle = NULL;

	delete TVPKrMovieHolder;
	TVPKrMovieHolder = NULL;
}
*/
//---------------------------------------------------------------------------
static std::vector<tTJSNI_VideoOverlay *> TVPVideoOverlayVector;
//---------------------------------------------------------------------------
static void TVPAddVideOverlay(tTJSNI_VideoOverlay *ovl)
{
	TVPVideoOverlayVector.push_back(ovl);
}
//---------------------------------------------------------------------------
static void TVPRemoveVideoOverlay(tTJSNI_VideoOverlay *ovl)
{
	std::vector<tTJSNI_VideoOverlay*>::iterator i;
	i = std::find(TVPVideoOverlayVector.begin(), TVPVideoOverlayVector.end(), ovl);
	if(i != TVPVideoOverlayVector.end())
		TVPVideoOverlayVector.erase(i);
}
//---------------------------------------------------------------------------
static void TVPShutdownVideoOverlay()
{
	// shutdown all overlay object and release krmovie.dll / krflash.dll
	std::vector<tTJSNI_VideoOverlay*>::iterator i;
	for(i = TVPVideoOverlayVector.begin(); i != TVPVideoOverlayVector.end(); i++)
	{
		(*i)->Shutdown();
	}

	if(TVPMovieVideoModule) delete TVPMovieVideoModule, TVPMovieVideoModule = NULL;
	if(TVPLayerMovieVideoModule) delete TVPLayerMovieVideoModule, TVPLayerMovieVideoModule = NULL;	// T.Imoto
	if(TVPFlashVideoModule) delete TVPFlashVideoModule, TVPFlashVideoModule = NULL;
}
static tTVPAtExit TVPShutdownVideoOverlayAtExit
	(TVP_ATEXIT_PRI_PREPARE, TVPShutdownVideoOverlay);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSNI_VideoOverlay
//---------------------------------------------------------------------------
tTJSNI_VideoOverlay::tTJSNI_VideoOverlay()
{
	VideoOverlay = NULL;
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = 320;
	Rect.bottom = 240;
	Visible = false;
	OwnerWindow = NULL;
	LocalTempStorageHolder = NULL;

	UtilWindow = AllocateHWnd(WndProc);

	Layer1 = NULL;
	Layer2 = NULL;
	Mode = vomOverlay;
//	Mode = vomLayer;
	Loop = false;
	IsPrepare = false;
	SegLoopStartFrame = -1;
	SegLoopEndFrame = -1;
	IsEventPast = false;
	EventFrame = -1;

	Bitmap[0] = Bitmap[1] = NULL;
	BmpBits[0] = BmpBits[1] = NULL;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNI_VideoOverlay::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = inherited::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_VideoOverlay::Invalidate()
{
	inherited::Invalidate();

	Close();

	if(UtilWindow) DeallocateHWnd(UtilWindow);
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Open(const ttstr &_name)
{
	// open

	// first, close
	Close();


	// check window
	if(!Window) TVPThrowExceptionMessage(TVPWindowAlreadyMissing);

	// open target storage
	ttstr name(_name);
	ttstr param;

	const tjs_char * param_pos;
	int param_pos_ind;
	param_pos = TJS_strchr(name.c_str(), TJS_W('?'));
	param_pos_ind = param_pos - name.c_str();
	if(param_pos != NULL)
	{
		param = param_pos;
		name = ttstr(name, param_pos_ind);
	}

	IStream *istream = NULL;
	long size;
	bool flash;
	ttstr ext = TVPExtractStorageExt(name).c_str();
	ext.ToLowerCase();

	tTVPVideoModule *mod = NULL;
	if(ext == TJS_W(".swf"))
	{
		// shockwave flash movie
		flash = true;

		// load krflash.dll
		mod = TVPGetFlashVideoModule();

		// prepare local storage
		if(LocalTempStorageHolder)
			delete LocalTempStorageHolder, LocalTempStorageHolder = NULL;

		// find local name
		ttstr placed = TVPSearchPlacedPath(name);

		// open and hold
		LocalTempStorageHolder =
			new tTVPLocalTempStorageHolder(placed);
	}
	else
	{
		flash = false;

		if( Mode == vomOverlay )
		{	// load krmovie.dll
			mod = TVPGetMovieVideoModule();
		}
		else
		{	// load krlmovie.dll
			mod = TVPGetLayerMovieVideoModule();
		}

		// prepate IStream
		tTJSBinaryStream *stream0 = NULL;
		try
		{
			stream0 = TVPCreateStream(name);
			size = (long)stream0->GetSize();
		}
		catch(...)
		{
			if(stream0) delete stream0;
			throw;
		}

		istream = new tTVPIStreamAdapter(stream0);
	}

	// 'istream' is an IStream instance at this point

	// create video overlay object
	try
	{
		if(flash)
		{
			mod->GetVideoOverlayObject(UtilWindow,
				NULL, (LocalTempStorageHolder->GetLocalName() + param).c_str(),
				ext.c_str(), 0, &VideoOverlay);
		}
		else
		{
			mod->GetVideoOverlayObject(UtilWindow,
				istream, name.c_str(), ext.c_str(),
				size, &VideoOverlay);
		}

		if( flash || (Mode == vomOverlay) )
		{
			ResetOverlayParams();
		}
		else
		{	// set font and back buffer to layerVideo
			long	width, height;
			long			size;
			VideoOverlay->GetVideoSize( &width, &height );
			
			if( width <= 0 || height <= 0 )
				TVPThrowExceptionMessage(TVPErrorInKrMovieDLL, L"Invalid video size");

			size = width * height * 4;
			if( Bitmap[0] != NULL )
				delete Bitmap[0];
			if( Bitmap[1] != NULL )
				delete Bitmap[1];
			Bitmap[0] = new tTVPBaseBitmap( width, height, 32 );
			Bitmap[1] = new tTVPBaseBitmap( width, height, 32 );

			BmpBits[0] = static_cast<BYTE*>(Bitmap[0]->GetBitmap()->GetScanLine( Bitmap[0]->GetBitmap()->GetHeight()-1 ));
			BmpBits[1] = static_cast<BYTE*>(Bitmap[1]->GetBitmap()->GetScanLine( Bitmap[1]->GetBitmap()->GetHeight()-1 ));

			VideoOverlay->SetVideoBuffer( BmpBits[0], BmpBits[1], size );
		}
	}
	catch(...)
	{
		if(istream) istream->Release();
		Close();
		throw;
	}
	if(istream) istream->Release();

	// set Status
	ClearWndProcMessages();
	SetStatus(ssStop);
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Close()
{
	// close
	// release VideoOverlay object
	if(VideoOverlay)
	{
		VideoOverlay->Release(), VideoOverlay = NULL;
		tjs_int ofsx, ofsy;
		::SetFocus(Window->GetWindowHandle(ofsx, ofsy));
	}
	if(LocalTempStorageHolder)
		delete LocalTempStorageHolder, LocalTempStorageHolder = NULL;
	ClearWndProcMessages();
	SetStatus(ssUnload);
//	TVPUnloadKrMovie();

	if( Bitmap[0] )
		delete Bitmap[0];
	if( Bitmap[1] )
		delete Bitmap[1];

	Bitmap[0] = Bitmap[1] = NULL;
	BmpBits[0] = BmpBits[1] = NULL;
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Shutdown()
{
	// shutdown the system
	// this functions closes the overlay object, but must not fire any events.
	bool c = CanDeliverEvents;
	ClearWndProcMessages();
	SetStatus(ssUnload);
	try
	{
		if(VideoOverlay) VideoOverlay->Release(), VideoOverlay = NULL;
	}
	catch(...)
	{
		CanDeliverEvents = c;
		throw;
	}
	CanDeliverEvents = c;
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Disconnect()
{
	// disconnect the object
	Shutdown();

	Window = NULL;
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Play()
{
	// start playing
	if(VideoOverlay)
	{
		VideoOverlay->Play();
		ClearWndProcMessages();
		SetStatus(ssPlay);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Stop()
{
	// stop playing
	if(VideoOverlay)
	{
		VideoOverlay->Stop();
		ClearWndProcMessages();
		SetStatus(ssStop);
	}
}
//---------------------------------------------------------------------------
// Start:	Add:	T.Imoto
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::Pause()
{
	// pause playing
	if(VideoOverlay)
	{
		VideoOverlay->Pause();
//		ClearWndProcMessages();
		SetStatus(ssPause);
	}
}
void tTJSNI_VideoOverlay::Rewind()
{
	// rewind playing
	if(VideoOverlay)
	{
		VideoOverlay->Rewind();
		ClearWndProcMessages();

		if( EventFrame >= 0 && IsEventPast )
			IsEventPast = false;
	}
}
void tTJSNI_VideoOverlay::Prepare()
{	// prepare movie
	if( VideoOverlay && (Mode == vomLayer) )
	{
		Pause();
		Rewind();
		IsPrepare = true;
		Play();
	}
}
void tTJSNI_VideoOverlay::SetSegmentLoop( int comeFrame, int goFrame )
{
	SegLoopStartFrame = comeFrame;
	SegLoopEndFrame = goFrame;
}
void tTJSNI_VideoOverlay::SetPeriodEvent( int eventFrame )
{
	EventFrame = eventFrame;

	if( eventFrame <= GetFrame() )
		IsEventPast = true;
	else
		IsEventPast = false;
}
//---------------------------------------------------------------------------
// End:		Add:	T.Imoto
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetRectangleToVideoOverlay()
{
	// set Rectangle to video overlay
	if(VideoOverlay && OwnerWindow)
	{
		tjs_int ofsx, ofsy;
		Window->GetWindowHandle(ofsx, ofsy);
		RECT r = {Rect.left + ofsx, Rect.top + ofsy,
			Rect.right + ofsx, Rect.bottom + ofsy};
		VideoOverlay->SetRect(&r);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetPosition(tjs_int left, tjs_int top)
{
	if( Mode == vomLayer )
	{
		if( Layer1 != NULL ) Layer1->SetPosition( left, top );
		if( Layer2 != NULL ) Layer2->SetPosition( left, top );
	}
	else
	{
		Rect.set_offsets(left, top);
		SetRectangleToVideoOverlay();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetSize(tjs_int width, tjs_int height)
{
	if( Mode == vomLayer ) return;

	Rect.set_size(width, height);
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetBounds(const tTVPRect & rect)
{
	if( Mode == vomLayer ) return;

	Rect = rect;
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetLeft(tjs_int l)
{
	if( Mode == vomLayer )
	{
		if( Layer1 != NULL ) Layer1->SetLeft( l );
		if( Layer2 != NULL ) Layer2->SetLeft( l );
	}
	else
	{
		Rect.set_offsets(l, Rect.top);
		SetRectangleToVideoOverlay();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetTop(tjs_int t)
{
	if( Mode == vomLayer )
	{
		if( Layer1 != NULL ) Layer1->SetTop( t );
		if( Layer2 != NULL ) Layer2->SetTop( t );
	}
	else
	{
		Rect.set_offsets(Rect.left, t);
		SetRectangleToVideoOverlay();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetWidth(tjs_int w)
{
	if( Mode == vomLayer ) return;

	Rect.right = Rect.left + w;
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetHeight(tjs_int h)
{
	if( Mode == vomLayer ) return;

	Rect.bottom = Rect.top + h;
	SetRectangleToVideoOverlay();
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetVisible(bool b)
{
	Visible = b;
	if(VideoOverlay)
	{
		if( Mode == vomLayer )
		{
			if( Layer1 != NULL ) Layer1->SetVisible( Visible );
			if( Layer2 != NULL ) Layer2->SetVisible( Visible );
		}
		else
		{
			VideoOverlay->SetVisible(Visible);
		}
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::ResetOverlayParams()
{
	// retrieve new window information from owner window and
	// set video owner window / message drain window.
	// also sets rectangle and visible state.
	if(VideoOverlay && Window && Mode == vomOverlay)
	{
		tjs_int ofsx, ofsy;
		OwnerWindow = Window->GetWindowHandle(ofsx, ofsy);
		VideoOverlay->SetWindow(OwnerWindow);

		VideoOverlay->SetMessageDrainWindow(Window->GetSurfaceWindowHandle());

		// set Rectangle
		SetRectangleToVideoOverlay();

		// set Visible
		VideoOverlay->SetVisible(Visible);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::DetachVideoOverlay()
{
	if(VideoOverlay && Window && Mode == vomOverlay)
	{
		VideoOverlay->SetWindow(NULL);
		VideoOverlay->SetMessageDrainWindow(UtilWindow);
			// once set to util window
	}
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::SetRectOffset(tjs_int ofsx, tjs_int ofsy)
{
	if(VideoOverlay)
	{
		RECT r = {Rect.left + ofsx, Rect.top + ofsy,
			Rect.right + ofsx, Rect.bottom + ofsy};
		VideoOverlay->SetRect(&r);
	}
}
//---------------------------------------------------------------------------
void __fastcall tTJSNI_VideoOverlay::WndProc(Messages::TMessage &Msg)
{
	// UtilWindow's message procedure
	if(VideoOverlay)
	{
		if(Msg.Msg == WM_GRAPHNOTIFY)
		{
			long evcode, p1, p2;
			bool got;
			do {
				VideoOverlay->GetEvent(&evcode, &p1, &p2, &got);
				if( got == false)
					return;

				switch( evcode )
				{
					case EC_COMPLETE:
						if( Status == ssPlay )
						{
							if( Loop )
							{
								Rewind();
								FirePeriodEvent(perLoop); // fire period event by loop rewind
							}
							else
							{
								// Graph manager seems not to complete playing
								// at this point (rewinding the movie at the event
								// handler called asynchronously from SetStatusAsync
								// makes continuing playing, but the graph seems to
								// be unstable).
								// We manually stop the manager anyway.
								VideoOverlay->Stop();
								SetStatusAsync(ssStop); // All data has been rendered
							}
						}
						break;
					case EC_UPDATE:
						if( Mode == vomLayer && Status == ssPlay )
						{
							int		curFrame = p1;
							if( Layer1 == NULL && Layer2 == NULL )	// nothing to do.
								return;

							// get video image size
							long	width, height;
							VideoOverlay->GetVideoSize( &width, &height );

							tTJSNI_BaseLayer	*l1 = Layer1;
							tTJSNI_BaseLayer	*l2 = Layer2;

							// Check layer image size
							if( l1 != NULL )
							{
								if( l1->GetImageWidth() != width || l1->GetImageHeight() != height )
									l1->SetImageSize( width, height );
								if( l1->GetWidth() != width || l1->GetHeight() != height )
									l1->SetSize( width, height );
							}
							if( l2 != NULL )
							{
								if( l2->GetImageWidth() != width || l2->GetImageHeight() != height )
									l2->SetImageSize( width, height );
								if( l2->GetWidth() != width || l2->GetHeight() != height )
									l2->SetSize( width, height );
							}
							BYTE *buff;
							VideoOverlay->GetFrontBuffer( &buff );
							if( buff == BmpBits[0] )
							{
								if( l1 ) l1->AssignMainImage( Bitmap[0] );
								if( l2 ) l2->AssignMainImage( Bitmap[0] );
							}
							else	// 0‚¶‚á‚È‚©‚Á‚½‚çA1‚Æ‚Ý‚È‚·B
							{
								if( l1 ) l1->AssignMainImage( Bitmap[1] );
								if( l2 ) l2->AssignMainImage( Bitmap[1] );
							}
							if( l1 ) l1->Update();
							if( l2 ) l2->Update();

							// ! Prepare mode ?
							if( !IsPrepare )
							{
								// Segment Loop ?
								if( SegLoopEndFrame >= 0 && curFrame >= SegLoopEndFrame )
								{
									SetFrame( SegLoopStartFrame );
								}
								// Send period event ?
								if( EventFrame >= 0 && !IsEventPast && curFrame >= EventFrame )
								{
									EventFrame = -1;
									FirePeriodEvent(perPeriod); // fire period event by setPeriodEvent()
								}
							}
							else
							{	// Prepare mode
								FirePeriodEvent(perPrepare); // fire period event by prepare()
								Pause();
								Rewind();
								IsPrepare = false;
							}
                        }
						break;
				}
				VideoOverlay->FreeEventParams( evcode, p1, p2 );
			} while( got );
			return;
		}
		else if(Msg.Msg == WM_CALLBACKCMD)
		{
			// wparam : command
			// lparam : argument
			FireCallbackCommand((tjs_char*)Msg.WParam, (tjs_char*)Msg.LParam);
			return;
		}
	}

	Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
}
//---------------------------------------------------------------------------
// Start:	Add:	T.Imoto
void tTJSNI_VideoOverlay::SetTimePosition( tjs_uint64 p )
{
	if(VideoOverlay)
	{
		VideoOverlay->SetPosition( p );
	}
}
tjs_uint64 tTJSNI_VideoOverlay::GetTimePosition()
{
	tjs_uint64	result = 0;
	if(VideoOverlay)
	{
		VideoOverlay->GetPosition( &result );
	}
	return result;
}
void tTJSNI_VideoOverlay::SetFrame( tjs_int f )
{
	if(VideoOverlay)
	{
		VideoOverlay->SetFrame( f );

		if( EventFrame >= f && IsEventPast )
			IsEventPast = false;
	}
}
tjs_int tTJSNI_VideoOverlay::GetFrame()
{
	tjs_int	result = 0;
	if(VideoOverlay)
	{
		VideoOverlay->GetFrame( &result );
	}
	return result;
}
tjs_real tTJSNI_VideoOverlay::GetFPS()
{
	tjs_real	result = 0.0;
	if(VideoOverlay)
	{
		VideoOverlay->GetFPS( &result );
	}
	return result;
}
tjs_int tTJSNI_VideoOverlay::GetNumberOfFrame()
{
	tjs_int	result = 0;
	if(VideoOverlay)
	{
		VideoOverlay->GetNumberOfFrame( &result );
	}
	return result;
}
tjs_int64 tTJSNI_VideoOverlay::GetTotalTime()
{
	tjs_int64	result = 0;
	if(VideoOverlay)
	{
		VideoOverlay->GetTotalTime( &result );
	}
	return result;
}
void tTJSNI_VideoOverlay::SetLoop( bool b )
{
	Loop = b;
}
void tTJSNI_VideoOverlay::SetLayer1( tTJSNI_BaseLayer *l )
{
	Layer1 = l;
}
void tTJSNI_VideoOverlay::SetLayer2( tTJSNI_BaseLayer *l )
{
	Layer2 = l;
}
void tTJSNI_VideoOverlay::SetMode( tTVPVideoOverlayMode m )
{
	Mode = m;
}
// End:		Add:	T.Imoto

//---------------------------------------------------------------------------
tjs_int tTJSNI_VideoOverlay::GetOriginalWidth()
{
	// retrieve original (coded in the video stream) width size
	if(!VideoOverlay) return 0;

	long	width, height;
	VideoOverlay->GetVideoSize( &width, &height );

	return (tjs_int)width;
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_VideoOverlay::GetOriginalHeight()
{
	// retrieve original (coded in the video stream) height size

	long	width, height;
	VideoOverlay->GetVideoSize( &width, &height );

	return (tjs_int)height;
}
//---------------------------------------------------------------------------
void tTJSNI_VideoOverlay::ClearWndProcMessages()
{
	// clear WndProc's message queue
	MSG msg;
	while(PeekMessage(&msg, UtilWindow, WM_GRAPHNOTIFY, WM_GRAPHNOTIFY+2, PM_REMOVE))
	{
		if(VideoOverlay)
		{
			long evcode, p1, p2;
			bool got;
			VideoOverlay->GetEvent(&evcode, &p1, &p2, &got); // dummy call
// Add: Start: T.Imoto
			if( got )
				VideoOverlay->FreeEventParams( evcode, p1, p2 );
// Add: End: T.Imoto
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_VideoOverlay::CreateNativeInstance : returns proper instance object
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_VideoOverlay::CreateNativeInstance()
{
	return new tTJSNI_VideoOverlay();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCreateNativeClass_VideoOverlay
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_VideoOverlay()
{
	return new tTJSNC_VideoOverlay();
}
//---------------------------------------------------------------------------

