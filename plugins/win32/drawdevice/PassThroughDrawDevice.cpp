//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include "tp_stub.h"
#include "PassThroughDrawDevice.h"
#include <vfw.h>
#include <ddraw.h>
#include <d3d.h>

/*
	PassThroughDrawDevice �N���X�ɂ́AWindow.PassThroughDrawDevice �Ƃ���
	�A�N�Z�X�ł���B�ʏ�AWindow �N���X�𐶐�����ƁA���� drawDevice �v���p
	�e�B�ɂ͎����I�ɂ��̃N���X�̃C���X�^���X���ݒ肳���̂ŁA(�ق���DrawDevice
	���g��Ȃ������) ���Ɉӎ�����K�v�͂Ȃ��B

	PassThroughDrawDevice �͈ȉ��̃��\�b�h�ƃv���p�e�B�����B

	recreate()
		Drawer (�����Ŏg�p���Ă���`���) ��؂�ւ���BpreferredDrawer �v���p�e�B
		�� dtNone �ȊO�ł���΂���ɏ]�����A�K���w�肳�ꂽ drawer ���g�p�����ۏ؂͂Ȃ��B

	preferredDrawer
		�g�p������ drawer ��\���v���p�e�B�B�ȉ��̂����ꂩ�̒l���Ƃ�B
		�l��ݒ肷�邱�Ƃ��\�Bnew ����̒l�� �R�}���h���C���I�v�V������ dbstyle ��
		�ݒ肵���l�ɂȂ�B
		drawer�����̒l�ɂȂ�ۏ؂͂Ȃ� (���Ƃ��� dtDBD3D ���w�肵�Ă��Ă����炩��
		����� Direct3D �̏���Ɏ��s�����ꍇ�� DirectDraw ���g�p�����\��������)�B
		�E�B���h�E�쐬����A�ŏ��Ƀv���C�}�����C�����쐬��������O�ɂ��̃v���p�e�B��
		�ݒ肷�鎖�ɂ��Arecreate() ���킴�킴���s���Ȃ��Ă��w��� drawer ���g�p
		�����邱�Ƃ��ł���B
		Window.PassThroughDrawDevice.dtNone			�w�肵�Ȃ�
		Window.PassThroughDrawDevice.dtDrawDib		�g��k�����K�v�ȏꍇ��GDI�A
													�����łȂ����DB�Ȃ�
		Window.PassThroughDrawDevice.dtDBGDI		GDI�ɂ��DB
		Window.PassThroughDrawDevice.dtDBDD			DirectDraw�ɂ��DB
		Window.PassThroughDrawDevice.dtDBD3D		Direct3D�ɂ��DB

	drawer
		���ݎg�p����Ă��� drawer ��\���v���p�e�B�B�ȉ��̂����ꂩ�̒l���Ƃ�B
		�ǂݎ���p�B
		Window.PassThroughDrawDevice.dtNone			���ʂ͂���͂Ȃ�
		Window.PassThroughDrawDevice.dtDrawDib		�_�u���o�b�t�@�����O(DB)�Ȃ�
		Window.PassThroughDrawDevice.dtDBGDI		GDI�ɂ��DB
		Window.PassThroughDrawDevice.dtDBDD			DirectDraw�ɂ��DB
		Window.PassThroughDrawDevice.dtDBD3D		Direct3D�ɂ��DB
*/

