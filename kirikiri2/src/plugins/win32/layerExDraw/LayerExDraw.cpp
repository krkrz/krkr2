#pragma comment(lib, "gdiplus.lib")
#include "LayerExDraw.hpp"

#include <stdio.h>
/**
 * ログ出力用
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf_s(msg, 1024, _TRUNCATE, format, args);
	TVPAddLog(msg);
	va_end(args);
}

// GDI+ 基本情報
static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR gdiplusToken;

/// プライベートフォント情報
static PrivateFontCollection *privateFontCollection = NULL;
static vector<void*> fontDatas;

// GDI+ 初期化
void initGdiPlus()
{
	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// GDI+ 終了
void deInitGdiPlus()
{
	// フォントデータの解放
	delete privateFontCollection;
	vector<void*>::const_iterator i = fontDatas.begin();
	while (i != fontDatas.end()) {
		delete[] *i;
		i++;
	}
	fontDatas.clear();
	GdiplusShutdown(gdiplusToken);
}

// --------------------------------------------------------
// フォント情報
// --------------------------------------------------------

/**
 * プライベートフォントの追加
 * @param fontFileName フォントファイル名
 */
void
GdiPlus::addPrivateFont(const tjs_char *fontFileName)
{
	if (!privateFontCollection) {
		privateFontCollection = new PrivateFontCollection();
	}
	ttstr filename = TVPGetPlacedPath(fontFileName);
	if (filename.length()) {
		if (!wcschr(filename.c_str(), '>')) {
			// 実ファイルが存在
			TVPGetLocalName(filename);
			privateFontCollection->AddFontFile(filename.c_str());
			return;
		} else {
			// メモリにロードして展開
			IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
			if (in) {
				STATSTG stat;
				in->Stat(&stat, STATFLAG_NONAME);
				// サイズあふれ無視注意
				ULONG size = (ULONG)stat.cbSize.QuadPart;
				char *data = new char[size];
				if (in->Read(data, size, &size) == S_OK) {
					privateFontCollection->AddMemoryFont(data, size);
					fontDatas.push_back(data);					
				} else {
					delete[] data;
				}
				in->Release();
				return;
			}
		}
	}
	TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + fontFileName).c_str());
}

/**
 * プライベートフォント一覧をログに出力
 */
void
GdiPlus::showPrivateFontList()
{
	if (!privateFontCollection)	return;
	int count = privateFontCollection->GetFamilyCount();

	// fontCollection.
	FontFamily *families = new FontFamily[count];
	if (families) {
		TVPAddLog("--- private font families ---");
		privateFontCollection->GetFamilies(count, families, &count);
		for (int i=0;i<count;i++) {
			WCHAR familyName[LF_FACESIZE];
			if (families[i].GetFamilyName(familyName) == Ok) {
				TVPAddLog(familyName);
			}
		}
	}
}

// --------------------------------------------------------
// フォント情報
// --------------------------------------------------------

/**
 * コンストラクタ
 */
FontInfo::FontInfo() : fontFamily(NULL), emSize(12), style(0) {}

/**
 * コンストラクタ
 * @param familyName フォントファミリー
 * @param emSize フォントのサイズ
 * @param style フォントスタイル
 */
FontInfo::FontInfo(const tjs_char *familyName, REAL emSize, INT style) : fontFamily(NULL)
{
	setFamilyName(familyName);
	setEmSize(emSize);
	setStyle(style);
}

/**
 * コピーコンストラクタ
 */
FontInfo::FontInfo(const FontInfo &orig)
{
	fontFamily = orig.fontFamily ? orig.fontFamily->Clone() : NULL;
	emSize = orig.emSize;
	style = orig.style;
}

/**
 * デストラクタ
 */
FontInfo::~FontInfo()
{
	clear();
}

/**
 * フォント情報のクリア
 */
void
FontInfo::clear()
{
	delete fontFamily;
	fontFamily = NULL;
	familyName = "";
}

/**
 * フォントの指定
 */
