#include "ncbind/ncbind.hpp"
#include <vector>
using namespace std;

//----------------------------------------------
// レイヤイメージ操作ユーティリティ

// バッファ参照用の型
typedef unsigned char       *WrtRefT;
typedef unsigned char const *ReadRefT;

/**
 * レイヤのサイズとバッファを取得する
 */
static bool
GetLayerSize(iTJSDispatch2 *lay, long &w, long &h, long &pitch)
{
	// レイヤインスタンス以外ではエラー
	if (!lay || TJS_FAILED(lay->IsInstanceOf(0, 0, 0, TJS_W("Layer"), lay))) return false;

	// レイヤイメージは在るか？
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("hasImage"), 0, &val, lay)) || (val.AsInteger() == 0)) return false;

	// レイヤサイズを取得
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("imageWidth"), 0, &val, lay))) return false;
	w = (long)val.AsInteger();

	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("imageHeight"), 0, &val, lay))) return false;
	h = (long)val.AsInteger();

	// ピッチ取得
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBufferPitch"), 0, &val, lay))) return false;
	pitch = (long)val.AsInteger();

	// 正常な値かどうか
	return (w > 0 && h > 0 && pitch != 0);
}

// 書き込み用
static bool
GetLayerBufferAndSize(iTJSDispatch2 *lay, long &w, long &h, WrtRefT &ptr, long &pitch)
{
	if (!GetLayerSize(lay, w, h, pitch)) return false;

	// バッファ取得
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBufferForWrite"), 0, &val, lay))) return false;
	ptr = reinterpret_cast<WrtRefT>(val.AsInteger());
	return  (ptr != 0);
}

/**
 * Layer.copyRightBlueToLeftAlpha
 * レイヤ右半分の Blue CHANNEL を左半分の Alpha CHANNEL に複製する
 */
static tjs_error TJS_INTF_METHOD
copyRightBlueToLeftAlpha(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// 書き込み先
	WrtRefT dbuf = 0;
	long dw, dh, dpitch;
	if (!GetLayerBufferAndSize(lay, dw, dh, dbuf, dpitch)) {
		TVPThrowExceptionMessage(TJS_W("dest must be Layer."));
	}

	// 半分
	dw /= 2;
	// コピー

	WrtRefT sbuf = dbuf + dw*4;
	dbuf += 3;
	for (int i=0;i<dh;i++) {
		WrtRefT p = sbuf;   // B領域
		WrtRefT q = dbuf;   // A領域
		for (int j=0;j<dw;j++) {
			*q = *p;
			p += 4;
			q += 4;
		}
		sbuf += dpitch;
		dbuf += dpitch;
	}
	return TJS_S_OK;
}

/**
 * Layer.copyBottomBlueToTopAlpha
 * レイヤ右半分の Blue CHANNEL を左半分の Alpha CHANNELに複製する
 */
static tjs_error TJS_INTF_METHOD
copyBottomBlueToTopAlpha(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// 書き込み先
	WrtRefT dbuf = 0;
	long dw, dh, dpitch;
	if (!GetLayerBufferAndSize(lay, dw, dh, dbuf, dpitch)) {
		TVPThrowExceptionMessage(TJS_W("dest must be Layer."));
	}

	// 半分
	dh /= 2;

	// コピー
	WrtRefT sbuf = dbuf + dh * dpitch;
	dbuf += 3;
	for (int i=0;i<dh;i++) {
		WrtRefT p = sbuf;   // B領域
		WrtRefT q = dbuf;   // A領域
		for (int j=0;j<dw;j++) {
			*q = *p;
			p += 4;
			q += 4;
		}
		sbuf += dpitch;
		dbuf += dpitch;
	}
	return TJS_S_OK;
}

static tjs_error TJS_INTF_METHOD
fillAlpha(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// 書き込み先
	WrtRefT dbuf = 0;
	long dw, dh, dpitch;
	if (!GetLayerBufferAndSize(lay, dw, dh, dbuf, dpitch)) {
		TVPThrowExceptionMessage(TJS_W("dest must be Layer."));
	}
	// 全部 0xffでうめる
	dbuf += 3;
	for (int i=0;i<dh;i++) {
		WrtRefT q = dbuf;   // A領域
		for (int j=0;j<dw;j++) {
			*q = 0xff;
			q += 4;
		}
		dbuf += dpitch;
	}
	return TJS_S_OK;
}

static tjs_error TJS_INTF_METHOD
copyAlphaToProvince(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	ReadRefT sbuf = 0;
	WrtRefT  dbuf = 0;
	long w, h, spitch, dpitch, threshold = -1;
	if (numparams > 0 && param[0]->Type() != tvtVoid) {
		threshold = (long)(param[0]->AsInteger());
	}

	if (!GetLayerSize(lay, w, h, spitch)) {
		TVPThrowExceptionMessage(TJS_W("src must be Layer."));
	}
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBuffer"), 0, &val, lay)) ||
		(sbuf = reinterpret_cast<ReadRefT>(val.AsInteger())) == NULL) {
		TVPThrowExceptionMessage(TJS_W("src has no image."));
	}

	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("provinceImageBufferForWrite"), 0, &val, lay)) ||
		(dbuf = reinterpret_cast<WrtRefT>(val.AsInteger())) == NULL) {
		TVPThrowExceptionMessage(TJS_W("dst has no province image."));
	}
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("provinceImageBufferPitch"), 0, &val, lay)) ||
		(dpitch = (long)val.AsInteger()) == 0) {
		TVPThrowExceptionMessage(TJS_W("dst has no province pitch."));
	}

	sbuf += 3;
	unsigned char th = (unsigned char)threshold;
	int mode = 0;
	if (threshold >= 0 && threshold < 256) mode = 1;
	else if (threshold >= 256) mode = 2;

	for (int y = 0; y < h; y++) {
		WrtRefT  p = dbuf;
		ReadRefT q = sbuf;
		switch (mode) {
		case 0:
			for (int x = 0; x < w; x++, q+=4) *p++ = *q;
			break;
		case 1:
			for (int x = 0; x < w; x++, q+=4) *p++ = (*q >= th);
			break;
		case 2:
			for (int x = 0; x < w; x++, q+=4) *p++ = 0;
			break;
		}
		sbuf += spitch;
		dbuf += dpitch;
	}
	return TJS_S_OK;
}


NCB_ATTACH_FUNCTION(copyRightBlueToLeftAlpha, Layer, copyRightBlueToLeftAlpha);
NCB_ATTACH_FUNCTION(copyBottomBlueToTopAlpha, Layer, copyBottomBlueToTopAlpha);
NCB_ATTACH_FUNCTION(fillAlpha, Layer, fillAlpha);

NCB_ATTACH_FUNCTION(copyAlphaToProvince, Layer, copyAlphaToProvince);
