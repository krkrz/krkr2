#include "ncbind/ncbind.hpp"
#include "IrrlichtSimpleDevice.h"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

// イベント処理
bool __stdcall
IrrlichtSimpleDevice::messageHandler(void *userdata, tTVPWindowMessage *Message)
{
	IrrlichtSimpleDevice *self = (IrrlichtSimpleDevice*)userdata;
	switch (Message->Msg) {
	case TVP_WM_DETACH:
		self->destroyWindow();
		break;
	case TVP_WM_ATTACH:
		self->createWindow((HWND)Message->LParam);
		break;
	default:
		break;
	}
	return false;
}

// ユーザメッセージレシーバの登録/解除
void
IrrlichtSimpleDevice::setReceiver(tTVPWindowMessageReceiver receiver, bool enable)
{
	tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
	tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
	tTJSVariant userdata = (tTVInteger)(tjs_int)this;
	tTJSVariant *p[] = {&mode, &proc, &userdata};
	if (window->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, window) != TJS_S_OK) {
		if (enable) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}
}

/**
 * ウインドウを生成
 * @param parent 親ウインドウ
 */
void
IrrlichtSimpleDevice::createWindow(HWND krkr)
{
	attach(krkr, width, height, -1);
}

/**
 * ウインドウを破棄
 */
void
IrrlichtSimpleDevice::destroyWindow()
{
	detach();
}

/**
 * コンストラクタ
 */
IrrlichtSimpleDevice::IrrlichtSimpleDevice(iTJSDispatch2 *window, int width, int height)
	: IrrlichtBase(), window(window), width(width), height(height)
{
	if (window == NULL || window->IsInstanceOf(0, NULL, NULL, L"Window", window) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"must set window object");
	}
	window->AddRef();
	setReceiver(messageHandler, true);
	
	tTJSVariant krkrHwnd; // 親のハンドル
	if (window->PropGet(0, TJS_W("HWND"), NULL, &krkrHwnd, window) == TJS_S_OK) {
		HWND hwnd = (HWND)(tjs_int)krkrHwnd;
		if (hwnd) {
			createWindow(hwnd);
		}
	}
}

/**
 * デストラクタ
 */
IrrlichtSimpleDevice::~IrrlichtSimpleDevice()
{
	destroyWindow();
	if (window) {
		setReceiver(messageHandler, false);
		window->Release();
		window = NULL;
	}
}

// -----------------------------------------------------------------------
// 固有機能
// -----------------------------------------------------------------------

void
IrrlichtSimpleDevice::_setSize()
{
	if (device) {
		IVideoDriver *driver = device->getVideoDriver();
		if (driver) {
			driver->OnResize(dimension2d<s32>(width, height));
		}
	}
}

/**
 */
void
IrrlichtSimpleDevice::updateToLayer(iTJSDispatch2 *layer)
{
	if (device && layer) {
		// 時間を進める XXX tick を外部から与えられないか？
		device->getTimer()->tick();
		
		IVideoDriver *driver = device->getVideoDriver();
		// 描画開始
		if (driver && driver->beginScene(true, true, irr::video::SColor(255,0,0,0))) {

			/// シーンマネージャの描画
			ISceneManager *smgr = device->getSceneManager();
			if (smgr) {
				smgr->drawAll();
			}
			
			// GUIの描画
			IGUIEnvironment *gui = device->getGUIEnvironment();
			if (gui) {
				gui->drawAll();
			}

			// レイヤ情報取得
			ncbPropAccessor obj(layer);
			tjs_int dwidth  = obj.GetValue(L"imageWidth", ncbTypedefs::Tag<tjs_int>());
			tjs_int dheight = obj.GetValue(L"imageHeight", ncbTypedefs::Tag<tjs_int>());
			tjs_int dPitch  = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
			unsigned char *dbuffer = (unsigned char *)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());

			// 描画先のビットマップを作る
			// 直接処理できるといいんだけど……
			BITMAPINFO biBMP;
			ZeroMemory(&biBMP, sizeof biBMP);
			biBMP.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			biBMP.bmiHeader.biBitCount = 32;
			biBMP.bmiHeader.biPlanes = 1;
			biBMP.bmiHeader.biWidth  = dwidth;
			biBMP.bmiHeader.biHeight = -dheight;
			void *buffer;
			HBITMAP hbmp = CreateDIBSection(NULL, &biBMP, DIB_RGB_COLORS, &buffer, NULL, 0);
			if (hbmp) {
				// 描画用にDCを作る
				HDC destDC = CreateCompatibleDC(NULL);
				if (destDC) {
					HBITMAP old = (HBITMAP)SelectObject(destDC, hbmp);
					// そのDCに対して描画してもらう
					irr::core::rect<s32> destRect(0,0,dwidth,dheight);
					driver->endScene(0, NULL, &destRect, destDC);
					SelectObject(destDC, old);
					DeleteDC(destDC);
				}
				// ビットマップからコピー
				for (tjs_int y = 0; y < dheight; y++) {
					unsigned char *src = (unsigned char *)(buffer) + dwidth * y * 4;
					CopyMemory(dbuffer, src, dwidth*4);
					dbuffer += dPitch;
				}
				// レイヤを更新
				layer->FuncCall(0, L"update", NULL, NULL, 0, NULL, layer);

				DeleteObject(hbmp);
			}
		}
	}
}