void
FontInfo::setFamilyName(const tjs_char *familyName)
{
	if (familyName) {
		clear();
		if (privateFontCollection) {
			fontFamily = new FontFamily(familyName, privateFontCollection);
			if (fontFamily->IsAvailable()) {
				this->familyName = familyName;
				return;
			} else {
				clear();
			}
		}
		fontFamily = new FontFamily(familyName);
		if (fontFamily->IsAvailable()) {
			this->familyName = familyName;
			return;
		} else {
			clear();
		}
	}
}

// --------------------------------------------------------
// アピアランス情報
// --------------------------------------------------------

Appearance::Appearance() {}

Appearance::~Appearance()
{
	clear();
}

/**
 * 情報のクリア
 */
void
Appearance::clear()
{
	drawInfos.clear();
}


/**
 * 固定色ブラシの追加
 * @param argb 色指定
 * @param ox 表示オフセットX
 * @param oy 表示オフセットY
 */
void
Appearance::addSolidBrush(ARGB argb, REAL ox, REAL oy)
{
	drawInfos.push_back(DrawInfo(ox, oy, new SolidBrush(Color(argb))));
}


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
void 
Appearance::addLinearGradientBrush(REAL x1, REAL y1, ARGB argb1, REAL x2, REAL y2, ARGB argb2, REAL ox, REAL oy)
{
	drawInfos.push_back(DrawInfo(ox, oy, new LinearGradientBrush(PointF(x1,y1), PointF(x2,y2), Color(argb1), Color(argb2))));
}

/**
 * 固定色ペンの追加
 * @param ox 表示オフセットX
 * @param oy 表示オフセットY
 * @param argb 色指定
 * @param width ペン幅
 */
void 
Appearance::addColorPen(ARGB argb, REAL width, REAL ox, REAL oy)
{
	Pen *pen = new Pen(Color(argb), width);
	// とりあえずまるく接続
	pen->SetLineCap(LineCapRound, LineCapRound, DashCapRound);
	pen->SetLineJoin(LineJoinRound);
	drawInfos.push_back(DrawInfo(ox, oy, pen));
}

// --------------------------------------------------------
// フォント描画系
// --------------------------------------------------------

/**
 * コンストラクタ
 */
LayerExDraw::LayerExDraw(DispatchT obj)
	: layerExBase(obj), width(-1), height(-1), pitch(0), buffer(NULL), bitmap(NULL), graphics(NULL), updateWhenDraw(true)
{
}

/**
 * デストラクタ
 */
LayerExDraw::~LayerExDraw()
{
	delete graphics;
	delete bitmap;
}

void
LayerExDraw::reset()
{
	layerExBase::reset();
	// 変更されている場合はつくりなおし
	if (!(graphics &&
		  width  == _width &&
		  height == _height &&
		  pitch  == _pitch &&
		  buffer == _buffer)) {
		delete graphics;
		delete bitmap;
		width  = _width;
		height = _height;
		pitch  = _pitch;
		buffer = _buffer;
		bitmap = new Bitmap(width, height, pitch, PixelFormat32bppARGB, (unsigned char*)buffer);
		graphics = new Graphics(bitmap);
		graphics->SetSmoothingMode(SmoothingModeAntiAlias);
		graphics->SetCompositingMode(CompositingModeSourceOver);
		graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
	}
}

/**
 * 画面の消去
 * @param argb 消去色
 */
void
LayerExDraw::clear(ARGB argb)
{
	graphics->Clear(Color(argb));
}

/**
 * パスを描画する
 * @param app 表示表現
 * @param path 描画するパス
 */
void
LayerExDraw::drawPath(const Appearance *app, const GraphicsPath *path)
{
	// 領域記録用
	Rect unionRect;
	Rect rect;

	// 描画情報を使って次々描画
	vector<Appearance::DrawInfo>::const_iterator i = app->drawInfos.begin();
	while (i != app->drawInfos.end()) {
		if (i->info) {
			Matrix matrix(1,0,0,1,i->ox,i->oy);
			graphics->SetTransform(&matrix);
			switch (i->type) {
			case 0:
				{
					Pen *pen = (Pen*)i->info;
					graphics->DrawPath(pen, path);
					if (updateWhenDraw) {
						path->GetBounds(&rect, &matrix, pen);
						unionRect.Union(unionRect, unionRect, rect);
					}
				}
				break;
			case 1:
				graphics->FillPath((Brush*)i->info, path);
				if (updateWhenDraw) {
					path->GetBounds(&rect, &matrix, NULL);
					unionRect.Union(unionRect, unionRect, rect);
				}
				break;
			}
		}
		i++;
	}

	if (updateWhenDraw) {
		// 更新処理
		tTJSVariant  vars [4] = { unionRect.X, unionRect.Y, unionRect.Width, unionRect.Height };
		tTJSVariant *varsp[4] = { vars, vars+1, vars+2, vars+3 };
		_pUpdate(4, varsp);
	}
}

