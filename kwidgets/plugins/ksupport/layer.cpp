//
// $Id$
//


#include <windows.h>
#undef max
#undef min

#include <algorithm>
#include "ncbind.hpp"


//----------------------------------------------------------------------
// ユーティリティ関数
enum map_t
{
  MAP_CONST,
  MAP_X,
  MAP_Y,
};

struct hsv_t
{
  double h; // 0.0-360.0
  double s; // 0.0-100.0
  double v; // 0.0-100.0
};

struct rgb_t
{
  int r; // 0.0-255
  int g; // 0.0-255
  int b; // 0.0-255
};

void 
hsv_to_rgb(const hsv_t *hsv, rgb_t *rgb)
{
  double h, s, v;
  double p, q, t, f;
  int hi;
  h = hsv->h;
  s = hsv->s / 100;
  v = hsv->v / 100;
  if (s == 0) {
    rgb->r = rgb->g = rgb->b = int(v * 255);
    return;
  }
  if (h == 360)
    h = 0;
  hi = int(h) / 60 % 6;
  f = h / 60 - hi;
  p = v * (1 - s);
  q = v * (1 - f * s);
  t = v * (1 - (1 - f) * s);
  double r, g, b;
  switch (hi) {
  case 0: r = v; g = t; b = p; break;
  case 1: r = q; g = v; b = p; break;
  case 2: r = p; g = v; b = t; break;
  case 3: r = p; g = q; b = v; break;
  case 4: r = t; g = p; b = v; break;
  case 5: r = v; g = p; b = q; break;
  }
  rgb->r = int(r * 255);
  rgb->g = int(g * 255);
  rgb->b = int(b * 255);
}

void 
rgb_to_hsv(const rgb_t *rgb, hsv_t *hsv)
{
  double r, g, b;
  double max_v, min_v;
  r = rgb->r / 255.0;
  g = rgb->g / 255.0;
  b = rgb->b / 255.0;
  max_v = double(std::max(r, std::max(g, b)));
  min_v = double(std::min(r, std::min(g, b)));
  if (max_v == 0) {
    hsv->h = 0;
    hsv->s = 0;
    hsv->v = 0;
    return;
  }
  if (max_v == min_v) {
    hsv->h = 0;
    hsv->s = 0;
    hsv->v = max_v * 100;
    return;
  }
  if (max_v == r)
    hsv->h = ((g - b) / (max_v - min_v)) * 60;
  else if (max_v == g)
    hsv->h = ((b - r) / (max_v - min_v)) * 60 + 120;
  else
    hsv->h = ((r - g) / (max_v - min_v)) * 60 + 240;
  if (hsv->h < 0)
    hsv->h += 360;
  hsv->s = (max_v - min_v) / max_v * 100;
  hsv->v = max_v * 100;
}


//----------------------------------------------------------------------
class LayerSupport
{
private:
  iTJSDispatch2 *mObjthis; //< オブジェクト情報の参照

public:
  LayerSupport(iTJSDispatch2 *objthis) : mObjthis(objthis) {
  }

  static tTJSVariant RGB2HSV(tjs_int r, tjs_int g, tjs_int b) {
    rgb_t rgb;
    hsv_t hsv;
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
    rgb_to_hsv(&rgb, &hsv);
    char buf[256];
    sprintf_s(buf, "%%[h:%d,s:%d,v:%d]", tjs_int(hsv.h), tjs_int(hsv.s), tjs_int(hsv.v));
    tTJSVariant result;
    TVPExecuteExpression(ttstr(buf), &result);
    return result;
  }

  static tTJSVariant HSV2RGB(tjs_real h, tjs_real s, tjs_real v) {
    rgb_t rgb;
    hsv_t hsv;
    hsv.h = h;
    hsv.s = s;
    hsv.v = v;
    hsv_to_rgb(&hsv, &rgb);
    char buf[256];
    sprintf_s(buf, "%%[r:%d,g:%d,b:%d]", rgb.r, rgb.g, rgb.b);
    tTJSVariant result;
    TVPExecuteExpression(ttstr(buf), &result);
    return result;
  }

