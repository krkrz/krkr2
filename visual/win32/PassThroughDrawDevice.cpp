//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" 描画デバイス管理
//---------------------------------------------------------------------------

#include "tjsCommHead.h"
#include <vfw.h>
#include "DrawDevice.h"
#include "PassThroughDrawDevice.h"
#include "LayerIntf.h"
#include "MsgIntf.h"
#include "SysInitIntf.h"
#include "WindowIntf.h"
#include "DebugIntf.h"
#include "ThreadIntf.h"

#include <ddraw.h>
#include <d3d.h>


//---------------------------------------------------------------------------
// オプション
//---------------------------------------------------------------------------
static tjs_int TVPPassThroughOptionsGeneration = 0;
static bool TVPZoomInterpolation = true;
static bool TVPForceDoublebuffer = false;
static bool TVPDBGDIPrefered = false;
static bool TVPDBDDrawPrefered = false;
//---------------------------------------------------------------------------
static void TVPInitPassThroughOptions()
{
	if(TVPPassThroughOptionsGeneration == TVPGetCommandLineArgumentGeneration()) return;
	TVPPassThroughOptionsGeneration = TVPGetCommandLineArgumentGeneration();

	bool initddraw = false;
	tTJSVariant val;

	if(TVPGetCommandLine(TJS_W("-dbstyle"), &val) )
	{
		ttstr str(val);
		if(str == TJS_W("none") || str == TJS_W("no"))
			TVPForceDoublebuffer = false;
		if(str == TJS_W("auto") || str == TJS_W("gdi") ||
			str == TJS_W("ddraw") || str == TJS_W("ddd3d") || str == TJS_W("yes"))
			TVPForceDoublebuffer = true,
			initddraw = true;
		if(str == TJS_W("gdi")) TVPDBGDIPrefered = true;
		if(str == TJS_W("ddraw"))  TVPDBDDrawPrefered = true;
	}

	if(TVPGetCommandLine(TJS_W("-smoothzoom"), &val))
	{
		ttstr str(val);
		if(str == TJS_W("no"))
			TVPZoomInterpolation = false;
		else
			TVPZoomInterpolation = true;
	}

	if(initddraw) TVPEnsureDirectDrawObject();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief	PassThrough で用いる描画方法用インターフェース
//---------------------------------------------------------------------------
class tTVPDrawer
{
protected:
	tTVPPassThroughDrawDevice * Device;
	tTVPRect DestRect;
	tjs_int SrcWidth;
	tjs_int SrcHeight;
	HWND TargetWindow;
	HDRAWDIB DrawDibHandle;
	bool DrawUpdateRectangle;
public:
	tTVPDrawer(tTVPPassThroughDrawDevice * device)
	{
		Device = device;
		SrcWidth = 0;
		SrcHeight = 0;
		DestRect.clear();
		TargetWindow = NULL;
		DrawDibHandle = NULL;
		DrawUpdateRectangle = NULL;
	} 
	virtual ~tTVPDrawer()
	{
		if(DrawDibHandle) DrawDibClose(DrawDibHandle), DrawDibHandle = NULL;
	}
	virtual ttstr GetName() = 0;

	virtual bool SetDestRectangle(const tTVPRect & rect) { DestRect = rect; return true; }
	virtual bool NotifyLayerResize(tjs_int w, tjs_int h) { SrcWidth = w; SrcHeight = h; return true; }
	virtual void SetTargetWindow(HWND wnd)
	{
		if(DrawDibHandle) DrawDibClose(DrawDibHandle), DrawDibHandle = NULL;
		TargetWindow = wnd;
		DrawDibHandle = DrawDibOpen();
	}
	virtual void StartBitmapCompletion() = 0;
	virtual void NotifyBitmapCompleted(tjs_int x, tjs_int y,
		const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect) = 0;
	virtual void EndBitmapCompletion() = 0;
	virtual void Show() {;}
	virtual void SetShowUpdateRect(bool b)  { DrawUpdateRectangle = b; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	GDIによる描画を必要とする基本クラス
//---------------------------------------------------------------------------
class tTVPDrawer_GDI : public tTVPDrawer
{
	typedef tTVPDrawer inherited;
protected:
	HDC TargetDC;

public:
	//! @brief	コンストラクタ
	tTVPDrawer_GDI(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TargetDC = NULL;
	}

	//! @brief	デストラクタ
	~tTVPDrawer_GDI()
	{
		if(TargetDC && TargetWindow) ReleaseDC(TargetWindow, TargetDC);
	}

	void SetTargetWindow(HWND wnd)
	{
		if(wnd)
		{
			// 描画用 DC を取得する
			TargetDC = GetDC(wnd);
		}
		else
		{
			// 描画用 DC を開放する
			if(TargetDC) ReleaseDC(TargetWindow, TargetDC), TargetDC = NULL;
		}

		inherited::SetTargetWindow(wnd);

	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief	DrawDibによるバッファ無し描画を行う基本クラス
//---------------------------------------------------------------------------
class tTVPDrawer_DrawDibNoBuffering : public tTVPDrawer_GDI
{
	typedef tTVPDrawer_GDI inherited;

	HPEN BluePen;
	HPEN YellowPen;

public:
	//! @brief	コンストラクタ
	tTVPDrawer_DrawDibNoBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer_GDI(device)
	{
		BluePen = NULL;
		YellowPen = NULL;
	}

	//! @brief	デストラクタ
	~tTVPDrawer_DrawDibNoBuffering()
	{
		if(BluePen)   DeleteObject(BluePen);
		if(YellowPen) DeleteObject(YellowPen);
	}

	virtual ttstr GetName() { return TJS_W("DrawDIB (no buffering)"); }

	bool SetDestRectangle(const tTVPRect & rect)
	{
		// このデバイスでは拡大縮小はできないので
		// 拡大縮小が必要な場合は false を返す
		tjs_int w, h;
		Device->GetSrcSize(w, h);
		if(rect.get_width() != w || rect.get_height() != h)
			return false;

		return inherited::SetDestRectangle(rect);
	}

	bool NotifyLayerResize(tjs_int w, tjs_int h)
	{
		return inherited::NotifyLayerResize(w, h);
	}

	void SetTargetWindow(HWND wnd)
	{
		inherited::SetTargetWindow(wnd);
	}

	void StartBitmapCompletion()
	{
		// やることなし
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw にて TargetDC に描画を行う
		if(DrawDibHandle && TargetDC)
			DrawDibDraw(DrawDibHandle,
				TargetDC,
				x + DestRect.left,
				y + DestRect.top,
				cliprect.get_width(),
				cliprect.get_height(),
				const_cast<BITMAPINFOHEADER*>(reinterpret_cast<const BITMAPINFOHEADER*>(bitmapinfo)),
				const_cast<void*>(bits),
				cliprect.left,
				cliprect.top,
				cliprect.get_width(),
				cliprect.get_height(),
				0);

		// 更新矩形の表示
		if(DrawUpdateRectangle)
		{
			if(!BluePen) BluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			if(!YellowPen) YellowPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

			HPEN oldpen;

			int ROP_save = GetROP2(TargetDC);

			tjs_int rleft   = x + DestRect.left;
			tjs_int rtop    = y + DestRect.top;
			tjs_int rright  = rleft + cliprect.get_width();
			tjs_int rbottom = rtop  + cliprect.get_height();

			POINT points[5];
			points[0].x = rleft;
			points[0].y = rtop;
			points[1].x = rright -1;
			points[1].y = rtop;
			points[2].x = rright -1;
			points[2].y = rbottom -1;
			points[3].x = rleft;
			points[3].y = rbottom -1;
			points[4] = points[0];

			oldpen = SelectObject(TargetDC, BluePen);
			SetROP2(TargetDC, R2_NOTMASKPEN);
			Polyline(TargetDC, points, 4);

			SelectObject(TargetDC, YellowPen);
			SetROP2(TargetDC, R2_MERGEPEN);
			Polyline(TargetDC, points, 5);

			SelectObject(TargetDC, oldpen);
		}
	}

	void EndBitmapCompletion()
	{
		// やることなし
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief	GDIによるダブルバッファリングを行うクラス
//---------------------------------------------------------------------------
class tTVPDrawer_GDIDoubleBuffering : public tTVPDrawer_GDI
{
	typedef tTVPDrawer_GDI inherited;
	HBITMAP OffScreenBitmap; //!< オフスクリーンビットマップ
	HDC OffScreenDC; //!< オフスクリーン DC
	HBITMAP OldOffScreenBitmap; //!< OffScreenDC に以前選択されていた ビットマップ

public:
	//! @brief	コンストラクタ
	tTVPDrawer_GDIDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer_GDI(device)
	{
		OffScreenBitmap = NULL;
		OffScreenDC = NULL;
		OldOffScreenBitmap = NULL;
	}

	//! @brief	デストラクタ
	~tTVPDrawer_GDIDoubleBuffering()
	{
		DestroyBitmap();
	}

	virtual ttstr GetName() { return TJS_W("GDI double buffering"); }

	void DestroyBitmap()
	{
		if(OffScreenBitmap && OffScreenDC)
		{
			SelectObject(OffScreenDC, OldOffScreenBitmap), OldOffScreenBitmap = NULL;
			if(OffScreenBitmap) DeleteObject(OffScreenBitmap), OffScreenBitmap = NULL;
			if(OffScreenDC)     DeleteDC(OffScreenDC), OffScreenDC = NULL;
		}
	}

	void CreateBitmap()
	{
		// スクリーン互換の DDB を作成する。
		// これはたいていの場合、ビデオメモリ上に作成される。
		DestroyBitmap();
		if(TargetWindow && SrcWidth > 0 && SrcHeight > 0)
		{
			try
			{
				HDC screendc = GetDC(TargetWindow);
				if(!screendc) TVPThrowExceptionMessage(TJS_W("Failed to create screen DC"));
				OffScreenBitmap = CreateCompatibleBitmap(screendc, SrcWidth, SrcHeight);
				if(!OffScreenBitmap) TVPThrowExceptionMessage(TJS_W("Failed to create offscreen bitmap"));
				OffScreenDC     = CreateCompatibleDC(screendc);
				if(!OffScreenDC) TVPThrowExceptionMessage(TJS_W("Failed to create offscreen DC"));
				ReleaseDC(TargetWindow, screendc);
				OldOffScreenBitmap = SelectObject(OffScreenDC, OffScreenBitmap);
			}
			catch(...)
			{
				DestroyBitmap();
				throw;
			}
		}
	}

	bool SetDestRectangle(const tTVPRect & rect)
	{
		return inherited::SetDestRectangle(rect);
	}

	bool NotifyLayerResize(tjs_int w, tjs_int h)
	{
		if(inherited::NotifyLayerResize(w, h))
		{
			CreateBitmap();
			return true;
		}
		return false;
	}

	void SetTargetWindow(HWND wnd)
	{
		inherited::SetTargetWindow(wnd);
		CreateBitmap();
	}

	void StartBitmapCompletion()
	{
		// やることなし
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw にて OffScreenDC に描画を行う
		if(DrawDibHandle && OffScreenDC)
			DrawDibDraw(DrawDibHandle,
				OffScreenDC,
				x,
				y,
				cliprect.get_width(),
				cliprect.get_height(),
				const_cast<BITMAPINFOHEADER*>(reinterpret_cast<const BITMAPINFOHEADER*>(bitmapinfo)),
				const_cast<void*>(bits),
				cliprect.left,
				cliprect.top,
				cliprect.get_width(),
				cliprect.get_height(),
				0);
	}

	void EndBitmapCompletion()
	{
	}

	void Show()
	{
		if(TargetDC)
		{
			// オフスクリーンビットマップを TargetDC に転送する
			if(TVPZoomInterpolation)
				SetStretchBltMode(TargetDC, HALFTONE);
			else
				SetStretchBltMode(TargetDC, COLORONCOLOR);
			SetBrushOrgEx(TargetDC, 0, 0, NULL);

			StretchBlt(TargetDC,
				DestRect.left,
				DestRect.top,
				DestRect.get_width(),
				DestRect.get_height(),
				OffScreenDC,
				0,
				0,
				SrcWidth,
				SrcHeight,
				SRCCOPY);
		}
	}

};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief	DirectDrawによるダブルバッファリングを行うクラス
//---------------------------------------------------------------------------
class tTVPDrawer_DDDoubleBuffering : public tTVPDrawer
{
	typedef tTVPDrawer inherited;

	HDC OffScreenDC;
	IDirectDrawSurface * Surface;
	IDirectDrawClipper * Clipper;

	bool LastOffScreenDCGot;

public:
	//! @brief	コンストラクタ
	tTVPDrawer_DDDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TVPEnsureDirectDrawObject();
		OffScreenDC = NULL;
		Surface = NULL;
		Clipper = NULL;
		LastOffScreenDCGot = true;
	}

	//! @brief	デストラクタ
	~tTVPDrawer_DDDoubleBuffering()
	{
		DestroyOffScreenSurface();
	}

	virtual ttstr GetName() { return TJS_W("DirectDraw double buffering"); }

	void DestroyOffScreenSurface()
	{
		if(OffScreenDC && Surface) Surface->ReleaseDC(OffScreenDC);
		if(Surface) Surface->Release(), Surface = NULL;
		if(Clipper) Clipper->Release(), Clipper = NULL;
	}

	void InvalidateAll()
	{
		// レイヤ演算結果をすべてリクエストする
		// サーフェースが lost した際に内容を再構築する目的で用いる
		Device->RequestInvalidation(tTVPRect(0, 0, DestRect.get_width(), DestRect.get_height()));
	}

	void CreateOffScreenSurface()
	{
		// オフスクリーンサーフェースを設定する
		DestroyOffScreenSurface();
		if(TargetWindow && SrcWidth > 0 && SrcHeight > 0)
		{
			IDirectDraw2 *object = TVPGetDirectDrawObjectNoAddRef();
			if(!object) TVPThrowExceptionMessage(TJS_W("DirectDraw not available"));

			// allocate secondary off-screen buffer
			DDSURFACEDESC ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			ddsd.dwWidth = SrcWidth;
			ddsd.dwHeight = SrcHeight;
			ddsd.ddsCaps.dwCaps =
				DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
			HRESULT hr;

			hr = object->CreateSurface(&ddsd, &Surface, NULL);

			if(hr != DD_OK)
				TVPThrowExceptionMessage(TJS_W("Cannot allocate off-screen surface/HR=%1"),
					TJSInt32ToHex(hr, 8));

			// check whether the surface is on video memory
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);

			hr = Surface->GetSurfaceDesc(&ddsd);

			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface description/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}

			if(ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY &&
				ddsd.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM)
			{
				// ok
			}
			else
			{
				TVPThrowExceptionMessage(TJS_W("Cannot allocate the surface on the local video memory"),
					TJSInt32ToHex(hr, 8));
			}


			// create clipper object
			hr = object->CreateClipper(0, &Clipper, NULL);
			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot create a clipper object/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}
			hr = Clipper->SetHWnd(0, TargetWindow);
			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot set the window handle to the clipper object/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}
		}
	}

	bool SetDestRectangle(const tTVPRect & rect)
	{
		return inherited::SetDestRectangle(rect);
	}

	bool NotifyLayerResize(tjs_int w, tjs_int h)
	{
		if(inherited::NotifyLayerResize(w, h))
		{
			try
			{
				CreateOffScreenSurface();
			}
			catch(const eTJS & e)
			{
				TVPAddLog(TJS_W("Failed to create DirectDraw off-screen buffer: ") + e.GetMessage());
				return false;
			}
			catch(...)
			{
				TVPAddLog(TJS_W("Failed to create DirectDraw off-screen buffer: unknown reason"));
				return false;
			}
			return true;
		}
		return false;
	}

	void SetTargetWindow(HWND wnd)
	{
		inherited::SetTargetWindow(wnd);
		CreateOffScreenSurface();
	}

	void StartBitmapCompletion()
	{
		// retrieve DC
		if(Surface && TargetWindow)
		{
			HDC dc = NULL;
			HRESULT hr = Surface->GetDC(&dc);
			if(hr == DDERR_SURFACELOST)
			{
				Surface->Restore();
				InvalidateAll();  // causes reconstruction of off-screen image
				hr = Surface->GetDC(&dc);
			}

			if(hr != DD_OK)
			{
				dc = NULL;
				InvalidateAll();  // causes reconstruction of off-screen image

				if(LastOffScreenDCGot)
				{
					// display this message only once since last success
					TVPAddLog(
						TJS_W("(inf) Off-screen surface, IDirectDrawSurface::GetDC failed/HR=") +
						TJSInt32ToHex(hr, 8) + TJS_W(", ignoring"));
				}
			}

			OffScreenDC = dc;

			if(OffScreenDC) LastOffScreenDCGot = true; else LastOffScreenDCGot = false;
		}
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw にて OffScreenDC に描画を行う
		if(DrawDibHandle && OffScreenDC && TargetWindow)
			DrawDibDraw(DrawDibHandle,
				OffScreenDC,
				x,
				y,
				cliprect.get_width(),
				cliprect.get_height(),
				const_cast<BITMAPINFOHEADER*>(reinterpret_cast<const BITMAPINFOHEADER*>(bitmapinfo)),
				const_cast<void*>(bits),
				cliprect.left,
				cliprect.top,
				cliprect.get_width(),
				cliprect.get_height(),
				0);
	}

	void EndBitmapCompletion()
	{
		if(!TargetWindow) return;
		if(!Surface) return;
		if(!OffScreenDC) return;

		Surface->ReleaseDC(OffScreenDC), OffScreenDC = NULL;
	}

	void Show()
	{
		if(!TargetWindow) return;
		if(!Surface) return;

		// Blt to the primary surface
		IDirectDrawSurface *pri = TVPGetDDPrimarySurfaceNoAddRef();
		if(!pri)
			TVPThrowExceptionMessage(TJS_W("Cannot retrieve primary surface object"));

		// set clipper
		TVPSetDDPrimaryClipper(Clipper);

		// get PaintBox's origin
		POINT origin; origin.x = DestRect.left, origin.y = DestRect.top;
		ClientToScreen(TargetWindow, &origin);

		// entire of the bitmap is to be transfered (this is not optimal. FIX ME!)
		RECT drect;
		drect.left   = origin.x;
		drect.top    = origin.y;
		drect.right  = origin.x + DestRect.get_width();
		drect.bottom = origin.y + DestRect.get_height();

		RECT srect;
		srect.left = 0;
		srect.top = 0;
		srect.right  = SrcWidth;
		srect.bottom = SrcHeight;

		HRESULT hr = pri->Blt(&drect, Surface, &srect, DDBLT_WAIT, NULL);
		if(hr == DDERR_SURFACELOST)
		{
			pri->Restore();
			Surface->Restore();
			InvalidateAll();  // causes reconstruction of off-screen image
		}
		else if(hr == DDERR_INVALIDRECT)
		{
			// ignore this error
		}
		else if(hr != DD_OK)
		{
			TVPThrowExceptionMessage(TJS_W("Primary surface, IDirectDrawSurface::Blt failed/HR=%1"),
				TJSInt32ToHex(hr, 8));
		}
	}

};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief	Direct3D7 によるダブルバッファリングを行うクラス
//! @note	tTVPDrawer_DDDoubleBuffering とよく似ているが別クラスになっている。
//!			修正を行う場合は、互いによく見比べ、似たようなところがあればともに修正を試みること。
//---------------------------------------------------------------------------
class tTVPDrawer_D3DDoubleBuffering : public tTVPDrawer
{
	typedef tTVPDrawer inherited;

/*
	note: Texture に対していったん描画された内容は Surface に転送され、
			さらにそこからプライマリサーフェースにコピーされる。
*/

	HDC OffScreenDC;
	IDirectDraw7 * DirectDraw7;
	IDirect3D7 * Direct3D7;
	IDirect3DDevice7 * Direct3DDevice7;
	IDirectDrawSurface7 * Surface;
	IDirectDrawClipper * Clipper;
	IDirectDrawSurface7 * Texture;

	bool LastOffScreenDCGot;

public:
	//! @brief	コンストラクタ
	tTVPDrawer_D3DDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TVPEnsureDirectDrawObject();
		OffScreenDC = NULL;
		DirectDraw7 = NULL;
		Direct3D7 = NULL;
		Surface = NULL;
		Clipper = NULL;
		Texture = NULL;
		Direct3DDevice7 = NULL;
		LastOffScreenDCGot = true;
	}

	//! @brief	デストラクタ
	~tTVPDrawer_D3DDoubleBuffering()
	{
		DestroyOffScreenSurface();
	}

	virtual ttstr GetName() { return TJS_W("Direct3D double buffering"); }

	void DestroyOffScreenSurface()
	{
		if(OffScreenDC && Surface) Surface->ReleaseDC(OffScreenDC);
		if(Surface) Surface->Release(), Surface = NULL;
		if(Clipper) Clipper->Release(), Clipper = NULL;
		if(Texture) Texture->Release(), Texture = NULL;
		if(DirectDraw7) DirectDraw7->Release(), DirectDraw7 = NULL;
		if(Direct3DDevice7) Direct3DDevice7->Release(), Direct3DDevice7 = NULL;
		if(Direct3D7) Direct3D7->Release(), Direct3D7 = NULL;
	}

	void InvalidateAll()
	{
		// レイヤ演算結果をすべてリクエストする
		// サーフェースが lost した際に内容を再構築する目的で用いる
		Device->RequestInvalidation(tTVPRect(0, 0, DestRect.get_width(), DestRect.get_height()));
	}

	void CreateOffScreenSurface()
	{
		// Direct3D デバイス、テクスチャなどを作成する
		DestroyOffScreenSurface();
		if(TargetWindow && SrcWidth > 0 && SrcHeight > 0)
		{
			HRESULT hr;

			// get DirectDraw7/Direct3D7 interface
			DirectDraw7 = TVPGetDirectDraw7ObjectNoAddRef();
			if(!DirectDraw7) TVPThrowExceptionMessage(TJS_W("DirectDraw7 not available"));

			DirectDraw7->AddRef();

			hr = DirectDraw7->QueryInterface(IID_IDirect3D7, (void**)&Direct3D7);
			if(FAILED(hr))
				TVPThrowExceptionMessage(TJS_W("Direct3D7 not available"));

			// check display mode
			DDSURFACEDESC2 ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			hr = DirectDraw7->GetDisplayMode(&ddsd);
			if(FAILED(hr) || ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
				TVPThrowExceptionMessage(TJS_W("Too less display color depth"));

			// create clipper object
			hr = DirectDraw7->CreateClipper(0, &Clipper, NULL);
			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot create a clipper object/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}
			hr = Clipper->SetHWnd(0, TargetWindow);
			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot set the window handle to the clipper object/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}

			// allocate secondary off-screen buffer
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			ddsd.dwWidth = DestRect.get_width();
			ddsd.dwHeight = DestRect.get_height();
			ddsd.ddsCaps.dwCaps =
				/*DDSCAPS_OFFSCREENPLAIN |*/ DDSCAPS_VIDEOMEMORY /*| DDSCAPS_LOCALVIDMEM*/ | DDSCAPS_3DDEVICE;

			hr = DirectDraw7->CreateSurface(&ddsd, &Surface, NULL);

			if(hr != DD_OK)
				TVPThrowExceptionMessage(TJS_W("Cannot allocate D3D off-screen surface/HR=%1"),
					TJSInt32ToHex(hr, 8));

			// check whether the surface is on video memory
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);

			hr = Surface->GetSurfaceDesc(&ddsd);

			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get D3D surface description/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}

			if(ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY &&
				ddsd.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM)
			{
				// ok
			}
			else
			{
				TVPThrowExceptionMessage(TJS_W("Cannot allocate the D3D surface on the local video memory"),
					TJSInt32ToHex(hr, 8));
			}

			// create Direct3D Device
			hr = Direct3D7->CreateDevice(IID_IDirect3DHALDevice, Surface, &Direct3DDevice7);
			if(FAILED(hr))
				TVPThrowExceptionMessage(TJS_W("Cannot create Direct3D device/HR=%1"),
					TJSInt32ToHex(hr, 8));

			// create Direct3D Texture
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS/* | DDSD_PIXELFORMAT*/;
			ddsd.dwWidth = SrcWidth;
			ddsd.dwHeight = SrcHeight;
			ddsd.ddsCaps.dwCaps =
				/*DDSCAPS_OFFSCREENPLAIN |*/ DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
/*
			ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 32;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0x00FF0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x0000FF00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x000000FF;
*/
			hr = DirectDraw7->CreateSurface(&ddsd, &Texture, NULL);

			if(hr != DD_OK)
				TVPThrowExceptionMessage(TJS_W("Cannot allocate D3D texture/HR=%1"),
					TJSInt32ToHex(hr, 8));

		}
	}

	bool SetDestRectangle(const tTVPRect & rect)
	{
	return false;
		if(inherited::SetDestRectangle(rect))
		{
			try
			{
				CreateOffScreenSurface();
			}
			catch(const eTJS & e)
			{
				TVPAddLog(TJS_W("Failed to create Direct3D devices: ") + e.GetMessage());
				return false;
			}
			catch(...)
			{
				TVPAddLog(TJS_W("Failed to create Direct3D devices: unknown reason"));
				return false;
			}
			return true;
		}
		return false;
	}

	bool NotifyLayerResize(tjs_int w, tjs_int h)
	{
		if(inherited::NotifyLayerResize(w, h))
		{
			try
			{
				CreateOffScreenSurface();
			}
			catch(const eTJS & e)
			{
				TVPAddLog(TJS_W("Failed to create Direct3D devices: ") + e.GetMessage());
				return false;
			}
			catch(...)
			{
				TVPAddLog(TJS_W("Failed to create Direct3D devices: unknown reason"));
				return false;
			}
			return true;
		}
		return false;
	}

	void SetTargetWindow(HWND wnd)
	{
		inherited::SetTargetWindow(wnd);
		CreateOffScreenSurface();
	}

	void StartBitmapCompletion()
	{
		// retrieve DC
		if(Texture && TargetWindow)
		{
			HDC dc = NULL;
			HRESULT hr = Texture->GetDC(&dc);
			if(hr == DDERR_SURFACELOST)
			{
				Texture->Restore();
				InvalidateAll();  // causes reconstruction of off-screen image
				hr = Texture->GetDC(&dc);
			}

			if(hr != DD_OK)
			{
				dc = NULL;
				InvalidateAll();  // causes reconstruction of off-screen image

				if(LastOffScreenDCGot)
				{
					// display this message only once since last success
					TVPAddLog(
						TJS_W("(inf) Texture, IDirectDrawSurface::GetDC failed/HR=") +
						TJSInt32ToHex(hr, 8) + TJS_W(", ignoring"));
				}
			}

			OffScreenDC = dc;

			if(OffScreenDC) LastOffScreenDCGot = true; else LastOffScreenDCGot = false;
		}
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw にて OffScreenDC に描画を行う
		if(DrawDibHandle && OffScreenDC && TargetWindow)
			DrawDibDraw(DrawDibHandle,
				OffScreenDC,
				x,
				y,
				cliprect.get_width(),
				cliprect.get_height(),
				const_cast<BITMAPINFOHEADER*>(reinterpret_cast<const BITMAPINFOHEADER*>(bitmapinfo)),
				const_cast<void*>(bits),
				cliprect.left,
				cliprect.top,
				cliprect.get_width(),
				cliprect.get_height(),
				0);
	}

	void EndBitmapCompletion()
	{
		if(!TargetWindow) return;
		if(!Texture) return;
		if(!Surface) return;
		if(!OffScreenDC) return;
		if(!Direct3DDevice7) return;

		Texture->ReleaseDC(OffScreenDC), OffScreenDC = NULL;

		// Blt to the primary surface

		// Blt texture to surface

		//- build vertex list
		struct tVertices
		{
			float x, y, z, rhw;
			float tu, tv;
		};

		float dw = (float)DestRect.get_width();
		float dh = (float)DestRect.get_height();

		float sw = (float)SrcWidth;
		float sh = (float)SrcHeight;


		tVertices vertices[] =
		{
			{0.0f - 0.5f, 0.0f - 0.5f, 1.0f, 1.0f, 0.0f, 0.0f},
			{dw   - 0.5f, 0.0f - 0.5f, 1.0f, 1.0f, 1.0f, 0.0f},
			{0.0f - 0.5f, dh   - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f},
			{dw   - 0.5f, dh   - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f}
		};

		HRESULT hr;

		// clear back surface
		DDBLTFX fx;
		ZeroMemory(&fx, sizeof(fx));
		fx.dwSize = sizeof(fx);
		fx.dwFillColor = 0;
		Surface->Blt(NULL, NULL, NULL, DDBLT_WAIT|DDBLT_COLORFILL, &fx);

		//- draw as triangles
		hr = Direct3DDevice7->SetTexture(0, Texture);
		if(FAILED(hr)) goto got_error;

		Direct3DDevice7->SetRenderState(D3DRENDERSTATE_LIGHTING			, FALSE);
		Direct3DDevice7->SetRenderState(D3DRENDERSTATE_BLENDENABLE		, FALSE);
		Direct3DDevice7->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE	, FALSE); 
		Direct3DDevice7->SetRenderState(D3DRENDERSTATE_CULLMODE			, D3DCULL_NONE);

		Direct3DDevice7->SetTextureStageState(0, D3DTSS_MAGFILTER,
			TVPZoomInterpolation ?  D3DTFG_LINEAR : D3DTFG_POINT);
		Direct3DDevice7->SetTextureStageState(0, D3DTSS_MINFILTER,
			TVPZoomInterpolation ?  D3DTFN_LINEAR : D3DTFN_POINT);
		Direct3DDevice7->SetTextureStageState(0, D3DTSS_MIPFILTER,
			TVPZoomInterpolation ?  D3DTFP_LINEAR : D3DTFP_POINT);
		Direct3DDevice7->SetTextureStageState(0, D3DTSS_ADDRESS  , D3DTADDRESS_CLAMP);

		hr = Direct3DDevice7->BeginScene();
		if(FAILED(hr)) goto got_error;

		hr = Direct3DDevice7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_TEX1,
												vertices, 4, D3DDP_WAIT);
		if(FAILED(hr)) goto got_error;

		Direct3DDevice7->EndScene();
		Direct3DDevice7->SetTexture(0, NULL);

	got_error:
		if(hr == DDERR_SURFACELOST)
		{
			Surface->Restore();
			Texture->Restore();
			InvalidateAll();  // causes reconstruction of off-screen image
		}
		else if(hr == DDERR_INVALIDRECT)
		{
			// ignore this error
		}
		else if(hr != DD_OK)
		{
			TVPThrowExceptionMessage(TJS_W("Primary surface, polygon drawing failed/HR=%1"),
				TJSInt32ToHex(hr, 8));
		}
	}


	void Show()
	{
		if(!TargetWindow) return;
		if(!Texture) return;
		if(!Surface) return;
		if(!Direct3DDevice7) return;

		HRESULT hr;

		// retrieve the primary surface
		IDirectDrawSurface *pri = TVPGetDDPrimarySurfaceNoAddRef();
		if(!pri)
			TVPThrowExceptionMessage(TJS_W("Cannot retrieve primary surface object"));

		// set clipper
		TVPSetDDPrimaryClipper(Clipper);

		// get PaintBox's origin
		POINT origin; origin.x = DestRect.left, origin.y = DestRect.top;
		ClientToScreen(TargetWindow, &origin);

		// entire of the bitmap is to be transfered (this is not optimal. FIX ME!)
		RECT drect;
		drect.left   = origin.x;
		drect.top    = origin.y;
		drect.right  = origin.x + DestRect.get_width();
		drect.bottom = origin.y + DestRect.get_height();

		RECT srect;
		srect.left   = 0;
		srect.top    = 0;
		srect.right  = DestRect.get_width();
		srect.bottom = DestRect.get_height();

		hr = pri->Blt(&drect, (IDirectDrawSurface*)Surface, &srect, DDBLT_WAIT, NULL);


	got_error:
		if(hr == DDERR_SURFACELOST)
		{
			pri->Restore();
			Surface->Restore();
			Texture->Restore();
			InvalidateAll();  // causes reconstruction of off-screen image
		}
		else if(hr == DDERR_INVALIDRECT)
		{
			// ignore this error
		}
		else if(hr != DD_OK)
		{
			TVPThrowExceptionMessage(TJS_W("Primary surface, IDirectDrawSurface::Blt failed/HR=%1"),
				TJSInt32ToHex(hr, 8));
		}
	}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tTVPPassThroughDrawDevice::tTVPPassThroughDrawDevice()
{
	TVPInitPassThroughOptions(); // read and initialize options
	TargetWindow = NULL;
	Drawer = NULL;
	DrawerType = dtNone;
	DDFailed = false;
	D3DFailed = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPPassThroughDrawDevice::~tTVPPassThroughDrawDevice()
{
	DestroyDrawer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPPassThroughDrawDevice::DestroyDrawer()
{
	if(Drawer) delete Drawer, Drawer = NULL;
	DrawerType = dtNone;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPPassThroughDrawDevice::tDrawerType
	tTVPPassThroughDrawDevice::SelectNewDrawerType(tDrawerType failedtype)
{
	// 失敗したタイプに対応するフラグを立てる
	if(failedtype == dtDBDD)  DDFailed = true;
	if(failedtype == dtDBD3D) D3DFailed = true;

	// drawer の種類を特定する
	tDrawerType newtype = dtDrawDib;

	// ズームは必要？
	bool zoom_required = false;
	tjs_int srcw, srch;
	GetSrcSize(srcw, srch);
	if(DestRect.get_width() != srcw || DestRect.get_height() != srch)
		zoom_required = true;

	if(!zoom_required && !TVPForceDoublebuffer)
	{
		// ズームが必要なく、かつ、ダブルバッファも要求されていない場合
		newtype = dtDrawDib;
	}
	else
	{
		// ズームが必要か、ダブルバッファが必要
		switch(failedtype)
		{
		case dtNone:
			// 前回が失敗していない場合は、好みに従って
			// デフォルトのタイプを設定する
			if(TVPDBDDrawPrefered || !zoom_required)
				newtype = dtDBDD;
			else if(TVPDBGDIPrefered)
				newtype = dtDBGDI;
			else
				newtype = dtDBD3D;
			break;
		case dtDrawDib:
			newtype = dtDrawDib; // これ以上fallbackできず
			break;
		case dtDBGDI:
			newtype = dtDBGDI; // これもこれ以上 fallback できない
			break;
		case dtDBDD:
			newtype = dtDBGDI;
			break;
		case dtDBD3D:
			newtype = dtDBD3D;
			break;
		}

		// 過去に失敗したタイプは二度と試みない
		if(newtype == dtDBD3D && D3DFailed)
			newtype = dtDBDD;
		if(newtype == dtDBDD && DDFailed)
			newtype = dtDBGDI;

		if((!TVPZoomInterpolation && newtype == dtDBDD))
		{
			// ズームで補完を行わない場合かつDirectDrawを使おうとしている場合
			// これはできないので dtDBGBI にフォールバックする
			newtype = dtDBGDI;
		}
	}

	return newtype;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPPassThroughDrawDevice::EnsureDrawer(tDrawerType failedtype)
{
	// このメソッドでは、以下の条件の際に drawer を作る(作り直す)。
	// 1. Drawer が NULL の場合
	// 2. 現在の Drawer のタイプが適切でなくなったとき
	TVPInitPassThroughOptions();

	tjs_int srcw, srch;
	GetSrcSize(srcw, srch);

	if(TargetWindow)
	{
		tDrawerType newtype = SelectNewDrawerType(failedtype);

		if(newtype != DrawerType)
		{
			// drawer がない (つまりDrawerType が dtNoneなのでnewtypeに一致しない)
			// あるいは DrawerType が newtype と違う場合
			if(Drawer) delete Drawer, Drawer = NULL;

			bool success;
			do
			{
				success = true;
				try
				{
					switch(newtype)
					{
					case dtNone:
						break;
					case dtDrawDib:
						Drawer = new tTVPDrawer_DrawDibNoBuffering(this);
						break;
					case dtDBGDI:
						Drawer = new tTVPDrawer_GDIDoubleBuffering(this);
						break;
					case dtDBDD:
						Drawer = new tTVPDrawer_DDDoubleBuffering(this);
						break;
					case dtDBD3D:
						Drawer = new tTVPDrawer_D3DDoubleBuffering(this);
						break;
					}
					Drawer->SetTargetWindow(TargetWindow);
					if(!Drawer->SetDestRectangle(DestRect))
						TVPThrowExceptionMessage(
							TJS_W("Failed to set destination rectangle to draw device drawer"));
					if(!Drawer->NotifyLayerResize(srcw, srch))
						TVPThrowExceptionMessage(
							TJS_W("Failed to set source layer size to draw device drawer"));
				}
				catch(...)
				{
					// 例外が発生した。
					if(newtype == dtDBDD || newtype == dtDBD3D)
					{
						// DirectDraw/Direct3D の場合は他のタイプにフォールバックを試みる
						newtype = SelectNewDrawerType(newtype);
						success = false;
					}
					else
					{
						// それ以外は fall back のしようがない
						throw;
					}
				}
			} while(!success);

			DrawerType = newtype;
			TVPAddLog(TJS_W("Using passthrough draw device: ") + Drawer->GetName());
		} // if(newtype != DrawerType)
	} // if(TargetWindow)

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	if(inherited::Managers.size() > 0)
	{
		// "Pass Through" デバイスでは２つ以上のLayer Managerを登録できない
		TVPThrowExceptionMessage(TJS_W("\"passthrough\" device does not support layer manager more than 1"));
			// TODO: i18n
	}
	inherited::AddLayerManager(manager);

	manager->SetDesiredLayerType(ltOpaque); // ltOpaque な出力を受け取りたい
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::SetTargetWindow(HWND wnd)
{
	TVPInitPassThroughOptions();
	DestroyDrawer();
	TargetWindow = wnd;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::SetDestRectangle(const tTVPRect & rect)
{
	inherited::SetDestRectangle(rect);

	// 拡大や縮小がある場合はdrawerを作り直さなければならないかもしれない
	if(Drawer)
	{
		if(!Drawer->SetDestRectangle(rect))
		{
			tDrawerType failedtype = DrawerType; // 失敗した drawer
			DestroyDrawer(); // DestRect の設定に失敗した場合は drawer を作り直す
			EnsureDrawer(failedtype);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::NotifyLayerResize(iTVPLayerManager * manager)
{
	inherited::NotifyLayerResize(manager);

	// 拡大や縮小がある場合はdrawerを作り直さなければならないかもしれない
	if(Drawer)
	{
		tjs_int w, h;
		GetSrcSize(w, h);
		if(!Drawer->NotifyLayerResize(w, h))
		{
			tDrawerType failedtype = DrawerType; // 失敗した drawer
			DestroyDrawer(); // DestRect の設定に失敗した場合は drawer を作り直す
			EnsureDrawer(failedtype);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::Show()
{
	if(Drawer)
	{
		Drawer->Show();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	EnsureDrawer(dtNone);

	if(Drawer) Drawer->StartBitmapCompletion();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo で表されるビットマップの cliprect の領域を、x, y に描画
	// する。
	// opacity と type は無視するしかないので無視する
	if(Drawer)
	{
		TVPInitPassThroughOptions();
		Drawer->NotifyBitmapCompleted(x, y, bits, bitmapinfo, cliprect);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	if(Drawer) Drawer->EndBitmapCompletion();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::SetShowUpdateRect(bool b)
{
	if(Drawer) Drawer->SetShowUpdateRect(b);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// tTJSNC_PassThroughDrawDevice : PassThroughDrawDevice TJS native class
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_PassThroughDrawDevice::ClassID = (tjs_uint32)-1;
tTJSNC_PassThroughDrawDevice::tTJSNC_PassThroughDrawDevice() :
	tTJSNativeClass(TJS_W("PassThroughDrawDevice"))
{
	// register native methods/properties

	TJS_BEGIN_NATIVE_MEMBERS(PassThroughDrawDevice)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
// constructor/methods
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_PassThroughDrawDevice,
	/*TJS class name*/PassThroughDrawDevice)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/PassThroughDrawDevice)
//----------------------------------------------------------------------

//---------------------------------------------------------------------------
//----------------------------------------------------------------------
// properties
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(interface)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PassThroughDrawDevice);
		*result = reinterpret_cast<tjs_int64>(_this->GetDevice());
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(interface)

//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
iTJSNativeInstance *tTJSNC_PassThroughDrawDevice::CreateNativeInstance()
{
	return new tTJSNI_PassThroughDrawDevice();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tTJSNI_PassThroughDrawDevice::tTJSNI_PassThroughDrawDevice()
{
	Device = new tTVPPassThroughDrawDevice();
}
//---------------------------------------------------------------------------
tTJSNI_PassThroughDrawDevice::~tTJSNI_PassThroughDrawDevice()
{
	if(Device) Device->Destruct(), Device = NULL;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNI_PassThroughDrawDevice::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_PassThroughDrawDevice::Invalidate()
{
	if(Device) Device->Destruct(), Device = NULL;
}
//---------------------------------------------------------------------------