/**
 * 円弧の描画
 * @param x 左上座標
 * @param y 左上座標
 * @param width 横幅
 * @param height 縦幅
 * @param startAngle 時計方向円弧開始位置
 * @param sweepAngle 描画角度
 */
void
LayerExDraw::drawArc(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle)
{
	GraphicsPath path;
	path.AddArc(x, y, width, height, startAngle, sweepAngle);
	drawPath(app, &path);
}

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
void
LayerExDraw::drawBezier(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4)
{
	GraphicsPath path;
	path.AddBezier(x1, y1, x2, y2, x3, y3, x4, y4);
	drawPath(app, &path);
}

/**
 * 円錐の描画
 * @param x 左上座標
 * @param y 左上座標
 * @param width 横幅
 * @param height 縦幅
 * @param startAngle 時計方向円弧開始位置
 * @param sweepAngle 描画角度
 */
void
LayerExDraw::drawPie(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle)
{
	GraphicsPath path;
	path.AddPie(x, y, width, height, startAngle, sweepAngle);
	drawPath(app, &path);
}

/**
 * 楕円の描画
 * @param app アピアランス
 * @param x
 * @param y
 * @param width
 * @param height
 */
void
LayerExDraw::drawEllipse(const Appearance *app, REAL x, REAL y, REAL width, REAL height)
{
	GraphicsPath path;
	path.AddEllipse(x, y, width, height);
	drawPath(app, &path);
}

/**
 * 線分の描画
 * @param app アピアランス
 * @param x1 始点X座標
 * @param y1 始点Y座標
 * @param x2 終点X座標
 * @param y2 終点Y座標
 */
void
LayerExDraw::drawLine(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2)
{
	GraphicsPath path;
	path.AddLine(x1, y1, x2, y2);
	drawPath(app, &path);
}

/**
 * 矩形の描画
 * @param app アピアランス
 * @param x
 * @param y
 * @param width
 * @param height
 */
void
LayerExDraw::drawRectangle(const Appearance *app, REAL x, REAL y, REAL width, REAL height)
{
	GraphicsPath path;
	RectF rect(x, y, width, height);
	path.AddRectangle(rect);
	drawPath(app, &path);
}

/**
 * 文字列の描画
 * @param font フォント
 * @param app アピアランス
 * @param x 描画位置X
 * @param y 描画位置Y
 * @param text 描画テキスト
 */
void
LayerExDraw::drawString(const FontInfo *font, const Appearance *app, REAL x, REAL y, const tjs_char *text)
{
	// 文字列のパスを準備
	GraphicsPath path;
	path.AddString(text, -1, font->fontFamily, font->style, font->emSize, PointF(x, y), NULL);
	drawPath(app, &path);
}

/**
 * 画像の描画
 * @param name 画像名
 * @param x 表示位置X
 * @param y 表示位置Y
 */
void
LayerExDraw::drawImage(REAL x, REAL y, const tjs_char *name)
{
	// 画像読み込み
	IStream *in = TVPCreateIStream(name, TJS_BS_READ);
	if(!in) {
		TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + ttstr(name)).c_str());
	}
	
	try {
		// 画像生成
		Image image(in);
		int ret;
		if ((ret = image.GetLastStatus()) != Ok) {
			TVPThrowExceptionMessage((ttstr(TJS_W("cannot load : ")) + ttstr(name) + ttstr(L" : ") + ttstr(ret)).c_str());
		}
		// 描画処理
		graphics->DrawImage(&image, x, y);
	} catch (...) {
		in->Release();
		throw;
	}
	in->Release();
}
