//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "Window" TJS Class implementation
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#define DIRECTDRAW_VERSION 0x0300
#include <ddraw.h>

#include "MsgIntf.h"
#include "WindowIntf.h"
#include "LayerIntf.h"
#include "WindowFormUnit.h"
#include "SysInitIntf.h"
#include "tjsHashSearch.h"
#include "StorageIntf.h"
#include "WideNativeFuncs.h"
#include "VideoOvlIntf.h"
#include "DebugIntf.h"
#include "PluginImpl.h"


//---------------------------------------------------------------------------
// Mouse Cursor management
//---------------------------------------------------------------------------
static tTJSHashTable<ttstr, tjs_int> TVPCursorTable;
static TVPCursorCount = 1;
tjs_int TVPGetCursor(const ttstr & name)
{
	// get placed path
	ttstr place(TVPSearchPlacedPath(name));

	// search in cache
	tjs_int * in_hash = TVPCursorTable.Find(place);
	if(in_hash) return *in_hash;

	// not found
	tTVPLocalTempStorageHolder file(place);

	HCURSOR handle;

	if(procLoadCursorFromFileW)
	{
		handle = procLoadCursorFromFileW(file.GetLocalName().c_str());
	}
	else
	{
		tTJSNarrowStringHolder holder(file.GetLocalName().c_str());
		handle = LoadCursorFromFileA(holder);
	}

	if(!handle) TVPThrowExceptionMessage(TVPCannotLoadCursor, place);

	TVPCursorCount++;
	Screen->Cursors[TVPCursorCount] = handle; // using VCL

	TVPCursorTable.Add(place, TVPCursorCount);

	return TVPCursorCount;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Color Format Detection
//---------------------------------------------------------------------------
tjs_int TVPDisplayColorFormat = 0;
static tjs_int TVPGetDisplayColorFormat()
{
	// detect current 16bpp display color format
	// return value:
	// 555 : 16bit 555 mode
	// 565 : 16bit 565 mode
	// 0   : other modes

	// create temporary bitmap and device contexts
	HDC desktopdc = GetDC(0);
	HDC bitmapdc = CreateCompatibleDC(desktopdc);
	HBITMAP bmp = CreateCompatibleBitmap(desktopdc, 1, 1);
	HBITMAP oldbmp = SelectObject(bitmapdc, bmp);

	int count;
	int r, g, b;
	COLORREF lastcolor;

	// red
	count = 0;
	lastcolor = 0xffffff;
	for(int i = 0; i < 256; i++)
	{
		SetPixel(bitmapdc, 0, 0, RGB(i, 0, 0));
		COLORREF rgb = GetPixel(bitmapdc, 0, 0);
		if(rgb != lastcolor) count ++;
		lastcolor = rgb;
	}
	r = count;

	// green
	count = 0;
	lastcolor = 0xffffff;
	for(int i = 0; i < 256; i++)
	{
		SetPixel(bitmapdc, 0, 0, RGB(0, i, 0));
		COLORREF rgb = GetPixel(bitmapdc, 0, 0);
		if(rgb != lastcolor) count ++;
		lastcolor = rgb;
	}
	g = count;

	// blue
	count = 0;
	lastcolor = 0xffffff;
	for(int i = 0; i < 256; i++)
	{
		SetPixel(bitmapdc, 0, 0, RGB(0, 0, i));
		COLORREF rgb = GetPixel(bitmapdc, 0, 0);
		if(rgb != lastcolor) count ++;
		lastcolor = rgb;
	}
	b = count;

	// free bitmap and device contexts
	SelectObject(bitmapdc, oldbmp);
	DeleteObject(bmp);
	DeleteDC(bitmapdc);
	ReleaseDC(0, desktopdc);

	// determine type
	if(r == 32 && g == 64 && b == 32)
	{
		TVPDisplayColorFormat = 565;
		return 565;
	}
	else if(r == 32 && g == 32 && b == 32)
	{
		TVPDisplayColorFormat = 555;
		return 555;
	}
	else
	{
		TVPDisplayColorFormat = 0;
		return 0;
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// DirectDraw/Full Screen and priamary surface management
//---------------------------------------------------------------------------
static IDirectDraw *TVPDirectDraw=NULL;
static IDirectDraw2 *TVPDirectDraw2=NULL;
static HRESULT WINAPI (*TVPDirectDrawCreate)
	( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter ) = NULL;
static HRESULT WINAPI (*TVPDirectDrawEnumerateA)
	( LPDDENUMCALLBACKA lpCallback, LPVOID lpContext ) = NULL;
static HRESULT WINAPI (*TVPDirectDrawEnumerateExA)
	( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags) = NULL;

const static GUID __GUID_IDirectDraw7 =
	{ 0x15e65ec0,0x3b9c,0x11d2, { 0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b }};

static HMODULE TVPDirectDrawDLLHandle=NULL;
static tjs_int TVPOrgScrWidth = 0;
static tjs_int TVPOrgScrHeight = 0;
static tjs_int TVPOrgBPP = 0;
static bool TVPOriginalScreenMetricsGot = false;
bool TVPUseChangeDisplaySettings = false;

static bool TVPInFullScreen = false;
static HWND TVPFullScreenWindow = NULL;
static tjs_int TVPFullScreenWidth = 0;
static tjs_int TVPFullScreenHeight = 0;
static tjs_int TVPFullScreenBPP = 0;
static IDirectDrawSurface * TVPDDPrimarySurface = NULL;
bool TVPDDPrimarySurfaceFailed = false;
//---------------------------------------------------------------------------
static BOOL WINAPI DDEnumCallbackEx( GUID *pGUID, LPSTR pDescription, LPSTR strName,
							  LPVOID pContext, HMONITOR hm )
{
	ttstr log(TJS_W("(info) DirectDraw Driver/Device found : "));
	if(pDescription)
		log += ttstr(pDescription);
	if(strName)
		log += TJS_W(" [") + ttstr(strName) + TJS_W("]");
	char tmp[60];
	sprintf(tmp, "0x%p", hm);
	log += TJS_W(" (monitor: ") + ttstr(tmp) + TJS_W(")");
	TVPAddImportantLog(log);

	return  DDENUMRET_OK;
}
//---------------------------------------------------------------------------
static BOOL WINAPI DDEnumCallback( GUID *pGUID, LPSTR pDescription,
							LPSTR strName, LPVOID pContext )
{
	return ( DDEnumCallbackEx( pGUID, pDescription, strName, pContext, NULL ) );
}
//---------------------------------------------------------------------------
static void TVPDumpDirectDrawDriverInformation(IDirectDraw7 *dd7)
{
	// dump directdraw information
	DDDEVICEIDENTIFIER2 DDID = {0};
	if(SUCCEEDED(dd7->GetDeviceIdentifier(&DDID, 0)))
	{
		ttstr infostart(TJS_W("(info)  "));
		ttstr log;

		// driver string
		log = infostart + ttstr(DDID.szDescription) + TJS_W(" [") + ttstr(DDID.szDriver) + TJS_W("]");
		TVPAddImportantLog(log);

		// driver version(reported)
		log = infostart + TJS_W("Driver version (reported) : ");
		char tmp[256];
		wsprintf( tmp, "%d.%02d.%02d.%04d ",
				  HIWORD( DDID.liDriverVersion.u.HighPart ),
				  LOWORD( DDID.liDriverVersion.u.HighPart ),
				  HIWORD( DDID.liDriverVersion.u.LowPart  ),
				  LOWORD( DDID.liDriverVersion.u.LowPart  ) );
		log += tmp;
		TVPAddImportantLog(log);

		// driver version(actual)
		char driverpath[1024];
		char *driverpath_filename = NULL;
		bool success = SearchPath(NULL, DDID.szDriver, NULL, 1023, driverpath, &driverpath_filename);

		if(!success)
		{
			char syspath[1024];
			GetSystemDirectory(syspath, 1023);
			strcat(syspath, "\\drivers"); // SystemDir\drivers
			success = SearchPath(syspath, DDID.szDriver, NULL, 1023, driverpath, &driverpath_filename);
		}

		if(!success)
		{
			char syspath[1024];
			GetWindowsDirectory(syspath, 1023);
			strcat(syspath, "\\system32"); // WinDir\system32
			success = SearchPath(syspath, DDID.szDriver, NULL, 1023, driverpath, &driverpath_filename);
		}

		if(!success)
		{
			char syspath[1024];
			GetWindowsDirectory(syspath, 1023);
			strcat(syspath, "\\system32\\drivers"); // WinDir\system32\drivers
			success = SearchPath(syspath, DDID.szDriver, NULL, 1023, driverpath, &driverpath_filename);
		}

		if(success)
		{
			log = infostart + TJS_W("Driver version (") + ttstr(driverpath) + TJS_W(") : ");
			tjs_int major, minor, release, build;
			if(TVPGetFileVersionOf(driverpath, major, minor, release, build))
			{
				wsprintf(tmp, "%d.%d.%d.%d", (int)major, (int)minor, (int)release, (int)build);
				log += tmp;
			}
			else
			{
				log += TJS_W("unknown");
			}
		}
		else
		{
			log = infostart + TJS_W("Driver ") + ttstr(DDID.szDriver) +
				TJS_W(" is not found in search path.");
		}
		TVPAddImportantLog(log);

		// device id
		wsprintf(tmp, "VendorId:%08X  DeviceId:%08X  SubSysId:%08X  Revision:%08X",
			DDID.dwVendorId, DDID.dwDeviceId, DDID.dwSubSysId, DDID.dwRevision);
		log = infostart + TJS_W("Device ids : ") + tmp;
		TVPAddImportantLog(log);

		// Device GUID
		GUID *pguid = &DDID.guidDeviceIdentifier;
		wsprintf( tmp, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
				  pguid->Data1,
				  pguid->Data2,
				  pguid->Data3,
				  pguid->Data4[0], pguid->Data4[1], pguid->Data4[2], pguid->Data4[3],
				  pguid->Data4[4], pguid->Data4[5], pguid->Data4[6], pguid->Data4[7] );
		log = infostart + TJS_W("Unique driver/device id : ") + tmp;
		TVPAddImportantLog(log);

		// WHQL level
		wsprintf(tmp, "%08x", DDID.dwWHQLLevel);
		log = infostart + TJS_W("WHQL level : ")  + tmp;
		TVPAddImportantLog(log);
	}
	else
	{
		TVPAddImportantLog(TJS_W("(info) Failed."));
	}

}
//---------------------------------------------------------------------------
static void TVPGetOriginalScreenMetrics()
{
	// retrieve original (un-fullscreened) information
	if(TVPOriginalScreenMetricsGot) return;
	TVPOrgScrWidth = Screen->Width;
	TVPOrgScrHeight = Screen->Height;
	HDC dc = GetDC(0);
	TVPOrgBPP = GetDeviceCaps(dc, BITSPIXEL);
	ReleaseDC(0, dc);
	TVPOriginalScreenMetricsGot = true;
}
//---------------------------------------------------------------------------
static void TVPUnloadDirectDraw();
static void TVPInitDirectDraw()
{
	if(!TVPDirectDrawDLLHandle)
	{
		// load ddraw.dll
		TVPDirectDrawDLLHandle = LoadLibrary("ddraw.dll");
		if(!TVPDirectDrawDLLHandle)
			TVPThrowExceptionMessage(TVPCannotInitDirectDraw,
				TJS_W("Cannot load ddraw.dll"));

		// Enumerate display drivers, for debugging information
		try
		{
			TVPDirectDrawEnumerateExA = (HRESULT WINAPI (*)
				( LPDDENUMCALLBACKEXA , LPVOID , DWORD )	)
					GetProcAddress(TVPDirectDrawDLLHandle, "DirectDrawEnumerateExA");
			if(TVPDirectDrawEnumerateExA)
			{
				TVPDirectDrawEnumerateExA( DDEnumCallbackEx, NULL,
										  DDENUM_ATTACHEDSECONDARYDEVICES |
										  DDENUM_DETACHEDSECONDARYDEVICES |
										  DDENUM_NONDISPLAYDEVICES );
			}
			else
			{
				TVPDirectDrawEnumerateA = (HRESULT WINAPI (*)
					( LPDDENUMCALLBACKA , LPVOID  ))
					GetProcAddress(TVPDirectDrawDLLHandle, "DirectDrawEnumerateA");
				if(TVPDirectDrawEnumerateA)
				{
			        TVPDirectDrawEnumerateA( DDEnumCallback, NULL );
				}
			}
		}
		catch(...)
		{
			// Ignore errors
		}
	}

	if(!TVPDirectDraw2)
	{
		try
		{
			// get DirectDrawCreaet function
			TVPDirectDrawCreate = (HRESULT(WINAPI*)(_GUID*,IDirectDraw**,IUnknown*))
				GetProcAddress(TVPDirectDrawDLLHandle, "DirectDrawCreate");
			if(!TVPDirectDrawCreate)
				TVPThrowExceptionMessage(TVPCannotInitDirectDraw,
					TJS_W("Missing DirectDrawCreate in ddraw.dll"));

			// create IDirectDraw object
			HRESULT hr;

			hr = TVPDirectDrawCreate(NULL, &TVPDirectDraw, NULL);
			if(FAILED(hr))
				TVPThrowExceptionMessage(TVPCannotInitDirectDraw,
					ttstr(TJS_W("DirectDrawCreate failed./HR="))+
						TJSInt32ToHex((tjs_uint32)hr));

			// retrieve IDirecDraw2 interface
			hr = TVPDirectDraw->QueryInterface(IID_IDirectDraw2,
				(void **)&TVPDirectDraw2);
			if(FAILED(hr))
				TVPThrowExceptionMessage(TVPCannotInitDirectDraw,
					ttstr(TJS_W("Querying of IID_IDirectDraw2 failed."
						" (DirectX on this system may be too old)/HR="))+
						TJSInt32ToHex((tjs_uint32)hr));

			TVPDirectDraw->Release(), TVPDirectDraw = NULL;

			// retrieve IDirectDraw7 interface

			IDirectDraw7 *dd7 = NULL;
			hr = TVPDirectDraw2->QueryInterface(IID_IDirectDraw7, (void**)&dd7);
			if(SUCCEEDED(hr) && dd7)
			{
				TVPAddImportantLog(TJS_W("(info) DirectDraw7 or higher detected. Retrieving current DirectDraw driver information..."));
				try
				{
					TVPDumpDirectDrawDriverInformation(dd7);
				}
				catch(...)
				{
					// ignore errors
				}
				dd7->Release();
			}


			// set cooperative level
			TVPDirectDraw2->SetCooperativeLevel(NULL, DDSCL_NORMAL);
		}
		catch(...)
		{
			TVPUnloadDirectDraw();
			throw;
		}
	}

	TVPGetDisplayColorFormat();
}
//---------------------------------------------------------------------------
static void TVPUninitDirectDraw()
{
	// release DirectDraw object ( DLL will not be released )
}
//---------------------------------------------------------------------------
static void TVPUnloadDirectDraw()
{
	// release DirectDraw object and /*release it's DLL */
	TVPUninitDirectDraw();
	if(TVPDDPrimarySurface) TVPDDPrimarySurface->Release(), TVPDDPrimarySurface = NULL;
	if(TVPDirectDraw2) TVPDirectDraw2->Release(), TVPDirectDraw2 = NULL;
	if(TVPDirectDraw) TVPDirectDraw -> Release(), TVPDirectDraw = NULL;
//	if(TVPDirectDrawDLLHandle)
//		FreeLibrary(TVPDirectDrawDLLHandle), TVPDirectDrawDLLHandle = NULL;

	TVPGetDisplayColorFormat();
}
//---------------------------------------------------------------------------
void TVPEnsureDirectDrawObject()
{
	try
	{
		TVPInitDirectDraw();
	}
	catch(...)
	{
	}
}
//---------------------------------------------------------------------------
IDirectDraw2 * TVPGetDirectDrawObjectNoAddRef()
{
	// retrieves DirectDraw2 interface
	return TVPDirectDraw2;
}
//---------------------------------------------------------------------------
IDirectDrawSurface * TVPGetDDPrimarySurfaceNoAddRef()
{
	if(TVPDDPrimarySurfaceFailed) return NULL;
	if(TVPDDPrimarySurface) return TVPDDPrimarySurface;

	TVPEnsureDirectDrawObject();

	if(!TVPDirectDraw2)
	{
		// DirectDraw not available
		TVPDDPrimarySurfaceFailed = true;
		return NULL;
	}

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	HRESULT hr;
	hr = TVPDirectDraw2->CreateSurface(&ddsd, &TVPDDPrimarySurface, NULL);
	if(hr != DD_OK)
	{
		// failed to create DirectDraw primary surface
		TVPDDPrimarySurface = NULL;
		TVPDDPrimarySurfaceFailed = true;
		return NULL;
	}

	return TVPDDPrimarySurface;
}
//---------------------------------------------------------------------------
void TVPSetDDPrimaryClipper(IDirectDrawClipper *clipper)
{
	// set clipper object

	IDirectDrawSurface * pri = TVPGetDDPrimarySurfaceNoAddRef();

	// set current clipper object
	if(pri) pri->SetClipper(clipper);
}
//---------------------------------------------------------------------------
static void TVPReleaseDDPrimarySurface()
{
	if(TVPDDPrimarySurface) TVPDDPrimarySurface->Release(), TVPDDPrimarySurface = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static tTVPAtExit
	TVPUnloadDirectDrawAtExit(TVP_ATEXIT_PRI_RELEASE, TVPUnloadDirectDraw);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
struct tTVPTestDisplayModeStruc
{
	tjs_uint w;
	tjs_uint h;
	std::vector<tjs_int> availbpp;
};
//---------------------------------------------------------------------------
struct tTVP_devicemodeA {
	// copy of DEVMODE, to avoid windows platform SDK version mismatch
#pragma pack(push, 1)
	BYTE   dmDeviceName[CCHDEVICENAME];
	WORD dmSpecVersion;
	WORD dmDriverVersion;
	WORD dmSize;
	WORD dmDriverExtra;
	DWORD dmFields;
	union {
	  struct {
		short dmOrientation;
		short dmPaperSize;
		short dmPaperLength;
		short dmPaperWidth;
	  };
	  POINTL dmPosition;
	};
	short dmScale;
	short dmCopies;
	short dmDefaultSource;
	short dmPrintQuality;
	short dmColor;
	short dmDuplex;
	short dmYResolution;
	short dmTTOption;
	short dmCollate;
	BYTE   dmFormName[CCHFORMNAME];
	WORD   dmLogPixels;
	DWORD  dmBitsPerPel;
	DWORD  dmPelsWidth;
	DWORD  dmPelsHeight;
	union {
		DWORD  dmDisplayFlags;
		DWORD  dmNup;
	};
	DWORD  dmDisplayFrequency;
	DWORD  dmICMMethod;
	DWORD  dmICMIntent;
	DWORD  dmMediaType;
	DWORD  dmDitherType;
	DWORD  dmReserved1;
	DWORD  dmReserved2;
#pragma pack(pop)
};
//---------------------------------------------------------------------------
static HRESULT WINAPI TVPEnumModesCallback(LPDDSURFACEDESC lpDDSufaceDesc,
	LPVOID lpContext)
{
	tTVPTestDisplayModeStruc *data = (tTVPTestDisplayModeStruc*)lpContext;

	if(data->w == lpDDSufaceDesc->dwWidth &&
		data->h == lpDDSufaceDesc->dwHeight)
	{
		// size matched; insert to vector
		std::vector<tjs_int>::iterator i =
			std::find(data->availbpp.begin(), data->availbpp.end(),
				(tjs_int)lpDDSufaceDesc->ddpfPixelFormat.dwRGBBitCount);
		if(i == data->availbpp.end())
			data->availbpp.push_back(
				(tjs_int)lpDDSufaceDesc->ddpfPixelFormat.dwRGBBitCount);
	}

	return DDENUMRET_OK;
}
void TVPTestDisplayMode(tjs_int w, tjs_int h, tjs_int & bpp)
{
	// search specified resolution mode and modify "bpp" as most proper bit depth mode.
	TVPGetOriginalScreenMetrics();

	if(!TVPUseChangeDisplaySettings)
	{
		try
		{
			TVPInitDirectDraw();
		}
		catch(eTJS &e)
		{
			TVPAddLog(e.GetMessage());
			TVPUseChangeDisplaySettings = true;
		}
		catch(...)
		{
			TVPUseChangeDisplaySettings = true;
		}
	}

	if(TVPUseChangeDisplaySettings)
	{
		tTVP_devicemodeA dm;
		ZeroMemory(&dm, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = w;
		dm.dmPelsHeight = h;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		if(TVPFullScreenBPP == 0)
		{
			// no change
			dm.dmBitsPerPel = TVPOrgBPP;
			if(ChangeDisplaySettings((DEVMODE*)&dm, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
				bpp = dm.dmBitsPerPel;
			else
				bpp = 24; // can fail
		}
		else
		{
			dm.dmBitsPerPel = TVPFullScreenBPP;
			if(ChangeDisplaySettings((DEVMODE*)&dm, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
			{
				bpp = TVPFullScreenBPP;
			}
			else
			{
				dm.dmBitsPerPel = TVPOrgBPP;
				if(ChangeDisplaySettings((DEVMODE*)&dm, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
					bpp = dm.dmBitsPerPel;
				else
					bpp = 24; // can fail
			}
		}
	}
	else
	{
		tTVPTestDisplayModeStruc data;
		data.w = w;
		data.h = h;

		TVPDirectDraw2->EnumDisplayModes(0, NULL, (void*)&data,
			TVPEnumModesCallback);

		if(TVPFullScreenBPP == 0)
		{
			// no change
			std::vector<tjs_int>::iterator i =
				std::find(data.availbpp.begin(), data.availbpp.end(),
					TVPOrgBPP);
			if(i == data.availbpp.end())
				bpp = 24; // can fail
			else
				bpp = TVPOrgBPP;
		}
		else
		{
			// check whether the specified BPP is in the list
			std::vector<tjs_int>::iterator i =
				std::find(data.availbpp.begin(), data.availbpp.end(),
					TVPFullScreenBPP);

			if(i == data.availbpp.end())
				bpp = 24; // can fail
			else
				bpp = TVPFullScreenBPP;
		}
	}
}
//---------------------------------------------------------------------------
void TVPSwitchToFullScreen(HWND window, tjs_int w, tjs_int h)
{
	TVPGetOriginalScreenMetrics();

	if(TVPInFullScreen) return;

	TVPReleaseDDPrimarySurface();

	if(!TVPUseChangeDisplaySettings)
	{
		try
		{
			TVPInitDirectDraw();
		}
		catch(eTJS &e)
		{
			TVPAddLog(e.GetMessage());
			TVPUseChangeDisplaySettings = true;
		}
		catch(...)
		{
			TVPUseChangeDisplaySettings = true;
		}
	}

	tjs_int bpp = 0;
	TVPTestDisplayMode(w, h, bpp);
	if(bpp == 0 || bpp <= 8)
		TVPThrowExceptionMessage(TVPCannotFindDisplayMode,
			ttstr(w) + ttstr(TJS_W("x")) + ttstr(h));

	if(TVPUseChangeDisplaySettings)
	{
		DEVMODE dm;
		ZeroMemory(&dm, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = w;
		dm.dmPelsHeight = h;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		dm.dmBitsPerPel = bpp;
		ChangeDisplaySettings((DEVMODE*)&dm, CDS_FULLSCREEN);

		SetWindowPos(window, HWND_TOP, 0, 0, w, h, SWP_SHOWWINDOW);
	}
	else
	{
		HRESULT hr;
		hr = TVPDirectDraw2->SetCooperativeLevel(window,
			DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT);

		if(FAILED(hr))
		{
			TVPUninitDirectDraw();
			TVPThrowExceptionMessage(TVPCannotSwitchToFullScreen,
				TJS_W("IDirectDraw2::SetCooperativeLevel failed."));
		}

		hr =TVPDirectDraw2->SetDisplayMode(w, h, bpp, 0, 0);
		if(FAILED(hr))
		{
			TVPUninitDirectDraw();
			TVPThrowExceptionMessage(TVPCannotSwitchToFullScreen,
				ttstr(TJS_W("IDirectDraw2::SetDisplayMode failed/")) + ttstr(w) +
				ttstr(TJS_W("/")) + ttstr(h));
		}
	}


	TVPInFullScreen = true;
	TVPFullScreenWidth = w;
	TVPFullScreenHeight = h;
	TVPFullScreenBPP = bpp;
	TVPFullScreenWindow = window;

	TVPGetDisplayColorFormat();
}
//---------------------------------------------------------------------------
void TVPRevertFromFullScreen(HWND window)
{
	if(!TVPInFullScreen) return;

	TVPReleaseDDPrimarySurface();

	if(TVPUseChangeDisplaySettings)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	else
	{
		if(TVPDirectDraw2)
		{
			TVPDirectDraw2->RestoreDisplayMode();
			TVPDirectDraw2->SetCooperativeLevel(window, DDSCL_NORMAL);
		}
	}
	TVPUninitDirectDraw();

	TVPInFullScreen = false;

	TVPGetDisplayColorFormat();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TVPMinimizeFullScreenWindowAtInactivation()
{
	// only works when TVPUseChangeDisplaySettings == true
	// (DirectDraw framework does this)

	if(!TVPInFullScreen) return;
	if(!TVPUseChangeDisplaySettings) return;

	ChangeDisplaySettings(NULL, 0);

	ShowWindow(TVPFullScreenWindow, SW_MINIMIZE);
}
//---------------------------------------------------------------------------
void TVPRestoreFullScreenWindowAtActivation()
{
	// only works when TVPUseChangeDisplaySettings == true
	// (DirectDraw framework does this)

	if(!TVPInFullScreen) return;
	if(!TVPUseChangeDisplaySettings) return;

	DEVMODE dm;
	ZeroMemory(&dm, sizeof(DEVMODE));
	dm.dmSize = sizeof(DEVMODE);
	dm.dmPelsWidth = TVPFullScreenWidth;
	dm.dmPelsHeight = TVPFullScreenHeight;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	dm.dmBitsPerPel = TVPFullScreenBPP;
	ChangeDisplaySettings((DEVMODE*)&dm, CDS_FULLSCREEN);

	ShowWindow(TVPFullScreenWindow, SW_RESTORE);
	SetWindowPos(TVPFullScreenWindow, HWND_TOP,
		0, 0, TVPFullScreenWidth, TVPFullScreenHeight, SWP_SHOWWINDOW);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TVPRestoreDisplayMode()
{
	// only works when TVPUseChangeDisplaySettings == true
	if(!TVPUseChangeDisplaySettings) return;
	if(!TVPInFullScreen) return;
	ChangeDisplaySettings(NULL, 0);
}
//---------------------------------------------------------------------------
static tTVPAtExit
	TVPRestoreDisplayModeAtExit(TVP_ATEXIT_PRI_CLEANUP, TVPUnloadDirectDraw);
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
// TVPGetModalWindowOwner
//---------------------------------------------------------------------------
HWND TVPGetModalWindowOwnerHandle()
{
	if(TVPFullScreenedWindow)
		return TVPFullScreenedWindow->Handle;
	else
		return Application->Handle;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNI_Window
//---------------------------------------------------------------------------
tTJSNI_Window::tTJSNI_Window()
{
	Form = NULL;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNI_Window::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = tTJSNI_BaseWindow::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;
	Form = new TTVPWindowForm(Application, this);

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_Window::Invalidate()
{
	tTJSNI_BaseWindow::Invalidate();
	if(Form)
	{
		Form->InvalidateClose();
		Form = NULL;
	}

	// remove all events
	TVPCancelSourceEvents(Owner);
	TVPCancelInputEvents(this);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::CanDeliverEvents() const
{
	if(!Form) return false;
	return GetVisible() && Form->GetFormEnabled();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::NotifyWindowClose()
{
	Form = NULL;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SendCloseMessage()
{
	if(Form) Form->SendCloseMessage();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::TickBeat()
{
	if(Form) Form->TickBeat();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::PostInputEvent(const ttstr &name, iTJSDispatch2 * params)
{
	// posts input event
	if(!Form) return;

	static ttstr key_name(TJS_W("key"));
	static ttstr shift_name(TJS_W("shift"));

	// check input event name
	enum tEventType
	{
		etUnknown, etOnKeyDown, etOnKeyUp, etOnKeyPress
	} type;

	if(name == TJS_W("onKeyDown"))
		type = etOnKeyDown;
	else if(name == TJS_W("onKeyUp"))
		type = etOnKeyUp;
	else if(name == TJS_W("onKeyPress"))
		type = etOnKeyPress;
	else
		type = etUnknown;

	if(type == etUnknown)
		TVPThrowExceptionMessage(TVPSpecifiedEventNameIsUnknown, name);


	if(type == etOnKeyDown || type == etOnKeyUp)
	{
		// this needs params, "key" and "shift"
		if(params == NULL)
			TVPThrowExceptionMessage(
				TVPSpecifiedEventNeedsParameter, name);


		tjs_uint key;
		tjs_uint32 shift = 0;

		tTJSVariant val;
		if(TJS_SUCCEEDED(params->PropGet(0, key_name.c_str(), key_name.GetHint(),
			&val, params)))
			key = (tjs_int)val;
		else
			TVPThrowExceptionMessage(TVPSpecifiedEventNeedsParameter2,
				name, TJS_W("key"));

		if(TJS_SUCCEEDED(params->PropGet(0, shift_name.c_str(), shift_name.GetHint(),
			&val, params)))
			shift = (tjs_int)val;
		else
			TVPThrowExceptionMessage(TVPSpecifiedEventNeedsParameter2,
				name, TJS_W("shift"));

		Word vcl_key = key;
		if(type == etOnKeyDown)
			Form->InternalKeyDown(key, shift);
		else if(type == etOnKeyUp)
			Form->OnKeyUp(Form, vcl_key, TVP_TShiftState_From_uint32(shift));
	}
	else if(type == etOnKeyPress)
	{
		// this needs param, "key"
		if(params == NULL)
			TVPThrowExceptionMessage(
				TVPSpecifiedEventNeedsParameter, name);


		tjs_uint key;

		tTJSVariant val;
		if(TJS_SUCCEEDED(params->PropGet(0, key_name.c_str(), key_name.GetHint(),
			&val, params)))
			key = (tjs_int)val;
		else
			TVPThrowExceptionMessage(TVPSpecifiedEventNeedsParameter2,
				name, TJS_W("key"));

		char vcl_key = key;
		Form->OnKeyPress(Form, vcl_key);
	}
}

//---------------------------------------------------------------------------
void tTJSNI_Window::NotifyLayerResize()
{
	tTJSNI_BaseWindow::NotifyLayerResize();

	// is called from primary layer
	// ( or from TWindowForm to reset paint box's size )
	tTJSNI_BaseLayer *lay = LayerManager->GetPrimaryLayer();
	if(Form && lay)
		Form->SetPaintBoxSize(lay->GetWidth(), lay->GetHeight());
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetDefaultMouseCursor()
{
	// set window mouse cursor to default
	if(Form) Form->SetDefaultMouseCursor();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetMouseCursor(tjs_int handle)
{
	// set window mouse cursor
	if(Form) Form->SetMouseCursor(handle);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::GetCursorPos(tjs_int &x, tjs_int &y)
{
	// get cursor pos in primary layer's coordinates
	if(Form) Form->GetCursorPos(x, y);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetCursorPos(tjs_int x, tjs_int y)
{
	// set cursor pos in primar layer's coordinates
	if(Form) Form->SetCursorPos(x, y);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetHintText(const ttstr & text)
{
	// set hint text to window
	if(Form) Form->SetHintText(text);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetAttentionPoint(tTJSNI_BaseLayer *layer,
	tjs_int l, tjs_int t)
{
	// set attention point to window
	if(Form)
	{
		TFont * font = NULL;
		if(layer)
		{
			tTVPBaseBitmap *bmp = layer->GetMainImage();
			if(bmp)
				font = bmp->GetFontCanvas()->Font;
		}

		Form->SetAttentionPoint(l, t, font);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_Window::DisableAttentionPoint()
{
	// disable attention point
	if(Form) Form->DisableAttentionPoint();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetImeMode(tTVPImeMode mode)
{
	// set ime mode
	if(Form) Form->SetImeMode(mode);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetDefaultImeMode(tTVPImeMode mode)
{
	// set default ime mode
	if(Form)
	{
		Form->SetDefaultImeMode(mode, LayerManager->GetFocusedLayer() == NULL);
	}
}
//---------------------------------------------------------------------------
tTVPImeMode tTJSNI_Window::GetDefaultImeMode() const
{
	if(Form) return Form->GetDefaultImeMode();
	return ::imDisable;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::ResetImeMode()
{
	// set default ime mode ( default mode is imDisable; IME is disabled )
	if(Form) Form->ResetImeMode();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::DrawCompleted(const tTVPRect &destrect,
	tTVPBaseBitmap *bmp, const tTVPRect &cliprect, tTVPLayerType, tjs_int)
{
	// is called from the layer
	// transfer the image to the PaintBox;

	if(Form) Form->DrawLayerImage(destrect, bmp, cliprect);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::BeginUpdate(const tTVPComplexRect &rects)
{
	tTJSNI_BaseWindow::BeginUpdate(rects);
	if(Form) Form->BeginDrawLayerImage(rects);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::EndUpdate()
{
	if(Form) Form->EndDrawLayerImage();
	tTJSNI_BaseWindow::EndUpdate();
}
//---------------------------------------------------------------------------
TMenuItem * tTJSNI_Window::GetRootMenuItem()
{
	if(!Form) return NULL;
	return Form->MainMenu->Items;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetMenuBarVisible(bool b)
{
	if(!Form) return;
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);

	Form->SetMenuBarVisible(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetMenuBarVisible() const
{
	if(!Form) return false;
	return Form->GetMenuBarVisible();
}
//---------------------------------------------------------------------------
HWND tTJSNI_Window::GetSurfaceWindowHandle()
{
	if(!Form) return NULL;
	return Form->GetSurfaceWindowHandle();
}
//---------------------------------------------------------------------------
HWND tTJSNI_Window::GetWindowHandle(tjs_int &ofsx, tjs_int &ofsy)
{
	if(!Form) return NULL;
	return Form->GetWindowHandle(ofsx, ofsy);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::ReadjustVideoRect()
{
	if(!Form) return;

	// re-adjust video rectangle.
	// this reconnects owner window and video offsets.

	tObjectListSafeLockHolder<tTJSNI_BaseVideoOverlay> holder(VideoOverlay);
	tjs_int count = VideoOverlay.GetSafeLockedObjectCount();

	for(tjs_int i = 0; i < count; i++)
	{
		tTJSNI_VideoOverlay * item = (tTJSNI_VideoOverlay*)
			VideoOverlay.GetSafeLockedObjectAt(i);
		if(!item) continue;
		item->ResetOverlayParams();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_Window::WindowMoved()
{
	// inform video overlays that the window has moved.
	// video overlays typically owns DirectDraw surface which is not a part of
	// normal window systems and does not matter where the owner window is.
	// so we must inform window moving to overlay window.

	tObjectListSafeLockHolder<tTJSNI_BaseVideoOverlay> holder(VideoOverlay);
	tjs_int count = VideoOverlay.GetSafeLockedObjectCount();
	for(tjs_int i = 0; i < count; i++)
	{
		tTJSNI_VideoOverlay * item = (tTJSNI_VideoOverlay*)
			VideoOverlay.GetSafeLockedObjectAt(i);
		if(!item) continue;
		item->SetRectangleToVideoOverlay();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_Window::DetachVideoOverlay()
{
	// detach video overlay window
	// this is done before the window is being fullscreened or un-fullscreened.
	tObjectListSafeLockHolder<tTJSNI_BaseVideoOverlay> holder(VideoOverlay);
	tjs_int count = VideoOverlay.GetSafeLockedObjectCount();
	for(tjs_int i = 0; i < count; i++)
	{
		tTJSNI_VideoOverlay * item = (tTJSNI_VideoOverlay*)
			VideoOverlay.GetSafeLockedObjectAt(i);
		if(!item) continue;
		item->DetachVideoOverlay();
	}
}
//---------------------------------------------------------------------------
HWND tTJSNI_Window::GetWindowHandleForPlugin()
{
	if(!Form) return NULL;
	return Form->GetWindowHandleForPlugin();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::RegisterWindowMessageReceiver(tTVPWMRRegMode mode,
		void * proc, const void *userdata)
{
	if(!Form) return;
	Form->RegisterWindowMessageReceiver(mode, proc, userdata);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::Close()
{
	if(Form) Form->Close();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::OnCloseQueryCalled(bool b)
{
	if(Form) Form->OnCloseQueryCalled(b);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::BeginMove()
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidMethodInFullScreen);
	if(Form) Form->BeginMove();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::BringToFront()
{
	if(Form) Form->BringToFront();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::Update(tTVPUpdateType type)
{
	if(Form) Form->UpdateWindow(type);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::ShowModal()
{
	if(Form && Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidMethodInFullScreen);
	if(Form)
	{
		TVPClearAllWindowInputEvents();
			// cancel all input events that can cause delayed operation
		Form->ShowWindowAsModal();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_Window::HideMouseCursor()
{
	if(Form) Form->HideMouseCursor();
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetVisible() const
{
	if(!Form) return false;
	return Form->GetVisible();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetVisible(bool s)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetVisible(s);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::GetCaption(ttstr & v) const
{
	if(Form) v = Form->Caption; else v.Clear();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetCaption(const ttstr & v)
{
	if(Form) Form->Caption = v.AsAnsiString();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetWidth(tjs_int w)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->Width = w;
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetWidth() const
{
	if(!Form) return 0;
	return Form->Width;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetHeight(tjs_int h)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->Height = h;
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetHeight() const
{
	if(!Form) return 0;
	return Form->Height;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetLeft(tjs_int l)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->Left = l;
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetLeft() const
{
	if(!Form) return 0;
	return Form->Left;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetTop(tjs_int t)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->Top = t;
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetTop() const
{
	if(!Form) return 0;
	return Form->Top;
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetSize(tjs_int w, tjs_int h)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetBounds(Form->Left, Form->Top, w, h);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetPosition(tjs_int l, tjs_int t)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetBounds(l, t, Form->Width, Form->Height);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetLayerLeft(tjs_int l)
{
	if(Form) Form->SetLayerLeft(l);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetLayerLeft() const
{
	if(!Form) return 0;
	return Form->GetLayerLeft();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetLayerTop(tjs_int t)
{
	if(Form) Form->SetLayerTop(t);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetLayerTop() const
{
	if(!Form) return 0;
	return Form->GetLayerTop();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetLayerPosition(tjs_int l, tjs_int t)
{
	if(Form) Form->SetLayerPosition(l, t);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetInnerSunken(bool b)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetInnerSunken(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetInnerSunken() const
{
	if(!Form) return true;
	return Form->GetInnerSunken();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetInnerWidth(tjs_int w)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetInnerWidth(w);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetInnerWidth() const
{
	if(!Form) return 0;
	return Form->GetInnerWidth();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetInnerHeight(tjs_int h)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetInnerHeight(h);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetInnerHeight() const
{
	if(!Form) return 0;
	return Form->GetInnerHeight();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetInnerSize(tjs_int w, tjs_int h)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetInnerSize(w, h);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetBorderStyle(tTVPBorderStyle st)
{
	if(Form->GetFullScreenMode())
		TVPThrowExceptionMessage(TVPInvalidPropertyInFullScreen);
	if(Form) Form->SetBorderStyle(st);
}
//---------------------------------------------------------------------------
tTVPBorderStyle tTJSNI_Window::GetBorderStyle() const
{
	if(!Form) return (tTVPBorderStyle)0;
	return Form->GetBorderStyle();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetStayOnTop(bool b)
{
	if(!Form) return;
	Form->SetStayOnTop(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetStayOnTop() const
{
	if(!Form) return false;
	return Form->GetStayOnTop();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetShowScrollBars(bool b)
{
	if(Form) Form->SetShowScrollBars(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetShowScrollBars() const
{
	if(!Form) return true;
	return Form->GetShowScrollBars();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetFullScreen(bool b)
{
	if(!Form) return;
	Form->SetFullScreenMode(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetFullScreen() const
{
	if(!Form) return false;
	return Form->GetFullScreenMode();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetUseMouseKey(bool b)
{
	if(!Form) return;
	Form->SetUseMouseKey(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetUseMouseKey() const
{
	if(!Form) return false;
	return Form->GetUseMouseKey();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetTrapKey(bool b)
{
	if(!Form) return;
	Form->SetTrapKey(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetTrapKey() const
{
	if(!Form) return false;
	return Form->GetTrapKey();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetMaskRegion(tjs_int threshold)
{
	if(!Form) return;
	if(!LayerManager) TVPThrowExceptionMessage(TVPWindowHasNoLayer);
	tTJSNI_BaseLayer *lay = LayerManager->GetPrimaryLayer();
	if(!lay) TVPThrowExceptionMessage(TVPWindowHasNoLayer);
	Form->SetMaskRegion(((tTJSNI_Layer*)lay)->CreateMaskRgn((tjs_uint)threshold));
}
//---------------------------------------------------------------------------
void tTJSNI_Window::RemoveMaskRegion()
{
	if(!Form) return;
	Form->RemoveMaskRegion();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetMouseCursorState(tTVPMouseCursorState mcs)
{
	if(!Form) return;
	Form->SetMouseCursorState(mcs);
}
//---------------------------------------------------------------------------
tTVPMouseCursorState tTJSNI_Window::GetMouseCursorState() const
{
	if(!Form) return mcsVisible;
	return Form->GetMouseCursorState();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetFocusable(bool b)
{
	if(!Form) return;
	Form->SetFocusable(b);
}
//---------------------------------------------------------------------------
bool tTJSNI_Window::GetFocusable()
{
	if(!Form) return true;
	return Form->GetFocusable();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetZoom(tjs_int numer, tjs_int denom)
{
	if(!Form) return;
	Form->SetZoom(numer, denom);
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetZoomNumer(tjs_int n)
{
	if(!Form) return;
	Form->SetZoomNumer(n);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetZoomNumer() const
{
	if(!Form) return 1;
	return Form->GetZoomNumer();
}
//---------------------------------------------------------------------------
void tTJSNI_Window::SetZoomDenom(tjs_int n)
{
	if(!Form) return;
	Form->SetZoomDenom(n);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_Window::GetZoomDenom() const
{
	if(!Form) return 1;
	return Form->GetZoomDenom();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// tTJSNC_Window::CreateNativeInstance : returns proper instance object
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_Window::CreateNativeInstance()
{
	return new tTJSNI_Window();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCreateNativeClass_Window
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Window()
{
	tTJSNativeClass *cls = new tTJSNC_Window();
	static tjs_uint32 TJS_NCM_CLASSID;
	TJS_NCM_CLASSID = tTJSNC_Window::ClassID;
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(registerMessageReceiver)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Window);
	if(numparams < 3) return TJS_E_BADPARAMCOUNT;

	_this->RegisterWindowMessageReceiver((tTVPWMRRegMode)((tjs_int)*param[0]),
		reinterpret_cast<void *>((tjs_int)(*param[1])),
		reinterpret_cast<const void *>((tjs_int)(*param[2])));

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL_OUTER(cls, registerMessageReceiver)
//---------------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(HWND)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_Window);
		*result = (tTVInteger)(tjs_uint)_this->GetWindowHandleForPlugin();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL_OUTER(cls, HWND)
//---------------------------------------------------------------------------

	TVPGetDisplayColorFormat(); // this will be ran only once here

	return cls;
}
//---------------------------------------------------------------------------


