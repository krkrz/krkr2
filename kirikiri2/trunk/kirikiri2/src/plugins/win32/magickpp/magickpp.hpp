#ifndef _magickpp_hpp_
#define _magickpp_hpp_

#include <Magick++.h>
#include <magick/version.h>

#include "ncbind.hpp"

typedef std::string StringT;

// exception フック
template <>
struct ncbNativeClassMethodBase::invokeHookAll<true> {
	template <typename T>
	static inline typename T::ResultT Do(T &t) {
		try {
			return t();
		} catch (Magick::Exception &err) {
			ttstr what(TJS_W("Magick++ Exception: ")); what += ttstr(err.what());
			TVPThrowExceptionMessage(what.c_str());
			throw;
		} catch (std::exception &err) {
			ttstr what(TJS_W("STL Exception: ")); what += ttstr(err.what());
			TVPThrowExceptionMessage(what.c_str());
			throw;
		}
	}
};

// 引き数と返り値に std::string をサポートさせる
NCB_TYPECONV_STL_STRING(StringT);

// 引き数と返り値に enum をサポート（面倒なので数値変換）
#define MAGICK_ENUM(e) NCB_TYPECONV_CAST_INTEGER(Magick::e)
MAGICK_ENUM(ChannelType); //##
MAGICK_ENUM(ClassType); //##
MAGICK_ENUM(ColorspaceType); //##
MAGICK_ENUM(CompositeOperator); //##
MAGICK_ENUM(CompressionType); //##
MAGICK_ENUM(DecorationType);
MAGICK_ENUM(EndianType); //##
MAGICK_ENUM(FillRule); //## 保留
MAGICK_ENUM(FilterTypes); //##
MAGICK_ENUM(GravityType);
MAGICK_ENUM(ImageType); //##
MAGICK_ENUM(InterlaceType); //##
MAGICK_ENUM(LineCap); //##
MAGICK_ENUM(LineJoin); //##
MAGICK_ENUM(NoiseType);
MAGICK_ENUM(OrientationType); //##
MAGICK_ENUM(PaintMethod); //##
MAGICK_ENUM(RenderingIntent); //##
MAGICK_ENUM(ResolutionType); //##
MAGICK_ENUM(StorageType);
MAGICK_ENUM(StretchType);
MAGICK_ENUM(StyleType);
//NCB_TYPECONV_CAST_INTEGER(QuantumTypes); // magick/quantum.h
//NCB_TYPECONV_CAST_INTEGER(ChannelType);  // magick/magick-type.h

#define MAGICK_OBJECT(obj) using Magick::obj; NCB_TYPECONV_BOXING(obj)
MAGICK_OBJECT(Blob);
MAGICK_OBJECT(CoderInfo);
MAGICK_OBJECT(Color);
MAGICK_OBJECT(Drawable);
MAGICK_OBJECT(Geometry);
MAGICK_OBJECT(Image);
MAGICK_OBJECT(TypeMetric);

#define MAGICK_CLASS(cls) \
	NCB_REGISTER_CLASS_DELAY(MagickPP_ ## cls, cls)

// 読み取り専用プロパティ短縮用
#define PROP_RO(prop) NCB_PROPERTY_RO(prop, prop)
#define PROP_WO(prop) NCB_PROPERTY_WO(prop, prop)
#define PROP_RW(type, prop) PROP_RW_TAG(prop, type, Const, type, Class)
#define PROP_rw(type, prop) PROP_RW_TAG(prop, type, Class, type, Class)

#define PROP_RW_TAG(prop, typeR, tagR, typeW, tagW) \
	NCB_PROPERTY_DETAIL(prop, \
						tagR, typeR, ClassT::prop, (), \
						tagW, void,  ClassT::prop, (typeW))
#define PROP_RW_TYPE(type, prop) \
	PROP_RW_TAG(prop, type, Const, type const&, Class)

#define PROP_BLOB(prop)     PROP_RW_TYPE(Blob,     prop)
#define PROP_COLOR(prop)    PROP_RW_TYPE(Color,    prop)
#define PROP_GEOMETRY(prop) PROP_RW_TYPE(Geometry, prop)
#define PROP_IMAGE(prop)    PROP_RW_TYPE(Image,    prop)
#define PROP_STRING(prop)   PROP_RW_TYPE(StringT,  prop)

#endif