  void update(void) {
    mObjthis->FuncCall(0, TJS_W("update"), NULL, NULL, 0, NULL, mObjthis);
  }

  void fillHSV(ttstr h, ttstr s, ttstr v) {
    ncbPropAccessor layerObj(mObjthis);
    map_t h_map, s_map, v_map;
    double h_const, s_const, v_const;
    tjs_int width, height, pitch;
    unsigned char *imageBuffer;

    if (h == TJS_W("x")) {
      h_map = MAP_X;
    } else if (h == TJS_W("y")) {
      h_map = MAP_Y;
    } else {
      h_map = MAP_CONST;
      h_const = double(h.AsInteger());
    }

    if (s == TJS_W("x")) {
      s_map = MAP_X;
    } else if (s == TJS_W("y")) {
      s_map = MAP_Y;
    } else {
      s_map = MAP_CONST;
      s_const = double(s.AsInteger());
    }

    if (v == TJS_W("x")) {
      v_map = MAP_X;
    } else if (v == TJS_W("y")) {
      v_map = MAP_Y;
    } else {
      v_map = MAP_CONST;
      v_const = double(v.AsInteger());
    }

    width = layerObj.GetValue(L"width",  ncbTypedefs::Tag<tjs_int>());
    height = layerObj.GetValue(L"height",  ncbTypedefs::Tag<tjs_int>());
    pitch = layerObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    imageBuffer = reinterpret_cast<unsigned char*>(layerObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));

