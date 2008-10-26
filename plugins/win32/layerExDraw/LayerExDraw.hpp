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
	 * フォントファミリー名を取得
	 * @param privateOnly true ならプライベートフォントのみ取得
	 */
	static tTJSVariant getFontList(bool privateOnly);
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

	REAL getAscent() {
		if (fontFamily) {
			return fontFamily->GetCellAscent(style) * emSize / fontFamily->GetEmHeight(style);
		} else {
			return 0;
		}
	}

	REAL getDescent() {
		if (fontFamily) {
			return fontFamily->GetCellDescent(style) * emSize / fontFamily->GetEmHeight(style);
		} else {
			return 0;
		}
	}

	REAL getLineSpacing() {
		if (fontFamily) {
			return fontFamily->GetLineSpacing(style) * emSize / fontFamily->GetEmHeight(style);
		} else {
			return 0;
		}
	}
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
	 * ブラシの追加
	 * @param colorOrBrush ARGB色指定またはブラシ情報（辞書）
	 * @param ox 表示オフセットX
	 * @param oy 表示オフセットY
	 */
	void addBrush(tTJSVariant colorOrBrush, REAL ox=0, REAL oy=0);
	
	/**
	 * ペンの追加
	 * @param colorOrBrush ARGB色指定またはブラシ情報（辞書）
	 * @param widthOrOption ペン幅またはペン情報（辞書）
	 * @param ox 表示オフセットX
	 * @param oy 表示オフセットY
	 */
	void addPen(tTJSVariant colorOrBrush, tTJSVariant widthOrOption, REAL ox=0, REAL oy=0);
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

	Image *getImage() {
		return (Image*)bitmap;
	}
	
public:	
	LayerExDraw(DispatchT obj);
	~LayerExDraw();
	virtual void reset();

	// ------------------------------------------------------------------
	// 描画メソッド群
	// ------------------------------------------------------------------

