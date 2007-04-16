#ifndef IRRLICHTDRAWDEVICE_H
#define IRRLICHTDRAWDEVICE_H

#include <windows.h>
#include "tp_stub.h"
#include "BasicDrawDevice.h"

#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;

#include <string>
using namespace std;

/**
 * Irrlicht ベースの DrawDevice
 */
class tTVPIrrlichtDrawDevice : public tTVPDrawDevice, public tTVPContinuousEventCallbackIntf
{
	typedef tTVPDrawDevice inherited;

	/// デバイス
	IrrlichtDevice *device;
	/// ドライバ
	video::IVideoDriver* driver;
	/// シーンマネージャ
	ISceneManager* smgr;

	/// レイヤ描画先テクスチャ
	ITexture *texture;
	
public:
	tTVPIrrlichtDrawDevice(); //!< コンストラクタ
private:
	virtual ~tTVPIrrlichtDrawDevice(); //!< デストラクタ

	IrrlichtDevice *create(HWND hwnd, video::E_DRIVER_TYPE type);
	void attach(HWND hwnd);
	void detach();

	int width;
	int height;

	unsigned char *destBuffer;
	int destWidth;
	int destHeight;
	int destPitch;

public:

	//---- 描画位置・サイズ関連
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd);

	//---- LayerManager からの画像受け渡し関連
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

	//---------------------------------------------------------------------------

	// テスト用処理
	void init();

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
};

#endif