//---------------------------------------------------------------------------
// �I�v�V����
//---------------------------------------------------------------------------
static tjs_int TVPPassThroughOptionsGeneration = 0;
static bool TVPZoomInterpolation = true;
static bool TVPForceDoublebuffer = false;
static tTVPPassThroughDrawDevice::tDrawerType TVPPreferredDrawType = tTVPPassThroughDrawDevice::dtNone;
//---------------------------------------------------------------------------
static void TVPInitPassThroughOptions()
{
	if(TVPPassThroughOptionsGeneration == TVPGetCommandLineArgumentGeneration()) return;
	TVPPassThroughOptionsGeneration = TVPGetCommandLineArgumentGeneration();

	bool initddraw = false;
	tTJSVariant val;

	TVPForceDoublebuffer = false;
	if(TVPGetCommandLine(TJS_W("-usedb"), &val) )
	{
		ttstr str(val);
		if(str == TJS_W("yes")) TVPForceDoublebuffer = true;
	}

	TVPPreferredDrawType = tTVPPassThroughDrawDevice::dtNone;
	if(TVPGetCommandLine(TJS_W("-dbstyle"), &val) )
	{
		ttstr str(val);
		if(str == TJS_W("none") || str == TJS_W("no") || str == TJS_W("auto"))
			TVPPreferredDrawType = tTVPPassThroughDrawDevice::dtNone;
		if(str == TJS_W("gdi"))
			TVPPreferredDrawType = tTVPPassThroughDrawDevice::dtDBGDI;
		if(str == TJS_W("ddraw"))
			TVPPreferredDrawType = tTVPPassThroughDrawDevice::dtDBDD;
		if(str == TJS_W("d3d"))
			TVPPreferredDrawType = tTVPPassThroughDrawDevice::dtDBD3D;
	}

	TVPZoomInterpolation = true;
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
//! @brief	PassThrough �ŗp����`���@�p�C���^�[�t�F�[�X
//---------------------------------------------------------------------------
class tTVPDrawer
{
protected:
	tTVPPassThroughDrawDevice * Device;
	tjs_int DestLeft;
	tjs_int DestTop;
	tjs_int DestWidth;
	tjs_int DestHeight;
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
		DestLeft = DestTop = DestWidth = DestHeight = 0;
		TargetWindow = NULL;
		DrawDibHandle = NULL;
		DrawUpdateRectangle = NULL;
	} 
	virtual ~tTVPDrawer()
	{
		if(DrawDibHandle) DrawDibClose(DrawDibHandle), DrawDibHandle = NULL;
	}
	virtual ttstr GetName() = 0;

	virtual bool SetDestPos(tjs_int left, tjs_int top)
		{ DestLeft = left; DestTop = top; return true; }
	virtual bool SetDestSize(tjs_int width, tjs_int height)
		{ DestWidth = width; DestHeight = height; return true; }
	void GetDestSize(tjs_int &width, tjs_int &height) const
		{ width = DestWidth; height = DestHeight; }
	virtual bool NotifyLayerResize(tjs_int w, tjs_int h)
		{ SrcWidth = w; SrcHeight = h; return true; }
	void GetSrcSize(tjs_int &w, tjs_int &h) const
		{ w = SrcWidth; h = SrcHeight; }
	virtual bool SetDestSizeAndNotifyLayerResize(tjs_int width, tjs_int height, tjs_int w, tjs_int h)
	{
		if(!SetDestSize(width, height)) return false;
		if(!NotifyLayerResize(w, h)) return false;
		return true;
	}
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
	virtual int GetInterpolationCapability() { return 3; }
		// bit 0 for point-on-point, bit 1 for bilinear interpolation

	virtual void InitTimings() {;} // notifies begining of benchmark
	virtual void ReportTimings() {;} // notifies end of benchmark
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	GDI�ɂ��`���K�v�Ƃ����{�N���X
//---------------------------------------------------------------------------
class tTVPDrawer_GDI : public tTVPDrawer
{
	typedef tTVPDrawer inherited;
protected:
	HDC TargetDC;

public:
	//! @brief	�R���X�g���N�^
	tTVPDrawer_GDI(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TargetDC = NULL;
	}

	//! @brief	�f�X�g���N�^
	~tTVPDrawer_GDI()
	{
		if(TargetDC && TargetWindow) ReleaseDC(TargetWindow, TargetDC);
	}

	void SetTargetWindow(HWND wnd)
	{
		if(wnd)
		{
			// �`��p DC ���擾����
			TargetDC = GetDC(wnd);
		}
		else
		{
			// �`��p DC ���J���
			if(TargetDC) ReleaseDC(TargetWindow, TargetDC), TargetDC = NULL;
		}

		inherited::SetTargetWindow(wnd);

	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief	DrawDib�ɂ��o�b�t�@�����`����s����{�N���X
//---------------------------------------------------------------------------
class tTVPDrawer_DrawDibNoBuffering : public tTVPDrawer_GDI
{
	typedef tTVPDrawer_GDI inherited;

	HPEN BluePen;
	HPEN YellowPen;

public:
	//! @brief	�R���X�g���N�^
	tTVPDrawer_DrawDibNoBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer_GDI(device)
	{
		BluePen = NULL;
		YellowPen = NULL;
	}

	//! @brief	�f�X�g���N�^
	~tTVPDrawer_DrawDibNoBuffering()
	{
		if(BluePen)   DeleteObject(BluePen);
		if(YellowPen) DeleteObject(YellowPen);
	}

	virtual ttstr GetName() { return TJS_W("DrawDIB (no buffering)"); }

	bool SetDestSize(tjs_int width, tjs_int height)
	{
		// ���̃f�o�C�X�ł͊g��k���͂ł��Ȃ��̂�
		// �g��k�����K�v�ȏꍇ�� false ��Ԃ�
		tjs_int w, h;
		Device->GetSrcSize(w, h);
		if(width != w || height != h)
			return false;

		return inherited::SetDestSize(width, height);
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
		// ��邱�ƂȂ�
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw �ɂ� TargetDC �ɕ`����s��
		if(DrawDibHandle && TargetDC)
			DrawDibDraw(DrawDibHandle,
				TargetDC,
				x + DestLeft,
				y + DestTop,
				cliprect.get_width(),
				cliprect.get_height(),
				const_cast<BITMAPINFOHEADER*>(reinterpret_cast<const BITMAPINFOHEADER*>(bitmapinfo)),
				const_cast<void*>(bits),
				cliprect.left,
				cliprect.top,
				cliprect.get_width(),
				cliprect.get_height(),
				0);

		// �X�V��`�̕\��
		if(DrawUpdateRectangle)
		{
			if(!BluePen) BluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			if(!YellowPen) YellowPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

			HPEN oldpen;

			int ROP_save = GetROP2(TargetDC);

			tjs_int rleft   = x + DestLeft;
			tjs_int rtop    = y + DestTop;
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
		// ��邱�ƂȂ�
	}

	virtual int GetInterpolationCapability() { return 1; }
		// bit 0 for point-on-point, bit 1 for bilinear interpolation

};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief	GDI�ɂ��_�u���o�b�t�@�����O���s���N���X
//---------------------------------------------------------------------------
class tTVPDrawer_GDIDoubleBuffering : public tTVPDrawer_GDI
{
	typedef tTVPDrawer_GDI inherited;
	HBITMAP OffScreenBitmap; //!< �I�t�X�N���[���r�b�g�}�b�v
	HDC OffScreenDC; //!< �I�t�X�N���[�� DC
	HBITMAP OldOffScreenBitmap; //!< OffScreenDC �ɈȑO�I������Ă��� �r�b�g�}�b�v
	bool ShouldShow; //!< show �Ŏ��ۂɉ�ʂɉ摜��]�����ׂ���
	bool InBenchMark; //!< �x���`�}�[�N�����ǂ���

public:
	//! @brief	�R���X�g���N�^
	tTVPDrawer_GDIDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer_GDI(device)
	{
		OffScreenBitmap = NULL;
		OffScreenDC = NULL;
		OldOffScreenBitmap = NULL;
		ShouldShow = false;
		InBenchMark = false;
	}

	//! @brief	�f�X�g���N�^
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
		// �X�N���[���݊��� DDB ���쐬����B
		// ����͂����Ă��̏ꍇ�A�r�f�I��������ɍ쐬�����B
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

	bool SetDestSize(tjs_int width, tjs_int height)
	{
		return inherited::SetDestSize(width, height);
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
		// ��邱�ƂȂ�
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw �ɂ� OffScreenDC �ɕ`����s��
		if(DrawDibHandle && OffScreenDC)
		{
			ShouldShow = true;
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
	}

	void EndBitmapCompletion()
	{
	}

	void Show()
	{
		if(TargetDC && OffScreenDC && ShouldShow)
		{
			// �I�t�X�N���[���r�b�g�}�b�v�� TargetDC �ɓ]������
			if(DestWidth == SrcWidth && DestHeight == SrcHeight)
			{
				// �g��E�k���͕K�v�Ȃ�
				BitBlt(TargetDC,
					DestLeft,
					DestTop,
					DestWidth,
					DestHeight,
					OffScreenDC,
					0,
					0,
					SRCCOPY);
			}
			else
			{
				// �g��E�k�����K�v
				if(TVPZoomInterpolation)
					SetStretchBltMode(TargetDC, HALFTONE);
				else
					SetStretchBltMode(TargetDC, COLORONCOLOR);
				SetBrushOrgEx(TargetDC, 0, 0, NULL);

				StretchBlt(TargetDC,
					DestLeft,
					DestTop,
					DestWidth,
					DestHeight,
					OffScreenDC,
					0,
					0,
					SrcWidth,
					SrcHeight,
					SRCCOPY);
			}

			if(InBenchMark)
			{
				// ��ʂ���̓ǂݏo�����s���֐������s����
				// �������Ȃ��� StrechBlt �Ȃǂ̓R�}���h�L���[�ɂ��������܂��
				// �����ŁA���ۂ̕`���҂����ɋA��\��������B
				(void)GetPixel(TargetDC, DestLeft + DestWidth / 2, DestTop + DestHeight / 2);
			}

			ShouldShow = false;
		}
	}

	virtual int GetInterpolationCapability() { return 1+2; }
		// bit 0 for point-on-point, bit 1 for bilinear interpolation

	virtual void InitTimings() { InBenchMark = true; }
	virtual void ReportTimings() { InBenchMark = false; }


};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief	DirectDraw�ɂ��_�u���o�b�t�@�����O���s���N���X
//---------------------------------------------------------------------------
class tTVPDrawer_DDDoubleBuffering : public tTVPDrawer
{
	typedef tTVPDrawer inherited;

	HDC OffScreenDC;
	IDirectDrawSurface * Surface;
	IDirectDrawClipper * Clipper;