protected:

	/**
	 * パスの更新領域情報を取得
	 * @param app 表示表現
	 * @param path 描画するパス
	 * @return 更新領域情報
	 */
	RectF getPathExtents(const Appearance *app, const GraphicsPath *path);

	/**
	 * パスの描画
	 * @param app アピアランス
	 * @param path 描画するパス
	 * @return 更新領域情報
	 */
	RectF drawPath(const Appearance *app, const GraphicsPath *path);


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
	 * @return 更新領域情報
	 */
	RectF drawArc(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);

	/**
	 * 円錐の描画
	 * @param app アピアランス
	 * @param x 左上座標
	 * @param y 左上座標
	 * @param width 横幅
	 * @param height 縦幅
	 * @param startAngle 時計方向円弧開始位置
	 * @param sweepAngle 描画角度
	 * @return 更新領域情報
	 */
	RectF drawPie(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle);
	
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
	 * @return 更新領域情報
	 */
	RectF drawBezier(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);

	/**
	 * 連続ベジェ曲線の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @return 更新領域情報
	 */
	RectF drawBeziers(const Appearance *app, tTJSVariant points);

	/**
	 * Closed cardinal spline の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @return 更新領域情報
	 */
	RectF drawClosedCurve(const Appearance *app, tTJSVariant points);

	/**
	 * Closed cardinal spline の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @pram tension tension
	 * @return 更新領域情報
	 */
	RectF drawClosedCurve2(const Appearance *app, tTJSVariant points, REAL tension);

	/**
	 * cardinal spline の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @return 更新領域情報
	 */
	RectF drawCurve(const Appearance *app, tTJSVariant points);

	/**
	 * cardinal spline の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @parma tension tension
	 * @return 更新領域情報
	 */
	RectF drawCurve2(const Appearance *app, tTJSVariant points, REAL tension);

	/**
	 * cardinal spline の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @param offset
	 * @param numberOfSegment
	 * @param tension tension
	 * @return 更新領域情報
	 */
	RectF drawCurve3(const Appearance *app, tTJSVariant points, int offset, int numberOfSegments, REAL tension);
	
	/**
	 * 楕円の描画
	 * @param app アピアランス
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 * @return 更新領域情報
	 */
	RectF drawEllipse(const Appearance *app, REAL x, REAL y, REAL width, REAL height);

	/**
	 * 線分の描画
	 * @param app アピアランス
	 * @param x1 始点X座標
	 * @param y1 始点Y座標
	 * @param x2 終点X座標
	 * @param y2 終点Y座標
	 * @return 更新領域情報
	 */
	RectF drawLine(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2);

	/**
	 * 連続線分の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @return 更新領域情報
	 */
	RectF drawLines(const Appearance *app, tTJSVariant points);

	/**
	 * 多角形の描画
	 * @param app アピアランス
	 * @param points 点の配列
	 * @return 更新領域情報
	 */
	RectF drawPolygon(const Appearance *app, tTJSVariant points);
	
	/**
	 * 矩形の描画
	 * @param app アピアランス
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 * @return 更新領域情報
	 */
	RectF drawRectangle(const Appearance *app, REAL x, REAL y, REAL width, REAL height);

	/**
	 * 複数矩形の描画
	 * @param app アピアランス
	 * @param rects 矩形情報の配列
	 * @return 更新領域情報
	 */
	RectF drawRectangles(const Appearance *app, tTJSVariant rects);
	
	/**
	 * 文字列の描画
	 * @param font フォント
	 * @param app アピアランス
	 * @param x 描画位置X
	 * @param y 描画位置Y
	 * @param text 描画テキスト
	 * @return 更新領域情報
	 */
	RectF drawString(const FontInfo *font, const Appearance *app, REAL x, REAL y, const tjs_char *text);

	/**
	 * 文字列の描画更新領域情報の取得
	 * @param font フォント
	 * @param text 描画テキスト
	 * @return 更新領域情報の辞書 left, top, width, height
	 */
	RectF measureString(const FontInfo *font, const tjs_char *text);

	// -----------------------------------------------------------------------------
	
	/**
	 * 画像の描画
	 * @param dleft コピー先左端
	 * @param dtop  コピー先上端
	 * @param image コピー元画像
	 */
	void drawImage(REAL dleft, REAL dtop, Image *src);

	/**
	 * 画像の矩形コピー
	 * @param dleft コピー先左端
	 * @param dtop  コピー先上端
	 * @param src コピー元画像
	 * @param sleft 元矩形の左端
	 * @param stop  元矩形の上端
	 * @param swidth 元矩形の横幅
	 * @param sheight  元矩形の縦幅
	 */
	void drawImageRect(REAL dleft, REAL dtop, Image *src, REAL sleft, REAL stop, REAL swidth, REAL sheight);

	/**
	 * 画像の拡大縮小コピー
	 * @param dleft コピー先左端
	 * @param dtop  コピー先上端
	 * @param dwidth コピー先の横幅
	 * @param dheight  コピー先の縦幅
	 * @param src コピー元画像
	 * @param sleft 元矩形の左端
	 * @param stop  元矩形の上端
	 * @param swidth 元矩形の横幅
	 * @param sheight  元矩形の縦幅
	 */
	void drawImageStretch(REAL dleft, REAL dtop, REAL dwidth, REAL dheight, Image *src, REAL sleft, REAL stop, REAL swidth, REAL sheight);

	/**
	 * 画像のアフィン変換コピー
	 * @param src コピー元画像
	 * @param sleft 元矩形の左端
	 * @param stop  元矩形の上端
	 * @param swidth 元矩形の横幅
	 * @param sheight  元矩形の縦幅
	 * @param affine アフィンパラメータの種類(true:変換行列, false:座標指定), 
	 */
	void drawImageAffine(Image *src, REAL sleft, REAL stop, REAL swidth, REAL sheight, bool affine, REAL A, REAL B, REAL C, REAL D, REAL E, REAL F);
};

#endif
