#include "vcl.h"
#include "graphics.hpp"

//using namespace Graphics;

//----------------------------------------------------------------------------
//class TGraphicsObject : public System::TObject
//{
//};
//----------------------------------------------------------------------------
//class TFont : public TGraphicsObject
//{
//public:
//	void Assign(TFont*);
//	PROPERTY(HFONT, Handle);
//	PROPERTY(int, Height);
//	PROPERTY(AnsiString, Name);
//	PROPERTY(TColor, Color);
//	PROPERTY(TFontStyles, Style);
//	PROPERTY(int, Size);
//};
//----------------------------------------------------------------------------
Graphics::TFont::TFont()
{
	m_wxFont = new wxFont();
	m_wxDCOwner = NULL;
}
Graphics::TFont::TFont(wxDC * owner)
{
	m_wxFont = new wxFont();
	m_wxDCOwner = owner;
}
//----------------------------------------------------------------------------
Graphics::TFont::~TFont()
{
	delete m_wxFont;
}

//----------------------------------------------------------------------------
//  Assign メソッドを実行すると、コピー元とコピー先のフォントオブジェクトの属性は同じになるが、
// PixelsPerInch プロパティだけはコピーされない。
void Graphics::TFont::Assign(TFont* src)
{
	if ( !src->m_wxFont->Ok() )
		return;
	delete m_wxFont;

	m_wxFont = new wxFont(src->m_wxFont->GetPointSize(),
				src->m_wxFont->GetFamily(),
				src->m_wxFont->GetStyle(),
				src->m_wxFont->GetWeight(),
				src->m_wxFont->GetUnderlined(),
				src->m_wxFont->GetFaceName(),
				src->m_wxFont->GetEncoding() );

	if ( m_wxDCOwner && m_wxFont )
		m_wxDCOwner->SetFont( *m_wxFont );
}
//----------------------------------------------------------------------------
//	PROPERTY(HFONT, Handle);
// フォントの高さのピクセル単位とポイント単位との関係は次式らしい
//   Font->Size = Font->Height * 72 / Font->PixelsPerInch
//   Font->Height = Font->Size / 72 * Font->PixelsPerInch
// ある実行結果：両方とも 96
// システムのDCを取ってこないと、テストできん
void Graphics::TFont::setHeight(int val)
{
	m_wxFont->SetPointSize( val * 72 / 96 );
}
int Graphics::TFont::getHeight(void) const
{
	return m_wxFont->GetPointSize() * 96 / 72;
}
//----------------------------------------------------------------------------
void Graphics::TFont::setName(AnsiString val)
{
	m_wxFont->SetFaceName( val.c_str() );
}
AnsiString Graphics::TFont::getName() const
{
	return m_wxFont->GetFaceName().c_str();
}
//----------------------------------------------------------------------------
// fsBold, fsItalic, fsUnderline, fsStrikeOut
void Graphics::TFont::setStyle(TFontStyles val)
{
	if ( val.Contains( fsBold ) )
		m_wxFont->SetWeight(wxFONTWEIGHT_BOLD);
	else
		m_wxFont->SetWeight(wxFONTWEIGHT_LIGHT);

	if ( val.Contains( fsItalic ) )
		m_wxFont->SetStyle(wxFONTSTYLE_ITALIC);
	else
		m_wxFont->SetStyle(wxFONTSTYLE_NORMAL);

	m_wxFont->SetUnderlined( val.Contains( fsUnderline ) );
}
TFontStyles Graphics::TFont::getStyle() const
{
	TFontStyles st;
	if ( m_wxFont->GetWeight() == wxFONTWEIGHT_BOLD)
		st << fsBold;
	if ( m_wxFont->GetStyle() == wxFONTSTYLE_ITALIC)
		st << fsItalic;
	if ( m_wxFont->GetUnderlined() )
		st << fsUnderline;
	return st;
}

