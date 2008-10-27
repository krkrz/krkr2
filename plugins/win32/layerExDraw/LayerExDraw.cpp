#pragma comment(lib, "gdiplus.lib")
#include "ncbind/ncbind.hpp"
#include "LayerExDraw.hpp"
#include <stdio.h>

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

/**
 * 画像読み込み処理
 * @param name ファイル名
 * @return 画像情報
 */
Image *loadImage(const tjs_char *name)
{
	Image *image = NULL;
	ttstr filename = TVPGetPlacedPath(name);
	if (filename.length()) {
		if (!wcschr(filename.c_str(), '>')) {
			// 実ファイルが存在
			TVPGetLocalName(filename);
			image = Image::FromFile(filename.c_str(),false);
		} else {
			// 直接吉里吉里からもらったストリームを使うとなぜかwmf/emfでOutOfMemory
			// なる場合があるようなのでいったんメモリにメモリに展開してから使う
			IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
			if (in) {
				STATSTG stat;
				in->Stat(&stat, STATFLAG_NONAME);
				// サイズあふれ無視注意
				ULONG size = (ULONG)stat.cbSize.QuadPart;
				HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, size);
				if (hBuffer)	{
					void* pBuffer = ::GlobalLock(hBuffer);
					if (pBuffer) {
						if (in->Read(pBuffer, size, &size) == S_OK) {
							IStream* pStream = NULL;
							if(::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK) 	{
								image = Image::FromStream(pStream,false);
								pStream->Release();
							}
						}
						::GlobalUnlock(hBuffer);
					}
					::GlobalFree(hBuffer);
				}
				in->Release();
			}
		}
	}
	if (image && image->GetLastStatus() != Ok) {
		delete image;
		image = NULL;
	}
	if (!image) {
		TVPThrowExceptionMessage((ttstr(L"cannot load : ") + name).c_str());
	}
	return image;
}

