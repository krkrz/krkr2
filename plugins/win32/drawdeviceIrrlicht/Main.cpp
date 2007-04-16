#pragma comment(lib, "Irrlicht.lib")
#include "ncbind/ncbind.hpp"
#include "IrrlichtDrawDevice.h"

/**
 * Irrlicht ベース DrawDevice のクラス
 */
class IrrlichtDrawDevice {
public:
	// デバイス情報
	iTVPDrawDevice *device;
public:
	/**
	 * コンストラクタ
	 */
	IrrlichtDrawDevice() {
		device = new tTVPIrrlichtDrawDevice();
	}

	~IrrlichtDrawDevice() {
		if (device) {
			device->Destruct();
			device = NULL;
		}
	}

	/**
	 * @return デバイス情報
	 */
	tjs_int64 GetDevice() {
		return reinterpret_cast<tjs_int64>(device);
	}

	// ---------------------------------------------
	// 以下 Irrlicht を制御するための機能を順次追加予定
	// ---------------------------------------------
	
};


NCB_REGISTER_CLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR(());
	NCB_PROPERTY_RO(interface, GetDevice);
}