	bool LastOffScreenDCGot;
	bool ShouldShow; //!< show �Ŏ��ۂɉ�ʂɉ摜��]�����ׂ���

public:
	//! @brief	�R���X�g���N�^
	tTVPDrawer_DDDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TVPEnsureDirectDrawObject();
		OffScreenDC = NULL;
		Surface = NULL;
		Clipper = NULL;
		LastOffScreenDCGot = true;
		ShouldShow = false;
	}

	//! @brief	�f�X�g���N�^
	~tTVPDrawer_DDDoubleBuffering()
	{
		DestroyOffScreenSurface();
	}

	virtual ttstr GetName() { return TJS_W("DirectDraw double buffering"); }

	void DestroyOffScreenSurface()
	{
		if(OffScreenDC && Surface) Surface->ReleaseDC(OffScreenDC);
		if(Clipper) Clipper->Release(), Clipper = NULL;
		if(Surface) Surface->Release(), Surface = NULL;
		TVPReleaseDDPrimarySurface();
	}

	void InvalidateAll()
	{
		// ���C�����Z���ʂ����ׂă��N�G�X�g����
		// �T�[�t�F�[�X�� lost �����ۂɓ��e���č\�z����ړI�ŗp����
		Device->RequestInvalidation(tTVPRect(0, 0, DestWidth, DestHeight));
	}

	void CreateOffScreenSurface()
	{
		// �I�t�X�N���[���T�[�t�F�[�X��ݒ肷��
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

	bool SetDestSize(tjs_int width, tjs_int height)
	{
		return inherited::SetDestSize(width, height);
	}

	bool NotifyLayerResize(tjs_int w, tjs_int h)
	{
		if(inherited::NotifyLayerResize(w, h))
		{
			try
			{
				CreateOffScreenSurface();
			}
/*			catch(const eTJS & e)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create DirectDraw off-screen buffer: ") + e.GetMessage());
				return false;
			}
*/			catch(...)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create DirectDraw off-screen buffer: unknown reason"));
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
		if(Surface && OffScreenDC) Surface->ReleaseDC(OffScreenDC), OffScreenDC = NULL;

		if(Surface && TargetWindow)
		{
			HDC dc = NULL;
			HRESULT hr = Surface->GetDC(&dc);
			if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
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
					TVPAddImportantLog(
						TJS_W("Passthrough: (inf) Off-screen surface, IDirectDrawSurface::GetDC failed/HR=") +
						TJSInt32ToHex(hr, 8) + TJS_W(", recreating drawer ..."));
					Device->DestroyDrawer(); // destroy self
					return; // return immediately
				}
			}

			OffScreenDC = dc;

			if(OffScreenDC) LastOffScreenDCGot = true; else LastOffScreenDCGot = false;
		}
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		// DrawDibDraw �ɂ� OffScreenDC �ɕ`����s��
		if(DrawDibHandle && OffScreenDC && TargetWindow)
		{
			ShouldShow = true;
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
		if(!ShouldShow) return;

		ShouldShow = false;

		// Blt to the primary surface
		IDirectDrawSurface *pri = TVPGetDDPrimarySurfaceNoAddRef();
		if(!pri)
			TVPThrowExceptionMessage(TJS_W("Cannot retrieve primary surface object"));

		// set clipper
		TVPSetDDPrimaryClipper(Clipper);

		// get PaintBox's origin
		POINT origin; origin.x = DestLeft, origin.y = DestTop;
		ClientToScreen(TargetWindow, &origin);

		// entire of the bitmap is to be transfered (this is not optimal. FIX ME!)
		RECT drect;
		drect.left   = origin.x;
		drect.top    = origin.y;
		drect.right  = origin.x + DestWidth;
		drect.bottom = origin.y + DestHeight;

		RECT srect;
		srect.left = 0;
		srect.top = 0;
		srect.right  = SrcWidth;
		srect.bottom = SrcHeight;

		HRESULT hr = pri->Blt(&drect, Surface, &srect, DDBLT_WAIT, NULL);
		if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
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
			TVPAddImportantLog(
				TJS_W("Passthrough: (inf) Primary surface, IDirectDrawSurface::Blt failed/HR=") +
				TJSInt32ToHex(hr, 8));
		}
	}

	virtual int GetInterpolationCapability()
	{
		// bit 0 for point-on-point, bit 1 for bilinear interpolation
		// ���āADirectDraw �� blt ����Ԃ��s�����ǂ������m�F����̂͂������
		// ��������ł���B
		// GetCaps �Ȃǂ̃��\�b�h�������āA�������瓾����l�ɕ�Ԃ��s�����ǂ�����
		// ��񂪂���Ȃ�Θb�͑������A����Ȃ���͒T��������ł݂͂���Ȃ��B
		// �v���C�}���T�[�t�F�[�X�֎��ۂɉ摜��]�����Ċm���߂Ă݂�Ƃ�����͂��邪
		// ��ʂ�������ɕ`�悵�Ă���m�F����܂ł̊Ԃɑ��̃A�v���������̉摜��
		// �����Ă��܂���������Ȃ� (��������Ȃ��񂾂��ł��Ȃ�)
		// ���傤���Ȃ��̂ŁA�Q���������I�t�X�N���[���T�[�t�F�[�X������Ă݂āA
		// �����̊ԓ��m�ł̓]�����s���Ă݂邱�Ƃɂ���B�I�t�X�N���[���T�[�t�F�[�X�ł�
		// ���邪�A�v���C�}���T�[�t�F�[�X�Ɠ������r�f�I��������ɔz�u�����̂�
		// �����悤�ȕ�Ԃ̎d������Ă����Ɗ�҂���B
		// ���������Ƃ����Ă��A�����x�̑傫�����Ȃ��ƕ�Ԃ��s���Ă���Ȃ�
		// �f�o�C�X�����݂��邩������Ȃ��̂ŁA100x100��200x200���m�ۂ��Ă݂邱�Ƃɂ���B
		IDirectDraw2 *object = TVPGetDirectDrawObjectNoAddRef();
		if(!object) return 0;

		int caps = 0;

		IDirectDrawSurface * s1 = NULL;
		IDirectDrawSurface * s2 = NULL;
		HDC s1dc = NULL;
		HDC s2dc = NULL;

		// �T�[�t�F�[�X�̊m��
		for(int i = 0; i < 2; i++)
		{
			IDirectDrawSurface * & surface = (i == 0) ? s1 : s2;

			// allocate secondary off-screen buffer
			DDSURFACEDESC ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			ddsd.dwWidth  = (i == 0) ? 100 : 200;
			ddsd.dwHeight = (i == 0) ? 100 : 200;
			ddsd.ddsCaps.dwCaps =
				DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
			HRESULT hr;

			hr = object->CreateSurface(&ddsd, &surface, NULL);

			if(hr != DD_OK) goto got_error;

			// check whether the surface is on video memory
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);

			hr = surface->GetSurfaceDesc(&ddsd);

			if(hr != DD_OK) goto got_error;

			if(ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY &&
				ddsd.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM)
			{
				// ok
			}
			else
			{
				goto got_error;
			}
		}

		// s1 �� ���� �� ���� �ׂ̍����c�̃X�g���C�v������
		while(true)
		{
			HDC dc = NULL;
			HRESULT hr = s1->GetDC(&s1dc);
			if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
			{
				s1->Restore();
				continue;
			}
			else if(FAILED(hr))
				goto got_error;

			// s1 �̃T�C�Y(100x100) �ɃX�g���C�v������
			HPEN white_pen   = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			HPEN black_pen   = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

			HPEN oldpen;

			oldpen = SelectObject(s1dc, white_pen);
			SetROP2(s1dc, R2_COPYPEN);
			for(int i = 0; i < 100; i += 2)
			{
				MoveToEx(s1dc, i, 0, NULL);
				LineTo(s1dc, i, 100);
			}

			SelectObject(s1dc, black_pen);
			for(int i = 1; i < 100; i += 2)
			{
				MoveToEx(s1dc, i, 0, NULL);
				LineTo(s1dc, i, 100);
			}
			SelectObject(s1dc, oldpen);

			s1->ReleaseDC(s1dc), s1dc = NULL;
			break;
		}

		// s1 �� s2 �Ɋg�� Blt ����
		RECT drect;
		drect.left   = 0;
		drect.top    = 0;
		drect.right  = 200;
		drect.bottom = 200;

		RECT srect;
		srect.left   = 0;
		srect.top    = 0;
		srect.right  = 100;
		srect.bottom = 100;

		if(FAILED(s2->Blt(&drect, s1, &srect, DDBLT_WAIT, NULL)))
			goto got_error;

		// s2 ���ǂ��g�傳�ꂽ���𒲍�����
		while(true)
		{
			HDC dc = NULL;
			HRESULT hr = s2->GetDC(&s2dc);
			if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
			{
				s2->Restore();
				continue;
			}
			else if(FAILED(hr))
				goto got_error;

			// �܂�Ȃ��ւ�̉�f�𒲂ׂ�
			bool halftone_detected = false;
			for(int i = 90; i < 110; i++)
			{
				// �F��get
				COLORREF color = GetPixel(s2dc, i, 100);
				// �����A��Ԃ��s���Ă���΁A����Ƃ���ȊO�̐F��
				// �o�Ă��Ă���͂�
				halftone_detected = halftone_detected ||
					(color != 0xffffff && color != 0x000000);
			}
			caps = halftone_detected ? 2 : 1;

			s2->ReleaseDC(s2dc), s2dc = NULL;
			break;
		}

		// ����
	got_error:
		if(s1dc && s1)
			s1->ReleaseDC(s1dc), s1dc = NULL;
		if(s1) s1->Release(), s1 = NULL;
		if(s2dc && s2)
			s2->ReleaseDC(s2dc), s2dc = NULL;
		if(s2) s2->Release(), s2 = NULL;

		switch(caps)
		{
		case 0:
			TVPAddImportantLog(TJS_W("Passthrough: Could not get IDirectDraw::Blt working."));
			break;
		case 1:
			TVPAddImportantLog(TJS_W("Passthrough: IDirectDraw::Blt seems to filter by nearest neighbor method."));
			break;
		case 2:
			TVPAddImportantLog(TJS_W("Passthrough: IDirectDraw::Blt seems to filter by some kind of interpolation method."));
			break;
		}

		return caps;
	}
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief	Direct3D7 �ɂ��_�u���o�b�t�@�����O���s���N���X
//! @note	tTVPDrawer_DDDoubleBuffering �Ƃ悭���Ă��邪�ʃN���X�ɂȂ��Ă���B
//!			�C�����s���ꍇ�́A�݂��ɂ悭����ׁA�����悤�ȂƂ��낪����΂Ƃ��ɏC�������݂邱�ƁB
//---------------------------------------------------------------------------
class tTVPDrawer_D3DDoubleBuffering : public tTVPDrawer
{
	typedef tTVPDrawer inherited;

/*
	note: Texture �ɑ΂��Ă�������`�悳�ꂽ���e�� Surface �ɓ]������A
			����ɂ�������v���C�}���T�[�t�F�[�X�ɃR�s�[�����B
*/

