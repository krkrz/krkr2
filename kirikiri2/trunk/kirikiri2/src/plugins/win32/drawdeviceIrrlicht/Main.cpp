#include "ncbind/ncbind.hpp"
#include "IrrlichtDrawDevice.h"

/**
 * ログ出力用
 */
void
message_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf_s(msg, 1024, _TRUNCATE, format, args);
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
	_vsnprintf_s(msg, 1024, _TRUNCATE, format, args);
	TVPAddImportantLog(ttstr(msg));
	va_end(args);
}

// ----------------------------------- クラスの登録

using namespace irr::core;

NCB_REGISTER_SUBCLASS(vector3df) {
	NCB_CONSTRUCTOR(());
};

NCB_REGISTER_SUBCLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR(());
	NCB_PROPERTY_RO(interface, GetDevice);
};

/**
 * 名前空間用ダミー
 */
struct Irrlicht {
};

#define ENUM(n) Variant(#n, (int)n)

NCB_REGISTER_CLASS(Irrlicht) {
	//enums
	//static
	//classes
	NCB_SUBCLASS(DrawDevice, IrrlichtDrawDevice);
}
