#include <windows.h>
#include "IrrlichtDrawDevice.h"
#include "LayerManagerInfo.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

extern void message_log(const char* format, ...);

/**
 * Irrlicht 呼び出し処理開始
 */
void
tTVPIrrlichtDrawDevice::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Irrlicht 呼び出し処理停止
 */
void
tTVPIrrlichtDrawDevice::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * Continuous コールバック
 * 吉里吉里が暇なときに常に呼ばれる
 * これが事実上のメインループになる
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnContinuousCallback(tjs_uint64 tick)
{
	if (device) {
		device->getTimer()->tick();

		/// ドライバ
		video::IVideoDriver* driver = device->getVideoDriver();

		dimension2d<s32> screenSize = driver->getScreenSize();
		
		// 描画開始
		driver->beginScene(true, true, irr::video::SColor(255,0,0,0));

		/// シーンマネージャの描画
		ISceneManager* smgr = device->getSceneManager();
		smgr->drawAll();

		// 個別レイヤマネージャの描画
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			LayerManagerInfo *info = (LayerManagerInfo*)(*i)->GetDrawDeviceData();
			if (info && info->texture) {
				driver->draw2DImage(info->texture, core::position2d<s32>(0,0),
									core::rect<s32>(0,0,screenSize.Width,screenSize.Height), 0, 
									video::SColor(255,255,255,255), true);
			}
		}

		// GUIの描画
		device->getGUIEnvironment()->drawAll();

		// SWF の描画 XXX
		
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
tTVPIrrlichtDrawDevice::OnEvent(irr::SEvent event)
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
		message_log("マウスイベント:%d x:%d y:%d wheel:%f",
			event.MouseInput.Event,
			event.MouseInput.X,
			event.MouseInput.Y,
			event.MouseInput.Wheel);
		break;
	case EET_KEY_INPUT_EVENT:
		message_log("キーイベント:%x", event.KeyInput.Key);
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
