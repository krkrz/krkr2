#include "IrrlichtBase.h"
#include "ncbind/ncbind.hpp"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

/**
 * コンストラクタ
 */
IrrlichtBase::IrrlichtBase(video::E_DRIVER_TYPE driverType) : driverType(driverType), device(NULL), driver(NULL)
{
}

/**
 * デストラクタ
 */
IrrlichtBase::~IrrlichtBase()
{
	detach();
}

/**
 * ドライバの情報表示
 */
void
IrrlichtBase::showDriverInfo()
{
	if (driver) {
		dimension2d<s32> size = driver->getScreenSize();
		message_log("デバイス生成後のスクリーンサイズ:%d, %d", size.Width, size.Height);
		size = driver->getCurrentRenderTargetSize();
		message_log("デバイス生成後のRenderTargetの:%d, %d", size.Width, size.Height);
	}
}

/**
 * ウインドウの再設定
 * @param hwnd ハンドル
 */
void
IrrlichtBase::attach(HWND hwnd)
{
	// デバイス生成
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<void*>(hwnd);
	params.DriverType    = driverType;
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	params.EventReceiver = this;
	params.AntiAlias = true;

	if ((device = irr::createDeviceEx(params))) {
		TVPAddLog(L"DirectX9で初期化");
	} else {
		TVPThrowExceptionMessage(L"Irrlicht デバイスの初期化に失敗しました");
	}
	driver = device->getVideoDriver();
	
	showDriverInfo();
	start();
}

/**
 * ウインドウの解除
 */
void
IrrlichtBase::detach()
{
	if (device) {
		device->drop();
		device = NULL;
		driver = NULL;
	}
	stop();
}

/**
 * Irrlicht 呼び出し処理開始
 */
void
IrrlichtBase::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Irrlicht 呼び出し処理停止
 */
void
IrrlichtBase::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * Continuous コールバック
 * 吉里吉里が暇なときに常に呼ばれる
 * これが事実上のメインループになる
 */
void TJS_INTF_METHOD
IrrlichtBase::OnContinuousCallback(tjs_uint64 tick)
{
	if (device) {
		// 時間を進める XXX tick を外部から与えられないか？
		device->getTimer()->tick();
		
		// 描画開始
		if (driver->beginScene(true, true, irr::video::SColor(255,0,0,0))) {
			/// シーンマネージャの描画
			device->getSceneManager()->drawAll();
			// 固有処理
			update(tick);
			// GUIの描画
			device->getGUIEnvironment()->drawAll();
			// 描画完了
			driver->endScene();
		}
	}
};

/**
 * イベント受理
 * HWND を指定して生成している関係で Irrlicht 自身はウインドウから
 * イベントを取得することはない。ので GUI Environment からのイベント
 * だけがここにくることになる？
 * @param event イベント情報
 * @return 処理したら true
 */
bool
IrrlichtBase::OnEvent(const irr::SEvent &event)
{
	switch (event.EventType) {
	case EET_GUI_EVENT:
		message_log("GUIイベント:%d", event.GUIEvent.EventType);
		switch(event.GUIEvent.EventType) {
		case EGET_BUTTON_CLICKED:
			message_log("ボタンおされた");
			break;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
#if 0
		message_log("マウスイベント:%d x:%d y:%d wheel:%f",
			event.MouseInput.Event,
			event.MouseInput.X,
			event.MouseInput.Y,
			event.MouseInput.Wheel);
#endif
		break;
	case EET_KEY_INPUT_EVENT:
//		message_log("キーイベント:%x", event.KeyInput.Key);
		{
			int shift = 0;
			if (event.KeyInput.Shift) {
				shift |= 0x01;
			}
			if (event.KeyInput.Control) {
				shift |= 0x04;
			}
		}
		break;
	case EET_LOG_TEXT_EVENT:
		message_log("ログレベル:%d ログ:%s",
					event.LogEvent.Level,
					event.LogEvent.Text);
		break;
	case EET_USER_EVENT:
		message_log("ユーザイベント");
		break;
	}
	return false;
}

/**
 * 画像のキャプチャ
 * @param dest 格納先レイヤ
 */
void
IrrlichtBase::captureScreenShot(iTJSDispatch2 *dest)
{
	// レイヤインスタンス以外ではエラー

	if (dest->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Layer",dest) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"dest is not layer");
	}
	// 画像をキャプチャ
	IImage *screen;
	if (driver == NULL || (screen = driver->createScreenShot()) == NULL) {
		TVPThrowExceptionMessage(L"can't get screenshot");
	}

	dimension2d<s32> size = screen->getDimension();
	int width  = size.Width;
	int height = size.Height;

	ncbPropAccessor layer(dest);
	layer.SetValue(L"width",  width);
	layer.SetValue(L"height", height);
	layer.SetValue(L"imageLeft",  0);
	layer.SetValue(L"imageTop",   0);
	layer.SetValue(L"imageWidth",  width);
	layer.SetValue(L"imageHeight", height);

	unsigned char *buffer = (unsigned char*)layer.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
	int pitch = layer.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());

	// 画像データのコピー
	if (screen->getColorFormat() == ECF_A8R8G8B8) {
		unsigned char *src = (unsigned char*)screen->lock();
		int slen   = width * 4;
		int spitch = screen->getPitch();
		for (int y=0;y<height;y++) {
			memcpy(buffer, src, slen);
			src    += spitch;
			buffer += pitch;
		}
		screen->unlock();
	} else {
		for (int y=0;y<height;y++) {
			u32 *line = (u32 *)buffer;
			for (int x=0;x<width;x++) {
				*line++ = screen->getPixel(x, y).color;
				buffer += pitch;
			}
		}
	}

	dest->FuncCall(0, L"update", NULL, NULL, 0, NULL, dest);
	screen->drop();
}
