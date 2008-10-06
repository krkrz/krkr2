#ifndef IRRLICHTDRIVER_H
#define IRRLICHTDRIVER_H

#include <windows.h>
#include "tp_stub.h"
#include <irrlicht.h>

/**
 * ドライバ操作用クラス
 */
class IrrlichtDriver {

protected:
	irr::video::IVideoDriver *driver;

public:
	IrrlichtDriver();
	IrrlichtDriver(irr::video::IVideoDriver *driver);
	IrrlichtDriver(const IrrlichtDriver &orig);
	~IrrlichtDriver();

	// ------------------------------------------------------------
	// ドライバ制御
	// ------------------------------------------------------------
public:
	/**
	 * 画像のキャプチャ
	 * @param dest 格納先レイヤ
	 */
	void captureScreenShot(iTJSDispatch2 *layer);

	// XXX 2D基本図形処理をつくるか？
	// シーンとは別に記録して描画するような形になると思われる
};

#endif
