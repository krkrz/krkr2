#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include <windows.h>
#include "tp_stub.h"
#include <iostream>
#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

#include <string>
using namespace std;

#include "BasicDrawDevice.h"


/**
 * レイヤマネージャ用付随情報
 */
class LayerManagerInfo {
	
public:
	LayerManagerInfo(ITexture *texture);
	virtual ~LayerManagerInfo();
	
private:
	// コピー処理用一時変数
	unsigned char *destBuffer;
	int destWidth;
	int destHeight;
	int destPitch;
	
public:
	// 割り当てテクスチャ
	ITexture *texture;

	// テクスチャ描画操作用
	void lock();
	void copy(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
			  const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	void unlock();
};

/**
 * Irrlicht ベースの DrawDevice
 */
class tTVPIrrlichtDrawDevice : public tTVPDrawDevice, public tTVPContinuousEventCallbackIntf, public IEventReceiver
{
	typedef tTVPDrawDevice inherited;

	/// デバイス
	IrrlichtDevice *device;

public:
	tTVPIrrlichtDrawDevice(); //!< コンストラクタ
private:
	virtual ~tTVPIrrlichtDrawDevice(); //!< デストラクタ

	// テクスチャの割り当て
	void allocInfo(iTVPLayerManager * manager);
	// テクスチャの解放
	void freeInfo(iTVPLayerManager * manager);
	
	IrrlichtDevice *create(HWND hwnd, video::E_DRIVER_TYPE type);
	void attach(HWND hwnd);
	void detach();

	int width;
	int height;

	// Irrlicht にイベントを送る
	bool postEvent(SEvent &ev);
	
public:
	//---- LayerManager の管理関連
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

	//---- 描画位置・サイズ関連
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd);

	//---- ユーザーインターフェース関連
	// window → drawdevice
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);
	
	//---- LayerManager からの画像受け渡し関連
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

	//---------------------------------------------------------------------------

	// テスト用処理
	void init();


	// ------------------------------------------------------------
	// 吉里吉里からの呼び出し制御用
	// ------------------------------------------------------------
	
public:
	/**
	 * Irrlicht 呼び出し処理開始
	 */
	void start();

	/**
	 * Irrlicht 呼び出し処理中断
	 */
	void stop();

	/**
	 * Continuous コールバック
	 * 吉里吉里が暇なときに常に呼ばれる
	 * 塗り直し処理
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick);

	// ------------------------------------------------------------
	// イベント処理用
	// ------------------------------------------------------------

	virtual bool OnEvent(SEvent event);
};

#endif
