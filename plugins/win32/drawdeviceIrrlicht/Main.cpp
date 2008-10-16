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

/**
 * Irrlicht オブジェクトのラッピング用テンプレートクラス
 */
template <class T>
class IrrWrapper {
	typedef T IrrClassT;
	typedef IrrWrapper<IrrClassT> WrapperT;
protected:
	IrrClassT *obj;
public:
	IrrWrapper() : obj(NULL) {}
	IrrWrapper(IrrClassT *obj) : obj(obj) {
		if (obj) {
			obj->grab();
		}
	}
	IrrWrapper(const IrrWrapper &orig) : obj(orig.obj) {
		if (obj) {
			obj->grab();
		}
	}
	~IrrWrapper() {
		if (obj) {
			obj->drop();
			obj=NULL;
		}
	}
	IrrClassT *getIrrObject() { return obj; }

	struct BridgeFunctor {
		IrrClassT* operator()(WrapperT *p) const {
			return p->getIrrObject();
		}
	};
};
/**
 * Irrlicht オブジェクトをラッピングしたクラス用のコンバータ
 */
template <class T>
struct IrrTypeConvertor {
	typedef typename ncbTypeConvertor::Stripper<T>::Type IrrClassT;
	typedef T *IrrClassP;
	typedef IrrWrapper<IrrClassT> WrapperT;
	typedef ncbInstanceAdaptor<WrapperT> AdaptorT;
	void operator ()(IrrClassP &dst, const tTJSVariant &src) {
		WrapperT *obj;
		if (src.Type() == tvtObject && (obj = AdaptorT::GetNativeInstance(src.AsObjectNoAddRef()))) {
			dst = obj->getIrrObject();
		} else {
			dst = NULL;
		}
	}
	void operator ()(tTJSVariant &dst, const IrrClassP &src) {
		if (src != NULL) {
			iTJSDispatch2 *adpobj = AdaptorT::CreateAdaptor(new WrapperT(src));
			if (adpobj) {
				dst = tTJSVariant(adpobj, adpobj);
				adpobj->Release();			
			} else {
				TVPThrowExceptionMessage(L"コンバータが型の初期化に失敗");
			}
		} else {
			dst.Clear();
		}
	}
};

#define NCB_IRR_CONVERTOR(type) \
NCB_SET_CONVERTOR(type*, IrrTypeConvertor<type>);\
NCB_SET_CONVERTOR(const type*, IrrTypeConvertor<const type>)

// ----------------------------------------------------
// その他特殊なコンバータ
// 数値パラメータ系は辞書を使えるようにしてある
// ----------------------------------------------------

#define NCB_SET_CONVERTOR_BOTH(type, convertor)\
NCB_TYPECONV_SRCMAP_SET(type, convertor<type>, true);\
NCB_TYPECONV_DSTMAP_SET(type, convertor<type>, true)

static bool IsArray(const tTJSVariant &var)
{
	if (var.Type() == tvtObject) {
		iTJSDispatch2 *obj = var.AsObjectNoAddRef();
		return obj->IsInstanceOf(0, NULL, NULL, L"Array", obj) == TJS_S_TRUE;
	}
	return false;
}

template <class T>
struct ColorConvertor { // 色用コンバータ
	void operator ()(T &dst, const tTJSVariant &src) {
		if (src.Type() == tvtInteger) {
			dst = T((u32)(tjs_int)src);
		} else {
			ncbPropAccessor info(src);
			if (IsArray(src)) {
				dst.setRed((u32)info.getIntValue(0, 0));
				dst.setGreen((u32)info.getIntValue(1, 0));
				dst.setBlue((u32)info.getIntValue(2, 0));
				dst.setAlpha((u32)info.getIntValue(3, 255));
			} else {
				dst.setRed((u32)info.getIntValue(L"r", 0));
				dst.setGreen((u32)info.getIntValue(L"g", 0));
				dst.setBlue((u32)info.getIntValue(L"b", 0));
				dst.setAlpha((u32)info.getIntValue(L"a", 255));
			}
		}
	}
	void operator ()(tTJSVariant &dst, const T &src) {
		dst = (tjs_int64)(tjs_uint64)src.color;
	}
};
NCB_SET_CONVERTOR_BOTH(SColor, ColorConvertor);

