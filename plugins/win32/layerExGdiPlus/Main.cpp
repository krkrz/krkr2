#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;
#include <vector>
using namespace std;
#include <stdio.h>

#include "tp_stub.h"

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

// GDI 基本情報
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

#include "LayerExBase.h"

//---------------------------------------------------------------------------

#define TJS_NATIVE_CLASSID_NAME ClassID_Brush
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;

/**
 * GDI+ ブラシオブジェクト
 */
class NI_Brush : public tTJSNativeInstance // ネイティブインスタンス
{
protected:
	Brush *brush;
	
	/**
	 * ブラシの消去
	 */
	void clearBrush() {
		delete brush;
		brush = NULL;
	}
	
public:
	/**
	 * 固定色ブラシの追加
	 * @param argb 色指定
	 */
	void setSolidBrush(ARGB argb) {
		clearBrush();
		brush = new SolidBrush(Color(argb));
	}

	/**
	 * 行グラデーションブラシの追加
	 */
	void setLinearGradientBrush(double x1, double y1, double x2, double y2, ARGB col1, ARGB col2) {
		clearBrush();
		brush = new LinearGradientBrush(PointF(x1,y1), PointF(x2, y2), Color(col1), Color(col2));
	}

	/**
	 * パスグラデーションブラシの追加
	 *
	 */
	void setPathGradientBrush(int numparams, tTJSVariant **param) {

		clearBrush();
		
		// 点の数
		int n = numparams / 2;
		
		// モード
		WrapMode mode = WrapModeClamp;
		if (n * 2 < numparams) {
			mode = (WrapMode)(int)*param[numparams - 1];
		}
		
		Point *points = new Point[n];
		for (int i=0;i<n;i++) {
			points[i].X = (int)*param[i*2];
			points[i].Y = (int)*param[i*2+1];
		}
		
		brush = new PathGradientBrush(points, n, mode);
		delete[] points;
	}
	
public:
	/**
	 * コンストラクタ
	 */
	NI_Brush() {
		brush = NULL;
	}

	/**
	 * TJS コンストラクタ
	 * @param numparams パラメータ数
	 * @param param
	 * @param tjs_obj this オブジェクト
	 */
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj) {
		if (numparams > 0) {
			setSolidBrush(param[0]->AsInteger());
		}
		return S_OK;
	}

	/**
	 * TJS invalidate
	 */
	void TJS_INTF_METHOD Invalidate() {
		clearBrush();
	}
	
	/**
	 * @param objthis オブジェクト
	 * @return GDI+ 用 Brush インスタンス。取得失敗したら NULL
	 */
	static Brush *getBrush(iTJSDispatch2 *objthis) {
		if (!objthis) return NULL;
		NI_Brush *_this;
		if (TJS_SUCCEEDED(objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
														 TJS_NATIVE_CLASSID_NAME, (iTJSNativeInstance**)&_this))) {
			return _this->brush;
		}
		return NULL;
	}

};

static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Brush()
{
	return new NI_Brush();
}

static iTJSDispatch2 * Create_NC_Brush()
{
	tTJSNativeClassForPlugin * classobj = TJSCreateNativeClassForPlugin(TJS_W("Brush"), Create_NI_Brush);

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Brush)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Brush,
			/*TJS class name*/Brush)
		{
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Brush)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setSolidBrush)
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
				/*var. type*/NI_Brush);
			
			if (numparams > 0) {
				_this->setSolidBrush(param[0]->AsInteger());
			}

			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setSolidBrush)

	TJS_END_NATIVE_MEMBERS

	/*
		この関数は classobj を返します。
	*/
	return classobj;
}

#undef TJS_NATIVE_CLASSID_NAME

//---------------------------------------------------------------------------

#define TJS_NATIVE_CLASSID_NAME ClassID_Pen
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;

/**
 * GDI+ ペンオブジェクト
 */
class NI_Pen : public tTJSNativeInstance // ネイティブインスタンス
{
protected:
	Pen *pen;

	/**
	 * ペンの消去
	 */
	void clearPen() {
		delete pen;
		pen = NULL;
	}

