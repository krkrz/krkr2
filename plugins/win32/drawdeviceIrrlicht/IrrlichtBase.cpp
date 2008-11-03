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
IrrlichtBase::IrrlichtBase() : device(NULL)
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
	IVideoDriver *driver = device->getVideoDriver();
	if (driver) {
		dimension2d<s32> size = driver->getScreenSize();
		message_log("デバイス生成後のスクリーンサイズ:%d, %d", size.Width, size.Height);
		size = driver->getCurrentRenderTargetSize();
		message_log("デバイス生成後のRenderTargetの:%d, %d", size.Width, size.Height);
	}
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
 * ウインドウの再設定
 * @param hwnd ハンドル
 */
void
IrrlichtBase::attach(HWND hwnd)
{
	// デバイス生成
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<void*>(hwnd);
	params.DriverType    = EDT_DIRECT3D9;
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	params.EventReceiver = this;
	params.AntiAlias = true;

	if ((device = irr::createDeviceEx(params))) {
		TVPAddLog(L"Irrlichtデバイス初期化");
	} else {
		TVPThrowExceptionMessage(L"Irrlicht デバイスの初期化に失敗しました");
	}
	
	showDriverInfo();
	start();
}

/**
 * ウインドウの解除
 */
void
IrrlichtBase::detach()
{
	stop();
	if (device) {
		device->drop();
		device = NULL;
	}
}

void
IrrlichtBase::show(irr::core::rect<s32> *destRect)
{
	if (device) {
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
			
			// 固有処理
			update(driver);

			// GUIの描画
			IGUIEnvironment *gui = device->getGUIEnvironment();
			if (gui) {
				gui->drawAll();
			}
			
			// 描画完了
			driver->endScene(0, NULL, destRect);
		}
	}
};

/**
 * イベント受理
 * HWND を指定して生成している関係で Irrlicht 自身はウインドウから
 * イベントを取得することはない。ので GUI Environment からのイベント
 * だけがここにくることになる。自分の適当なメソッドを呼び出すように要修正 XXX
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