	HDC OffScreenDC;
	IDirectDraw7 * DirectDraw7;
	IDirect3D7 * Direct3D7;
	IDirect3DDevice7 * Direct3DDevice7;
	IDirectDrawSurface7 * Surface;
	IDirectDrawSurface7 * Texture;
	IDirectDrawClipper * Clipper;

	void * TextureBuffer; //!< �e�N�X�`���̃T�[�t�F�[�X�ւ̃������|�C���^
	long TexturePitch; //!< �e�N�X�`���̃s�b�`

	tjs_uint TextureWidth; //!< �e�N�X�`���̉���
	tjs_uint TextureHeight; //!< �e�N�X�`���̏c��

	bool LastOffScreenDCGot;
	bool ShouldShow; //!< show �Ŏ��ۂɉ�ʂɉ摜��]�����ׂ���
	bool UseDirectTransfer; //!< ���������ړ]�����s�����ǂ���

public:
	//! @brief	�R���X�g���N�^
	tTVPDrawer_D3DDoubleBuffering(tTVPPassThroughDrawDevice * device) : tTVPDrawer(device)
	{
		TVPEnsureDirectDrawObject();
		OffScreenDC = NULL;
		DirectDraw7 = NULL;
		Direct3D7 = NULL;
		Direct3DDevice7 = NULL;
		Surface = NULL;
		Texture = NULL;
		Clipper = NULL;
		LastOffScreenDCGot = true;
		ShouldShow = false;
		UseDirectTransfer = false;
		TextureBuffer = NULL;
		TextureWidth = TextureHeight = 0;
	}

	//! @brief	�f�X�g���N�^
	~tTVPDrawer_D3DDoubleBuffering()
	{
		DestroyOffScreenSurface();
	}

	virtual ttstr GetName() { return TJS_W("Direct3D double buffering"); }

	void DestroyOffScreenSurface()
	{
		if(TextureBuffer && Texture) Texture->Unlock(NULL), TextureBuffer = NULL;
		if(OffScreenDC && Surface) Surface->ReleaseDC(OffScreenDC), OffScreenDC = NULL;
		if(Texture) Texture->Release(), Texture = NULL;
		if(Direct3DDevice7) Direct3DDevice7->Release(), Direct3DDevice7 = NULL;
		if(Surface) Surface->Release(), Surface = NULL;
		if(Direct3D7) Direct3D7->Release(), Direct3D7 = NULL;
		if(DirectDraw7) DirectDraw7->Release(), DirectDraw7 = NULL;
		TVPReleaseDDPrimarySurface();
		if(Clipper) Clipper->Release(), Clipper = NULL;
	}

	void InvalidateAll()
	{
		// ���C�����Z���ʂ����ׂă��N�G�X�g����
		// �T�[�t�F�[�X�� lost �����ۂɓ��e���č\�z����ړI�ŗp����
		Device->RequestInvalidation(tTVPRect(0, 0, DestWidth, DestHeight));
	}

	void GetDirect3D7Device()
	{
		// get DirectDraw7/Direct3D7 interface
		if(DirectDraw7) DirectDraw7->Release(), DirectDraw7 = NULL;
		DirectDraw7 = TVPGetDirectDraw7ObjectNoAddRef();
		if(!DirectDraw7) TVPThrowExceptionMessage(TJS_W("DirectDraw7 not available"));

		DirectDraw7->AddRef();

		if(Direct3D7) Direct3D7->Release(), Direct3D7 = NULL;
		HRESULT hr = DirectDraw7->QueryInterface(IID_IDirect3D7, (void**)&Direct3D7);
		if(FAILED(hr))
			TVPThrowExceptionMessage(TJS_W("Direct3D7 not available"));
	}

