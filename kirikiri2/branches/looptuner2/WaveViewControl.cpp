//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WaveViewControl.h"
#include "WaveReader.h"

//---------------------------------------------------------------------------
__fastcall TWaveView::TWaveView(TWinControl *owner) :
	TCustomControl(owner)
{
	FReader = NULL;
	FDrawer = new TWaveDrawer(this);
	FDrawer->Parent = this;
	FDrawer->Align = alClient;
	Color = clNone;
	Brush->Style = bsClear;
	DoubleBuffered = true;
}
//---------------------------------------------------------------------------
__fastcall TWaveView::~TWaveView()
{
	delete FDrawer;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CreateParams(TCreateParams &params)
{
	TCustomControl::CreateParams(params);
	params.Style |= WS_HSCROLL; // create horizontal scroll bar
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetReader(TWaveReader * reader)
{
	FReader = reader;
	FDrawer->Reader = Reader;
	SetScrollBarRange();
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::GetMagnify()
{
	return FDrawer->Magnify;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetMagnify(int m)
{
	FDrawer->Magnify = m;
	SetScrollBarRange();

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

	si.fMask = SIF_POS;
	si.nPos = FDrawer->Start;
	SetScrollInfo(Handle, SB_HORZ, &si, true);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::Paint(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::Resize()
{
	SetScrollBarRange();
	TCustomControl::Resize();

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

	FDrawer->Start = si.nPos;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetScrollBarRange()
{
	if(!FReader || !FReader->ReadDone)
	{
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_DISABLENOSCROLL;
		SetScrollInfo(Handle, SB_HORZ, &si, true);
		EnableScrollBar(Handle, SB_HORZ, ESB_DISABLE_BOTH);
		return;
	}

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_RANGE/* | SIF_DISABLENOSCROLL*/;
	si.nMin = 0;
	si.nMax = FReader->NumSamples;
	si.nPage = FDrawer->PixelToSample(ClientWidth);

	SetScrollInfo(Handle, SB_HORZ, &si, true);

	EnableScrollBar(Handle, SB_HORZ, ESB_ENABLE_BOTH);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::WMHScroll(TWMHScroll &msg)
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

	int org = si.nPos;

	if(msg.ScrollCode == SB_THUMBTRACK)
	{
		si.nPos = si.nTrackPos;
	}
	else if(msg.ScrollCode == SB_LINELEFT)
	{
		si.nPos = si.nPos - FDrawer->PixelToSample(ClientWidth)/20;
		if(si.nPos<0) si.nPos=0;
	}
	else if(msg.ScrollCode == SB_LINERIGHT)
	{
		si.nPos = si.nPos + FDrawer->PixelToSample(ClientWidth)/20;
		if((int)(si.nPos) >= (int)(si.nMax - si.nPage)) si.nPos=si.nMax- si.nPage;
	}
	else if(msg.ScrollCode == SB_PAGELEFT)
	{
		si.nPos = si.nPos - si.nPage;
		if(si.nPos<0) si.nPos=0;
	}
	else if(msg.ScrollCode == SB_PAGERIGHT)
	{
		si.nPos = si.nPos + si.nPage;
		if(si.nPos >=si.nMax) si.nPos=si.nMax-1;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(Handle, SB_HORZ, &si, true);

	FDrawer->Start = si.nPos;
}
//---------------------------------------------------------------------------


