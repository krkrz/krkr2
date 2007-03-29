//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" 描画デバイス管理
//---------------------------------------------------------------------------
#include <windows.h>

#include <vfw.h>
#include "BasicDrawDevice.h"
#include "PassThroughDrawDevice.h"
#define DIRECTDRAW_VERSION 0x0300
#include <ddraw.h>



//---------------------------------------------------------------------------
// オプション
//---------------------------------------------------------------------------
static bool TVPPassThroughOptionsInit = false;
static bool TVPWaitVSync = false;
static bool TVPZoomInterpolation = true;
static bool TVPForceDoublebuffer = false;
static bool TVPDBGDIPrefered = false;
//---------------------------------------------------------------------------
static void TVPInitPassThroughOptions()
{
	if(TVPPassThroughOptionsInit) return;

	bool initddraw = false;
	tTJSVariant val;

	if(TVPGetCommandLine(TJS_W("-waitvsync"), &val))
	{
		ttstr str(val);
		if(str == TJS_W("yes"))
		{
			TVPWaitVSync = true;
			initddraw = true;
		}
	}

	if(TVPGetCommandLine(TJS_W("-dbstyle"), &val) )
	{
		ttstr str(val);
		if(str == TJS_W("none") || str == TJS_W("no"))
			TVPForceDoublebuffer = false;
		if(str == TJS_W("auto") || str == TJS_W("gdi") || str == TJS_W("ddraw") || str == TJS_W("yes"))
			TVPForceDoublebuffer = true,
			initddraw = true;
		if(str == TJS_W("gdi")) TVPDBGDIPrefered = true;
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

	TVPPassThroughOptionsInit = true;
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
public:
	tTVPDrawer(tTVPPassThroughDrawDevice * device)
	{
		Device = device;
		SrcWidth = 0;
		SrcHeight = 0;
		DestRect.clear();
		TargetWindow = NULL;
		DrawDibHandle = NULL;
	} 
	virtual ~tTVPDrawer()
	{
		if(DrawDibHandle) DrawDibClose(DrawDibHandle), DrawDibHandle = NULL;
	}

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

public:
	//! @brief	コンストラクタ
	tTVPDrawer_DrawDibNoBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer_GDI(device)
	{
	}

	//! @brief	デストラクタ
	~tTVPDrawer_DrawDibNoBuffering()
	{
	}

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

public:
	//! @brief	コンストラクタ
	tTVPDrawer_DDDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TVPEnsureDirectDrawObject();
		OffScreenDC = NULL;
		Surface = NULL;
		Clipper = NULL;
	}

	//! @brief	デストラクタ
	~tTVPDrawer_DDDoubleBuffering()
	{
		DestroyOffScreenSurface();
	}

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
				Surface->Release(), Surface = NULL;
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
				Surface->Release(), Surface = NULL;
				TVPThrowExceptionMessage(TJS_W("Cannot allocate the surface on the local video memory"),
					TJSInt32ToHex(hr, 8));
			}


			// create clipper object
			hr = object->CreateClipper(0, &Clipper, NULL);
			if(hr != DD_OK)
			{
				Surface->Release(), Surface = NULL;
				TVPThrowExceptionMessage(TJS_W("Cannot create a clipper object/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}
			hr = Clipper->SetHWnd(0, TargetWindow);
			if(hr != DD_OK)
			{
				Clipper->Release(), Clipper = NULL;
				Surface->Release(), Surface = NULL;
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
			HDC dc;
			HRESULT hr = Surface->GetDC(&dc);
			if(hr == DDERR_SURFACELOST)
			{
				Surface->Restore();
				InvalidateAll();  // causes reconstruction of off-screen image
				hr = Surface->GetDC(&dc);
			}

			if(hr != DD_OK)
			{
				TVPThrowExceptionMessage(TJS_W("Off-screen surface, IDirectDrawSurface::GetDC failed/HR=%1"),
					TJSInt32ToHex(hr, 8));
			}

			OffScreenDC = dc;
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
tTVPPassThroughDrawDevice::tTVPPassThroughDrawDevice()
{
	TVPInitPassThroughOptions(); // read and initialize options
	TargetWindow = NULL;
	Drawer = NULL;
	DrawerType = dtNone;
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
void tTVPPassThroughDrawDevice::EnsureDrawer(tDrawerType failedtype)
{
	// このメソッドでは、以下の条件の際に drawer を作る(作り直す)。
	// 1. Drawer が NULL の場合
	// 2. 現在の Drawer のタイプが適切でなくなったとき

	if(TargetWindow)
	{
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
			if(!TVPZoomInterpolation || failedtype == dtDBDD || TVPDBGDIPrefered)
			{
				// 1) ズームで補完を行わない場合(補完なしはGDIでしかできない)
				// 2) DirectDraw でダブルバッファリングに失敗した場合
				// 3) GDIがオプションで指定された場合
				// のいずれかの場合は GDI を用いる
				newtype = dtDBGDI;
			}
			else
			{
				// そうでなければ DirectDraw
				newtype = dtDBDD;
			}
		}

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
					if(newtype == dtDBDD)
					{
						// DirectDraw の場合は GDI に fall back を試みる
						newtype = dtDBGDI;
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
		if(TVPWaitVSync)
		{
			tjs_int w, h;
			GetSrcSize(w, h);
			if(cliprect.get_width() == w && cliprect.get_height() == h)
			{
				// 全画面を更新するときだけ、vsync をまつ。
				IDirectDraw2 *object = TVPGetDirectDrawObjectNoAddRef();
				if(object) object->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
			}
		}

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









