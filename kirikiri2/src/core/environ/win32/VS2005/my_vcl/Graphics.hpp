#pragma once

#include "vcl_base.h"

//----------------------------------------------------------------------------
class TCanvas;
namespace Graphics
{
	//----------------------------------------------------------------------------
	class TGraphicsObject : public System::TObject
	{
	};
	//----------------------------------------------------------------------------
	class TFont : public TGraphicsObject
	{
		friend class TCanvas;
	private:
		wxFont* m_wxFont;
		wxDC* m_wxDCOwner;
		TFont(wxDC*);
	public:
		TFont();
		virtual ~TFont();
		void Assign(TFont*);
		PROPERTY(HFONT, Handle);
		PROPERTY(int, Height);
		PROPERTY(AnsiString, Name);
		PROPERTY_VAR0(TColor, Color);
		PROPERTY(TFontStyles, Style);
		PROPERTY(int, Size);
	};
	//----------------------------------------------------------------------------
	class TBrush : public TGraphicsObject
	{
	private:
		wxBrush * m_wxBrush;
	public:
		TBrush();
		virtual ~TBrush();
		PROPERTY( TBrushStyle, Style );
		PROPERTY(TColor, Color);
	};
	//----------------------------------------------------------------------------
	class TPen : public TGraphicsObject
	{
	private:
		wxPen * m_wxPen;
	public:
		TPen();
		virtual ~TPen();
		PROPERTY(TPenStyle, Style);
		PROPERTY(int, Width);
		PROPERTY(TPenMode, Mode);
		PROPERTY(TColor, Color);
	};
	//----------------------------------------------------------------------------
	class TPicture : public System::TObject
	{
	public:
		void LoadFromFile(const AnsiString & FileName);
	};
	//----------------------------------------------------------------------------
	// TInterfacedPersistent ‚Í—ª
	class TGraphic : public System::TObject
	{
		friend class TCanvas;
	protected:
		wxBitmap* m_wxBitmap;
	public:
		TGraphic();
		virtual ~TGraphic();
	};
	//----------------------------------------------------------------------------
	class TBitmap : public TGraphic
	{
	public:
		TBitmap();
		virtual ~TBitmap();
		PROPERTY(unsigned int, Width);
		PROPERTY(unsigned int, Height);
		PROPERTY_VAR_R(TCanvas*, Canvas);
		PROPERTY_VAR0(HBITMAP, Handle);
		PROPERTY(TPixelFormat, PixelFormat);

		PROPERTY_ARY_R(void*, ScanLine);
	};
	//----------------------------------------------------------------------------
	class TIcon : public TGraphic
	{
	};
};

using namespace Graphics;


#include "Canvas.h"

extern COLORREF ColorToRGB(TColor); // VCL color to RGB
