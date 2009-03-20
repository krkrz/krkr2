#include "ncbind/ncbind.hpp"
#include <vector>
using namespace std;

//----------------------------------------------
// レイヤイメージ操作ユーティリティ

// バッファ参照用の型
typedef unsigned char       *WrtRefT;

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
 * レイヤ右半分の Blue CHANNEL を左半分の Alpha 領域に複製する
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

NCB_ATTACH_FUNCTION(copyRightBlueToLeftAlpha, Layer, copyRightBlueToLeftAlpha);
