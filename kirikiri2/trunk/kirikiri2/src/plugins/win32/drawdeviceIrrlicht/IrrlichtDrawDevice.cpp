#include "IrrlichtDrawDevice.h"
#include "LayerManagerInfo.h"

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
IrrlichtDrawDevice::IrrlichtDrawDevice() : IrrlichtBase()
{
}

/**
 * デストラクタ
 */
IrrlichtDrawDevice::~IrrlichtDrawDevice()
{
}

/**
 * テクスチャの割り当て
 * @param manager レイヤマネージャ
 */
void 
IrrlichtDrawDevice::allocInfo(iTVPLayerManager * manager)
{
	if (driver) {
	
		// テクスチャ割り当てXXX サイズ判定が必要…
		ITexture *texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
		if (texture == NULL) {
			TVPThrowExceptionMessage(L"テクスチャの割り当てに失敗しました");
		}

		// 設定
		tjs_int w, h;
		manager->GetPrimaryLayerSize(w, h);
		manager->SetDrawDeviceData((void*)new LayerManagerInfo(texture, w, h));
		
		// 更新要求
		manager->RequestInvalidation(tTVPRect(0,0,w,h));
	}
}

/**
 *  テクスチャの解放
 * @param manager レイヤマネージャ
 */
void
IrrlichtDrawDevice::freeInfo(iTVPLayerManager * manager)
{
	if (driver) {
		LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
		if (info != NULL) {
			driver->removeTexture(info->texture);
			manager->SetDrawDeviceData(NULL);
			delete info;
		}
	}
}

/**
 * ウインドウの解除
 */
void
IrrlichtDrawDevice::detach()
{
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		freeInfo(*i);
	}
	IrrlichtBase::detach();
}

/**
 * Irrlicht へのイベント送信
 */
bool
IrrlichtDrawDevice::postEvent(SEvent &ev)
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
 * レイヤマネージャの登録
 * @param manager レイヤマネージャ
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	allocInfo(manager);
	tTVPDrawDevice::AddLayerManager(manager);
}

/**
 * レイヤマネージャの削除
 * @param manager レイヤマネージャ
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::RemoveLayerManager(iTVPLayerManager * manager)
{
	freeInfo(manager);
	tTVPDrawDevice::RemoveLayerManager(manager);
}


/***
 * ウインドウの指定
 * @param wnd ウインドウハンドラ
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::SetTargetWindow(HWND wnd, bool is_main)
{
	detach();
	if (wnd != NULL) {
		attach(wnd);
		// マネージャに対するテクスチャの割り当て
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			allocInfo(*i);
		}
		// 駆動開始
		start();
	}
}

void
IrrlichtDrawDevice::Show()
{
}

// -------------------------------------------------------------------------------------
// 入力イベント処理用
// -------------------------------------------------------------------------------------

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if (driver) {
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = x;
		ev.MouseInput.Y = y;
		ev.MouseInput.Wheel = 0;
		switch ((mb & 0xff)) {
		case mbLeft:
			ev.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;
			break;
		case mbMiddle:
			ev.MouseInput.Event = EMIE_MMOUSE_PRESSED_DOWN;
			break;
		case mbRight:
			ev.MouseInput.Event = EMIE_RMOUSE_PRESSED_DOWN;
			break;
		}
		if (postEvent(ev)) {
			return;
		}
	}
	// XXX 画面サイズに応じた補正が必要
	tTVPDrawDevice::OnMouseDown(x, y, mb, flags);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if (driver) {
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = x;
		ev.MouseInput.Y = y;
		ev.MouseInput.Wheel = 0;
		switch ((mb & 0xff)) {
		case mbLeft:
			ev.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;
			break;
		case mbMiddle:
			ev.MouseInput.Event = EMIE_MMOUSE_LEFT_UP;
			break;
		case mbRight:
			ev.MouseInput.Event = EMIE_RMOUSE_LEFT_UP;
			break;
		}
		if (postEvent(ev)) {
			return;
		}
	}
	// XXX 画面サイズに応じた補正が必要
	tTVPDrawDevice::OnMouseUp(x, y, mb, flags);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
{
	if (driver) {
		SEvent ev;
		ev.EventType = EET_MOUSE_INPUT_EVENT;
		ev.MouseInput.X = x;
		ev.MouseInput.Y = y;
		ev.MouseInput.Wheel = 0;
		ev.MouseInput.Event = EMIE_MOUSE_MOVED;
		if (postEvent(ev)) {
			return;
		}
	}
	// XXX 画面サイズに応じた補正が必要
	tTVPDrawDevice::OnMouseMove(x, y, flags);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnKeyDown(tjs_uint key, tjs_uint32 shift)
{
	tTVPDrawDevice::OnKeyDown(key, shift);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnKeyUp(tjs_uint key, tjs_uint32 shift)
{
	tTVPDrawDevice::OnKeyUp(key, shift);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnKeyPress(tjs_char key)
{
	tTVPDrawDevice::OnKeyPress(key);
}

void TJS_INTF_METHOD
IrrlichtDrawDevice::OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)
{
	tTVPDrawDevice::OnMouseWheel(shift, delta, x, y);
}

// -------------------------------------------------------------------------------------
// 描画処理用
// -------------------------------------------------------------------------------------

/**
 * ビットマップコピー処理開始
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->lock();
	}
}

/**
 * ビットマップコピー処理
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->copy(x, y, bits, bitmapinfo, cliprect, type, opacity);
	}
}

/**
 * ビットマップコピー処理終了
 */
void TJS_INTF_METHOD
IrrlichtDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->unlock();
	}
}

// -------------------------------------------------------------------------------------
// 画面更新処理
// -------------------------------------------------------------------------------------

void
IrrlichtDrawDevice::update(tjs_uint64 tick)
{
	dimension2d<s32> screenSize = driver->getScreenSize();

	// 個別レイヤマネージャの描画
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		LayerManagerInfo *info = (LayerManagerInfo*)(*i)->GetDrawDeviceData();
		if (info && info->texture) {
			// XXX レイヤを3D空間配置？
			driver->draw2DImage(info->texture, core::position2d<s32>(0,0),
								//core::rect<s32>(0,0,screenSize.Width,screenSize.Height), 0, 
								core::rect<s32>(0,0,info->layerWidth,info->layerHeight), 0, 
								video::SColor(255,255,255,255), true);
		}
	}
}