	/**
	 * ブラシペンの追加
	 * @param brush ブラシ
	 * @param width ペン幅
	 */
	void setBrushPen(const Brush *brush, REAL width=1.0) {
		clearPen();
		pen = new Pen(brush, width);
	}

	/**
	 * 固定色ペンの追加
	 * @param argb 色指定
	 * @param width ペン幅
	 */
	void setColorPen(ARGB argb, REAL width=1.0) {
		clearPen();
		pen = new Pen(Color(argb), width);
	}

	
public:
	/**
	 * コンストラクタ
	 */
	NI_Pen() {
		pen = NULL;
	}

	/**
	 * TJS コンストラクタ
	 * @param numparams パラメータ数
	 * @param param
	 * @param tjs_obj this オブジェクト
	 */
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj) {

		// 線の太さの指定がある場合
		if (numparams > 1) {
			const Brush *brush = NI_Brush::getBrush(param[0]->AsObjectNoAddRef());
			if (brush) {
				setBrushPen(brush, param[1]->AsReal());
			} else {
				setColorPen(param[0]->AsInteger(), param[1]->AsReal());
			}
		} else if (numparams > 0) {
			const Brush *brush = NI_Brush::getBrush(param[0]->AsObjectNoAddRef());
			if (brush) {
				setBrushPen(brush);
			} else {
				setColorPen(param[0]->AsInteger());
			}
		} else {
			pen = new Pen(Color(0));
		}
		return S_OK;
	}

	/**
	 * TJS invalidate
	 */
	void TJS_INTF_METHOD Invalidate() {
		clearPen();
	}
	
	/**
	 * @param objthis オブジェクト
	 * @return GDI+ 用 Penインスタンス。取得失敗したら NULL
	 */
	static Pen *getPen(iTJSDispatch2 *objthis) {
		if (!objthis) return NULL;
		NI_Pen *_this;
		if (TJS_SUCCEEDED(objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
														 TJS_NATIVE_CLASSID_NAME, (iTJSNativeInstance**)&_this))) {
			return _this->pen;
		}
		return NULL;
	}
	
};

static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Pen()
{
	return new NI_Pen();
}

static iTJSDispatch2 * Create_NC_Pen()
{
	tTJSNativeClassForPlugin * classobj = TJSCreateNativeClassForPlugin(TJS_W("Pen"), Create_NI_Pen);

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Pen)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Pen,
			/*TJS class name*/Pen)
		{
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Pen)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setAlignment) 
		{
			Pen *pen;
			if (!objthis || !(pen = NI_Pen::getPen(objthis))) {
				return TJS_E_NATIVECLASSCRASH;
			}
			if (numparams < 1) {
				return TJS_E_BADPARAMCOUNT;
			}
			pen->SetAlignment((PenAlignment)param[0]->AsInteger());
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setAlignment)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setBrush) 
		{
			Pen *pen;
			if (!objthis || !(pen = NI_Pen::getPen(objthis))) {
				return TJS_E_NATIVECLASSCRASH;
			}
			if (numparams < 1) {
				return TJS_E_BADPARAMCOUNT;
			}

			Brush *brush = NI_Brush::getBrush(param[0]->AsObjectNoAddRef());
			if (brush) {
				pen->SetBrush(brush);
			} else {
				TVPThrowExceptionMessage(L"第一引数はブラシである必要があります");
			}
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setBrush)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setColor) 
		{
			Pen *pen;
			if (!objthis || !(pen = NI_Pen::getPen(objthis))) {
				return TJS_E_NATIVECLASSCRASH;
			}
			if (numparams < 1) {
				return TJS_E_BADPARAMCOUNT;
			}
			pen->SetColor(Color(param[0]->AsInteger()));
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setColor)
		
		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setWidth) 
		{
			Pen *pen;
			if (!objthis || !(pen = NI_Pen::getPen(objthis))) {
				return TJS_E_NATIVECLASSCRASH;
			}
			if (numparams < 1) {
				return TJS_E_BADPARAMCOUNT;
			}
			pen->SetWidth(param[0]->AsReal());
			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/setWidth)

	TJS_END_NATIVE_MEMBERS

	/*
		この関数は classobj を返します。
	*/
	return classobj;
}

