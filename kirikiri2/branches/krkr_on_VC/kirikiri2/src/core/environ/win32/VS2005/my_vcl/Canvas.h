#pragma once

#include "vcl_base.h"
#include "Component.h"

//----------------------------------------------------------------------------
// MFC ‚Ì CDC‚É‘Š“–‚·‚é
class TCanvas : public System::TObject
{
private:
	wxDC* m_wxDC;
	bool m_bCanDelete;
public:
	TCanvas(wxDC * p, bool bCanDelete=false);
	~TCanvas();

	PROPERTY_VAR_R(HDC, Handle);
	PROPERTY(TRect, ClipRect);

	TFont  * Font;
	TPen   * Pen;
	TBrush * Brush;

	void Draw( int X, int Y, TGraphic* Graphic );
	void Polyline(TPoint*, int);
	void FillRect(TRect);
	int TextHeight(const AnsiString & Text);
	void TextOut(int x, int y, const AnsiString & Text);
};
