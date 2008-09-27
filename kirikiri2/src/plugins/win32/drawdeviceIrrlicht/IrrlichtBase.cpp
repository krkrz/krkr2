#include "IrrlichtBase.h"

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
IrrlichtBase::IrrlichtBase()
{
	device = NULL;
	driver = NULL;
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
 * ウインドウの解除
 */
void
IrrlichtBase::detach()
{
	if (device) {
		device->closeDevice();
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
		driver->beginScene(true, true, irr::video::SColor(255,0,0,0));
		
		/// シーンマネージャの描画
		device->getSceneManager()->drawAll();
		
		// 固有処理
		update(tick);
		
		// GUIの描画
		device->getGUIEnvironment()->drawAll();
		
		// 描画完了
		driver->endScene();
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
