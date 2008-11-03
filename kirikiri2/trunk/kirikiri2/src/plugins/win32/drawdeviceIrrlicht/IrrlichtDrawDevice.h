#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include "IrrlichtBase.h"
#include "BasicDrawDevice.h"

/**
 * Irrlicht ベースの DrawDevice
 */
class IrrlichtDrawDevice :
	public IrrlichtBase,
	public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;

protected:
	tjs_int screenWidth;
	tjs_int screenHeight;
	tjs_int width;
	tjs_int height;
	bool zoomMode;
	irr::core::rect<irr::s32> destRect;

public:
	// コンストラクタ
	IrrlichtDrawDevice(int width, int height);
	// デストラクタ
	virtual ~IrrlichtDrawDevice();

	// -----------------------------------------------------------------------
	// continuous handler
	// -----------------------------------------------------------------------
public:
	/**
	 * Continuous コールバック
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick);
	
protected:
	// zoomMode 更新
	void updateZoomMode();

	// テクスチャの割り当て
	void allocInfo(iTVPLayerManager * manager);
	// テクスチャの解放
	void freeInfo(iTVPLayerManager * manager);

	// デバイス解放処理
	virtual void detach();
	
	// Irrlicht にイベントを送る
	bool postEvent(irr::SEvent &ev);

	/**
	 * Device→Irrlicht方向の座標の変換を行う
	 * @param		x		X位置
	 * @param		y		Y位置
	 * @note		x, y は DestRectの (0,0) を原点とする座標として渡されると見なす
	 */
	void TransformTo(tjs_int &x, tjs_int &y);

	/** Irrlicht→Device方向の座標の変換を行う
	 * @param		x		X位置
	 * @param		y		Y位置
	 * @note		x, y は レイヤの (0,0) を原点とする座標として渡されると見なす
	 */
	void TransformFrom(tjs_int &x, tjs_int &y);

	// ------------------------------------------------------------
	// 更新処理
	// ------------------------------------------------------------
protected:
	/**
	 * クラス固有更新処理
	 * シーンマネージャの処理後、GUIの処理前に呼ばれる
	 */
	void update(irr::video::IVideoDriver *driver);
	
public:
	//---- LayerManager の管理関連
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

	//---- 描画位置・サイズ関連
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager) {}

	//---- ユーザーインターフェース関連
	// window → drawdevice
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);

	//---- 再描画関連
	virtual void TJS_INTF_METHOD Show();
	
	//---- LayerManager からの画像受け渡し関連
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

	// -----------------------------------------------------------------------
	// 共通メソッド呼び出し用
	// -----------------------------------------------------------------------

public:
	irr::video::IVideoDriver *getVideoDriver() {
		return IrrlichtBase::getVideoDriver();
	}

	irr::scene::ISceneManager *getSceneManager() {
		return IrrlichtBase::getSceneManager();
	}

	irr::gui::IGUIEnvironment *getGUIEnvironment() {
		return IrrlichtBase::getGUIEnvironment();
	}

	// -----------------------------------------------------------------------
	// 固有メソッド
	// -----------------------------------------------------------------------
	
public:
	/**
	 * @return デバイス情報
	 */
	tjs_int64 getDevice() {
		return reinterpret_cast<tjs_int64>((tTVPDrawDevice*)this);
	}

	bool getZoomMode() {
		return zoomMode;
	}
	
	void setZoomMode(bool zoomMode) {
		this->zoomMode = zoomMode;
		updateZoomMode();
	}

	tjs_int getWidth() {
		return width;
	}
	
	void setWidth(tjs_int width) {
		this->width = width;
		Window->NotifySrcResize();
	}

	tjs_int getHeight() {
		return height;
	}
	
	void setHeight(tjs_int height) {
		this->height = height;
		Window->NotifySrcResize();
	}

	void setSize(tjs_int width, tjs_int height) {
		this->width = width;
		this->height = height;
		Window->NotifySrcResize();
	}
};

#endif
