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

// ----------------------------------------------------------------
// 実体型の登録
// 数値パラメータ系は配列か辞書を使えるような特殊コンバータを構築
// ----------------------------------------------------------------

// 両方自前コンバータ
#define NCB_SET_CONVERTOR_BOTH(type, convertor)\
NCB_TYPECONV_SRCMAP_SET(type, convertor<type>, true);\
NCB_TYPECONV_DSTMAP_SET(type, convertor<type>, true)

// SRCだけ自前コンバータ
#define NCB_SET_CONVERTOR_SRC(type, convertor)\
NCB_TYPECONV_SRCMAP_SET(type, convertor<type>, true);\
NCB_TYPECONV_DSTMAP_SET(type, ncbNativeObjectBoxing::Unboxing, true)

// DSTだけ自前コンバータ
#define NCB_SET_CONVERTOR_DST(type, convertor)\
NCB_TYPECONV_SRCMAP_SET(type, ncbNativeObjectBoxing::Boxing,   true); \
NCB_TYPECONV_DSTMAP_SET(type, convertor<type>, true)

/**
 * 配列かどうかの判定
 * @param var VARIANT
 * @return 配列なら true
 */
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
	typedef ncbInstanceAdaptor<T> AdaptorT;
	template <typename ANYT>
	void operator ()(ANYT &adst, const tTJSVariant &src) {
		if (src.Type() == tvtObject) {
			T *obj = AdaptorT::GetNativeInstance(src.AsObjectNoAddRef());
			if (obj) {
				dst = *obj;
			} else {
				ncbPropAccessor info(src);
				if (IsArray(src)) { // 配列から変換
					dst.setRed((u32)info.getIntValue(0, 0));
					dst.setGreen((u32)info.getIntValue(1, 0));
					dst.setBlue((u32)info.getIntValue(2, 0));
					dst.setAlpha((u32)info.getIntValue(3, 255));
				} else { // 辞書から変換
					dst.setRed((u32)info.getIntValue(L"r", 0));
					dst.setGreen((u32)info.getIntValue(L"g", 0));
					dst.setBlue((u32)info.getIntValue(L"b", 0));
					dst.setAlpha((u32)info.getIntValue(L"a", 255));
				}
			}
		} else {
			dst = T((u32)(tjs_int)src);
		}
		adst = ncbTypeConvertor::ToTarget<ANYT>::Get(&dst);
	}
// XXX 自前登録サンプル
//	template <typename ANYT>
//	void operator ()(const tTJSVariant &dst, const ANYT &asrc) {
//		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
//		if (dict != NULL) {
//			T const* src = ncbTypeConvertor::ToPointer<const ANYT&>::Get(asrc);
//			tTJSVariant r(src->getRed());
//			dict->PropSet(TJS_MEMBERENSURE, L"r", NULL, &r, dict);
//			dst = tTJSVariant(dict, dict);
//			dict->Release();
//		}
//	}
private:
	T dst;
};
NCB_SET_CONVERTOR_DST(SColor, ColorConvertor);
NCB_REGISTER_SUBCLASS_DELAY(SColor) {
	NCB_CONSTRUCTOR((u32));
	NCB_PROPERTY(red, getRed, setRed);
	NCB_PROPERTY(blue, getBlue, setBlue);
	NCB_PROPERTY(green, getGreen, setGreen);
	NCB_PROPERTY(alpha, getAlpha, setAlpha);
};

