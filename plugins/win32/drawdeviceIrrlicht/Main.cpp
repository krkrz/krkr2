#pragma comment(lib, "Irrlicht.lib")
#include "ncbind/ncbind.hpp"
#include "IrrlichtDrawDevice.h"
#include "SWFMovie.hpp"

/**
 * ログ出力用
 */
void
message_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf(msg, 1024, format, args);
	TVPAddLog(ttstr(msg));
	va_end(args);
}

/**
 * エラーログ出力用
 */
void
error_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf(msg, 1024, format, args);
	TVPAddImportantLog(ttstr(msg));
	va_end(args);
}

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

	// SWF ファイル生成？
};


NCB_REGISTER_CLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR(());
	NCB_PROPERTY_RO(interface, GetDevice);
}

/**
 * ファイル名変換用 proxy
 */
void swfload(SWFMovie *swf, const char *name)
{
	ttstr path(name);
	TVPGetLocalName(path);
	int len = path.GetNarrowStrLen() + 1;
	char *filename = new char[len];
	path.ToNarrowStr(filename, len);
	swf->load(filename);
	delete filename;
}

NCB_REGISTER_CLASS(SWFMovie) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD_PROXY(load, swfload);
	NCB_METHOD(update);
	NCB_METHOD(notifyMouse);
	NCB_METHOD(play);
	NCB_METHOD(stop);
	NCB_METHOD(restart);
	NCB_METHOD(back);
	NCB_METHOD(next);
	NCB_METHOD(gotoFrame);
}

extern void initSWFMovie();
extern void destroySWFMovie();

NCB_POST_REGIST_CALLBACK(initSWFMovie);
NCB_PRE_UNREGIST_CALLBACK(destroySWFMovie);
