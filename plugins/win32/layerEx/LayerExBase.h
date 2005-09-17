#ifndef __LayerExBase__
#define __LayerExBase__

#include <windows.h>
#include "tp_stub.h"

/**
 * レイヤ拡張 基本情報保持用ネイティブインスタンス。
 */
class NI_LayerExBase : public tTJSNativeInstance
{
protected:
	// レイヤから情報を取得するためのプロパティ
	// 少しでも高速化するためキャッシュしておく
	iTJSDispatch2 * _leftProp;
	iTJSDispatch2 * _topProp;
	iTJSDispatch2 * _widthProp;
	iTJSDispatch2 * _heightProp;
	iTJSDispatch2 * _pitchProp;
	iTJSDispatch2 * _bufferProp;
	iTJSDispatch2 * _updateProp;
	
	// レイヤ情報比較保持用
	tjs_int _width;
	tjs_int _height;
	tjs_int _pitch;
	tjs_uint8* _buffer;

public:
	/**
	 * 再描画要請
	 */
	virtual void redraw(iTJSDispatch2 *layerobj);
	
	/**
	 * グラフィックを初期化する
	 * レイヤのビットマップ情報が変更されている可能性があるので毎回チェックする。
	 * 変更されている場合は描画用のコンテキストを組みなおす
	 * @param layerobj レイヤオブジェクト
	 */
	virtual void reset(iTJSDispatch2 *layerobj);
	
public:
	/**
	 * コンストラクタ
	 */
	NI_LayerExBase(iTJSDispatch2 *layerobj);

	/**
	 * デストラクタ
	 */
	~NI_LayerExBase();
};

#endif