#undef TJS_NATIVE_CLASSID_NAME

//---------------------------------------------------------------------------

#define TJS_NATIVE_CLASSID_NAME ClassID_Font
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;

/**
 * GDI+ フォントオブジェクト
 */
class NI_Font : public tTJSNativeInstance // ネイティブインスタンス
{
	/// フォント保持用
	Font *font;

	void clearFont() {
		delete font;
		font = NULL;
	}

public:
	/**
	 * フォント設定
	 * @param familyName フォントファミリー名
	 * @param emSize サイズ指定(pixcel単位)
	 * @param style フォントスタイル
	 */
	void setFont(const tjs_char *familyName, REAL emSize=12, INT style=FontStyleRegular) {
		if (familyName) {
			FontFamily  fontFamily(familyName);
			font = new Font(&fontFamily, emSize, FontStyleRegular, UnitPixel);
		}
	}
	
public:
	/**
	 * コンストラクタ
	 */
	NI_Font() {
		font = NULL;
	}

	/**
	 * TJS コンストラクタ
	 * @param numparams パラメータ数
	 * @param param
	 * @param tjs_obj this オブジェクト
	 */
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj) {
		if (numparams > 2) {
			setFont(param[0]->GetString(), param[1]->AsReal(), param[2]->AsInteger());
		} else if (numparams > 1) {
			setFont(param[0]->GetString(), param[1]->AsReal());
		} else if (numparams > 0) {
			setFont(param[0]->GetString());
		}
		return S_OK;
	}

	/**
	 * TJS invalidate
	 */
	void TJS_INTF_METHOD Invalidate() {
		clearFont();
	}
	
	/**
	 * @param objthis オブジェクト
	 * @return GDI+ 用 Font インスタンス。取得失敗したら NULL
	 */
	static Font *getFont(iTJSDispatch2 *objthis) {
		if (!objthis) return NULL;
		NI_Font *_this;
		if (TJS_SUCCEEDED(objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
														 TJS_NATIVE_CLASSID_NAME, (iTJSNativeInstance**)&_this))) {
			return _this->font;
		}
		return NULL;
	}
	
};

static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Font()
{
	return new NI_Font();
}

static iTJSDispatch2 * Create_NC_Font()
{
	tTJSNativeClassForPlugin * classobj = TJSCreateNativeClassForPlugin(TJS_W("Font"), Create_NI_Font);

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Font)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Font,
			/*TJS class name*/Font)
		{
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Font)

		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/print) // print メソッド
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
				/*var. type*/NI_Font);

//			_this->Print();

			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/print)

	TJS_END_NATIVE_MEMBERS

	/*
		この関数は classobj を返します。
	*/
	return classobj;
}

#undef TJS_NATIVE_CLASSID_NAME

//---------------------------------------------------------------------------

/*
 * レイヤに直結した GDI+ 情報を保持するためのネイティブインスタンス
 */
class NI_GdiPlusInfo : public tTJSNativeInstance
{
protected:

	/// レイヤを参照するビットマップ
	Bitmap * _bitmap;
	/// レイヤに対して描画するコンテキスト
	Graphics * _graphics;

	// レイヤ情報比較保持用
	tjs_int _width;
	tjs_int _height;
	tjs_int _pitch;
	unsigned char * _buffer;

public:

	/**
	 * GDI+ 用描画グラフィックを初期化する
	 * レイヤのビットマップ情報が変更されている可能性があるので毎回チェックする。
	 * 変更されている場合は描画用のコンテキストを組みなおす
	 * @param layerobj レイヤオブジェクト
	 */
	void reset(iTJSDispatch2 *layerobj) {
		
		NI_LayerExBase *base = NI_LayerExBase::getNative(layerobj, false);
		base->reset(layerobj);

		// 変更されてない場合はつくりなおし
		if (!(_graphics &&
			  _width  == base->_width &&
			  _height == base->_height &&
			  _pitch  == base->_pitch &&
			  _buffer == base->_buffer)) {
			delete _graphics;
			delete _bitmap;
			_width  = base->_width;
			_height = base->_height;
			_pitch  = base->_pitch;
			_buffer = base->_buffer;
			_bitmap = new Bitmap(_width, _height, _pitch, PixelFormat32bppARGB, (unsigned char*)_buffer);
			_graphics = new Graphics(_bitmap);

			// Graphics 初期化

			
			// 座標系初期化
			
			// マトリックス適用
				
			// スムージング指定
			_graphics->SetSmoothingMode(SmoothingModeHighQuality);
		}
	}

