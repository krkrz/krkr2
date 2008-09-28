#ifndef IRRLICHTBASE_H
#define IRRLICHTBASE_H

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
	/// デバイス
	irr::IrrlichtDevice *device;
	/// ドライバ
	irr::video::IVideoDriver *driver;

	void showDriverInfo();
	
public:
	IrrlichtBase(); //!< コンストラクタ
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
	virtual void update(tjs_uint64 tick) {};

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

	// ------------------------------------------------------------
	// シーン制御
	// ------------------------------------------------------------
public:
	/**
	 * 画像のキャプチャ
	 * @param dest 格納先レイヤ
	 */
	void captureScreenShot(iTJSDispatch2 *layer);

	/**
	 * シーンデータファイルの読み込み
	 */
	void loadScene(const char *name);

	/**
	 * カメラシーンの設定
	 */
	//void addCameraSceneNode(int no, irr::core::vector3df *a, irr::core::vector3df *b);
};

#endif