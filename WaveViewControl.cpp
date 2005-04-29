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
	FFollowingMarker = true;
	FWaitingMarker = true;
	FSoftCenteringStartTick = 0;
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
int __fastcall TWaveView::GetMarkerPos()
{
	return FDrawer->MarkerPos;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetMarkerPos(int n)
{
	FDrawer->MarkerPos = n;
	if(FFollowingMarker)
	{
		int viewsamples = FDrawer->PixelToSample(FDrawer->Width);
		if(viewsamples * 1000 / FReader->Frequency < 500)
		{
			// too fast to track
			SetView(n);
			FWaitingMarker = false;
			DoubleBuffered = true;
		}
		else
		{
			DoubleBuffered = false;

			int px = FDrawer->SampleToPixel(n - FDrawer->Start);
			if(px < 0 || px >= FDrawer->Width)
			{
				SetView(n, 1);
				FWaitingMarker = true;
				FSoftCenteringStartTick = 0;
			}
			else
			{
				if(FWaitingMarker)
				{
					if(px >= FDrawer->Width *9 / 10)
					{
						FWaitingMarker = false; // start following
						FSoftCenteringStartTick = GetTickCount();
						FSoftCenteringPos = px - FDrawer->Width / 2;
					}
				}
				else
				{
					// soft centering
					int elapsed = GetTickCount() - FSoftCenteringStartTick;
					int slew = FSoftCenteringPos - elapsed / 20;
					if(slew < 0) slew = 0;
					int sn = n;
					if(px > ClientWidth/2)
					{
						sn -= FDrawer->PixelToSample((ClientWidth/2) + slew);
					}
					else
					{
						sn -= FDrawer->PixelToSample(ClientWidth / 2);
					}
					SetView(sn, 0);
				}
			}
		}
	}
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

	DoubleBuffered = true;

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
void __fastcall TWaveView::SetView(int n, int r)
{
	// set left edge of the viewport
	int clwp = FDrawer->PixelToSample(ClientWidth);
	clwp = clwp / 10 * r;
	n -= clwp;
	if(n < 0) n = 0;

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = n;
	SetScrollInfo(Handle, SB_HORZ, &si, true);

	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

	FDrawer->Start = si.nPos;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::WMHScroll(TWMHScroll &msg)
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

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
void __fastcall TWaveView::CMMouseLeave(TMessage &msg)
{
	FDrawer->MouseLeave();
}
//---------------------------------------------------------------------------


