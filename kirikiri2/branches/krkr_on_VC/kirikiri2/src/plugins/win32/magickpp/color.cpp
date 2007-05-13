#include "magickpp.hpp"

struct ColorProxy {
	typedef Magick::ColorRGB  ColorRGB;
	typedef Magick::ColorHSL  ColorHSL;
	typedef Magick::ColorYUV  ColorYUV;
	typedef Magick::ColorGray ColorGray;
	typedef Magick::ColorMono ColorMono;

#define PROP_PROXY(type, base, tag, method) \
	static type Get ## tag(Color const *c)   { return base(*c).method(); } \
	static void Set ## tag(Color *c, type v) { base tmp(*c); tmp.method(v); *c = tmp; }


	PROP_PROXY(double, ColorRGB, R, red);
	PROP_PROXY(double, ColorRGB, G, green);
	PROP_PROXY(double, ColorRGB, B, blue);

	PROP_PROXY(double, ColorHSL, H, hue);
	PROP_PROXY(double, ColorHSL, S, saturation);
	PROP_PROXY(double, ColorHSL, L, luminosity);

	PROP_PROXY(double, ColorYUV, Y, y);
	PROP_PROXY(double, ColorYUV, U, u);
	PROP_PROXY(double, ColorYUV, V, v);

	PROP_PROXY(double, ColorGray, Gray, shade);
	PROP_PROXY(bool,   ColorMono, Mono, mono);
};


// Color
MAGICK_SUBCLASS(Color) {
	NCB_CONSTRUCTOR(());
	typedef Magick::Quantum Quantum;
	PROP_RW(Quantum, redQuantum);
	PROP_RW(Quantum, greenQuantum);
	PROP_RW(Quantum, blueQuantum);
	PROP_RW(Quantum, alphaQuantum);
	PROP_RW(double,  alpha);
	PROP_RW(bool, isValid);
//	PROP_RO(intensity);

	NCB_PROPERTY_DETAIL(string,
						Const, StringT, Class::operator StringT, (),
						Class, Color const&, Class::operator =, (StringT const&)
						);
	NCB_METHOD(scaleDoubleToQuantum);
	NCB_METHOD_DETAIL(scaleQuantumToDouble, Static, double, Class::scaleQuantumToDouble, (double));

	NCB_PROPERTY_PROXY(red,        ColorProxy::GetR, ColorProxy::SetR);
	NCB_PROPERTY_PROXY(green,      ColorProxy::GetG, ColorProxy::SetG);
	NCB_PROPERTY_PROXY(blue,       ColorProxy::GetB, ColorProxy::SetB);

	NCB_PROPERTY_PROXY(hue,        ColorProxy::GetH, ColorProxy::SetH);
	NCB_PROPERTY_PROXY(saturation, ColorProxy::GetS, ColorProxy::SetS);
	NCB_PROPERTY_PROXY(luminosity, ColorProxy::GetL, ColorProxy::SetL);

	NCB_PROPERTY_PROXY(y,          ColorProxy::GetY, ColorProxy::SetY);
	NCB_PROPERTY_PROXY(u,          ColorProxy::GetU, ColorProxy::SetU);
	NCB_PROPERTY_PROXY(v,          ColorProxy::GetV, ColorProxy::SetV);

	NCB_PROPERTY_PROXY(shade,      ColorProxy::GetGray, ColorProxy::SetGray);
	NCB_PROPERTY_PROXY(mono,       ColorProxy::GetMono, ColorProxy::SetMono);
}
