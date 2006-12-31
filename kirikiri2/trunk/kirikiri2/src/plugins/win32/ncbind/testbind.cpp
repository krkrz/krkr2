#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ncbind.hpp"

template <typename T> struct ttstrWrap { T operator()(T t) { return t; } };
#define TTSTRCAST(type, cast) template <> struct ttstrWrap<type> { cast operator()(type t) { return t; } }
#define TTSTRCAST_INT(type)  TTSTRCAST(type, tjs_int)



TTSTRCAST_INT(  signed char);
TTSTRCAST_INT(  signed short);
TTSTRCAST_INT(  signed int);
TTSTRCAST_INT(  signed long);
TTSTRCAST_INT(unsigned char);
TTSTRCAST_INT(unsigned short);
TTSTRCAST_INT(unsigned int);
TTSTRCAST_INT(unsigned long);

struct ttstrFormat {
	ttstrFormat(tjs_nchar const *fmt) : _format(fmt) {}
	template <typename T>
	tjs_nchar const* operator()(T t) {
#if _MSC_VER >= 1400
		_snprintf_s(_buff, sizeof(_buff), _TRUNCATE, _format, t);
#else
		_snprintf(_buff, sizeof(_buff)-1, _format, t);
#endif
		return _buff;
	}
private:
	tjs_nchar const *_format;
	tjs_nchar _buff[256];
};
template <> struct ttstrWrap<float>  : public ttstrFormat { ttstrWrap() : ttstrFormat("%f" ) {} };
template <> struct ttstrWrap<double> : public ttstrFormat { ttstrWrap() : ttstrFormat("%lf") {} };


#undef  FOREACH_START
#define FOREACH_START 1
#undef  FOREACH_END
#define FOREACH_END   FOREACH_MAX
#undef  FOREACH

#define DEF_EXT(n) typename T ## n
#define REF_EXT(n)          T ## n t ## n
#define STR_EXT(n)           ttstr(ttstrWrap<T##n>()(t ## n)) +

#define FOREACH \
	template <        FOREACH_COMMA_EXT(DEF_EXT)> \
		void mes(     FOREACH_COMMA_EXT(REF_EXT)) { \
			TVPAddLog(FOREACH_SPACE_EXT(STR_EXT) ttstr("")); }
#include FOREACH_INCLUDE


////////////////////////////////////////

struct TypeConvChecker {
	TypeConvChecker() : _name(TJS_W("TypeConvChecker")) {
		mes(_name, "::TypeConvChecker()");
	}
	bool           Bool(           bool  b) const { mes(_name, "::Bool(",  (b ? "True" : "False"), ")"); return b; }

	signed   char  SChar(   signed char  n) const { mes(_name, "::SChar(",  n, ")"); return n; }
	signed   short SShort(  signed short n) const { mes(_name, "::SShort(", n, ")"); return n; }
	signed   int   SInt(    signed int   n) const { mes(_name, "::SInt(",   n, ")"); return n; }
	signed   long  SLong(   signed long  n) const { mes(_name, "::SLong(",  n, ")"); return n; }

	unsigned char  UChar( unsigned char  n) const { mes(_name, "::UChar(",  n, ")"); return n; }
	unsigned short UShort(unsigned short n) const { mes(_name, "::UShort(", n, ")"); return n; }
	unsigned int   UInt(  unsigned int   n) const { mes(_name, "::UInt(",   n, ")"); return n; }
	unsigned long  ULong( unsigned long  n) const { mes(_name, "::ULong(",  n, ")"); return n; }

	float          Float(          float f) const { mes(_name, "::Float(",  f, ")"); return f; }
	double         Double(        double d) const { mes(_name, "::Double(", d, ")"); return d; }

	char const*    ConstCharP(char const *p)const { mes(_name, "::ConstCharP(", p, ")"); return p; }

//private:
	tjs_char const *_name;
};

static tjs_error TJS_INTF_METHOD
RawCallback1(tTJSVariant *result, tjs_int numparams,
			 tTJSVariant **param, iTJSDispatch2 *objthis) {
	mes("RawCallback1");
	return TJS_S_OK;
}
static tjs_error TJS_INTF_METHOD
RawCallback2(tTJSVariant *result, tjs_int numparams,
			 tTJSVariant **param, TypeConvChecker *objthis) {
	mes("RawCallback2", objthis->_name);
	return TJS_S_OK;
}


NCB_REGISTER_CLASS(TypeConvChecker) {
	NCB_CONSTRUCTOR(());

	NCB_METHOD(Bool);

	NCB_METHOD(SChar);
	NCB_METHOD(SShort);
	NCB_METHOD(SInt);
	NCB_METHOD(SLong);

	NCB_METHOD(UChar);
	NCB_METHOD(UShort);
	NCB_METHOD(UInt);
	NCB_METHOD(ULong);

	NCB_METHOD(Float);
	NCB_METHOD(Double);
	NCB_METHOD(ConstCharP);

	NCB_METHOD_RAW_CALLBACK(Raw1, RawCallback1, 0);
	NCB_METHOD_RAW_CALLBACK(Raw2, RawCallback2, 0);
}

struct OverloadTest {
	static void Method(int a, int  b) { mes("Method(int, int) : ", a, ",", b); }
	static void Method(char const *p) { mes("Method(char const*) : ", p); }
};

NCB_REGISTER_CLASS(OverloadTest) {
//	NCB_CONSTRUCTOR(());
	NCB_METHOD_DETAIL(Method1, Static, void, Method, (int, int));
	NCB_METHOD_DETAIL(Method2, Static, void, Method, (char const*));
}



////////////////////////////////////////

struct PropertyTest {
	PropertyTest() {}
	int  Get() const { return i; }
	void Set(int n)   { i = n; }

	static int  StaticGet()      { return s; }
	static void StaticSet(int n) { s = n; }
private:
	int i;
	static int s;
};
int PropertyTest::s = 0;

NCB_REGISTER_CLASS(PropertyTest) {
	NCB_CONSTRUCTOR(());

	NCB_PROPERTY(   Prop,   Get, Set);
	NCB_PROPERTY_RO(PropRO, Get);
	NCB_PROPERTY_WO(PropWO, Set);

	NCB_PROPERTY(   StaticProp,   StaticGet, StaticSet);
//	NCB_PROPERTY_RO(StaticPropRO, StaticGet);
//	NCB_PROPERTY_WO(StaticPropWO, StaticSet);
}





////////////////////////////////////////

void PreRegistCallbackTest()    { mes("PreRegistCallbackTest"); }
void PostRegistCallbackTest()   { mes("PostRegistCallbackTest"); }
void PreUnregistCallbackTest()  { mes("PreUnregistCallbackTest"); }
void PostUnregistCallbackTest() { mes("PostUnregistCallbackTest"); }

NCB_PRE_REGIST_CALLBACK(   PreRegistCallbackTest);
NCB_POST_REGIST_CALLBACK(  PostRegistCallbackTest);
NCB_PRE_UNREGIST_CALLBACK( PreUnregistCallbackTest);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallbackTest);




