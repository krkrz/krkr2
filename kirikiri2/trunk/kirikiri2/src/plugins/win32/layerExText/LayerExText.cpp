#pragma comment(lib, "gdiplus.lib")
#include "LayerExText.hpp"

#include <stdio.h>
/**
 * ログ出力用
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
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
FontInfo::addPrivateFont(const tjs_char *fontFileName)
{
	ttstr filename = TVPGetPlacedPath(fontFileName);
	if (filename.length()) {
		ttstr localname = filename;
		TVPGetLocalName(localname);

		if (!privateFontCollection) {
			privateFontCollection = new PrivateFontCollection();
		}
		if (localname.length()) {
			// 実ファイルが存在した場合
			privateFontCollection->AddFontFile(localname.c_str());
			return;
		} else {
			// メモリにロードして展開
			IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
			if (in) {
				STATSTG stat;
				in->Stat(&stat, STATFLAG_NONAME);
				// サイズあふれ無視注意
				ULONG size = stat.cbSize.QuadPart;
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
FontInfo::showPrivateFontList()
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
	clearFont();
}

/**
 * フォント情報のクリア
 */
void
FontInfo::clearFont()
{
	delete fontFamily;
	fontFamily = NULL;
}

/**
 * フォントの指定
 */
void
FontInfo::setFamilyName(const tjs_char *familyName)
{
	if (familyName) {
		clearFont();
		if (privateFontCollection) {
			fontFamily = new FontFamily(familyName, privateFontCollection);
			if (fontFamily->IsAvailable()) {
				return;
			} else {
				clearFont();
			}
		}
		fontFamily = new FontFamily(familyName);
		if (fontFamily->IsAvailable()) {
			return;
		} else {
			clearFont();
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
LayerExText::LayerExText(DispatchT obj)
	: layerExBase(obj), width(-1), height(-1), pitch(0), buffer(NULL), bitmap(NULL), graphics(NULL)
{
}

/**
 * デストラクタ
 */
LayerExText::~LayerExText()
{
	delete graphics;
	delete bitmap;
}

void
LayerExText::reset()
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
 * パスでの文字列の描画（ブラシ＋ペン）
 * @param text 描画テキスト
 * @param x 原点X
 * @param y 原点Y
 */
void
LayerExText::drawString(FontInfo *font, Appearance *app, REAL x, REAL y, const tjs_char *text, bool noupdate)
{
	// 文字列のパスを準備
	GraphicsPath path;
	path.AddString(text, -1, font->fontFamily, font->style, font->emSize, PointF(x, y), NULL);

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
					graphics->DrawPath(pen, &path);
					if (!noupdate) {
						path.GetBounds(&rect, &matrix, pen);
						unionRect.Union(unionRect, unionRect, rect);
					}
				}
				break;
			case 1:
				graphics->FillPath((Brush*)i->info, &path);
				if (!noupdate) {
					path.GetBounds(&rect, &matrix, NULL);
					unionRect.Union(unionRect, unionRect, rect);
				}
				break;
			}
		}
		i++;
	}

	if (!noupdate) {
		// 更新処理
		tTJSVariant  vars [4] = { unionRect.X, unionRect.Y, unionRect.Width, unionRect.Height };
		tTJSVariant *varsp[4] = { vars, vars+1, vars+2, vars+3 };
		_pUpdate(4, varsp);
	}
}