RectF *getBounds(Image *image)
{
	RectF srcRect;
	Unit srcUnit;
	image->GetBounds(&srcRect, &srcUnit);
	REAL dpix = image->GetHorizontalResolution();
	REAL dpiy = image->GetVerticalResolution();

	// ピクセルに変換
	REAL x, y, width, height;
	switch (srcUnit) {
	case UnitPoint:		// 3 -- Each unit is a printer's point, or 1/72 inch.
		x = srcRect.X * dpix / 72;
		y = srcRect.Y * dpiy / 72;
		width  = srcRect.Width * dpix / 72;
		height = srcRect.Height * dpix / 72;
		break;
	case UnitInch:       // 4 -- Each unit is 1 inch.
		x = srcRect.X * dpix;
		y = srcRect.Y * dpiy;
		width  = srcRect.Width * dpix;
		height = srcRect.Height * dpix;
		break;
	case UnitDocument:   // 5 -- Each unit is 1/300 inch.
		x = srcRect.X * dpix / 300;
		y = srcRect.Y * dpiy / 300;
		width  = srcRect.Width * dpix / 300;
		height = srcRect.Height * dpix / 300;
		break;
	case UnitMillimeter: // 6 -- Each unit is 1 millimeter.
		x = srcRect.X * dpix / 25.4F;
		y = srcRect.Y * dpiy / 25.4F;
		width  = srcRect.Width * dpix / 25.4F;
		height = srcRect.Height * dpix / 25.4F;
		break;
	default:
		x = srcRect.X;
		y = srcRect.Y;
		width  = srcRect.Width;
		height = srcRect.Height;
		break;
	}
	return new RectF(x, y, width, height);
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
 * 配列にフォントのファミリー名を格納
 * @param array 格納先配列
 * @param fontCollection フォント名を取得する元の FontCollection
 */
static void addFontFamilyName(iTJSDispatch2 *array, FontCollection *fontCollection)
{
	int count = fontCollection->GetFamilyCount();
	FontFamily *families = new FontFamily[count];
	if (families) {
		fontCollection->GetFamilies(count, families, &count);
		for (int i=0;i<count;i++) {
			WCHAR familyName[LF_FACESIZE];
			if (families[i].GetFamilyName(familyName) == Ok) {
				tTJSVariant name(familyName), *param = &name;
				array->FuncCall(0, TJS_W("add"), NULL, 0, 1, &param, array);
			}
		}
		delete families;
	}
}

/**
 * フォント一覧の取得
 * @param privateOnly true ならプライベートフォントのみ取得
 */
tTJSVariant
GdiPlus::getFontList(bool privateOnly)
{
	iTJSDispatch2 *array = TJSCreateArrayObject();
	if (privateFontCollection)	{
		addFontFamilyName(array, privateFontCollection);
	}
	if (!privateOnly) {
		InstalledFontCollection installedFontCollection;
		addFontFamilyName(array, &installedFontCollection);
	}
	tTJSVariant ret(array,array);
	array->Release();
	return ret;
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

// --------------------------------------------------------
// 各型変換処理
// --------------------------------------------------------

extern bool IsArray(const tTJSVariant &var);

/**
 * 座標情報の生成
 */
extern PointF getPoint(const tTJSVariant &var);

/**
 * 点の配列を取得
 */
static void getPoints(const tTJSVariant &var, vector<PointF> &points)
{
	ncbPropAccessor info(var);
	int c = info.GetArrayCount();
	for (int i=0;i<c;i++) {
		tTJSVariant p;
		if (info.checkVariant(i, p)) {
			points.push_back(getPoint(p));
		}
	}
}

static void getPoints(ncbPropAccessor &info, int n, vector<PointF> &points)
{
	tTJSVariant var;
	if (info.checkVariant(n, var)) {
		getPoints(var, points);
	}
}

static void getPoints(ncbPropAccessor &info, const tjs_char *n, vector<PointF> &points)
{
	tTJSVariant var;
	if (info.checkVariant(n, var)) {
		getPoints(var, points);
	}
}

// -----------------------------

/**
 * 矩形情報の生成
 */
extern RectF getRect(const tTJSVariant &var);

/**
 * 矩形の配列を取得
 */
static void getRects(const tTJSVariant &var, vector<RectF> &rects)
{
	ncbPropAccessor info(var);
	int c = info.GetArrayCount();
	for (int i=0;i<c;i++) {
		tTJSVariant p;
		if (info.checkVariant(i, p)) {
			rects.push_back(getRect(p));
		}
	}
}

// -----------------------------

/**
 * 実数の配列を取得
 */
static void getReals(const tTJSVariant &var, vector<REAL> &points)
{
	ncbPropAccessor info(var);
	int c = info.GetArrayCount();
	for (int i=0;i<c;i++) {
		points.push_back((REAL)info.getRealValue(i));
	}
}

static void getReals(ncbPropAccessor &info, int n, vector<REAL> &points)
{
	tTJSVariant var;
	if (info.checkVariant(n, var)) {
		getReals(var, points);
	}
}

static void getReals(ncbPropAccessor &info, const tjs_char *n, vector<REAL> &points)
{
	tTJSVariant var;
	if (info.checkVariant(n, var)) {
		getReals(var, points);
	}
}

// -----------------------------

/**
 * 色の配列を取得
 */
static void getColors(const tTJSVariant &var, vector<Color> &colors)
{
	ncbPropAccessor info(var);
	int c = info.GetArrayCount();
	for (int i=0;i<c;i++) {
		colors.push_back(Color((ARGB)info.getIntValue(i)));
	}
}

static void getColors(ncbPropAccessor &info, int n, vector<Color> &colors)
{
	tTJSVariant var;
	if (info.checkVariant(n, var)) {
		getColors(var, colors);
	}
}

static void getColors(ncbPropAccessor &info, const tjs_char *n, vector<Color> &colors)
{
	tTJSVariant var;
	if (info.checkVariant(n, var)) {
		getColors(var, colors);
	}
}

template <class T>
void commonBrushParameter(ncbPropAccessor &info, T *brush)
{
	tTJSVariant var;
	// SetBlend
	if (info.checkVariant(L"blend", var)) {
		vector<REAL> factors;
		vector<REAL> positions;
		ncbPropAccessor binfo(var);
		if (IsArray(var)) {
			getReals(binfo, 0, factors);
			getReals(binfo, 1, positions);
		} else {
			getReals(binfo, L"blendFactors", factors);
			getReals(binfo, L"blendPositions", positions);
		}
		int count = (int)factors.size();
		if ((int)positions.size() > count) {
			count = (int)positions.size();
		}
		if (count > 0) {
			brush->SetBlend(&factors[0], &positions[0], count);
		}
	}
	// SetBlendBellShape
	if (info.checkVariant(L"blendBellShape", var)) {
		ncbPropAccessor sinfo(var);
		if (IsArray(var)) {
			brush->SetBlendBellShape((REAL)sinfo.getRealValue(0),
									 (REAL)sinfo.getRealValue(1));
		} else {
			brush->SetBlendBellShape((REAL)info.getRealValue(L"focus"),
									 (REAL)info.getRealValue(L"scale"));
		}
	}
	// SetBlendTriangularShape
	if (info.checkVariant(L"blendTriangularShape", var)) {
		ncbPropAccessor sinfo(var);
		if (IsArray(var)) {
			brush->SetBlendTriangularShape((REAL)sinfo.getRealValue(0),
										   (REAL)sinfo.getRealValue(1));
		} else {
			brush->SetBlendTriangularShape((REAL)info.getRealValue(L"focus"),
										   (REAL)info.getRealValue(L"scale"));
		}
	}
	// SetGammaCorrection
	if (info.checkVariant(L"useGammaCorrection", var)) {
		brush->SetGammaCorrection((BOOL)var);
	}
	// SetInterpolationColors
	if (info.checkVariant(L"interpolationColors", var)) {
		vector<Color> colors;
		vector<REAL> positions;
		ncbPropAccessor binfo(var);
		if (IsArray(var)) {
			getColors(binfo, 0, colors);
			getReals(binfo, 1, positions);
		} else {
			getColors(binfo, L"presetColors", colors);
			getReals(binfo, L"blendPositions", positions);
		}
		int count = (int)colors.size();
		if ((int)positions.size() > count) {
			count = (int)positions.size();
		}
		if (count > 0) {
			brush->SetInterpolationColors(&colors[0], &positions[0], count);
		}
	}
}

/**
 * ブラシの生成
 */
Brush* createBrush(const tTJSVariant colorOrBrush)
{
	Brush *brush;
	if (colorOrBrush.Type() != tvtObject) {
		brush = new SolidBrush(Color((tjs_int)colorOrBrush));
	} else {
		// 種別ごとに作り分ける
		ncbPropAccessor info(colorOrBrush);
		BrushType type = (BrushType)info.getIntValue(L"type", BrushTypeSolidColor);
		switch (type) {
		case BrushTypeSolidColor:
			brush = new SolidBrush(Color((ARGB)info.getIntValue(L"color", 0xffffffff)));
			break;
		case BrushTypeHatchFill:
			brush = new HatchBrush((HatchStyle)info.getIntValue(L"hatchStyle", HatchStyleHorizontal),
								   Color((ARGB)info.getIntValue(L"foreColor", 0xffffffff)),
								   Color((ARGB)info.getIntValue(L"backColor", 0xff000000)));
			break;
		case BrushTypeTextureFill:
			{
				ttstr imgname = info.GetValue(L"image", ncbTypedefs::Tag<ttstr>());
				Image *image = loadImage(imgname.c_str());
				WrapMode wrapMode = (WrapMode)info.getIntValue(L"wrapMode", WrapModeTile);
				tTJSVariant dstRect;
				if (info.checkVariant(L"dstRect", dstRect)) {
					brush = new TextureBrush(image, wrapMode, getRect(dstRect));
				} else {
					brush = new TextureBrush(image, wrapMode);
				}
				delete image;
				break;
			}
		case BrushTypePathGradient:
			{
				PathGradientBrush *pbrush;
				vector<PointF> points;
				getPoints(info, L"points", points);
				WrapMode wrapMode = (WrapMode)info.getIntValue(L"wrapMode", WrapModeTile);
				pbrush = new PathGradientBrush(&points[0], (int)points.size(), wrapMode);

				// 共通パラメータ
				commonBrushParameter(info, pbrush);

				tTJSVariant var;
				//SetCenterColor
				if (info.checkVariant(L"centerColor", var)) {
					pbrush->SetCenterColor(Color((ARGB)(tjs_int)var));
				}
				//SetCenterPoint
				if (info.checkVariant(L"centerPoint", var)) {
					pbrush->SetCenterPoint(getPoint(var));
				}
				//SetFocusScales
				if (info.checkVariant(L"focusScales", var)) {
					ncbPropAccessor sinfo(var);
					if (IsArray(var)) {
						pbrush->SetFocusScales((REAL)sinfo.getRealValue(0),
											   (REAL)sinfo.getRealValue(1));
					} else {
						pbrush->SetFocusScales((REAL)info.getRealValue(L"xScale"),
											   (REAL)info.getRealValue(L"yScale"));
					}
				}
				//SetSurroundColors
				if (info.checkVariant(L"interpolationColors", var)) {
					vector<Color> colors;
					getColors(var, colors);
					int size = (int)colors.size();
					pbrush->SetSurroundColors(&colors[0], &size);
				}
				brush = pbrush;
			}
			break;
		case BrushTypeLinearGradient:
			{
				LinearGradientBrush *lbrush;
				Color color1((ARGB)info.getIntValue(L"color1", 0));
				Color color2((ARGB)info.getIntValue(L"color2", 0));

				tTJSVariant var;
				if (info.checkVariant(L"point1", var)) {
					PointF point1 = getPoint(var);
					info.checkVariant(L"point2", var);
					PointF point2 = getPoint(var);
					lbrush = new LinearGradientBrush(point1, point2, color1, color2);
				} else if (info.checkVariant(L"rect", var)) {
					RectF rect = getRect(var);
					if (info.HasValue(L"angle")) {
						// アングル指定がある場合
						lbrush = new LinearGradientBrush(rect, color1, color2,
														 (REAL)info.getRealValue(L"angle", 0),
														 (BOOL)info.getIntValue(L"isAngleScalable", 0));
					} else {
						// 無い場合はモードを参照
						lbrush = new LinearGradientBrush(rect, color1, color2,
														 (LinearGradientMode)info.getIntValue(L"mode", LinearGradientModeHorizontal));
					}
				} else {
					TVPThrowExceptionMessage(L"must set point1,2 or rect");
				}

				// 共通パラメータ
				commonBrushParameter(info, lbrush);

				// SetWrapMode
				if (info.checkVariant(L"wrapMode", var)) {
					lbrush->SetWrapMode((WrapMode)(tjs_int)var);
				}
				brush = lbrush;
			}
			break;
		default:
			TVPThrowExceptionMessage(L"invalid brush type");
			break;
		}
	}
	return brush;
}

/**
 * ブラシの追加
 * @param colorOrBrush ARGB色指定またはブラシ情報（辞書）
 * @param ox 表示オフセットX
 * @param oy 表示オフセットY
 */
void
Appearance::addBrush(tTJSVariant colorOrBrush, REAL ox, REAL oy)
{
	drawInfos.push_back(DrawInfo(ox, oy, createBrush(colorOrBrush)));
}

/**
 * ペンの追加
 * @param colorOrBrush ARGB色指定またはブラシ情報（辞書）
 * @param widthOrOption ペン幅またはペン情報（辞書）
 * @param ox 表示オフセットX
 * @param oy 表示オフセットY
 */
void
Appearance::addPen(tTJSVariant colorOrBrush, tTJSVariant widthOrOption, REAL ox, REAL oy)
{
	Pen *pen;
	REAL width = 1.0;
	if (colorOrBrush.Type() == tvtObject) {
		Brush *brush = createBrush(colorOrBrush);
		pen = new Pen(brush, width);
		delete brush;
	} else {
		pen = new Pen(Color((ARGB)(tjs_int)colorOrBrush), width);
	}
	if (widthOrOption.Type() != tvtObject) {
		pen->SetWidth((REAL)(tjs_real)widthOrOption);
	} else {
		ncbPropAccessor info(widthOrOption);
		
		tTJSVariant var;

		// SetWidth
		if (info.checkVariant(L"width", var)) {
			pen->SetWidth((REAL)(tjs_real)var);
		}
		
		// SetAlignment
		if (info.checkVariant(L"alignment", var)) {
			pen->SetAlignment((PenAlignment)(tjs_int)var);
		}
		// SetCompoundArray
		if (info.checkVariant(L"compoundArray", var)) {
			vector<REAL> reals;
			getReals(var, reals);
			pen->SetCompoundArray(&reals[0], (int)reals.size());
		}

		// SetDashCap
		if (info.checkVariant(L"dashCap", var)) {
			pen->SetDashCap((DashCap)(tjs_int)var);
		}
		// SetDashOffset
		if (info.checkVariant(L"dashOffset", var)) {
			pen->SetDashOffset((REAL)(tjs_real)var);
		}

		// SetDashStyle
		// SetDashPattern
		if (info.checkVariant(L"dashStyle", var)) {
			if (IsArray(var)) {
				vector<REAL> reals;
				getReals(var, reals);
				pen->SetDashStyle(DashStyleCustom);
				pen->SetDashPattern(&reals[0], (int)reals.size());
			} else {
				pen->SetDashStyle((DashStyle)(tjs_int)var);
			}
		}

		// SetStartCap
		// SetCustomStartCap XXX
		if (info.checkVariant(L"startCap", var)) {
			pen->SetStartCap((LineCap)(tjs_int)var);
		}

		// SetEndCap
		// SetCustomEndCap XXX
		if (info.checkVariant(L"endCap", var)) {
			pen->SetEndCap((LineCap)(tjs_int)var);
		}

		// SetLineJoin
		if (info.checkVariant(L"lineJoin", var)) {
			pen->SetLineJoin((LineJoin)(tjs_int)var);
		}
		
		// SetMiterLimit
		if (info.checkVariant(L"miterLimit", var)) {
			pen->SetMiterLimit((REAL)(tjs_real)var);
		}
	}
	drawInfos.push_back(DrawInfo(ox, oy, pen));
}

// --------------------------------------------------------
// フォント描画系
// --------------------------------------------------------

void
LayerExDraw::updateRect(RectF &rect)
{
	if (updateWhenDraw) {
		// 更新処理
		tTJSVariant  vars [4] = { rect.X, rect.Y, rect.Width, rect.Height };
		tTJSVariant *varsp[4] = { vars, vars+1, vars+2, vars+3 };
		_pUpdate(4, varsp);
	}
}

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
		graphics->SetPageUnit(UnitPixel);
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
 * パスの領域情報を取得
 * @param app 表示表現
 * @param path 描画するパス
 */
RectF
LayerExDraw::getPathExtents(const Appearance *app, const GraphicsPath *path)
{
	// 領域記録用
	RectF rect;

	// 描画情報を使って次々描画
	bool first = true;
	vector<Appearance::DrawInfo>::const_iterator i = app->drawInfos.begin();
	while (i != app->drawInfos.end()) {
		if (i->info) {
			Matrix matrix(1,0,0,1,i->ox,i->oy);
			graphics->SetTransform(&matrix);
			switch (i->type) {
			case 0:
				{
					Pen *pen = (Pen*)i->info;
					if (first) {
						path->GetBounds(&rect, &matrix, pen);
						first = false;
					} else {
						RectF r;
						path->GetBounds(&r, &matrix, pen);
						rect.Union(rect, rect, r);
					}
				}
				break;
			case 1:
				if (first) {
					path->GetBounds(&rect, &matrix, NULL);
					first = false;
				} else {
					RectF r;
					path->GetBounds(&r, &matrix, NULL);
					rect.Union(rect, rect, r);
				}
				break;
			}
		}
		i++;
	}
	return rect;
}

/**
 * パスを描画する
 * @param app 表示表現
 * @param path 描画するパス
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawPath(const Appearance *app, const GraphicsPath *path)
{
	// 領域記録用
	RectF rect;

	// 描画情報を使って次々描画
	bool first = true;
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
					if (first) {
						path->GetBounds(&rect, &matrix, pen);
						first = false;
					} else {
						RectF r;
						path->GetBounds(&r, &matrix, pen);
						rect.Union(rect, rect, r);
					}
				}
				break;
			case 1:
				graphics->FillPath((Brush*)i->info, path);
				if (first) {
					path->GetBounds(&rect, &matrix, NULL);
					first = false;
				} else {
					RectF r;
					path->GetBounds(&r, &matrix, NULL);
					rect.Union(rect, rect, r);
				}
				break;
			}
		}
		i++;
	}
	updateRect(rect);
	return rect;
}

/**
 * 円弧の描画
 * @param x 左上座標
 * @param y 左上座標
 * @param width 横幅
 * @param height 縦幅
 * @param startAngle 時計方向円弧開始位置
 * @param sweepAngle 描画角度
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawArc(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle)
{
	GraphicsPath path;
	path.AddArc(x, y, width, height, startAngle, sweepAngle);
	return drawPath(app, &path);
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
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawBezier(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4)
{
	GraphicsPath path;
	path.AddBezier(x1, y1, x2, y2, x3, y3, x4, y4);
	return drawPath(app, &path);
}

/**
 * 連続ベジェ曲線の描画
 * @param app アピアランス
 * @param points 点の配列
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawBeziers(const Appearance *app, tTJSVariant points)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddBeziers(&ps[0], (int)ps.size());
	return drawPath(app, &path);
}

/**
 * Closed cardinal spline の描画
 * @param app アピアランス
 * @param points 点の配列
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawClosedCurve(const Appearance *app, tTJSVariant points)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddClosedCurve(&ps[0], (int)ps.size());
	return drawPath(app, &path);
}

/**
 * Closed cardinal spline の描画
 * @param app アピアランス
 * @param points 点の配列
 * @pram tension tension
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawClosedCurve2(const Appearance *app, tTJSVariant points, REAL tension)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddClosedCurve(&ps[0], (int)ps.size(), tension);
	return drawPath(app, &path);
}

/**
 * cardinal spline の描画
 * @param app アピアランス
 * @param points 点の配列
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawCurve(const Appearance *app, tTJSVariant points)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddCurve(&ps[0], (int)ps.size());
	return drawPath(app, &path);
}

/**
 * cardinal spline の描画
 * @param app アピアランス
 * @param points 点の配列
 * @parma tension tension
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawCurve2(const Appearance *app, tTJSVariant points, REAL tension)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddCurve(&ps[0], (int)ps.size(), tension);
	return drawPath(app, &path);
}

/**
 * cardinal spline の描画
 * @param app アピアランス
 * @param points 点の配列
 * @param offset
 * @param numberOfSegments
 * @param tension tension
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawCurve3(const Appearance *app, tTJSVariant points, int offset, int numberOfSegments, REAL tension)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddCurve(&ps[0], (int)ps.size(), offset, numberOfSegments, tension);
	return drawPath(app, &path);
}

/**
 * 円錐の描画
 * @param x 左上座標
 * @param y 左上座標
 * @param width 横幅
 * @param height 縦幅
 * @param startAngle 時計方向円弧開始位置
 * @param sweepAngle 描画角度
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawPie(const Appearance *app, REAL x, REAL y, REAL width, REAL height, REAL startAngle, REAL sweepAngle)
{
	GraphicsPath path;
	path.AddPie(x, y, width, height, startAngle, sweepAngle);
	return drawPath(app, &path);
}

/**
 * 楕円の描画
 * @param app アピアランス
 * @param x
 * @param y
 * @param width
 * @param height
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawEllipse(const Appearance *app, REAL x, REAL y, REAL width, REAL height)
{
	GraphicsPath path;
	path.AddEllipse(x, y, width, height);
	return drawPath(app, &path);
}

/**
 * 線分の描画
 * @param app アピアランス
 * @param x1 始点X座標
 * @param y1 始点Y座標
 * @param x2 終点X座標
 * @param y2 終点Y座標
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawLine(const Appearance *app, REAL x1, REAL y1, REAL x2, REAL y2)
{
	GraphicsPath path;
	path.AddLine(x1, y1, x2, y2);
	return drawPath(app, &path);
}

/**
 * 連続線分の描画
 * @param app アピアランス
 * @param points 点の配列
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawLines(const Appearance *app, tTJSVariant points)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddLines(&ps[0], (int)ps.size());
	return drawPath(app, &path);
}

/**
 * 多角形の描画
 * @param app アピアランス
 * @param points 点の配列
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawPolygon(const Appearance *app, tTJSVariant points)
{
	vector<PointF> ps;
	getPoints(points, ps);
	GraphicsPath path;
	path.AddPolygon(&ps[0], (int)ps.size());
	return drawPath(app, &path);
}


/**
 * 矩形の描画
 * @param app アピアランス
 * @param x
 * @param y
 * @param width
 * @param height
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawRectangle(const Appearance *app, REAL x, REAL y, REAL width, REAL height)
{
	GraphicsPath path;
	RectF rect(x, y, width, height);
	path.AddRectangle(rect);
	return drawPath(app, &path);
}

/**
 * 複数矩形の描画
 * @param app アピアランス
 * @param rects 矩形情報の配列
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawRectangles(const Appearance *app, tTJSVariant rects)
{
	vector<RectF> rs;
	getRects(rects, rs);
	GraphicsPath path;
	path.AddRectangles(&rs[0], (int)rs.size());
	return drawPath(app, &path);
}

/**
 * 文字列の描画
 * @param font フォント
 * @param app アピアランス
 * @param x 描画位置X
 * @param y 描画位置Y
 * @param text 描画テキスト
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawString(const FontInfo *font, const Appearance *app, REAL x, REAL y, const tjs_char *text)
{
	// 文字列のパスを準備
	GraphicsPath path;
	path.AddString(text, -1, font->fontFamily, font->style, font->emSize, PointF(x, y), StringFormat::GenericDefault());
	return drawPath(app, &path);
}


/**
 * 文字列の描画領域情報の取得
 * @param font フォント
 * @param app アピアランス
 * @param text 描画テキスト
 * @return 描画領域情報
 */
RectF
LayerExDraw::measureString(const FontInfo *font, const tjs_char *text)
{
	RectF rect;
	Font f(font->fontFamily, font->emSize, font->style, UnitPixel);
	graphics->MeasureString(text, -1, &f, PointF(0,0), StringFormat::GenericDefault(), &rect);
	return rect;
}


/**
 * 画像の描画。コピー先は元画像の Bounds を配慮した位置、サイズは Pixel 指定になります。
 * @param x コピー先原点
 * @param y  コピー先原点
 * @param src コピー元画像
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawImage(REAL x, REAL y, Image *src) 
{
	RectF rect;
	if (src) {
		RectF *bounds = getBounds(src);
		graphics->DrawImage(src, (rect.X = x + bounds->X), (rect.Y = y + bounds->Y), bounds->Width, bounds->Height);
		rect.Width  = bounds->Width;
		rect.Height = bounds->Height;
		updateRect(rect);
		delete bounds;
	}
	return rect;
}


/**
 * 画像の矩形コピー
 * @param dleft コピー先左端
 * @param dtop  コピー先上端
 * @param src コピー元画像
 * @param sleft 元矩形の左端
 * @param stop  元矩形の上端
 * @param swidth 元矩形の横幅
 * @param sheight  元矩形の縦幅
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawImageRect(REAL dleft, REAL dtop, Image *src, REAL sleft, REAL stop, REAL swidth, REAL sheight)
{
	return drawImageStretch(dleft, dtop, swidth , sheight, src, sleft, stop, swidth, sheight);
}

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
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawImageStretch(REAL dleft, REAL dtop, REAL dwidth, REAL dheight, Image *src, REAL sleft, REAL stop, REAL swidth, REAL sheight)
{
	RectF destRect(dleft, dtop, dwidth, dheight);
	if (src) {
		graphics->DrawImage(src, destRect, sleft, stop, swidth, sheight, UnitPixel);
	}
	updateRect(destRect);
	return destRect;
}

static int compare_REAL(const REAL *a, const REAL *b)
{
	return (int)(*a - *b);
}

/**
 * 画像のアフィン変換コピー
 * @param sleft 元矩形の左端
 * @param stop  元矩形の上端
 * @param swidth 元矩形の横幅
 * @param sheight  元矩形の縦幅
 * @param affine アフィンパラメータの種類(true:変換行列, false:座標指定), 
 * @return 更新領域情報
 */
RectF
LayerExDraw::drawImageAffine(Image *src, REAL sleft, REAL stop, REAL swidth, REAL sheight, bool affine, REAL A, REAL B, REAL C, REAL D, REAL E, REAL F)
{
	RectF rect;
	RectF srcRect(sleft, stop, swidth, sheight);
	REAL x[4], y[4]; // 元座標値
	if (affine) {
		REAL x2 = sleft+swidth;
		REAL y2 = stop +sheight;
#define AFFINEX(x,y) A*x+C*y+E
#define AFFINEY(x,y) B*x+D*y+F
		x[0] = AFFINEX(sleft,stop);
		y[0] = AFFINEY(sleft,stop);
		x[1] = AFFINEX(x2,stop);
		y[1] = AFFINEY(x2,stop);
		x[2] = AFFINEX(sleft,y2);
		y[2] = AFFINEY(sleft,y2);
		x[3] = AFFINEX(x2,y2);
		y[3] = AFFINEY(x2,y2);
	} else {
		x[0] = A;
		y[0] = B;
		x[1] = C;
		y[1] = D;
		x[2] = E;
		y[2] = F;
		x[3] = C-A+E;
		y[3] = D-B+F;
	}
	PointF dests[3] = { PointF(x[0],y[0]), PointF(x[1],y[1]), PointF(x[2],y[2]) };
	if (src) {
		graphics->DrawImage(src, dests, 3, sleft, stop, swidth, sheight, UnitPixel, NULL, NULL, NULL);
	}
	qsort(x, 4, sizeof(REAL), (int (*)(const void*, const void*))compare_REAL);
	qsort(y, 4, sizeof(REAL), (int (*)(const void*, const void*))compare_REAL);
	rect.X = x[0];
	rect.Y = y[0];
	rect.Width = x[3]-x[0];
	rect.Height = y[3]-y[0];
	updateRect(rect);
	return rect;
}
