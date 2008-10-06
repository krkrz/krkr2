#include "ncbind/ncbind.hpp"
#include "IrrlichtDrawDevice.h"
#include "IrrlichtWindow.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

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

// ----------------------------------- コンバータの登録

static bool IsArray(const tTJSVariant &var)
{
	if (var.Type() == tvtObject) {
		iTJSDispatch2 *obj = var.AsObjectNoAddRef();
		return obj->IsInstanceOf(0, NULL, NULL, L"Array", obj) == TJS_S_TRUE;
	}
	return false;
}

struct DimensionConvertor { // コンバータ
	void operator ()(dimension2df &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.Width  = (f32)info.getRealValue(0);
			dst.Height = (f32)info.getRealValue(1);
		} else {
			dst.Width  = (f32)info.getRealValue(L"width");
			dst.Height = (f32)info.getRealValue(L"height");
		}
	}
};
NCB_SET_TOVALUE_CONVERTOR(dimension2df, DimensionConvertor);

struct PositionConvertor {
	void operator ()(position2df &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.X = (f32)info.getRealValue(0);
			dst.Y = (f32)info.getRealValue(1);
		} else {
			dst.X = (f32)info.getRealValue(L"x");
			dst.Y = (f32)info.getRealValue(L"y");
		}
	}
};
NCB_SET_TOVALUE_CONVERTOR(position2df, PositionConvertor);

struct Vector2DConvertor {
	void operator ()(vector2df &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.X = (f32)info.getRealValue(0);
			dst.Y = (f32)info.getRealValue(1);
		} else {
			dst.X = (f32)info.getRealValue(L"x");
			dst.Y = (f32)info.getRealValue(L"y");
		}
	}
};
NCB_SET_TOVALUE_CONVERTOR(vector2df, Vector2DConvertor);

struct Vector3DConvertor {
	void operator ()(vector3df &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.X = (f32)info.getRealValue(0);
			dst.Y = (f32)info.getRealValue(1);
			dst.Z = (f32)info.getRealValue(2);
		} else {
			dst.X = (f32)info.getRealValue(L"x");
			dst.Y = (f32)info.getRealValue(L"y");
			dst.Z = (f32)info.getRealValue(L"z");
		}
	}
};
NCB_SET_TOVALUE_CONVERTOR(vector3df, Vector3DConvertor);

struct RectConvertor {
	void operator ()(rect<s32> &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.UpperLeftCorner.X  = info.getIntValue(0);
			dst.UpperLeftCorner.Y  = info.getIntValue(1);
			dst.LowerRightCorner.X = info.getIntValue(2);
			dst.LowerRightCorner.Y = info.getIntValue(3);
		} else {
			int x = info.getIntValue(L"x");
			int y = info.getIntValue(L"y");
			int x2, y2;
			if (info.HasValue(L"width")) {
				x2 = x + info.getIntValue(L"width") - 1;
				y2 = x + info.getIntValue(L"height") - 1;
			} else {
				x2 = info.getIntValue(L"x2");
				y2 = info.getIntValue(L"y2");
			}
			dst.UpperLeftCorner.X  = x;
			dst.UpperLeftCorner.Y  = y;
			dst.LowerRightCorner.X = x2;
			dst.LowerRightCorner.Y = y2;
		}
	}
};
NCB_SET_CONVERTOR(rect<s32>, RectConvertor);

// ----------------------------------- クラスの登録

NCB_REGISTER_SUBCLASS(IrrlichtDriver) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD(captureScreenShot);
};

NCB_REGISTER_SUBCLASS(IrrlichtSceneManager) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD(loadScene);
	NCB_METHOD(addCameraSceneNode);
};

NCB_REGISTER_SUBCLASS(IrrlichtGUIEnvironment) {
	NCB_CONSTRUCTOR(());
};

#define BASE_METHOD \
	NCB_PROPERTY_RO(driver, getDriver);\
	NCB_PROPERTY_RO(sceneManager, getSceneManager);\
	NCB_PROPERTY_RO(guiEnvironment, getGUIEnvironment)

NCB_REGISTER_SUBCLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR((int));
	BASE_METHOD;
	NCB_PROPERTY_RO(interface, GetDevice);
};

NCB_REGISTER_SUBCLASS(IrrlichtWindow) {
	NCB_CONSTRUCTOR((int, iTJSDispatch2 *, int, int, int, int));
	BASE_METHOD;
	NCB_PROPERTY(left, getLeft, setLeft);
	NCB_PROPERTY(top, getTop, setTop);
	NCB_PROPERTY(width, getWidth, setWidth);
	NCB_PROPERTY(height, getHeight, setHeight);
	NCB_PROPERTY(visible, getVisible, setVisible);
	NCB_METHOD(setPos);
	NCB_METHOD(setSize);
};

/**
 * 名前空間用ダミー
 */
struct Irrlicht {
};

#define ENUM(n) Variant(#n, (int)n)

NCB_REGISTER_CLASS(Irrlicht) {
	//enums
	ENUM(EDT_SOFTWARE);
	ENUM(EDT_BURNINGSVIDEO);
	ENUM(EDT_DIRECT3D9);
	ENUM(EDT_OPENGL);
	//static
	//classes
	NCB_SUBCLASS(Driver, IrrlichtDriver);
	NCB_SUBCLASS(SceneManager, IrrlichtSceneManager);
	NCB_SUBCLASS(GUIEnvironment, IrrlichtGUIEnvironment);
	NCB_SUBCLASS(DrawDevice, IrrlichtDrawDevice);
	NCB_SUBCLASS(Window, IrrlichtWindow);
}

/**
 * 登録処理前
 */
void PreRegistCallback()
{
	registerWindowClass();
}

/**
 * 開放処理後
 */
void PostUnregistCallback()
{
	unregisterWindowClass();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