	void redraw(iTJSDispatch2 *layerobj) {
		NI_LayerExBase *base = NI_LayerExBase::getNative(layerobj, false);
		base->redraw(layerobj);
	}
	

public:
	/**
	 * コンストラクタ
	 */
	NI_GdiPlusInfo(iTJSDispatch2 *layerobj) {
		_bitmap = NULL;
		_graphics = NULL;
		_width = 0;
		_height = 0;
		_pitch = 0;
		_buffer = NULL;
	}

	/**
	 * デストラクタ
	 */
	~NI_GdiPlusInfo() {
		delete _graphics;
		delete _bitmap;
	}

	// ------------------------------------------------------------------
	// 描画メソッド群
	// ------------------------------------------------------------------
public:

	/**
	 * 画面の消去
	 * @param argb 消去色
	 */
	void clear(ARGB argb = 0x00000000) {
		_graphics->Clear(Color(argb));
	}

	/**
	 * 楕円の描画
	 * @param pen ペン
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	void drawEllipse(Pen *pen, REAL x, REAL y, REAL width, REAL height) {
		if (pen) {
			_graphics->DrawEllipse(pen, x, y, width, height);
		}
	}

	/**
	 * 楕円の塗りつぶし
	 * @param brush ブラシ
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	void fillEllipse(Brush *brush, REAL x, REAL y, REAL width, REAL height) {
		if (brush) {
			_graphics->FillEllipse(brush, x, y, width, height);
		}
	}
	
	/**
	 * 線分の描画
	 * @param x1 始点X座標
	 * @param y1 始点Y座標
	 * @param x2 終点X座標
	 * @param y2 終点Y座標
	 */
	void drawLine(Pen *pen, REAL x1, REAL y1, REAL x2, REAL y2) {
		if (pen) {
			_graphics->DrawLine(pen, x1, y1, x2, y2);
		}
	}

	/**
	 * 矩形の描画
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	void drawRectangle(Pen *pen, REAL x, REAL y, REAL width, REAL height) {
		if (pen) {
			_graphics->DrawRectangle(pen, x, y, width, height);
		}
	}

	/**
	 * 矩形の塗りつぶし
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	void fillRectangle(Brush *brush, REAL x, REAL y, REAL width, REAL height) {
		if (brush) {
			_graphics->FillRectangle(brush, x, y, width, height);
		}
	}
	
	/**
	 * 文字列の描画
	 * @param text 描画テキスト
	 * @param font フォント
	 * @param x 原点X
	 * @param y 原点Y
	 * @param brush ブラシ
	 */
	void drawString(const tjs_char *text, Font *font, REAL x, REAL y, Brush *brush) {
		if (text && font && brush) {
			PointF origin(x, y);
			_graphics->DrawString(text, -1, font, origin, brush);
		}
	}
	
	/**
	 * 画像の描画
	 * @param name 画像名
	 * @param x 表示位置X
	 * @param y 表示位置Y
	 * @param rect 表示元領域指定。NULL の場合は全体
	 */
	void drawImage(const ttstr &name, REAL x=0, REAL y=0, RectF *rect = NULL) {

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
			if (rect) {
				_graphics->DrawImage(&image, x, y, rect->X, rect->Y, rect->Width, rect->Height, UnitPixel);
			} else {
				_graphics->DrawImage(&image, x, y);
			}
		} catch (...) {
			in->Release();
			throw;
		}
		in->Release();
	}