template <class T>
struct ColorfConvertor { // 色用コンバータ
	typedef ncbInstanceAdaptor<T> AdaptorT;
	template <typename ANYT>
	void operator ()(ANYT &adst, const tTJSVariant &src) {
		if (src.Type() == tvtObject) {
			T *obj = AdaptorT::GetNativeInstance(src.AsObjectNoAddRef());
			if (obj) {
				dst = *obj;
			} else {
				ncbPropAccessor info(src);
				if (IsArray(src)) { // 配列から変換
					dst.r = (f32)info.getRealValue(0, 0);
					dst.g = (f32)info.getRealValue(1, 0);
					dst.b = (f32)info.getRealValue(2, 0);
					dst.a = (f32)info.getRealValue(3, 1.0);
				} else { // 辞書から変換
					dst.r = (f32)info.getRealValue(L"r", 0);
					dst.g = (f32)info.getRealValue(L"g", 0);
					dst.b = (f32)info.getRealValue(L"b", 0);
					dst.a = (f32)info.getRealValue(L"a", 1.0);
				}
			}
		} else {
			dst = T(SColor((u32)(tjs_int)src));
		}
		adst = ncbTypeConvertor::ToTarget<ANYT>::Get(&dst);
	}
private:
	T dst;
};
NCB_SET_CONVERTOR_DST(SColorf, ColorfConvertor);
NCB_REGISTER_SUBCLASS_DELAY(SColorf) {
	NCB_CONSTRUCTOR((SColor));
//	NCB_PROPERTY(red, getRed, setRed);
//	NCB_PROPERTY(blue, getBlue, setBlue);
//	NCB_PROPERTY(green, getGreen, setGreen);
//	NCB_PROPERTY(alpha, getAlpha, setAlpha);
	NCB_METHOD(getInterpolated);
	NCB_METHOD(getInterpolated_quadratic);
	NCB_METHOD(setColorComponentValue);
	NCB_METHOD(toSColor);
};


template <class T>
struct DimensionConvertor { // コンバータ
	typedef ncbInstanceAdaptor<T> AdaptorT;
	template <typename ANYT>
	void operator ()(ANYT &dst, const tTJSVariant &src) {
		if (src.Type() == tvtObject) {
			T *obj = AdaptorT::GetNativeInstance(src.AsObjectNoAddRef());
			if (obj) {
				dst = *obj;
			} else {
				ncbPropAccessor info(src);
				if (IsArray(src)) { // 配列から変換
					dst.Width  = (f32)info.getRealValue(0);
					dst.Height = (f32)info.getRealValue(1);
				} else { // 辞書から変換
					dst.Width  = (f32)info.getRealValue(L"width");
					dst.Height = (f32)info.getRealValue(L"height");
				}
			}
		} else {
			dst = T();
		}
		adst = ncbTypeConvertor::ToTarget<ANYT>::Get(&dst);
	}
private:
	T dst;
};
NCB_SET_CONVERTOR_DST(dimension2df, DimensionConvertor);
NCB_REGISTER_SUBCLASS_DELAY(dimension2df) {
	NCB_CONSTRUCTOR(());
//	NCB_PROPERTY(width, getWidth, setWidth);
//	NCB_PROPERTY(height, getHeight, setHeight);
}