    for (tjs_int y = 0; y < height; y++) {
      unsigned char *p = imageBuffer;
      for (tjs_int x = 0; x < width; x++) {
	hsv_t hsv;
	rgb_t rgb;
	switch (h_map) {
	case MAP_X: hsv.h = 360.0 * x / (width - 1); break;
	case MAP_Y: hsv.h = 360.0 * (height - 1 - y) / (height - 1); break;
	case MAP_CONST: hsv.h = h_const; break;
	}
	switch (s_map) {
	case MAP_X: hsv.s = 100.0 * x / (width - 1); break;
	case MAP_Y: hsv.s = 100.0 * (height - 1 - y) / (height - 1); break;
	case MAP_CONST: hsv.s = s_const; break;
	}
	switch (v_map) {
	case MAP_X: hsv.v = 100.0 * x / (width - 1); break;
	case MAP_Y: hsv.v = 100.0 * (height - 1 - y) / (height - 1); break;
	case MAP_CONST: hsv.v = v_const; break;
	}
	hsv_to_rgb(&hsv, &rgb);
	p[0] = (unsigned char)rgb.b;
	p[1] = (unsigned char)rgb.g;
	p[2] = (unsigned char)rgb.r;
	p[3] = 255;
	p += 4;
      }
      imageBuffer += pitch;
    }
    update();
  }

  void fillRGB(ttstr r, ttstr g, ttstr b) {
    ncbPropAccessor layerObj(mObjthis);
    map_t r_map, g_map, b_map;
    tjs_int r_const, g_const, b_const;
    tjs_int width, height, pitch;
    unsigned char *imageBuffer;

    if (r == TJS_W("x")) {
      r_map = MAP_X;
    } else if (r == TJS_W("y")) {
      r_map = MAP_Y;
    } else {
      r_map = MAP_CONST;
      r_const = tjs_int(r.AsInteger());
    }

    if (g == TJS_W("x")) {
      g_map = MAP_X;
    } else if (g == TJS_W("y")) {
      g_map = MAP_Y;
    } else {
      g_map = MAP_CONST;
      g_const = tjs_int(g.AsInteger());
    }

    if (b == TJS_W("x")) {
      b_map = MAP_X;
    } else if (b == TJS_W("y")) {
      b_map = MAP_Y;
    } else {
      b_map = MAP_CONST;
      b_const = tjs_int(b.AsInteger());
    }

    width = layerObj.GetValue(L"width",  ncbTypedefs::Tag<tjs_int>());
    height = layerObj.GetValue(L"height",  ncbTypedefs::Tag<tjs_int>());
    pitch = layerObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    imageBuffer = reinterpret_cast<unsigned char*>(layerObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));

    for (tjs_int y = 0; y < height; y++) {
      unsigned char *p = imageBuffer;
      for (tjs_int x = 0; x < width; x++) {
	rgb_t rgb;
	switch (r_map) {
	case MAP_X: rgb.r = 255 * x / (width - 1); break;
	case MAP_Y: rgb.r = 255 * (height - 1 - y) / (height - 1); break;
	case MAP_CONST: rgb.r = r_const; break;
	}
	switch (g_map) {
	case MAP_X: rgb.g = 255 * x / (width - 1); break;
	case MAP_Y: rgb.g = 255 * (height - 1 - y) / (height - 1); break;
	case MAP_CONST: rgb.g = g_const; break;
	}
	switch (b_map) {
	case MAP_X: rgb.b = 255 * x / (width - 1); break;
	case MAP_Y: rgb.b = 255 * (height - 1 - y) / (height - 1); break;
	case MAP_CONST: rgb.b = b_const; break;
	}
	p[0] = (unsigned char)rgb.b;
	p[1] = (unsigned char)rgb.g;
	p[2] = (unsigned char)rgb.r;
	p[3] = 255;
	p += 4;
      }
      imageBuffer += pitch;
    }
    update();
  }

  void copyWrappedRect(tjs_int dleft, tjs_int dtop, tjs_int dwidth, tjs_int dheight,
                       tTJSVariant src, tjs_int sleft, tjs_int stop, tjs_int swidth, tjs_int sheight,
                       tjs_int shiftLeft, tjs_int shiftTop) {
    ncbPropAccessor dstObj(mObjthis), srcObj(src);
    tjs_int dright = dleft + dwidth, dbottom = dtop + dheight;
    dleft = std::max(dleft, dstObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>()));
    dtop = std::max(dtop, dstObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>()));
    dright = std::min(dright, dstObj.GetValue(L"clipLeft", ncbTypedefs::Tag<tjs_int>()) + dstObj.GetValue(L"clipWidth", ncbTypedefs::Tag<tjs_int>()));
    dbottom = std::min(dbottom, dstObj.GetValue(L"clipTop", ncbTypedefs::Tag<tjs_int>()) + dstObj.GetValue(L"clipHeight", ncbTypedefs::Tag<tjs_int>()));
    dwidth = dright - dleft;
    dheight = dbottom - dtop;
    tjs_int dstPitch = dstObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    unsigned char *dstBuffer = reinterpret_cast<unsigned char*>(dstObj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int64>()));
    tjs_int srcPitch = srcObj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
    const unsigned char *srcBuffer = reinterpret_cast<const unsigned char*>(srcObj.GetValue(L"mainImageBuffer", ncbTypedefs::Tag<tjs_int64>()));
    for (tjs_int y = dtop; y < dbottom; y++) {
      unsigned char *dst = dstBuffer + dstPitch * y + dleft * 4;
      const unsigned char *srcBase = srcBuffer + (stop + tjs_uint(shiftTop + y) % sheight) * srcPitch;
      for (tjs_int x = dleft; x < dright; x++) {
        const unsigned char *src = srcBase + (sleft + tjs_uint(shiftLeft + x) % swidth) * 4;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
      }
    }
    update();
  }
};

NCB_GET_INSTANCE_HOOK(LayerSupport)
{
  NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
    ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
    if (!obj) {
      obj = new ClassT(objthis);				// ない場合は生成する
      SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
    }
    return obj;
  }
};

NCB_ATTACH_CLASS_WITH_HOOK(LayerSupport, Layer) {
  NCB_METHOD(RGB2HSV);
  NCB_METHOD(HSV2RGB);
  NCB_METHOD(fillHSV);
  NCB_METHOD(fillRGB);
  NCB_METHOD(copyWrappedRect);
};


