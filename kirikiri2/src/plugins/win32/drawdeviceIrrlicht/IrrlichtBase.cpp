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
IrrlichtBase::IrrlichtBase(iTJSDispatch2 *objthis) : objthis(objthis), device(NULL), attached(false)
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

// イベント起動
void
IrrlichtBase::sendEvent(const tjs_char *eventName)
{
	tTJSVariant method;
	if (TJS_SUCCEEDED(objthis->PropGet(0, eventName, NULL, &method, objthis))) { // イベントメソッドを取得
		if (method.Type() == tvtObject) {
			iTJSDispatch2 *m = method.AsObjectNoAddRef();
			if (TJS_SUCCEEDED(m->IsInstanceOf(0, NULL, NULL, L"Function", m))) { // ファンクションかどうか
				tTJSVariant self(objthis, objthis);
				tTJSVariant *params[] = {&self};
				m->FuncCall(0, NULL, NULL, NULL, 1, params, method.AsObjectThisNoAddRef());
			}
		}
	}
}

/**
 * ウインドウの再設定
 * @param hwnd ハンドル
 */
void
IrrlichtBase::attach(HWND hwnd, int width, int height)
{
	if (!attached && hwnd) {
		// デバイス生成
		SIrrlichtCreationParameters params;
		params.WindowId     = reinterpret_cast<void*>(hwnd);
		params.DriverType    = EDT_DIRECT3D9;
		params.Stencilbuffer = true;
		params.Vsync = true;
		params.EventReceiver = this;
		params.AntiAlias = true;
		if (width != 0 && height != 0) {
			params.WindowSize = core::dimension2d<s32>(width, height);
		}
		if ((device = irr::createDeviceEx(params))) {
			TVPAddLog(L"Irrlichtデバイス初期化");
			// テクスチャのα合成時にも常にZテストを行うように。
			// device->getSceneManager()->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
			showDriverInfo();
			onAttach();
			sendEvent(L"onAttach");
		} else {
			TVPThrowExceptionMessage(L"Irrlicht デバイスの初期化に失敗しました");
		}
		attached = true;
	}
}

/**
 * ウインドウの解除
 */
void
IrrlichtBase::detach()
{
	if (device) {
		sendEvent(L"onDetach");
		onDetach();
		device->drop();
		device = NULL;
	}
	attached = false;
}

/**
 * Irrlicht描画処理
 * @param destRect 描画先領域
 * @param srcRect 描画元領域
 * @param destDC 描画先DC
 * @return 描画された
 */
bool
IrrlichtBase::show(irr::core::rect<irr::s32> *destRect, irr::core::rect<irr::s32> *srcRect, HDC destDC)
{
	if (device) {
		// 時間を進める XXX tick を外部から与えられないか？
		device->getTimer()->tick();
		
		IVideoDriver *driver = device->getVideoDriver();
		// 描画開始
		if (driver && driver->beginScene(true, true, irr::video::SColor(0,0,0,0))) {

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
			driver->endScene(0, srcRect, destRect, destDC);
			return true;
		}
	}
	return false;
};

/**
 * Irrlicht へのイベント送信
 */
bool
IrrlichtBase::postEvent(SEvent &ev)
{
	if (device) {
		if (device->getGUIEnvironment()->postEventFromUser(ev) ||
			device->getSceneManager()->postEventFromUser(ev)) {
			return true;
		}
	}
	return false;
}

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

// --------------------------------------------------------------------------------

/**
 * Irrlicht 呼び出し処理開始
 */
void
IrrlichtBaseUpdate::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Irrlicht 呼び出し処理停止
 */
void
IrrlichtBaseUpdate::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * コンストラクタ
 */
IrrlichtBaseUpdate::IrrlichtBaseUpdate(iTJSDispatch2 *objthis) : IrrlichtBase(objthis)
{
}


/**
 * デストラクタ
 */
IrrlichtBaseUpdate::~IrrlichtBaseUpdate()
{
	stop();
}