template <class T>
struct PointConvertor {
	template <typename ANYT>
	void operator ()(ANYT &adst, const tTJSVariant &src) {
		ncbPropAccessor info(src);
		if (IsArray(src)) {
			dst.X = (f32)info.getRealValue(0);
			dst.Y = (f32)info.getRealValue(1);
		} else {
			dst.X = (f32)info.getRealValue(L"x");
			dst.Y = (f32)info.getRealValue(L"y");
		}
		adst = ncbTypeConvertor::ToTarget<ANYT>::Get(&dst);
	}
	template <typename ANYT>
	void operator ()(const tTJSVariant &dst, const ANYT &asrc) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			T const* src = ncbTypeConvertor::ToPointer<const ANYT&>::Get(asrc);
			tTJSVariant x(src->X);
			tTJSVariant y(src->Y);
			dict->PropSet(TJS_MEMBERENSURE, L"x", NULL, &x, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"y", NULL, &y, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
private:
	T dst;
};
NCB_SET_CONVERTOR_BOTH(position2df, PointConvertor);
NCB_SET_CONVERTOR_BOTH(vector2df, PointConvertor);

template <class T>
struct Point3DConvertor {
	template <typename ANYT>
	void operator ()(ANYT &adst, const tTJSVariant &src) {
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
private:
	T dst;
};

NCB_SET_CONVERTOR_DST(vector3df, Point3DConvertor);
NCB_REGISTER_SUBCLASS_DELAY(vector3df) {
	NCB_CONSTRUCTOR((f32,f32,f32));
	NCB_METHOD(crossProduct);
	NCB_METHOD(dotProduct);
};

template <class T>
struct RectConvertor {
	template <typename ANYT>
	void operator ()(ANYT &adst, const tTJSVariant &src) {
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
		adst = ncbTypeConvertor::ToTarget<ANYT>::Get(&dst);
	}
	template <typename ANYT>
	void operator ()(tTJSVariant &dst, const ANYT &asrc) {
		iTJSDispatch2 *dict = TJSCreateDictionaryObject();
		if (dict != NULL) {
			T const* src = ncbTypeConvertor::ToPointer<const ANYT&>::Get(asrc);
			int x1 = src->UpperLeftCorner.X;
			int y1 = src->UpperLeftCorner.Y;
			tTJSVariant x(x1);
			tTJSVariant y(y1);
			tTJSVariant width(src->LowerRightCorner.X - x1 + 1);
			tTJSVariant height(src->LowerRightCorner.Y - y1 + 1);
			dict->PropSet(TJS_MEMBERENSURE, L"x", NULL, &x, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"y", NULL, &y, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"width", NULL, &width, dict);
			dict->PropSet(TJS_MEMBERENSURE, L"height", NULL, &height, dict);
			dst = tTJSVariant(dict, dict);
			dict->Release();
		}
	}
private:
	T dst;
};
NCB_SET_CONVERTOR_BOTH(rect<s32>, RectConvertor);

// --------------------------------------------------------------------
// Irrlicht の参照オブジェクトの型の登録
// Irrlicht のインターフェースをそのまま保持できるように工夫
// --------------------------------------------------------------------

/**
 * Irrlichtの参照オブジェクトのラッピング用テンプレートクラス
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
 * Irrlichtの参照オブジェクトをラッピングしたクラス用のコンバータ（汎用）
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

// Irrlicht参照オブジェクト用のコンバータの登録マクロ
#define NCB_IRR_CONVERTOR(type) \
NCB_SET_CONVERTOR(type*, IrrTypeConvertor<type>);\
NCB_SET_CONVERTOR(const type*, IrrTypeConvertor<const type>)

// ラッピング処理用
#define NCB_REGISTER_IRR_SUBCLASS(Class) NCB_IRR_CONVERTOR(Class);NCB_REGISTER_SUBCLASS(IrrWrapper<Class>)
#define NCB_IRR_SUBCLASS(Class) NCB_SUBCLASS(Class, IrrWrapper<Class>)
#define NCB_IRR_METHOD(Class, name)  Method(TJS_W(# name), &Class::name, Bridge<IrrWrapper<Class>::BridgeFunctor>())
#define NCB_IRR_PROPERTY(Class, name,get,set)  Property(TJS_W(# name), &Class::get, &Class::set, Bridge<IrrWrapper<Class>::BridgeFunctor>())
#define NCB_IRR_PROPERTY_RO(Class, name,get)   Property(TJS_W(# name), &Class::get, (int)0, Bridge<IrrWrapper<Class>::BridgeFunctor>())
#define NCB_IRR_PROPERTY_WO(Class, name,set)   Property(TJS_W(# name), (int)0, &Class::set, Bridge<IrrWrapper<Class>::BridgeFunctor>())

// XXX 多態化が必要
NCB_REGISTER_IRR_SUBCLASS(ISceneNode) {
	NCB_CONSTRUCTOR(());
};

NCB_REGISTER_IRR_SUBCLASS(ICameraSceneNode) {
	NCB_CONSTRUCTOR(());
};

NCB_REGISTER_IRR_SUBCLASS(ILightSceneNode) {
	NCB_CONSTRUCTOR(());
};

NCB_REGISTER_IRR_SUBCLASS(IImage) {
	NCB_CONSTRUCTOR(());
};

NCB_REGISTER_IRR_SUBCLASS(IVideoDriver) {
	NCB_CONSTRUCTOR(());
	NCB_IRR_METHOD(IVideoDriver, createScreenShot);
};

static bool ISceneManagerLoadScene(IrrWrapper<ISceneManager> *obj, const char *filename)
{
	return obj->getIrrObject()->loadScene(filename);
}

NCB_REGISTER_IRR_SUBCLASS(ISceneManager) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD_PROXY(loadScene, ISceneManagerLoadScene);
	NCB_IRR_METHOD(ISceneManager, addCameraSceneNode);
	NCB_IRR_METHOD(ISceneManager, addLightSceneNode);
	NCB_IRR_PROPERTY(ISceneManager, ambientLight, getAmbientLight, setAmbientLight);
	NCB_IRR_PROPERTY(ISceneManager, shadowColor, getShadowColor, setShadowColor);
};

NCB_REGISTER_IRR_SUBCLASS(IGUIEnvironment) {
	NCB_CONSTRUCTOR(());
};

// --------------------------------------------------------------------
// Irrlicht 操作用の基本オブジェクトの登録
// --------------------------------------------------------------------

#define BASE_METHOD \
	NCB_PROPERTY_RO(videoDriver, getVideoDriver);\
	NCB_PROPERTY_RO(sceneManager, getSceneManager);\
	NCB_PROPERTY_RO(guiEnvironment, getGUIEnvironment)

NCB_REGISTER_SUBCLASS(IrrlichtDrawDevice) {
	NCB_CONSTRUCTOR((int));
	BASE_METHOD;
	NCB_PROPERTY_RO(interface, GetDevice);
};

NCB_REGISTER_SUBCLASS(IrrlichtWindow) {
	NCB_CONSTRUCTOR((int, iTJSDispatch2 *, int, int, int, int));
//	BASE_METHOD;
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
#define NCB_SUBCLASS_NAME(name) NCB_SUBCLASS(name, name)

NCB_REGISTER_CLASS(Irrlicht) {
	//enums
	ENUM(EDT_SOFTWARE);
	ENUM(EDT_BURNINGSVIDEO);
	ENUM(EDT_DIRECT3D9);
	ENUM(EDT_OPENGL);
	//static

	// Irrlicht データ型クラス
	NCB_SUBCLASS_NAME(SColor);
	NCB_SUBCLASS_NAME(SColorf);
	NCB_SUBCLASS_NAME(dimension2df);
	NCB_SUBCLASS_NAME(vector3df);

	// Irrlicht 参照用クラス
	NCB_IRR_SUBCLASS(ICameraSceneNode);
	NCB_IRR_SUBCLASS(ILightSceneNode);
	NCB_IRR_SUBCLASS(IImage);

	NCB_IRR_SUBCLASS(IVideoDriver);
	NCB_IRR_SUBCLASS(ISceneManager);
	NCB_IRR_SUBCLASS(IGUIEnvironment);

	// Irrlicht 操作ベースクラス
	NCB_SUBCLASS(DrawDevice, IrrlichtDrawDevice);
	NCB_SUBCLASS(Window, IrrlichtWindow);
}

/**
 * レイヤに IImage からの吸い出し処理を拡張
 */
static tjs_error TJS_INTF_METHOD
copyIImage(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;

	typedef IrrWrapper<IImage> WrapperT;
	typedef ncbInstanceAdaptor<WrapperT> AdaptorT;
	WrapperT *obj;
	IImage *image;
	if (param[0]->Type() != tvtObject ||
		(obj = AdaptorT::GetNativeInstance(param[0]->AsObjectNoAddRef())) == NULL ||
		(image = obj->getIrrObject()) == NULL) {
		TVPThrowExceptionMessage(TJS_W("src must be IImage."));
	}
	
	dimension2d<s32> size = image->getDimension();
	int width  = size.Width;
	int height = size.Height;

	// レイヤのサイズを調整
	ncbPropAccessor layer(objthis);
	layer.SetValue(L"width",  width);
	layer.SetValue(L"height", height);
	layer.SetValue(L"imageLeft",  0);
	layer.SetValue(L"imageTop",   0);
	layer.SetValue(L"imageWidth",  width);
	layer.SetValue(L"imageHeight", height);
	
	unsigned char *buffer = (unsigned char*)layer.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
	int pitch = layer.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
	
	// 画像データのコピー
	if (image->getColorFormat() == ECF_A8R8G8B8) {
		unsigned char *src = (unsigned char*)image->lock();
		int slen   = width * 4;
		int spitch = image->getPitch();
		for (int y=0;y<height;y++) {
			memcpy(buffer, src, slen);
			src    += spitch;
			buffer += pitch;
		}
		image->unlock();
	} else {
		for (int y=0;y<height;y++) {
			u32 *line = (u32 *)buffer;
			for (int x=0;x<width;x++) {
				*line++ = image->getPixel(x, y).color;
				buffer += pitch;
			}
		}
	}

	// レイヤを更新
	objthis->FuncCall(0, L"update", NULL, NULL, 0, NULL, objthis);

	return TJS_S_OK;
}
NCB_ATTACH_FUNCTION(copyIImage, Layer, copyIImage);

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
