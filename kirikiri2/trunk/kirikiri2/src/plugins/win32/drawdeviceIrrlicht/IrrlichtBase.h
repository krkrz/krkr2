#ifndef IRRLICHTBASE_H
#define IRRLICHTBASE_H

#include "IrrlichtDriver.h"
#include "IrrlichtSceneManager.h"
#include "IrrlichtGUIEnvironment.h"

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * Irrlicht 処理のベース
 */
class IrrlichtBase :
	public tTVPContinuousEventCallbackIntf,
	public irr::IEventReceiver
{

protected:
	// ドライバ種別
	irr::video::E_DRIVER_TYPE  driverType;
	/// デバイス
	irr::IrrlichtDevice *device;

	void showDriverInfo();
	
public:
	IrrlichtBase(irr::video::E_DRIVER_TYPE driverType); //!< コンストラクタ
	virtual ~IrrlichtBase(); //!< デストラクタ

protected:
	// デバイス割り当て
	virtual void attach(HWND hwnd);
	
	// デバイスの破棄
	virtual void detach();

	// ------------------------------------------------------------
	// 吉里吉里からの呼び出し制御用
	// ------------------------------------------------------------
protected:
	/**
	 * クラス固有更新処理
	 * シーンマネージャの処理後、GUIの処理前に呼ばれる
	 */
	virtual void update(irr::video::IVideoDriver *driver, tjs_uint64 tick) {};

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
	// Irrlicht イベント処理用
	// ------------------------------------------------------------
public:
	/**
	 * イベント受理
	 * GUI Environment からのイベントがここに送られてくる
	 * @param event イベント情報
	 * @return 処理したら true
	 */
	virtual bool OnEvent(const irr::SEvent &event);

public:
	/**
	 * @return ドライバ情報の取得
	 */
	IrrlichtDriver getDriver();
	
	/**
	 * @return シーンマネージャ情報の取得
	 */
	IrrlichtSceneManager getSceneManager();

	/**
	 * @return GUI環境情報の取得
	 */
	IrrlichtGUIEnvironment getGUIEnvironment();
};

#endif