	void CreateOffScreenSurface()
	{
		// Direct3D �f�o�C�X�A�e�N�X�`���Ȃǂ��쐬����
		DestroyOffScreenSurface();
		if(TargetWindow && SrcWidth > 0 && SrcHeight > 0)
		{
			HRESULT hr;

			// get DirectDraw7/Direct3D7 interface
			GetDirect3D7Device();

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
			ddsd.dwWidth  = DestWidth;
			ddsd.dwHeight = DestHeight;
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

			// retrieve device caps
			D3DDEVICEDESC7 caps;
			ZeroMemory(&caps, sizeof(caps));
			if(FAILED(Direct3DDevice7->GetCaps(&caps)))
				TVPThrowExceptionMessage(TJS_W("Failed to retrieve Direct3D device caps/HR=%1"),
					TJSInt32ToHex(hr, 8));

			// decide texture size
			TextureWidth = SrcWidth;
			TextureHeight = SrcHeight;
			if(caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
			{
				// only square textures are supported
				TextureWidth = std::max(TextureHeight, TextureWidth);
				TextureHeight = TextureWidth;
			}

			if(caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2)
			{
				// power of 2 size of texture dimentions are required
				tjs_uint sz;

				sz = 1; while(sz < TextureWidth) sz <<= 1;
				TextureWidth = sz;

				sz = 1; while(sz < TextureHeight) sz <<= 1;
				TextureHeight = sz;
			}

			if(caps.dwMinTextureWidth  > TextureWidth) TextureWidth = caps.dwMinTextureWidth;
			if(caps.dwMinTextureHeight > TextureHeight) TextureHeight = caps.dwMinTextureHeight;
			if(	caps.dwMaxTextureWidth  < TextureWidth ||
				caps.dwMaxTextureHeight < TextureHeight)
			{
				TVPThrowExceptionMessage(TJS_W("Could not allocate texture size of %1x%2"),
					ttstr((int)TextureWidth), ttstr((int)TextureHeight));
			}

			// create Direct3D Texture
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
			ddsd.dwWidth  = TextureWidth;
			ddsd.dwHeight = TextureHeight;
			ddsd.ddsCaps.dwCaps =
				/*DDSCAPS_OFFSCREENPLAIN |*/ DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_LOCALVIDMEM;

			ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBBitCount	= 32;
			ddsd.ddpfPixelFormat.dwRBitMask		= 0x00FF0000;
			ddsd.ddpfPixelFormat.dwGBitMask		= 0x0000FF00;
			ddsd.ddpfPixelFormat.dwBBitMask		= 0x000000FF;

			hr = DirectDraw7->CreateSurface(&ddsd, &Texture, NULL);

			if(hr == DD_OK)
			{
				UseDirectTransfer = true; // ���ڂ̃������]����L��ɂ���
			}
			else /*if(hr != DD_OK) */
			{
				// �s�N�Z���t�H�[�}�b�g���w�肹���ɐ��������݂�

				ZeroMemory(&ddsd, sizeof(ddsd));
				ddsd.dwSize = sizeof(ddsd);
				ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS/* | DDSD_PIXELFORMAT*/;
				ddsd.dwWidth = SrcWidth;
				ddsd.dwHeight = SrcHeight;
				ddsd.ddsCaps.dwCaps =
					/*DDSCAPS_OFFSCREENPLAIN |*/ DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_LOCALVIDMEM;

				hr = DirectDraw7->CreateSurface(&ddsd, &Texture, NULL);

				if(FAILED(hr))
					TVPThrowExceptionMessage(TJS_W("Cannot allocate D3D texture/HR=%1"),
						TJSInt32ToHex(hr, 8));

				TVPAddImportantLog("Passthrough: Using non 32bit ARGB texture format");
			}
		}
	}

	bool SetDestSize(tjs_int width, tjs_int height)
	{
		if(inherited::SetDestSize(width, height))
		{
			try
			{
				CreateOffScreenSurface();
			}
/*			catch(const eTJS & e)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create Direct3D devices: ") + e.GetMessage());
				return false;
			}
*/			catch(...)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create Direct3D devices: unknown reason"));
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
/*			catch(const eTJS & e)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create Direct3D devices: ") + e.GetMessage());
				return false;
			}
*/			catch(...)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create Direct3D devices: unknown reason"));
				return false;
			}
			return true;
		}
		return false;
	}

