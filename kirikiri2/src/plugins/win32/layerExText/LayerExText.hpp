#ifndef _layExText_hpp_
#define _layExText_hpp_

#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <vector>
using namespace std;

#include "layerExBase.hpp"

/**
 * フォント情報
 */
class FontInfo {
	friend class LayerExText;

protected:
	FontFamily *fontFamily; //< フォントファミリー
	REAL emSize; //< フォントサイズ 
	INT style; //< フォントスタイル

	/**
	 * フォント情報のクリア
	 */
	void clearFont();

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
	void setEmSize(REAL emSize) { this->emSize = emSize; }
	REAL getEmSize() {  return emSize; }
	void setStyle(INT style) { this->style = style; }
	INT getStyle() { return style; }

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
 * 描画外観情報
 */
class Appearance {
	friend class LayerExText;
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
class LayerExText : public layerExBase
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

public:	
	LayerExText(DispatchT obj);
	~LayerExText();
	virtual void reset();

public:
	/**
	 * 登録済みブラシ/ペンによる文字列の描画
	 * @param font フォント
	 * @param app アピアランス
	 * @param x 描画位置X
	 * @param y 描画位置Y
	 * @param text 描画テキスト
	 * @param noupdate 画面更新処理を行わない
	 */
	void drawString(FontInfo *font, Appearance *app, REAL x, REAL y, const tjs_char *text, bool noupdate=false);
};

#endif
