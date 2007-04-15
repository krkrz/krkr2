#ifndef __WindowExBase__
#define __WindowExBase__

#include <windows.h>
#include "tp_stub.h"

/**
 * ウインドウ拡張 基本情報保持用ネイティブインスタンス。
 */
class NI_WindowExBase : public tTJSNativeInstance
{
protected:
	// ウインドウから情報を取得するためのプロパティ
	static iTJSDispatch2 * _HWNDProp;
	static iTJSDispatch2 * _innerWidthProp;
	static iTJSDispatch2 * _innerHeightProp;
	static iTJSDispatch2 * _primaryLayerProp;
	
	// レイヤから情報を取得するためのプロパティ
	static iTJSDispatch2 * _leftProp;
	static iTJSDispatch2 * _topProp;
	static iTJSDispatch2 * _widthProp;
	static iTJSDispatch2 * _heightProp;
	static iTJSDispatch2 * _pitchProp;
	static iTJSDispatch2 * _bufferProp;
	static iTJSDispatch2 * _updateProp;

	// リセット処理用 work 変数
	iTJSDispatch2 *_windowobj;
public:
	// リセット処理実行
	void _reset();
	
public:
	// 情報保持用
	HWND _hwnd;
	int _innerWidth;
	int _innerHeight;

	// レイヤ情報比較保持用
	tjs_int _width;
	tjs_int _height;
	tjs_int _pitch;
	unsigned char *_buffer;

public:
	// クラスＩＤ保持用
	static int classId;

	/**
	 * 初期化処理実行
	 * @param windowobj ウインドウクラスオブジェクト
	 * @parma layerobj  レイヤクラスオブジェクト
	 */
	static void init(iTJSDispatch2 *windowobj, iTJSDispatch2 *layerobj);

	/**
	 * 解放処理実行
	 */
	static void unInit();
	
	/**
	 * ネイティブオブジェクトの取得
	 * @param objthis this
	 * @parma crete true なら存在しない場合作成
	 * @return ネイティブオブジェクト
	 */
	static NI_WindowExBase *getNative(iTJSDispatch2 *objthis, bool create=true);

	/**
	 * グラフィックを初期化する
	 * レイヤのビットマップ情報が変更されている可能性があるので毎回チェックする。
	 * 変更されている場合は描画用のコンテキストを組みなおす
	 * @param windowobj レイヤオブジェクト
	 * @return 初期化実行された場合は true を返す
	 */
	void reset(iTJSDispatch2 *windowobj);
	
public:
	/**
	 * コンストラクタ
	 */
	NI_WindowExBase();
};

#endif