template <class T>
struct ColorfConvertor { // 色用コンバータ
	void operator ()(T &dst, const tTJSVariant &src) {
		if (src.Type() == tvtInteger) {
			dst = T(SColor((u32)(tjs_int)src));
		} else {
			ncbPropAccessor info(src);
			if (IsArray(src)) {
				dst.r = (f32)info.getRealValue(0, 0);
				dst.g = (f32)info.getRealValue(1, 0);
				dst.b = (f32)info.getRealValue(2, 0);
				dst.a = (f32)info.getRealValue(3, 1.0);
			} else {
				dst.r = (f32)info.getRealValue(L"r", 0);
				dst.g = (f32)info.getRealValue(L"g", 0);
				dst.b = (f32)info.getRealValue(L"b", 0);
				dst.a = (f32)info.getRealValue(L"a", 1.0);
			}
		}
	}
	void operator ()(tTJSVariant &dst, const T &src) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			tTJSVariant a(src.a);
			tTJSVariant r(src.r);
			tTJSVariant g(src.g);
			tTJSVariant b(src.b);
			dict->PropSet(TJS_MEMBERENSURE, L"a", NULL, &a, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"r", NULL, &r, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"g", NULL, &g, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"b", NULL, &b, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
};
NCB_SET_CONVERTOR_BOTH(SColorf, ColorfConvertor);

template <class T>
struct DimensionConvertor { // コンバータ
	void operator ()(T &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.Width  = (f32)info.getRealValue(0);
			dst.Height = (f32)info.getRealValue(1);
		} else {
			dst.Width  = (f32)info.getRealValue(L"width");
			dst.Height = (f32)info.getRealValue(L"height");
		}
	}
	void operator ()(const tTJSVariant &dst, const T &src) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			tTJSVariant width(src.Width);
			tTJSVariant height(src.Height);
			dict->PropSet(TJS_MEMBERENSURE, L"width", NULL, &width, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"height", NULL, &height, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
};
NCB_SET_CONVERTOR_BOTH(dimension2df, DimensionConvertor);

template <class T>
struct PointConvertor {
	void operator ()(T &dst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.X = (f32)info.getRealValue(0);
			dst.Y = (f32)info.getRealValue(1);
		} else {
			dst.X = (f32)info.getRealValue(L"x");
			dst.Y = (f32)info.getRealValue(L"y");
		}
	}
	void operator ()(const tTJSVariant &dst, const T &src) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			tTJSVariant x(src.X);
			tTJSVariant y(src.Y);
			dict->PropSet(TJS_MEMBERENSURE, L"x", NULL, &x, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"y", NULL, &y, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
};
NCB_SET_CONVERTOR_BOTH(position2df, PointConvertor);
NCB_SET_CONVERTOR_BOTH(vector2df, PointConvertor);

template <class T>
struct Point3DConvertor {
	template <typename ANYT>
	void operator ()(ANYT &adst, const tTJSVariant &src) {
		T dst;
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
		adst = ncbTypeConvertor::ToTarget<ANYT>::Get(&dst);
	}
	template <typename ANYT>
	void operator ()(tTJSVariant &dst, const ANYT &asrc) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			T const* src = ncbTypeConvertor::ToPointer<const ANYT&>::Get(asrc);
			tTJSVariant x(src->X);
			tTJSVariant y(src->Y);
			tTJSVariant z(src->Z);
			dict->PropSet(TJS_MEMBERENSURE, L"x", NULL, &x, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"y", NULL, &y, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"z", NULL, &z, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
};
NCB_SET_CONVERTOR_BOTH(vector3df, Point3DConvertor);

template <class T>
struct RectConvertor {
	void operator ()(T &dst, const tTJSVariant &src) {
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
	void operator ()(tTJSVariant &dst, const T &src) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			int x1 = src.UpperLeftCorner.X;
			int y1 = src.UpperLeftCorner.Y;
			tTJSVariant x(x1);
			tTJSVariant y(y1);
			tTJSVariant width(src.LowerRightCorner.X - x1 + 1);
			tTJSVariant height(src.LowerRightCorner.Y - y1 + 1);
			dict->PropSet(TJS_MEMBERENSURE, L"x", NULL, &x, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"y", NULL, &y, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"width", NULL, &width, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"height", NULL, &height, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
};
NCB_SET_CONVERTOR_BOTH(rect<s32>, RectConvertor);

// ----------------------------------- クラスの登録

// ラッピング処理用
#define NCB_IRR_METHOD(Class, name)  Method(TJS_W(# name), &Class::name, Bridge<IrrWrapper<Class>::BridgeFunctor>())
#define NCB_IRR_PROPERTY(Class, name,get,set)  Property(TJS_W(# name), &Class::get, &Class::set, Bridge<IrrWrapper<Class>::BridgeFunctor>())
#define NCB_IRR_PROPERTY_RO(Class, name,get)   Property(TJS_W(# name), &Class::get, (int)0, Bridge<IrrWrapper<Class>::BridgeFunctor>())
#define NCB_IRR_PROPERTY_WO(Class, name,set)   Property(TJS_W(# name), (int)0, &Class::set, Bridge<IrrWrapper<Class>::BridgeFunctor>())

NCB_IRR_CONVERTOR(ISceneNode);
NCB_REGISTER_SUBCLASS(IrrWrapper<ISceneNode>) {
	NCB_CONSTRUCTOR(());
};

NCB_IRR_CONVERTOR(ICameraSceneNode);
NCB_REGISTER_SUBCLASS(IrrWrapper<ICameraSceneNode>) {
	NCB_CONSTRUCTOR(());
};

NCB_IRR_CONVERTOR(ILightSceneNode);
NCB_REGISTER_SUBCLASS(IrrWrapper<ILightSceneNode>) {
	NCB_CONSTRUCTOR(());
};

static bool ISceneManagerLoadScene(IrrWrapper<ISceneManager> *obj, const char *filename)
{
	return obj->getIrrObject()->loadScene(filename);
}

static ILightSceneNode *ISceneManagerAddLightSceneNode(IrrWrapper<ISceneManager> *obj, ISceneNode *parent, const vector3df position, SColorf color, f32 radius, s32 id)
{
	return obj->getIrrObject()->addLightSceneNode(parent, position, color, radius, id);
}

static ICameraSceneNode *ISceneManagerAddCameraSceneNode(IrrWrapper<ISceneManager> *obj, ISceneNode *parent, const vector3df position, const vector3df lookat, s32 id)
{
	return obj->getIrrObject()->addCameraSceneNode(parent, position, lookat, id);
}

NCB_IRR_CONVERTOR(ISceneManager);
NCB_REGISTER_SUBCLASS(IrrWrapper<ISceneManager>) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD_PROXY(loadScene, ISceneManagerLoadScene);
	NCB_METHOD_PROXY(addLightSceneNode, ISceneManagerAddLightSceneNode);
//	NCB_METHOD_PROXY(addCameraSceneNode, ISceneManagerAddCameraSceneNode);
	NCB_IRR_METHOD(ISceneManager, addCameraSceneNode);
//	NCB_IRR_PROPERTY(ISceneManager, ambientLight, getAmbientLight, setAmbientLight);
	NCB_IRR_PROPERTY(ISceneManager, shadowColor, getShadowColor, setShadowColor);
};

NCB_IRR_CONVERTOR(IGUIEnvironment);
NCB_REGISTER_SUBCLASS(IrrWrapper<IGUIEnvironment>) {
	NCB_CONSTRUCTOR(());
};

#define BASE_METHOD \
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
	NCB_SUBCLASS(SceneManager, IrrWrapper<ISceneManager>);
	NCB_SUBCLASS(GUIEnvironment, IrrWrapper<IGUIEnvironment>);
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