public:
	/**
	 * パスを描画する
	 * @param path 描画するパス
	 */
	void drawPath(Pen *pen, GraphicsPath *path) {
		_graphics->DrawPath(pen, path);
	}

	/**
	 * パスで塗りつぶす
	 * @param path 描画するパス
	 */
	void fillPath(Brush *brush, GraphicsPath *path) {
		_graphics->FillPath(brush, path);
	}
};

// クラスID
static tjs_int32 ClassID_GdiPlusInfo = -1;

/**
 * レイヤオブジェクトから GDI+ 用ネイティブインスタンスを取得する。
 * ネイティブインスタンスを持ってない場合は自動的に割り当てる
 * @param objthis レイヤオブジェクト
 * @return GDI+ 用ネイティブインスタンス。取得失敗したら NULL
 */
NI_GdiPlusInfo *
getGdiPlusNative(iTJSDispatch2 *layerobj)
{
	if (!layerobj) return NULL;

	NI_GdiPlusInfo *_this;
	if (TJS_FAILED(layerobj->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
												   ClassID_GdiPlusInfo, (iTJSNativeInstance**)&_this))) {
		// レイヤ拡張部生成
		if (NI_LayerExBase::getNative(layerobj) == NULL) {
			return NULL;
		}
		_this = new NI_GdiPlusInfo(layerobj);
		if (TJS_FAILED(layerobj->NativeInstanceSupport(TJS_NIS_REGISTER,
													   ClassID_GdiPlusInfo, (iTJSNativeInstance **)&_this))) {
			return NULL;
		}
	}

	_this->reset(layerobj);
	return _this;
}

/**
 * 矩形描画
 */
class tDrawEllipseFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 5) return TJS_E_BADPARAMCOUNT;

		_this->drawEllipse(NI_Pen::getPen(param[0]->AsObjectNoAddRef()),
						   param[1]->AsReal(),   // x
						   param[2]->AsReal(),   // y
						   param[3]->AsReal(),   // width
						   param[4]->AsReal());  // height
		return TJS_S_OK;
	}
};

/**
 * 矩形塗りつぶし
 */
class tFillEllipseFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 5) return TJS_E_BADPARAMCOUNT;

		_this->fillEllipse(NI_Brush::getBrush(param[0]->AsObjectNoAddRef()),
						   param[1]->AsReal(),   // x
						   param[2]->AsReal(),   // y
						   param[3]->AsReal(),   // width
						   param[4]->AsReal());  // height
		return TJS_S_OK;
	}
};

/**
 * 線分描画
 */
class tDrawLineFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 5) return TJS_E_BADPARAMCOUNT;

		_this->drawLine(NI_Pen::getPen(param[0]->AsObjectNoAddRef()),
						param[1]->AsReal(),   // x1
						param[2]->AsReal(),   // y1
						param[3]->AsReal(),   // x2
						param[4]->AsReal());  // y2
		return TJS_S_OK;
	}
};

/**
 * 矩形描画
 */
class tDrawRectangleFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 5) return TJS_E_BADPARAMCOUNT;

		_this->drawRectangle(NI_Pen::getPen(param[0]->AsObjectNoAddRef()),
							 param[1]->AsReal(),   // x
							 param[2]->AsReal(),   // y
							 param[3]->AsReal(),   // width
							 param[4]->AsReal());  // height
		return TJS_S_OK;
	}
};

/**
 * 矩形塗りつぶし
 */
class tFillRectangleFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 5) return TJS_E_BADPARAMCOUNT;

		_this->fillRectangle(NI_Brush::getBrush(param[0]->AsObjectNoAddRef()),
							 param[1]->AsReal(),   // x
							 param[2]->AsReal(),   // y
							 param[3]->AsReal(),   // width
							 param[4]->AsReal());  // height
		return TJS_S_OK;
	}
};

/**
 * テキスト描画
 */
class tDrawStringFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 5) return TJS_E_BADPARAMCOUNT;
		
		_this->drawString(param[0]->GetString(),
						  NI_Font::getFont(param[1]->AsObjectNoAddRef()),
						  param[2]->AsReal(),
						  param[3]->AsReal(),
						  NI_Brush::getBrush(param[4]->AsObjectNoAddRef()));
		return TJS_S_OK;
	}
};