	bool SetDestSizeAndNotifyLayerResize(tjs_int width, tjs_int height, tjs_int w, tjs_int h)
	{
		if(inherited::SetDestSize(width, height) && inherited::NotifyLayerResize(w, h))
		{
			try
			{
				CreateOffScreenSurface();
			}
/*			catch(const eTJS & e)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create Direct3D devices: ") + e.GetMessage());
				return false;
			}
*/			catch(...)
			{
				TVPAddImportantLog(TJS_W("Passthrough: Failed to create Direct3D devices: unknown reason"));
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
//#define TVPD3DTIMING
#ifdef TVPD3DTIMING
	DWORD StartTick;

	DWORD GetDCTime;
	DWORD DrawDibDrawTime;
	DWORD ReleaseDCTime;
	DWORD DrawPrimitiveTime;
	DWORD BltTime;
	void InitTimings()
	{
		GetDCTime = 0;
		DrawDibDrawTime = 0;
		ReleaseDCTime = 0;
		DrawPrimitiveTime = 0;
		BltTime = 0;
	}

	void ReportTimings()
	{
		TVPAddLog(TJS_W("GetDC / Lock : ") + ttstr((int)GetDCTime));
		TVPAddLog(TJS_W("DrawDibDraw / memcpy : ") + ttstr((int)DrawDibDrawTime));
		TVPAddLog(TJS_W("ReleaseDC / Unlock : ") + ttstr((int)ReleaseDCTime));
		TVPAddLog(TJS_W("DrawPrimitive : ") + ttstr((int)DrawPrimitiveTime));
		TVPAddLog(TJS_W("Blt : ") + ttstr((int)BltTime));
	}
#endif

	void StartBitmapCompletion()
	{
		// retrieve DC
		if(Texture && TargetWindow)
		{
			if(UseDirectTransfer)
			{
#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
				if(TextureBuffer)
				{
					TVPAddImportantLog(TJS_W("Passthrough: Texture has already been locked (StartBitmapCompletion() has been called twice without EndBitmapCompletion()), unlocking the texture."));
					Texture->Unlock(NULL), TextureBuffer = NULL;
				}


				DDSURFACEDESC2 ddsd;
				ZeroMemory(&ddsd, sizeof(ddsd));
				ddsd.dwSize = sizeof(ddsd);
				HRESULT hr = Texture->Lock(NULL, &ddsd, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);

				if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
				{
					Texture->Restore();
					InvalidateAll();  // causes reconstruction of off-screen image
					hr = Texture->Lock(NULL, &ddsd, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);
				}

				if(hr != DD_OK)
				{
					TextureBuffer = NULL;
					InvalidateAll();  // causes reconstruction of off-screen image

					if(LastOffScreenDCGot)
					{
						// display this message only once since last success
						TVPAddImportantLog(
							TJS_W("Passthrough: (inf) Texture, IDirectDrawSurface::Lock failed/HR=") +
							TJSInt32ToHex(hr, 8) + TJS_W(", recreating drawer ..."));
						Device->DestroyDrawer(); // destroy self
						return;
					}
				}
				else /*if(hr == DD_OK) */
				{
					TextureBuffer = ddsd.lpSurface;
					TexturePitch = ddsd.lPitch;
				}


#ifdef TVPD3DTIMING
GetDCTime += timeGetTime() - StartTick;
#endif
			}
			else
			{
				HDC dc = NULL;
#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
				HRESULT hr = Texture->GetDC(&dc);
#ifdef TVPD3DTIMING
GetDCTime += timeGetTime() - StartTick;
#endif
				if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
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
						TVPAddImportantLog(
							TJS_W("Passthrough: (inf) Texture, IDirectDrawSurface::GetDC failed/HR=") +
							TJSInt32ToHex(hr, 8) + TJS_W(", recreating drawer ..."));
						Device->DestroyDrawer(); // destroy self
						return;
					}
				}

				OffScreenDC = dc;

				if(OffScreenDC) LastOffScreenDCGot = true; else LastOffScreenDCGot = false;
			}
		}
	}

	void NotifyBitmapCompleted(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect)
	{
		if(UseDirectTransfer)
		{
			// ���ڃ������]����p���ĕ`����s��
#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
			if(DrawDibHandle && TextureBuffer && TargetWindow &&
				!(x < 0 || y < 0 ||
					x + cliprect.get_width() > SrcWidth ||
					y + cliprect.get_height() > SrcHeight) &&
				!(cliprect.left < 0 || cliprect.top < 0 ||
					cliprect.right > bitmapinfo->bmiHeader.biWidth ||
					cliprect.bottom > bitmapinfo->bmiHeader.biHeight))
			{
				// �͈͊O�̓]����(�ꕔ�����]������̂ł͂Ȃ���)�������Ă悢
				ShouldShow = true;

				// bitmapinfo �ŕ\���ꂽ cliprect �̗̈�� x,y �ɃR�s�[����
				long src_y       = cliprect.top;
				long src_y_limit = cliprect.bottom;
				long src_x       = cliprect.left;
				long width_bytes   = cliprect.get_width() * 4; // 32bit
				long dest_y      = y;
				long dest_x      = x;
				const tjs_uint8 * src_p = (const tjs_uint8 *)bits;
				long src_pitch;

				if(bitmapinfo->bmiHeader.biHeight < 0)
				{
					// bottom-down
					src_pitch = bitmapinfo->bmiHeader.biWidth * 4;
				}
				else
				{
					// bottom-up
					src_pitch = -bitmapinfo->bmiHeader.biWidth * 4;
					src_p += bitmapinfo->bmiHeader.biWidth * 4 * (bitmapinfo->bmiHeader.biHeight - 1);
				}

				for(; src_y < src_y_limit; src_y ++, dest_y ++)
				{
					const void *srcp = src_p + src_pitch * src_y + src_x * 4;
					void *destp = (tjs_uint8*)TextureBuffer + TexturePitch * dest_y + dest_x * 4;
					memcpy(destp, srcp, width_bytes);
				}
			}
#ifdef TVPD3DTIMING
DrawDibDrawTime += timeGetTime() - StartTick;
#endif
		}
		else
		{
			// DrawDibDraw �ɂ� OffScreenDC �ɕ`����s��
#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
			if(DrawDibHandle && OffScreenDC && TargetWindow)
			{
				ShouldShow = true;
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
#ifdef TVPD3DTIMING
DrawDibDrawTime += timeGetTime() - StartTick;
#endif
		}
	}

	void EndBitmapCompletion()
	{
		if(!TargetWindow) return;
		if(!Texture) return;
		if(!Surface) return;
		if(!Direct3DDevice7) return;

		if(UseDirectTransfer)
		{
#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
			if(!TextureBuffer) return;
			Texture->Unlock(NULL), TextureBuffer = NULL;
#ifdef TVPD3DTIMING
ReleaseDCTime += timeGetTime() - StartTick;
#endif
		}
		else
		{
#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
			if(!OffScreenDC) return;
			Texture->ReleaseDC(OffScreenDC), OffScreenDC = NULL;
#ifdef TVPD3DTIMING
ReleaseDCTime += timeGetTime() - StartTick;
#endif
		}

		// Blt to the primary surface

		// Blt texture to surface

		//- build vertex list
		struct tVertices
		{
			float x, y, z, rhw;
			float tu, tv;
		};

		float dw = (float)DestWidth;
		float dh = (float)DestHeight;

		float sw = (float)SrcWidth  / (float)TextureWidth;
		float sh = (float)SrcHeight / (float)TextureHeight;


		tVertices vertices[] =
		{
			{0.0f - 0.5f, 0.0f - 0.5f, 1.0f, 1.0f, 0.0f, 0.0f},
			{dw   - 0.5f, 0.0f - 0.5f, 1.0f, 1.0f, sw  , 0.0f},
			{0.0f - 0.5f, dh   - 0.5f, 1.0f, 1.0f, 0.0f, sh  },
			{dw   - 0.5f, dh   - 0.5f, 1.0f, 1.0f, sw  , sh  }
		};

		HRESULT hr;

#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
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

#ifdef TVPD3DTIMING
DrawPrimitiveTime += timeGetTime() - StartTick;
#endif
	got_error:
		if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
		{
			Surface->Restore();
			Texture->Restore();
			InvalidateAll();  // causes reconstruction of off-screen image
		}
		else if(hr == DDERR_INVALIDRECT)
		{
			// ignore this error
		}
		else if(hr != D3D_OK)
		{
			TVPAddImportantLog(
				TJS_W("Passthrough: (inf) Polygon drawing failed/HR=") +
				TJSInt32ToHex(hr, 8));
		}
	}


	void Show()
	{
		if(!TargetWindow) return;
		if(!Texture) return;
		if(!Surface) return;
		if(!Direct3DDevice7) return;
		if(!ShouldShow) return;

		ShouldShow = false;

		HRESULT hr;

		// retrieve the primary surface
		IDirectDrawSurface *pri = TVPGetDDPrimarySurfaceNoAddRef();
		if(!pri)
			TVPThrowExceptionMessage(TJS_W("Cannot retrieve primary surface object"));

		// set clipper
		TVPSetDDPrimaryClipper(Clipper);

#ifdef TVPD3DTIMING
StartTick = timeGetTime();
#endif
		// get PaintBox's origin
		POINT origin; origin.x = DestLeft, origin.y = DestTop;
		ClientToScreen(TargetWindow, &origin);

		// entire of the bitmap is to be transfered (this is not optimal. FIX ME!)
		RECT drect;
		drect.left   = origin.x;
		drect.top    = origin.y;
		drect.right  = origin.x + DestWidth;
		drect.bottom = origin.y + DestHeight;

		RECT srect;
		srect.left   = 0;
		srect.top    = 0;
		srect.right  = DestWidth;
		srect.bottom = DestHeight;

		hr = pri->Blt(&drect, (IDirectDrawSurface*)Surface, &srect, DDBLT_WAIT, NULL);

#ifdef TVPD3DTIMING
BltTime += timeGetTime() - StartTick;
#endif

	got_error:
		if(hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY)
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
			TVPAddImportantLog(
				TJS_W("Passthrough: (inf) Primary surface, IDirectDrawSurface::Blt failed/HR=") +
				TJSInt32ToHex(hr, 8));
		}
	}

	virtual int GetInterpolationCapability()
	{
		// bit 0 for point-on-point, bit 1 for bilinear interpolation
		GetDirect3D7Device();
		if(Direct3DDevice7)
		{
			HRESULT hr;
			D3DDEVICEDESC7 desc;
			ZeroMemory(&desc, sizeof(desc));
			if(SUCCEEDED(Direct3DDevice7->GetCaps(&desc)))
			{
				int caps = 0;
				if(desc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
					caps += 2;
				if(desc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)
					caps += 1;
				return caps;
			}
			return 3;
		}
		else
		{
			return 3;
		}
	}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tTVPPassThroughDrawDevice::tTVPPassThroughDrawDevice()
{
	TVPInitPassThroughOptions(); // read and initialize options
	PreferredDrawerType = TVPPreferredDrawType;
	TargetWindow = NULL;
	Drawer = NULL;
	DrawerType = dtNone;
	DestSizeChanged = false;
	SrcSizeChanged = false;
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
void tTVPPassThroughDrawDevice::CreateDrawer(tDrawerType type)
{
	if(Drawer) delete Drawer, Drawer = NULL;

	switch(type)
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

	try
	{

		if(Drawer)
			Drawer->SetTargetWindow(TargetWindow);

		if(Drawer)
		{
			if(!Drawer->SetDestPos(DestRect.left, DestRect.top))
			{
				TVPAddImportantLog(
					TJS_W("Passthrough: Failed to set destination position to draw device drawer"));
				delete Drawer, Drawer = NULL;
			}
		}

		if(Drawer)
		{
			tjs_int srcw, srch;
			GetSrcSize(srcw, srch);
			if(!Drawer->SetDestSizeAndNotifyLayerResize(DestRect.get_width(), DestRect.get_height(), srcw, srch))
			{
				TVPAddImportantLog(
					TJS_W("Passthrough: Failed to set destination size and source layer size to draw device drawer"));
				delete Drawer, Drawer = NULL;
			}
		}

		if(Drawer) DrawerType = type; else DrawerType = dtNone;

		RequestInvalidation(tTVPRect(0, 0, DestRect.get_width(), DestRect.get_height()));
	}
/*	catch(const eTJS & e)
	{
		TVPAddImportantLog(TJS_W("Passthrough: Failed to create drawer: ") + e.GetMessage());
		DestroyDrawer();
	}
*/	catch(...)
	{
		TVPAddImportantLog(TJS_W("Passthrough: Failed to create drawer: unknown reason"));
		DestroyDrawer();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPPassThroughDrawDevice::CreateDrawer(bool zoom_required, bool should_benchmark)
{
	// �v���C�}�����C���̃T�C�Y���擾
	tjs_int srcw, srch;
	GetSrcSize(srcw, srch);

	// �������� Drawer ���폜
	tDrawerType last_type = DrawerType;
	DestroyDrawer();

	// �v���C�}�����C�����Ȃ��Ȃ�� DrawDevice �͍쐬���Ȃ�
	if(srcw == 0 || srch == 0) return;

	// should_benchmark ���U�ŁA�O�� Drawer ���쐬���Ă���΁A����Ɠ����^�C�v��
	// Drawer ��p����
	if(!Drawer && !should_benchmark && last_type != dtNone)
		CreateDrawer(last_type);

	// PreferredDrawerType ���w�肳��Ă���΂�����g��
	if(!Drawer)
	{
		// PreferredDrawerType �� dtDrawDib �̏ꍇ�́A�Y�[�����K�v�ȏꍇ��
		// dtGDI ��p����
		if (PreferredDrawerType == dtDrawDib)
			CreateDrawer(zoom_required ? dtDBGDI : dtDrawDib);
		else if(PreferredDrawerType != dtNone)
			CreateDrawer(PreferredDrawerType);
	}

	// �����Y�[�����K�v�Ȃ��A�_�u���o�b�t�@�����O���K�v�Ȃ��Ȃ��
	// ��Ԋ�{�I�� DrawDib �̂���g��
	if(!Drawer && !zoom_required && !TVPForceDoublebuffer)
		CreateDrawer(dtDrawDib);

	if(!Drawer)
	{
		// ���C���E�B���h�E�ȊO�̏ꍇ�̓Y�[�����K�v�Ȃ���Ί�{�I�ȃ��\�b�h���g��
		if(!IsMainWindow && !zoom_required)
			CreateDrawer(dtDrawDib);
	}

	if(!Drawer)
	{
		// �܂� Drawer ���쐬����ĂȂ���

		// �x���`�}�[�N���܂�����
		static tDrawerType bench_types[] = { dtDBDD, dtDBGDI, dtDBD3D };
		const static tjs_char * type_names[] = { TJS_W("DirectDraw"), TJS_W("GDI"), TJS_W("Direct3D") };
		static const int num_types = sizeof(bench_types) / sizeof(bench_types[0]);
		struct tBenchmarkResult
		{
			float score;
			tDrawerType type;
		} results[num_types];

		// �x���`�}�[�N�p�̌��摜���m��
		BITMAPINFOHEADER bmi;
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biWidth = srcw;
		bmi.biHeight = srch;
		bmi.biPlanes = 1;
		bmi.biBitCount = 32;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = srcw * 4 * srch; // 32bpp �̏ꍇ�͂���ł���
		bmi.biXPelsPerMeter = 0;
		bmi.biYPelsPerMeter = 0;
		bmi.biClrUsed = 0;
		bmi.biClrImportant = 0;

		void * memblk = GlobalAlloc(GMEM_FIXED, bmi.biSizeImage + 64); // 64 = �]�T(�����Ă�������������Ȃ�)
		ZeroMemory(memblk, bmi.biSizeImage);

		tTVPRect cliprect;
		cliprect.left = 0;
		cliprect.top = 0;
		cliprect.right = srcw;
		cliprect.bottom = srch;

		// �x���`�}�[�N���s��
		for(int i = 0; i < num_types; i++)
		{
			results[i].type = bench_types[i];
			results[i].score = -1.0f;

			try
			{
				// drawer ���쐬
				CreateDrawer(results[i].type);
				if(!Drawer)
				{
					TVPAddImportantLog(TJS_W("Passthrough: Could not create drawer object ") + ttstr(type_names[i]));
					continue;
				}

				// �Y�[����Ԃ̐ݒ�͎󂯓����邩�H
				int caps = Drawer->GetInterpolationCapability();
				if(TVPZoomInterpolation && !(caps & 2))
				{
					TVPAddImportantLog(TJS_W("Passthrough: Drawer object ") + ttstr(type_names[i]) +
						TJS_W(" does not have smooth zooming functionality"));
					continue;
				}
				else if(!TVPZoomInterpolation && !(caps & 1))
				{
					TVPAddImportantLog(TJS_W("Passthrough: Drawer object ") + ttstr(type_names[i]) +
						TJS_W(" does not have point-on-point zooming functionality"));
					continue;
				}

				// �x���`�}�[�N���s��
				// �������Ԗ�333ms�ŁA���̊Ԃɉ���]�����s���邩������
				Drawer->InitTimings();
				static const DWORD timeout = 333;
				DWORD start_tick = timeGetTime();
				int count = 0;
				while(timeGetTime() - start_tick < timeout)
				{
					Drawer->StartBitmapCompletion();
					Drawer->NotifyBitmapCompleted(0, 0, memblk, (const BITMAPINFO *)&bmi, cliprect);
					Drawer->EndBitmapCompletion();
					Drawer->Show();
					count ++;
				}
				DWORD end_tick = timeGetTime();
				Drawer->ReportTimings();

				// ���ʂ��i�[�A����ƃf�o�b�O�p�ɕ\��
				results[i].score = count * 1000 / (float)(end_tick - start_tick);
				char msg[80];
				sprintf(msg, "%.2f fps", (float)results[i].score);
				TVPAddImportantLog(TJS_W("Passthrough: benchmark result: ") + ttstr(type_names[i]) + TJS_W(" : ") +
					msg);

				// GDI �͍Ō�̎�i
				// ���ʂ����͌v���Ă������A���ꂪ���ɂȂ�̂�
				// �ق���drawer�Ɏ��s�����Ƃ��̂�
				if(results[i].type == dtDBGDI)
					results[i].score = 0.0f;

			}
			catch(...)
			{
				DestroyDrawer();
			}
			DestroyDrawer();
		}

		// �x���`�}�[�N�Ɏg�����摜�����
		GlobalFree((HGLOBAL)memblk);


		// ���ʂ��X�R�A���Ƀ\�[�g
		// ����Ȃɐ��͑����Ȃ��̂Ō��n�I�Ƀo�u���\�[�g
		while(true)
		{
			bool swapped = false;
			for(int i = 0; i < num_types - 1; i++)
			{
				if(results[i].score < results[i+1].score)
				{
					tBenchmarkResult tmp = results[i];
					results[i] = results[i+1];
					results[i+1] = tmp;
					swapped = true;
				}
			}
			if(!swapped) break;
		}
	
		// �X�R�A�̍���������쐬�����݂�
		for(int i = 0; i < num_types; i++)
		{
			CreateDrawer(results[i].type);
			if(Drawer) break;
		}

	}

	if(!Drawer)
	{
		// Drawer ��S���쐬�ł��Ȃ�����
		// ����̓��o��
		// �܂����蓾�Ȃ����v���I�B
		TVPThrowExceptionMessage(TJS_W("Fatal: Could not create any drawer objects."));
	}

	if(Drawer)
	{
		if(IsMainWindow)
			TVPAddImportantLog(TJS_W("Passthrough: Using passthrough draw device: ") + Drawer->GetName());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPPassThroughDrawDevice::EnsureDrawer()
{
	// ���̃��\�b�h�ł́A�ȉ��̏����̍ۂ� drawer �����(��蒼��)�B
	// 1. Drawer �� NULL �̏ꍇ
	// 2. ���݂� Drawer �̃^�C�v���K�؂łȂ��Ȃ����Ƃ�
	// 3. ���̃��C���̃T�C�Y���ύX���ꂽ�Ƃ�
	TVPInitPassThroughOptions();

	if(TargetWindow)
	{
		// �Y�[���͕K�v���������H
		bool zoom_was_required = false;
		if(Drawer)
		{
			tjs_int srcw, srch;
			Drawer->GetSrcSize(srcw, srch);
			tjs_int destw, desth;
			Drawer->GetDestSize(destw, desth);
			if(destw != srcw || desth != srch)
				zoom_was_required = true;
		}

		// �Y�[����(�����)�K�v���H
		bool zoom_is_required = false;
		tjs_int srcw, srch;
		GetSrcSize(srcw, srch);
		if(DestRect.get_width() != srcw || DestRect.get_height() != srch)
			zoom_is_required = true;


		bool need_recreate = false;
		bool should_benchmark = false;
		if(!Drawer) need_recreate = true;
		if(zoom_was_required != zoom_is_required) need_recreate = true;
		if(need_recreate) should_benchmark = true;
		if(SrcSizeChanged) { SrcSizeChanged = false; need_recreate = true; }
			// SrcSizeChanged �Ƃ������R�����ł� should_benchmark �͐^�ɂ�
			// �ݒ肵�Ȃ�

		if(need_recreate)
		{
			// Drawer �̍č쐬���K�v
			CreateDrawer(zoom_is_required, should_benchmark);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	if(inherited::Managers.size() > 0)
	{
		// "Pass Through" �f�o�C�X�ł͂Q�ȏ��Layer Manager��o�^�ł��Ȃ�
		TVPThrowExceptionMessage(TJS_W("\"passthrough\" device does not support layer manager more than 1"));
			// TODO: i18n
	}
	inherited::AddLayerManager(manager);

	manager->SetDesiredLayerType(ltOpaque); // ltOpaque �ȏo�͂��󂯎�肽��
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::SetTargetWindow(HWND wnd, bool is_main)
{
	TVPInitPassThroughOptions();
	DestroyDrawer();
	TargetWindow = wnd;
	IsMainWindow = is_main;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::SetDestRectangle(const tTVPRect & rect)
{
	// �ʒu�����̕ύX�̏ꍇ���ǂ������`�F�b�N����
	if(rect.get_width() == DestRect.get_width() && rect.get_height() == DestRect.get_height())
	{
		// �ʒu�����̕ύX��
		if(Drawer) Drawer->SetDestPos(rect.left, rect.top);
		inherited::SetDestRectangle(rect);
	}
	else
	{
		// �T�C�Y���Ⴄ
		DestSizeChanged = true;
		inherited::SetDestRectangle(rect);
		EnsureDrawer();
		if(Drawer)
		{
			if(!Drawer->SetDestSize(rect.get_width(), rect.get_height()))
				DestroyDrawer(); // �G���[���N�������̂ł��� drawer ��j���
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::NotifyLayerResize(iTVPLayerManager * manager)
{
	inherited::NotifyLayerResize(manager);
	SrcSizeChanged = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::Show()
{
	if(Drawer) Drawer->Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	EnsureDrawer();

	// ���̒��� DestroyDrawer ���Ă΂��\���ɒ��ӂ��邱��
	if(Drawer) Drawer->StartBitmapCompletion();

	if(!Drawer)
	{
		// ���g���C����
		EnsureDrawer();
		if(Drawer) Drawer->StartBitmapCompletion();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPPassThroughDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo �ŕ\�����r�b�g�}�b�v�� cliprect �̗̈���Ax, y �ɕ`��
	// ����B
	// opacity �� type �͖������邵���Ȃ��̂Ŗ�������
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