//----------------------------------------------------------------------------
void Graphics::TFont::setSize(int val)
{
	m_wxFont->SetPointSize( val );
}
int Graphics::TFont::getSize() const
{
	return m_wxFont->GetPointSize();
}

//----------------------------------------------------------------------------
//	PROPERTY(HFONT, Handle);
#include "wx/fontutil.h" // for wxNativeFontInfo
void Graphics::TFont::setHandle(HFONT h)
{
	wxNativeFontInfo info;
//  if ( info.FromString(fontdesc) )
//    (void)Create(info, (WXHFONT)h);
	if ( m_wxFont )
	{
		m_wxFont->Create(info, (WXHFONT) h);
		info.InitFromFont(*m_wxFont);

		OutputDebugString(m_wxFont->GetNativeFontInfoDesc().c_str());
		OutputDebugString("\n");

		if ( m_wxDCOwner )
			m_wxDCOwner->SetFont( *m_wxFont );
	}
}

HFONT Graphics::TFont::getHandle() const
{
	return (HFONT)m_wxFont->GetHFONT();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Graphics::TBrush::TBrush()
{
	m_wxBrush = new wxBrush();
	m_wxBrush->SetStyle( wxSOLID );
}
Graphics::TBrush::~TBrush()
{
	delete m_wxBrush;
}

//----------------------------------------------------------------------------
//PROPERTY( TBrushStyle, Style )
// enum TBrushStyle { bsSolid=0, bsClear=1 };
// この２つの値についてのみ実装する
void Graphics::TBrush::setStyle(TBrushStyle val)
{
	if ( val == bsClear )
		m_wxBrush->SetStyle( wxTRANSPARENT );
	else
		m_wxBrush->SetStyle( wxSOLID );
}
TBrushStyle Graphics::TBrush::getStyle() const
{
	if ( m_wxBrush->GetStyle() == wxTRANSPARENT )
		return bsClear;
	else
		return bsSolid;
}

//----------------------------------------------------------------------------
//PROPERTY(TColor, Color);
// TColor を定義済みの定数名で指定せずに、4 バイトの 16 進数で指定する場合、
// 下位の 3 バイトはそれぞれ青、緑、赤の RGB カラーの各成分を指定する。
// 0x00BBGGRR
void Graphics::TBrush::setColor(TColor val)
{
	m_wxBrush->SetColour( (val>>16) & 0xff, (val>>8) & 0xff, val & 0xff );
}
TColor Graphics::TBrush::getColor() const
{
	// COLORREFを返す関数を使う。TColorはCOLOREFとだいたい同じ
	return m_wxBrush->GetColour().GetPixel();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Graphics::TPen::TPen()
{
	m_wxPen = new wxPen();
}
Graphics::TPen::~TPen()
{
	delete m_wxPen;
}

//----------------------------------------------------------------------------
// PROPERTY(TPenStyle, Style);
// enum TPenStyle { psSolid }; // pen style
void Graphics::TPen::setStyle(TPenStyle val)
{
	m_wxPen->SetStyle(wxSOLID);
}
TPenStyle Graphics::TPen::getStyle() const
{
	return psSolid;
}


//----------------------------------------------------------------------------
// PROPERTY(TPenMode, Mode);
// enum TPenMode { pmCopy=4, pmMerge=10, pmNotMask=13 }; // pen mode
//  D    Destination 描画先の画面色
//  P    Pen ペンの色
//   pmCopy 4 P（デフォルト） 
//   pmMerge 10 P | D 
//   pmNotMask 13 ~(P & D) 
void Graphics::TPen::setMode(TPenMode val)
{
	// wxWidgets ではサポート不可能
}
TPenMode Graphics::TPen::getMode() const
{
	return pmCopy;
}

//----------------------------------------------------------------------------
// PROPERTY(TPenStyle, Style);
void Graphics::TPen::setColor(TColor val)
{
	m_wxPen->SetColour( (val>>16) & 0xff, (val>>8) & 0xff, val & 0xff );
}
TColor Graphics::TPen::getColor() const
{
	// COLORREFを返す関数を使う。TColorはCOLOREFとだいたい同じ
	return m_wxPen->GetColour().GetPixel();
}

//----------------------------------------------------------------------------
// PROPERTY(int, Width);
void Graphics::TPen::setWidth(int val)
{
	m_wxPen->SetWidth(val);
}
int TPen::getWidth() const
{
	return m_wxPen->GetWidth();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TPicture::LoadFromFile(const AnsiString & FileName)
{
}

//----------------------------------------------------------------------------
// TInterfacedPersistent は略
//class TGraphic : public System::TObject
//----------------------------------------------------------------------------
Graphics::TGraphic::TGraphic()
{
	m_wxBitmap = NULL;
}
Graphics::TGraphic::~TGraphic()
{
}
//----------------------------------------------------------------------------
//class TBitmap : public TGraphic
//----------------------------------------------------------------------------
Graphics::TBitmap::TBitmap()
{
	m_wxBitmap = new wxBitmap();
	m_Canvas = new TCanvas(new wxMemoryDC(*m_wxBitmap), true);
}
Graphics::TBitmap::~TBitmap()
{
	if ( m_Canvas ) delete m_Canvas;
	delete m_wxBitmap;
}
//----------------------------------------------------------------------------
//	PROPERTY(unsigned int, Width)
void Graphics::TBitmap::setWidth(unsigned int val)
{
	m_wxBitmap->SetWidth(val);
}
unsigned int Graphics::TBitmap::getWidth() const
{
	return m_wxBitmap->GetWidth();
}
//----------------------------------------------------------------------------
//	PROPERTY(unsigned int, Height)
void Graphics::TBitmap::setHeight(unsigned int val)
{
	m_wxBitmap->SetHeight(val);
}
unsigned int Graphics::TBitmap::getHeight() const
{
	return m_wxBitmap->GetHeight();
}
//	PROPERTY(HBITMAP, Handle)
//----------------------------------------------------------------------------
//	PROPERTY(TPixelFormat, PixelFormat)
//enum TPixelFormat {pfDevice=0, pf1bit=1, pf4bit=2, pf8bit=3, pf15bit=4, pf16bit=5, pf24bit=6, pf32bit=7, pfCustom=8 };
void Graphics::TBitmap::setPixelFormat(TPixelFormat val)
{
	switch(val)
	{
	default:
	case pfCustom:
	case pfDevice:
	case pf1bit:  m_wxBitmap->SetDepth(1); break;
	case pf4bit:  m_wxBitmap->SetDepth(4); break;
	case pf8bit:  m_wxBitmap->SetDepth(8); break;
	case pf15bit: m_wxBitmap->SetDepth(16); break;
	case pf16bit: m_wxBitmap->SetDepth(16); break;
	case pf24bit: m_wxBitmap->SetDepth(24); break;
	case pf32bit: m_wxBitmap->SetDepth(32); break;
	}
}
TPixelFormat Graphics::TBitmap::getPixelFormat() const
{
	switch(m_wxBitmap->GetDepth())
	{
	default:
	case 1:  return pf1bit;
	case 4:  return pf4bit;
	case 8:  return pf8bit;
	case 15: return pf15bit;//15なんて返ってくるの？
	case 16: return pf16bit;
	case 24: return pf24bit;
	case 32: return pf32bit;
	}
}
//----------------------------------------------------------------------------
//	PROPERTY_ARY_R(void*, ScanLine)
void* TBitmap::getScanLine(int index) const
{
	return NULL;
}
//----------------------------------------------------------------------------
//class TIcon : public TGraphic
//{
//};



//----------------------------------------------------------------------------
// 関数
//----------------------------------------------------------------------------
//Graphics.hpp
//TColor 値を RGB 値に変換する
COLORREF ColorToRGB(TColor col)
{
	return col & 0xFFFFFF;
}

