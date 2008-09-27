#ifndef _layerExText_hpp_
#define _layerExText_hpp_

#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <vector>
using namespace std;

#include "layerExBase.hpp"

/**
 * GDIPlus 固有処理用
 */
struct GdiPlus {
	/**
	 * プライベートフォントの追加
	 * @param fontFileName フォントファイル名
	 */
	static void addPrivateFont(const tjs_char *fontFileName);

	/**
	 * プライベートフォント一覧をログに出力
	 */
	static void showPrivateFontList();
};

/**
 * フォント情報
 */
class FontInfo {
	friend class LayerExDraw;

protected:
	FontFamily *fontFamily; //< フォントファミリー
	ttstr familyName;
	REAL emSize; //< フォントサイズ 
	INT style; //< フォントスタイル

	/**
	 * フォント情報のクリア
	 */
	void clear();

public:

	FontInfo();
	/**
	 * コンストラクタ
	 * @param familyName フォントファミリー
	 * @param emSize フォントのサイズ
	 * @param style フォントスタイル
	 */
	FontInfo(const tjs_char *familyName, REAL emSize, INT style);
	FontInfo(const FontInfo &orig);

	/**
	 * デストラクタ
	 */
	virtual ~FontInfo();

	void setFamilyName(const tjs_char *familyName);
	const tjs_char *getFamilyName() { return familyName.c_str(); }
	void setEmSize(REAL emSize) { this->emSize = emSize; }
	REAL getEmSize() {  return emSize; }
	void setStyle(INT style) { this->style = style; }
	INT getStyle() { return style; }
};

/**
 * 描画外観情報
 */
class Appearance {
	friend class LayerExDraw;
public:
	// 描画情報
	struct DrawInfo{
		int type;   // 0:ブラシ 1:ペン
		void *info; // 情報オブジェクト
		REAL ox; //< 表示オフセット
		REAL oy; //< 表示オフセット
		DrawInfo() : ox(0), oy(0), type(0), info(NULL) {}
		DrawInfo(REAL ox, REAL oy, Pen *pen) : ox(ox), oy(oy), type(0), info(pen) {}
		DrawInfo(REAL ox, REAL oy, Brush *brush) : ox(ox), oy(oy), type(1), info(brush) {}
		DrawInfo(const DrawInfo &orig) {
			ox = orig.ox;
			oy = orig.oy;
			type = orig.type;
			if (orig.info) {
				switch (type) {
				case 0:
					info = (void*)((Pen*)orig.info)->Clone();
					break;
				case 1:
					info = (void*)((Brush*)orig.info)->Clone();
					break;
				}
			} else {
				info = NULL;
			}
		}
		virtual ~DrawInfo() {
			if (info) {
				switch (type) {
				case 0:
					delete (Pen*)info;
					break;
				case 1:
					delete (Brush*)info;
					break;
				}
			}
		}	
	};
	vector<DrawInfo>drawInfos;

public:
	Appearance();
	virtual ~Appearance();

	/**
	 * 情報のクリア
	 */
	void clear();
	
	/**
	 * 固定色ブラシの追加
	 * @param argb 色指定
	 * @param ox 表示オフセットX
	 * @param oy 表示オフセットY
	 */
	void addSolidBrush(ARGB argb, REAL ox=0, REAL oy=0);

	/**
	 * グラデーションブラシの追加
	 * @param x1
	 * @param y1
	 * @param argb1 色指定その１
	 * @param x1
	 * @param y1
	 * @param argb1 色指定その２
	 * @param ox 表示オフセットX
	 * @param oy 表示オフセットY
	 */
	void addLinearGradientBrush(REAL x1, REAL y1, ARGB argb1, REAL x2, REAL y2, ARGB argb2, REAL ox=0, REAL oy=0);
	
	/**
	 * 固定色ペンの追加
	 * @param argb 色指定
	 * @param width ペン幅
	 * @param ox 表示オフセットX
	 * @param oy 表示オフセットY
	 */
	void addColorPen(ARGB argb, REAL width=1.0, REAL ox=0, REAL oy=0);
};

/*
 * アウトラインベースのテキスト描画メソッドの追加
 */
class LayerExDraw : public layerExBase
{
protected:
	// 情報保持用
	GeometryT width, height;
	BufferT   buffer;
	PitchT    pitch;
	
	/// レイヤを参照するビットマップ
	Bitmap *bitmap;
	/// レイヤに対して描画するコンテキスト
	Graphics *graphics;

	bool updateWhenDraw;

public:
	void setUpdateWhenDraw(int updateWhenDraw) {
		this->updateWhenDraw = updateWhenDraw != 0;
	}
	int getUpdateWhenDraw() { return updateWhenDraw ? 1 : 0; }
	
public:	
	LayerExDraw(DispatchT obj);
	~LayerExDraw();
	virtual void reset();

	// ------------------------------------------------------------------
	// 描画メソッド群
	// ------------------------------------------------------------------

protected:
	/**
	 * パスの描画
	 * @param app アピアランス
	 * @param path 描画するパス
	 */
	void drawPath(const Appearance *app, const GraphicsPath *path);


public:
	/**
	 * 画面の消去
	 * @param argb 消去色
	 */
	void clear(ARGB argb);

	/**
	 * 円弧の描画
	 * @param app アピアランス
	 * @param x 左上座標
	 * @param y 左上座標
	 * @param width 横幅
	 * @param height 縦幅
	 * @param startAngle 時計方向円弧開始位置
	 * @param sweepAngle 描画角度
	 */
	void drawArc(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);

	/**
	 * 円錐の描画
	 * @param app アピアランス
	 * @param x 左上座標
	 * @param y 左上座標
	 * @param width 横幅
	 * @param height 縦幅
	 * @param startAngle 時計方向円弧開始位置
	 * @param sweepAngle 描画角度
	 */
	void drawPie(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
	
	/**
	 * ベジェ曲線の描画
	 * @param app アピアランス
	 * @param x1
	 * @param y1
	 * @param x2
	 * @param y2
	 * @param x3
	 * @param y3
	 * @param x4
	 * @param y4
	 */
	void drawBezier(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);
	
	/**
	 * 楕円の描画
	 * @param app アピアランス
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	void drawEllipse(const Appearance *app, REAL x, REAL y, REAL width, REAL height);

	/**
	 * 線分の描画
	 * @param app アピアランス
	 * @param x1 始点X座標
	 * @param y1 始点Y座標
	 * @param x2 終点X座標
	 * @param y2 終点Y座標
	 */
	void drawLine(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2);

	/**
	 * 矩形の描画
	 * @param app アピアランス
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	void drawRectangle(const Appearance *app, REAL x, REAL y, REAL width, REAL height);
	
	/**
	 * 文字列の描画
	 * @param font フォント
	 * @param app アピアランス
	 * @param x 描画位置X
	 * @param y 描画位置Y
	 * @param text 描画テキスト
	 */
	void drawString(const FontInfo *font, const Appearance *app, REAL x, REAL y, const tjs_char *text);

	/**
	 * 画像の描画
	 * @param x 表示位置X
	 * @param y 表示位置Y
	 * @param name 画像名
	 */
	void drawImage(REAL x, REAL y, const tjs_char *name);
};

#endif