/**
 * 矩形描画
 */
class tDrawImageFunction : public tTJSDispatch
{
protected:
public:
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {

		NI_GdiPlusInfo *_this;
		if ((_this = getGdiPlusNative(objthis)) == NULL) return TJS_E_NATIVECLASSCRASH;
		
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;

		if (numparams < 3) {
			_this->drawImage(*param[0]);
		} else if (numparams < 7) {
			_this->drawImage(*param[0], param[1]->AsReal(), param[2]->AsReal());
		} else {
			RectF rect(param[3]->AsReal(), param[4]->AsReal(), param[5]->AsReal(), param[6]->AsReal());
			_this->drawImage(*param[0], param[1]->AsReal(), param[2]->AsReal(), &rect);
		}
		
		return TJS_S_OK;
	}
};

//---------------------------------------------------------------------------

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

static void
addMember(iTJSDispatch2 *dispatch, const tjs_char *name, iTJSDispatch2 *member)
{
	tTJSVariant var = tTJSVariant(member);
	member->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		name, // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&var, // 登録する値
		dispatch // コンテキスト
		);
}

static void
delMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	dispatch->DeleteMember(
		0, // フラグ ( 0 でよい )
		name, // メンバ名
		NULL, // ヒント
		dispatch // コンテキスト
		);
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// クラスオブジェクトチェック
	if ((NI_LayerExBase::classId = TJSFindNativeClassID(L"LayerExBase")) <= 0) {
		NI_LayerExBase::classId = TJSRegisterNativeClass(L"LayerExBase");
	}
	
	// クラスオブジェクト登録
	ClassID_GdiPlusInfo = TJSRegisterNativeClass(TJS_W("GdiPlusInfo"));
	
	{
		// TJS のグローバルオブジェクトを取得する
		iTJSDispatch2 * global = TVPGetScriptDispatch();

		// Layer クラスオブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Layer"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			// プロパティ初期化
			NI_LayerExBase::init(dispatch);

			// 固有拡張クラス追加
			addMember(dispatch, L"Pen",    Create_NC_Pen());
			addMember(dispatch, L"Brush",  Create_NC_Brush());
			addMember(dispatch, L"Font",   Create_NC_Font());
			
			// メンバ追加
			addMember(dispatch, L"drawEllipse",   new tDrawEllipseFunction());
			addMember(dispatch, L"fillEllipse",   new tFillEllipseFunction());
			addMember(dispatch, L"drawLine",      new tDrawLineFunction());
			addMember(dispatch, L"drawRectangle", new tDrawRectangleFunction());
			addMember(dispatch, L"fillRectangle", new tFillRectangleFunction());
			addMember(dispatch, L"drawString",    new tDrawStringFunction());
			addMember(dispatch, L"drawImage",     new tDrawImageFunction());
		}
		
		global->Release();
	}
			
	// この時点での TVPPluginGlobalRefCount の値を
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
	// 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
	// 解放時にはこれと同じか、これよりも少なくなってないとならない。
	// そうなってなければ、どこか別のところで関数などが参照されていて、
	// プラグインは解放できないと言うことになる。

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数。

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す

	// プロパティ開放
	NI_LayerExBase::unInit();

	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// - global の DeleteMember メソッドを用い、オブジェクトを削除する
	if(global)
	{
		// TJS 自体が既に解放されていたときなどは
		// global は NULL になり得るので global が NULL でない
		// ことをチェックする

		// Layer クラスオブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Layer"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			delMember(dispatch, L"Pen");
			delMember(dispatch, L"Brush");
			delMember(dispatch, L"Font");

			delMember(dispatch, L"drawEllipse");
			delMember(dispatch, L"fillEllipse");
			delMember(dispatch, L"drawString");
			delMember(dispatch, L"drawLine");
			delMember(dispatch, L"drawRectangle");
			delMember(dispatch, L"fillRectangle");
			delMember(dispatch, L"drawImage");
		}

		global->Release();
	}

	GdiplusShutdown(gdiplusToken);
	
	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}
