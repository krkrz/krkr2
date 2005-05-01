//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <math.h>
#include "WaveView.h"
#include "WaveReader.h"


//---------------------------------------------------------------------------
// Color schemes - currently fixed colors only
//---------------------------------------------------------------------------
const TColor C_DISABLEED_CLIENT	= clBtnFace;
const TColor C_CLIENT			= clWhite;
const TColor C_WAVE				= clBlack;
const TColor C_WAVE2			= clGray;
const TColor C_INF_LINE			= clGray;
const TColor C_DISABLED_TIME_CLIENT		= ((TColor)0xf0f0f0);
const TColor C_TIME_CLIENT		= clBtnFace;
const TColor C_TIME_COLOR		= clBlack;
const TColor C_LINK_WAVE_MARK	= ((TColor)0x0000f0);
const TColor C_LINK_CLIENT		= clWhite;
const TColor C_LINK_SEPARETOR   = ((TColor)0xe0e0e0);
const TColor C_LINK_LINE		= clBlack;
const TColor C_LINK_HOVER		= clBlue;
const TColor C_LINK_FOCUS		= clRed;
const TColor C_LINK_WEAK_LINE	= clGray;
//---------------------------------------------------------------------------
const int LinkArrowSize = 4;
const int LinkDirectionArrowWidth = 10;
const int LinkDirectionInterval = 200;
//---------------------------------------------------------------------------
static std::vector<tTVPWaveLoopLink> Links;
static std::vector<tTVPWaveLoopLink> &  GetLinks()
{
	if(Links.size() == 0)
	{
		tTVPWaveLoopLink link;
		link.From = 1900000;
		link.To = 550000;
		link.Smooth = false;
		link.Condition = false;
		link.CondVar = -1;
		Links.push_back(link);


		link.From = 850000;
		link.To = 2005000;
		link.Smooth = false;
		link.Condition = false;
		link.CondVar = -1;
		Links.push_back(link);

		link.From = 1250000;
		link.To = 2005000;
		link.Smooth = false;
		link.Condition = false;
		link.CondVar = -1;
		Links.push_back(link);

	}
	return Links;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
__fastcall TWaveView::TWaveView(Classes::TComponent* AOwner) :
	TCustomControl(AOwner)
{
	Color = clNone;

	FReader = NULL;
	FMagnify = -12;
	FStart = 0;

	FFollowingMarker = true;
	FWaitingMarker = true;
	FSoftCenteringStartTick = 0;

	FCaretPos = 0;
	FDrawCaret = true;
	FCaretVisiblePhase = true;

	FMinRulerMajorWidth = 0;
	FMinRulerMajorHeight = 0;
	FRulerUnit = 0;
	FDrawRuler = true;
	FMarkerPos = -1;
	Canvas->Font->Height = -12;

	FDrawLinks = true;
	FLinkTierCount = 0;
	FHoveredLink = -1; // -1 for non visible
	FFocusedLink = -1; // -1 for not focused
	NotifyLinkChanged();

	FBlinkTimer = new TTimer(this);
	FBlinkTimer->OnTimer = OnBlinkTimer;
	FBlinkTimer->Interval = GetCaretBlinkTime();
	FBlinkTimer->Enabled = true;
/*
	FOnWaveLButtonDown = NULL;
	FOnLinkLButtonDown = NULL;
	FOnLinkDragStart   = NULL;
	FOnLinkDragOver    = NULL;
	FOnLinkDragDrop    = NULL;
*/
	LastMouseDownX = -1;
	LastMouseDownLinkNum = -1;
	LastMouseDownLinkFromOrTo = false;
	DraggingState = dsNone;

	Cursor = crIBeam;
}
//---------------------------------------------------------------------------
__fastcall TWaveView::~TWaveView()
{
	delete FBlinkTimer;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::Paint(void)
{
	DrawWave(FStart, true);
	DrawLink();
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::PixelToSample(int pixel)
{
	if(FMagnify <= 0)
	{
		// shrink
		return pixel << (-FMagnify);
	}
	else
	{
		// expand
		return pixel >> FMagnify;
	}
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::SampleToPixel(int sample)
{
	if(FMagnify <= 0)
	{
		// shrink
		return sample >> (-FMagnify);
	}
	else
	{
		// expand
		return sample << FMagnify;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetReader(TWaveReader * reader)
{
	// set wave reader
	FReader = reader;
	Invalidate();

	SetScrollBarRange();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CreateParams(TCreateParams &params)
{
	TCustomControl::CreateParams(params);
	params.Style |= WS_HSCROLL; // create horizontal scroll bar
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

	SetStart(si.nPos);
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
	si.nPage = PixelToSample(ClientWidth);

	SetScrollInfo(Handle, SB_HORZ, &si, true);

	EnableScrollBar(Handle, SB_HORZ, ESB_ENABLE_BOTH);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetView(int n, int r)
{
	// set left edge of the viewport
	int clwp = PixelToSample(ClientWidth);
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

	SetStart(si.nPos);
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
		si.nPos = si.nPos - PixelToSample(ClientWidth)/20;
		if(si.nPos<0) si.nPos=0;
	}
	else if(msg.ScrollCode == SB_LINERIGHT)
	{
		si.nPos = si.nPos + PixelToSample(ClientWidth)/20;
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

	SetStart(si.nPos);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CMMouseLeave(TMessage &msg)
{
	MouseLeave();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::InvalidateCaret(int pos)
{
	RECT r;
	r.top = GetHeadSize();
	r.bottom = ClientHeight;
	int p_pos = SampleToPixel(pos - FStart);
	if(p_pos >= 0 && p_pos < ClientWidth)
	{
		r.left = p_pos;
		r.right = p_pos + 1;
		InvalidateRect(Handle, &r, false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetCaretPos(int pos)
{
	// set caret position.

	if(pos != FCaretPos)
	{
		// invalidate new position and old position
		if(pos >= 0) FocusedLink = -1; // caret and link are exclusive
		if(FDrawCaret)
		{
			InvalidateCaret(FCaretPos);
			InvalidateCaret(pos);
		}

		if(pos >= 0)
		{
			FCaretVisiblePhase = true;
			FBlinkTimer->Enabled = false;
			FBlinkTimer->Enabled = FDrawCaret;
		}
		FCaretPos = pos;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetDrawCaret(bool b)
{
	if(b != FDrawCaret)
	{
		FDrawCaret = b;

		if(FCaretPos >= 0)
		{
			FCaretVisiblePhase = true;
			InvalidateCaret(FCaretPos);
			FBlinkTimer->Enabled = b;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::OnBlinkTimer(TObject * sender)
{
	FCaretVisiblePhase = !FCaretVisiblePhase;
	InvalidateCaret(FCaretPos);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetStart(int n)
{
	RECT r;

	// set start position
	n = PixelToSample(SampleToPixel(n)); // quantize
	r.left = 0;
	r.top = 0;
	r.right = ClientWidth;
	r.bottom = ClientHeight;
	int d = SampleToPixel(FStart) - SampleToPixel(n);
	ScrollWindowEx(Handle, d, 0,
		&r, NULL, NULL, NULL, SW_INVALIDATE);

	FStart = n;

}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetMagnify(int m)
{
	// set magnification
	if(m > 3) m = 3;
	if(m < -16) m = -16;
	if(FMagnify != m)
	{
		int center;
		int c_pos = SampleToPixel(FCaretPos - FStart);
		if(c_pos >= 0 && c_pos < ClientWidth)
			center = FCaretPos;
		else
			center = FStart + PixelToSample(ClientWidth >> 1);
		FMagnify = m;
		int left = center - PixelToSample(ClientWidth >> 1);
		int view = PixelToSample(ClientWidth);
		if(left < 0) left = 0;
		if(left > FReader->NumSamples - view) left = FReader->NumSamples - view;
		if(left < 0) left = 0;

		FStart = PixelToSample(SampleToPixel(left)); // quantize
		FMinRulerMajorWidth = 0;
		FMinRulerMajorHeight = 0;
		NotifyLinkChanged();
		Invalidate();

		// set scroll bar range
		SetScrollBarRange();

		SCROLLINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(Handle, SB_HORZ, &si);

		si.fMask = SIF_POS;
		si.nPos = FStart;
		SetScrollInfo(Handle, SB_HORZ, &si, true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetMarkerPos(int n)
{
	// set marker position.
	// p = -1 for hide the marker.
	if(!FReader || !FReader->ReadDone) n = -1;

	if(n != FMarkerPos)
	{
		// invalidate new position and old position
		int p_pos;
		RECT r;
		r.top = GetHeadSize();
		r.bottom = ClientHeight - GetFootSize();

		if(n != -1)
		{
			p_pos = SampleToPixel(n - FStart);
			if(p_pos >= 0 && p_pos < ClientWidth)
			{
				r.left = p_pos;
				r.right = p_pos + 1;
				InvalidateRect(Handle, &r, false);
			}
		}

		if(FMarkerPos != -1)
		{
			p_pos = SampleToPixel(FMarkerPos - FStart);
			if(p_pos >= 0 && p_pos < ClientWidth)
			{
				r.left = p_pos;
				r.right = p_pos + 1;
				InvalidateRect(Handle, &r, false);
			}
		}

		FMarkerPos = n;
	}

	// marker following
	if(FFollowingMarker)
	{
		int viewsamples = PixelToSample(ClientWidth);
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

			int px = SampleToPixel(n - FStart);
			if(px < 0 || px >= ClientWidth)
			{
				SetView(n, 1);
				FWaitingMarker = true;
				FSoftCenteringStartTick = 0;
			}
			else
			{
				if(FWaitingMarker)
				{
					if(px >= ClientWidth *9 / 10)
					{
						FWaitingMarker = false; // start following
						FSoftCenteringStartTick = GetTickCount();
						FSoftCenteringPos = px - ClientWidth / 2;
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
						sn -= PixelToSample((ClientWidth/2) + slew);
					}
					else
					{
						sn -= PixelToSample(ClientWidth / 2);
					}
					SetView(sn, 0);
				}
			}
		}
	}

}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawWave(int start, bool clear)
{
	// draw waveform.
	// this does not clear the background unless 'clear' is true.
	// 'magnify' is magnify level in logarithm based 2;
	//  2 = x 4
	//  1 = x 2
	//  0 = x 1
	// -1 = x 1 / 2
	// -2 = x 1 / 4
	if(!FReader || !FReader->ReadDone)
	{
		TRect r;
		r.left = Canvas->ClipRect.left, r.top = 0, r.right = Canvas->ClipRect.right, r.bottom = ClientHeight;
		Canvas->Brush->Style = bsSolid;
		Canvas->Brush->Color = C_DISABLEED_CLIENT;
		Canvas->FillRect(r);
		return ; // reader not available
	}

	// calc minimum major ruler size
	if(FDrawRuler && FMinRulerMajorWidth == 0)
	{
		AnsiString zero("00:00:00.000");
		FMinRulerMajorWidth = Canvas->TextWidth(zero);
		FMinRulerMajorWidth += 3;
		FMinRulerMajorHeight = Canvas->TextHeight(zero);
		FMinRulerMajorHeight += FMinRulerMajorHeight / 3;

		// decice time unit
		double sample_per_pixel;
		if(FMagnify > 0)
			sample_per_pixel = 1.0 / (1<<FMagnify);
		else
			sample_per_pixel = 1.0 * (1<<(-FMagnify));

		double time_per_pixel = 10000.0 * // time in msec/10
			sample_per_pixel / FReader->Frequency;

		// crank up to upper reasonable time unit
		int n;
		int nn;
		for(n = 10000000; n >= 1; n /= 10)
		{
			double unit_size;
			nn = n * 1;
			unit_size = nn / time_per_pixel;
			if(unit_size <= FMinRulerMajorWidth) break;
		}
		FRulerUnit = nn;
	}

	// calc dimentions
	int xstep = FMagnify <= 0 ? 1 : (1<<FMagnify);
	int sstep = FMagnify <= 0 ? (1<<(-FMagnify)) : 1;

	int head_size = GetHeadSize();
	int foot_size = GetFootSize();
	int bottom_limit = ClientHeight - foot_size;
	int one_height = ((ClientHeight - head_size - foot_size) / FReader->Channels) & ~1;
	int one_height_half = one_height >> 1;
	int one_client_height = one_height - 2; // 2 : padding margin

	// calc start point
	int dest_left  = Canvas->ClipRect.left;
	int dest_right = Canvas->ClipRect.right;

	//- align destination start and end
	dest_left = (int)(dest_left / xstep) * xstep;
	dest_right = ((int)(dest_right / xstep) + 1) * xstep;

	//- calc pcm start point
	int pos = start + (int)(dest_left / xstep) * sstep;
	if(pos < 0)
	{
		// start position is negative
		dest_left += (-pos) * xstep;
		pos = 0;
		if(dest_left > dest_right) dest_left = dest_right;
	}


	//- calc pcm end point
	int vr = SampleToPixel(FReader->NumSamples - pos) + 1 + dest_left;
	if(vr < dest_right) dest_right = vr;

	// clear the background
	#define		CONV_Y(v, ch) ((((v) * one_client_height) /65536) + \
							one_height_half + (ch) * one_height + head_size)

	if(clear)
	{
		// negative
		if(Canvas->ClipRect.left < dest_left)
		{
			TRect r;
			r.left = Canvas->ClipRect.left, r.top = head_size, r.right = dest_left, r.bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_DISABLEED_CLIENT;
			Canvas->FillRect(r);
		}


		// main
		if(dest_left < dest_right)
		{
			TRect r;
			r.left = dest_left, r.top = head_size, r.right = dest_right, r.bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_CLIENT;
			Canvas->FillRect(r);
		}

		// draw -Inf line
		Canvas->Pen->Color = C_INF_LINE;
		for(int ch = 0; ch < FReader->Channels; ch ++)
		{
			Canvas->MoveTo(dest_left,  CONV_Y(0, ch));
			Canvas->LineTo(dest_right, CONV_Y(0, ch));
		}


		// over
		if(Canvas->ClipRect.right > dest_right)
		{
			TRect r;
			r.left = dest_right, r.top = head_size, r.right = Canvas->ClipRect.right, r.bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_DISABLEED_CLIENT;
			Canvas->FillRect(r);
		}

		// draw time base line
		if(FDrawRuler)
		{
			TRect r;
			r.left = dest_left, r.top = 0, r.right = dest_right, r.bottom = head_size;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_TIME_CLIENT;
			Canvas->FillRect(r);
			if(dest_right < Canvas->ClipRect.right)
			{
				r.left = dest_right;
				r.right = Canvas->ClipRect.right;
				Canvas->Brush->Color = C_DISABLED_TIME_CLIENT;
				Canvas->FillRect(r);
			}
			Canvas->Pen->Color = C_TIME_COLOR;
			Canvas->MoveTo(dest_left, head_size - 1);
			Canvas->LineTo(Canvas->ClipRect.right, head_size - 1);
		}


	}


	// main loop
	int rpos = pos;
	Canvas->Pen->Color = C_WAVE;
	Canvas->Brush->Color = C_WAVE;
	if(dest_left >= dest_right) return; // no more samples
	for(int x = dest_left; x <= dest_right;
		x += xstep, pos += sstep)
	{
		for(int ch = 0; ch < FReader->Channels; ch ++)
		{
			// draw line to previous sample
			if(pos >= FReader->NumSamples) break;

			if(pos > 0)
			{
				int pv = FReader->GetSampleAt(pos-1, ch);
				Canvas->MoveTo(x-xstep, CONV_Y(pv, ch));
			}
			else
			{
				Canvas->MoveTo(x-xstep, CONV_Y(0, ch));
			}

			int cv;
			cv = FReader->GetSampleAt(pos, ch);
			Canvas->LineTo(x, CONV_Y(cv, ch));

			if(FMagnify < 0)
			{
				// shrink view
				// draw peaks
				int high, low;
				FReader->GetPeak(high, low, pos, ch, sstep);
				Canvas->MoveTo(x, CONV_Y(high, ch));
				Canvas->LineTo(x, CONV_Y(low , ch));
			}
			if(FMagnify >= 3)
			{
				// draw sample point
				RECT r;
				r.left = x - 1;
				r.right = x + 2;
				r.top = CONV_Y(cv, ch) - 1;
				r.bottom = r.top + 3;
				Canvas->FillRect(r);
			}
		}
	}

	// draw marker
	if(FMarkerPos != -1)
	{
		int p_pos = SampleToPixel(FMarkerPos - FStart);
		if(p_pos >= 0 && p_pos < ClientWidth)
		{
			RECT r;
			r.top = head_size;
			r.bottom = bottom_limit;
			r.left = p_pos;
			r.right = p_pos + 1;
			InvertRect(Canvas->Handle, &r);
		}
	}


	// draw time line
	if(FDrawRuler && Canvas->ClipRect.top < head_size)
	{
		// find start ruler unit
		Canvas->Brush->Style = bsClear;
		Canvas->Pen->Color = C_TIME_COLOR;
		Canvas->Font->Color = C_TIME_COLOR;
		double sample_per_pixel;

		int time_index;

		if(FMagnify > 0)
			sample_per_pixel = 1.0 / (1<<FMagnify);
		else
			sample_per_pixel = 1.0 * (1<<(-FMagnify));

		double time_per_pixel = // time in msec/10
			sample_per_pixel / FReader->Frequency * 10000;

		double start_time = (double)rpos / (double)FReader->Frequency * 10000;
		time_index = (int)(start_time / FRulerUnit) - 10;
		start_time = (double)time_index * (double)FRulerUnit;

		double x_per_unit = FRulerUnit / time_per_pixel;

		double x = start_time / time_per_pixel - SampleToPixel(rpos);

		for(; x < dest_right; x += x_per_unit, time_index ++)
		{
			int ix = (int)floor(x + dest_left);
			int st;
			bool write_time = false;
			if(time_index % 10 == 0)
			{
				st = 0;
				write_time = true;
			}
			else if(time_index % 5 == 0)
			{
				if(FRulerUnit >= 5 && x_per_unit * 5 > FMinRulerMajorWidth)
				{
					st = 0;
					write_time = true;
				}
				else
				{
					st = (head_size >> 3) + (head_size >> 1);
				}
			}
			else
			{
				if(FRulerUnit >= 1 && x_per_unit > FMinRulerMajorWidth)
				{
					st = 0;
					write_time = true;
				}
				else
				{
					st = (head_size >> 2) + (head_size >> 1);
				}
			}
			Canvas->MoveTo(ix, st);
			Canvas->LineTo(ix, head_size - 1);
			if(write_time)
			{
				// write time
				int t = (int)((__int64)time_index * FRulerUnit / 10);
				char buf[100];
				if(FRulerUnit >= 5000)
				{
					sprintf(buf, "%02d:%02d:%02d",
						t / 1000 / 60 / 60,
						t / 1000 / 60 % 60,
						t / 1000 % 60);
				}
				else
				{
					sprintf(buf, "%02d:%02d:%02d.%03d",
						t / 1000 / 60 / 60,
						t / 1000 / 60 % 60,
						t / 1000 % 60,
						t % 1000);
				}
				Canvas->TextOut(ix+2, 0, buf);
			}
		}
	}

	// draw link 'from' and 'to' lines
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/
	Canvas->Pen->Color = C_LINK_WAVE_MARK;
	Canvas->Pen->Style = psDashDot;
	for(unsigned int i = 0; i < links.size(); i++)
	{
		const tTVPWaveLoopLink & link = links[i];
		int x;

		x = SampleToPixel(link.From - Start);
		Canvas->MoveTo(x, head_size);
		Canvas->LineTo(x, bottom_limit);

		x = SampleToPixel(link.To - Start);
		Canvas->MoveTo(x, head_size);
		Canvas->LineTo(x, bottom_limit);
	}
	Canvas->Pen->Style = psSolid;

	// draw caret
	if(FDrawCaret && FCaretVisiblePhase)
	{
		int p_pos = SampleToPixel(FCaretPos - FStart);
		if(p_pos >= 0 && p_pos < ClientWidth)
		{
			RECT r;
			r.top = head_size;
			r.bottom = ClientHeight;
			r.left = p_pos;
			r.right = p_pos + 1;
			InvertRect(Canvas->Handle, &r);
		}
	}

}
//---------------------------------------------------------------------------
inline static bool PartIntersect(int a, int b, int i, int j)
{
	if(a > b)
	{
		int t = b;
		b = a;
		a = t;
	}
	if(i > j)
	{
		int t = i;
		j = i;
		i = t;
	}

	return
		a >= i && a <= j ||
		b >= i && b <= j ||
		i >= a && i <= b ||
		j >= a && j <= b;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::NotifyLinkChanged()
{
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	// re-check focused or hovered state
	if(FFocusedLink >= (int)links.size()) FFocusedLink = -1;
	if(FHoveredLink >= (int)links.size()) FHoveredLink = -1;

	// re-check tier
	int tier_count = 0;

	for(unsigned int i = 0; i < links.size(); i++)
	{
		// search free tier
		TTierMinMaxInfo cinfo;
		GetLinkMinMaxPixel(links[i], cinfo);

		int found_link_tier = 0;
		int found_from_tier = 0;
		int found_to_tier   = 0;

		for(unsigned int j = 0; j < i; j++)
		{
			TTierMinMaxInfo jinfo;
			GetLinkMinMaxPixel(links[j], jinfo);

			// for link
			if(PartIntersect(jinfo.LinkMin, jinfo.LinkMax, cinfo.LinkMin, cinfo.LinkMax))
			{
				// intersection found
				found_link_tier = links[j].LinkTier + 1;
			}

			// for 'from'
			if(	PartIntersect(jinfo.ToMin, jinfo.ToMax, cinfo.FromMin, cinfo.FromMax) )
			{
				// intersection found
				found_from_tier = links[j].ToTier + 1;
			}
			if( PartIntersect(jinfo.FromMin, jinfo.FromMax, cinfo.FromMin, cinfo.FromMax))
			{
				// intersection found
				found_from_tier = links[j].FromTier + 1;
			}

			// for 'to'
			if( PartIntersect(jinfo.ToMin, jinfo.ToMax, cinfo.ToMin, cinfo.ToMax))
			{
				// intersection found
				found_to_tier = links[j].ToTier + 1;
			}
			if(	PartIntersect(jinfo.FromMin, jinfo.FromMax, cinfo.ToMin, cinfo.ToMax))
			{
				// intersection found
				found_to_tier = links[j].FromTier + 1;
			}
		}

		links[i].LinkTier = found_link_tier;
		links[i].FromTier = found_from_tier;
		links[i].ToTier   = found_to_tier;

		if(tier_count < found_link_tier + 1) tier_count = found_link_tier + 1;
	}

	if(FLinkTierCount != tier_count)
		Invalidate();
	FLinkTierCount = tier_count;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::GetLinkMinMaxPixel(const tTVPWaveLoopLink & link,
		TTierMinMaxInfo & info)
{
	int from_px = SampleToPixel(link.From);
	int to_px   = SampleToPixel(link.To);

	int min;
	int max;

	min = from_px - 1;
	max = from_px + 1;

	if(min > to_px - (LinkArrowSize+1)) min = to_px - (LinkArrowSize+1);
	if(max < to_px + (LinkArrowSize+1)) max = to_px + (LinkArrowSize+1);

	info.LinkMin = min;
	info.LinkMax = max;
	info.FromMin = from_px - 1;
	info.FromMax = from_px + 1;
	info.ToMin   = to_px   - (LinkArrowSize+1);
	info.ToMax   = to_px   + (LinkArrowSize+1);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLinkArrowAt(int tier, int where)
{
	// arrow bitmap:
	// <> = 1 pixel

	//       <>
	//       <>
	//     <><><>
	//     <><><>
	//   <><><><><>
	//   <><><><><>
	// <><><><><><><>
	// <><><><><><><>
	//       <>
	//       <>

	// draw up-arrow at given position
	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;
	int y = y_start + tier * TVP_LGD_TIER_HEIGHT;

	// draw arrow body
	Canvas->Brush->Style = bsSolid;
	Canvas->Brush->Color = Canvas->Pen->Color;
	TRect r;

	for(int i = 1; i < LinkArrowSize; i++)
	{
		r.left   = where - i;
		r.top    = y + i*2;
		r.right  = where + (i+1);
		r.bottom = y + (i*2+2);
		Canvas->FillRect(r);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawDirectionArrowAt(int tier, bool dir, int where)
{
	// arrow bitmap:
	// <> = 1 pixel

	//         <><>
	//     <><><><>
	// <><><><><><><><><><>
	//     <><><><>
	//         <><>

	//         <><>
	//         <><><><>
	// <><><><><><><><><><>
	//         <><><><>
	//         <><>

	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;
	int y = y_start + tier * TVP_LGD_TIER_HEIGHT + TVP_LGD_TIER_HEIGHT - 3;

	Canvas->Brush->Style = bsSolid;
	Canvas->Brush->Color = Canvas->Pen->Color;
	TRect r;

	if(dir)
	{
		// dir = true: right arrow
		r.left   = where + 0;
		r.top    = y - 2;
		r.right  = where + 2;
		r.bottom = y + 3;
		Canvas->FillRect(r);

		r.left   = where + 2;
		r.top    = y - 1;
		r.right  = where + 4;
		r.bottom = y + 2;
		Canvas->FillRect(r);
	}
	else
	{
		// dir = false: left arrow
		r.left   = where + 0;
		r.top    = y - 1;
		r.right  = where + 2;
		r.bottom = y + 2;
		Canvas->FillRect(r);

		r.left   = where + 2;
		r.top    = y - 2;
		r.right  = where + 4;
		r.bottom = y + 3;
		Canvas->FillRect(r);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLinkOf(const tTVPWaveLoopLink & link)
{
	int dest_left  = Canvas->ClipRect.left;
	int dest_right = Canvas->ClipRect.right;
	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;

	int x_from;
	int x_to;
	int y_link = y_start + link.LinkTier * TVP_LGD_TIER_HEIGHT + TVP_LGD_TIER_HEIGHT - 3;

	int dir_step = PixelToSample(LinkDirectionInterval);

	// for from line
	x_from = SampleToPixel(link.From - Start);
	if(x_from >= dest_left || x_from < dest_right)
	{
		int y0 = y_start + link.FromTier * TVP_LGD_TIER_HEIGHT;
		Canvas->MoveTo(x_from, y0);
		Canvas->LineTo(x_from, y_link);
	}

	// for to arrow
	x_to = SampleToPixel(link.To - Start);
	int xmin = x_to - (LinkArrowSize+1);
	int xmax = x_to + (LinkArrowSize+1);
	if(PartIntersect(xmin, xmax, dest_left, dest_right -1))
	{
		DrawLinkArrowAt(link.ToTier, x_to);
		int y0 = y_start + link.ToTier * TVP_LGD_TIER_HEIGHT;
		Canvas->MoveTo(x_to, y0);
		Canvas->LineTo(x_to, y_link);
	}

	// for horizontal link line
	if(PartIntersect(x_from, x_to, dest_left, dest_right-1))
	{
		if(x_from < x_to)
		{
			Canvas->MoveTo(x_from, y_link);
			Canvas->LineTo(x_to+1, y_link);
		}
		else
		{
			Canvas->MoveTo(x_from, y_link);
			Canvas->LineTo(x_to-1, y_link);
		}
	}

	// for direction mark
	int m_start = x_to < x_from ? x_to : x_from;
	int m_end   = x_to < x_from ? x_from : x_to;
	if(m_end - m_start > LinkDirectionArrowWidth * 2)
	{
		int s = (Start / dir_step) * dir_step;

		for(;;)
		{
			int x = SampleToPixel(s - Start);
			if(x > dest_right) break;

			if(x > m_start + LinkDirectionArrowWidth && x < m_end - LinkDirectionArrowWidth)
				DrawDirectionArrowAt(link.LinkTier, x_from < x_to, x);

			s += dir_step;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLink(void)
{
	if(!FReader || !FReader->ReadDone) return;

	// calc start point
	int dest_left  = Canvas->ClipRect.left;
	int dest_right = Canvas->ClipRect.right;
	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;

	if(!PartIntersect(Canvas->ClipRect.top, Canvas->ClipRect.bottom -1,
		y_start, ClientHeight - 1)) return;

	// erase background
/*
	TRect r;
	r.left = dest_left, r.top = y_start,
	r.right = dest_right, r.bottom = ClientHeight;
	Canvas->Brush->Style = bsSolid;
	Canvas->Brush->Color = C_LINK_CLIENT;
	Canvas->FillRect(r);
*/

	// draw separator
	Canvas->Pen->Color = C_LINK_SEPARETOR;
	Canvas->MoveTo(dest_left,  y_start);
	Canvas->LineTo(dest_right, y_start);

	// draw each link (for vertical weak line)
	Canvas->Pen->Color = C_LINK_WEAK_LINE;
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	for(unsigned int i = 0; i < links.size(); i++)
	{
		const tTVPWaveLoopLink & link = links[i];
		tjs_int x;

		x = SampleToPixel(link.From - Start);
		if(x >= dest_left || x < dest_right)
		{
			int y = y_start + link.FromTier * TVP_LGD_TIER_HEIGHT - 1;
			Canvas->MoveTo(x, y_start+1);
			Canvas->LineTo(x, y);
		}
		x = SampleToPixel(link.To - Start);
		if(x >= dest_left || x < dest_right)
		{
			int y = y_start + link.ToTier * TVP_LGD_TIER_HEIGHT - 1;
			Canvas->MoveTo(x, y_start+1);
			Canvas->LineTo(x, y);
		}
	}

	// draw each link
	for(unsigned int i = 0; i < links.size(); i++)
	{
		if((int)i == FFocusedLink)
			continue;
		else if((int)i == FHoveredLink)
			continue;
		else
			Canvas->Pen->Color = C_LINK_LINE;

		const tTVPWaveLoopLink & link = links[i];

		DrawLinkOf(link);
	}
	if(FHoveredLink != -1)
	{
		Canvas->Pen->Color = C_LINK_HOVER;
		const tTVPWaveLoopLink & link = links[FHoveredLink];
		DrawLinkOf(link);
	}

	if(FFocusedLink != -1)
	{
		Canvas->Pen->Width = 2;
		Canvas->Pen->Color = C_LINK_FOCUS;
		const tTVPWaveLoopLink & link = links[FFocusedLink];
		DrawLinkOf(link);
		Canvas->Pen->Width = 1;
	}

}
//---------------------------------------------------------------------------
void __fastcall TWaveView::InvalidateLink(int linknum)
{
	// invalidate displaying area of link 'linknum'.
	// currently not completely implemented.

	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	if(linknum < 0 || linknum >= (int)links.size()) return;

	const tTVPWaveLoopLink & link = links[linknum];

	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;
	RECT r;
	r.top = y_start;
	r.bottom = ClientHeight;
	r.left = 0;
	r.right = ClientWidth;
	InvalidateRect(Handle, &r, false);

	// also invalidate wave link marks
	int x_from = SampleToPixel(link.From - Start);
	int x_to   = SampleToPixel(link.To   - Start);

	r.top = GetHeadSize();
	r.bottom = ClientHeight - GetFootSize();

	if(x_from >= 0 && x_from < ClientWidth)
	{
		r.left  = x_from;
		r.right = x_from + 1;
		InvalidateRect(Handle, &r, false);
	}

	if(x_to >= 0 && x_to < ClientWidth)
	{
		r.left  = x_to;
		r.right = x_to + 1;
		InvalidateRect(Handle, &r, false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetHoveredLink(int l)
{
	if(FHoveredLink != l)
	{
		DoubleBuffered = true;
		InvalidateLink(FHoveredLink);
		FHoveredLink = l;
		InvalidateLink(FHoveredLink);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetFocusedLink(int l)
{
	if(FFocusedLink != l)
	{
		DoubleBuffered = true;
		InvalidateLink(FFocusedLink);
		FFocusedLink = l;
		InvalidateLink(FFocusedLink);
		if(l != -1) CaretPos = -1; // link and caret are exclusive
	}
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::IsLinkAt(int linknum, int x, int y)
{
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	if(linknum < 0 || linknum >= (int)links.size()) return false;

	const tTVPWaveLoopLink & link = links[linknum];
	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;

	int x_from;
	int x_to;
	int y_link = y_start + link.LinkTier * TVP_LGD_TIER_HEIGHT +
		TVP_LGD_TIER_HEIGHT - 3 - TVP_LGD_TIER_HEIGHT / 2;

	x_from = SampleToPixel(link.From - Start);
	x_to   = SampleToPixel(link.To   - Start);
	if(x_from > x_to)
		std::swap(x_from, x_to);

	return x >= x_from && x <= x_to &&
		y >= y_link && y < y_link + TVP_LGD_TIER_HEIGHT;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::GetLinkAt(int x, int y)
{
	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;
	if(y < y_start) return -1;

	// give priority to current focuesd link
	if(FFocusedLink != -1)
		if(IsLinkAt(FFocusedLink, x, y)) return FFocusedLink;

	// walk through
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	for(unsigned int i = 0; i < links.size(); i++)
	{
		if((int)i != FFocusedLink && IsLinkAt(i, x, y)) return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
#define MARK_GRIP_MARGIN 2
bool __fastcall TWaveView::IsLinkWaveMarkAt(int x, int linknum, bool &from_or_to)
{
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	const tTVPWaveLoopLink & link = links[linknum];

	int from_pos = SampleToPixel(link.From - FStart);
	int to_pos = SampleToPixel(link.To - FStart);

	bool from_match = false;
	bool to_match = false;
	if(from_pos - MARK_GRIP_MARGIN <= x && x <= from_pos + MARK_GRIP_MARGIN)
		from_match = true;

	if(to_pos - MARK_GRIP_MARGIN <= x && x <= to_pos + MARK_GRIP_MARGIN)
		to_match = true;

	if(to_match && from_match)
	{
		// both from and to are matched
		// check which one should be taken

		int center = (from_pos + to_pos) / 2;
		if(center <= x)
		{
			if(link.From > link.To)
				to_match = false;
			else
				from_match = false;
		}
		else
		{
			if(link.From > link.To)
				from_match = false;
			else
				to_match = false;
		}
	}

	if(to_match || from_match)
	{
		from_or_to = to_match;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::GetLinkWaveMarkAt(int x, int &linknum, bool &from_or_to)
{
	// give priority to focused link
	if(FFocusedLink != -1)
	{
		if(IsLinkWaveMarkAt(x, FFocusedLink, from_or_to))
		{
			linknum = FFocusedLink;
			return true;
		}
	}

	// for each links
	std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

	for(unsigned int i = 0; i < links.size(); i++)
	{
		if((int)i != FFocusedLink)
		{
			if(IsLinkWaveMarkAt(x, i, from_or_to))
			{
				linknum = i;
				return true;
			}
		}
	}

	return false;
}
//---------------------------------------------------------------------------
int TWaveView::MouseXPosToSamplePos(int x)
{
	int least = SampleToPixel(1);
	x += least / 2;
	return PixelToSample(x) + FStart;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseDown(TMouseButton button, TShiftState shift, int x, int y)
{
	if(!FReader || !FReader->ReadDone) return;

	// mouse downed
	int foot_size = GetFootSize();
	int foot_start = ClientHeight - foot_size;

	if(button == mbLeft)
	{
		if(y < foot_start)
		{
			int linknum = 0;
			bool from_or_to = false;
			if(GetLinkWaveMarkAt(x, linknum, from_or_to))
			{
				LastMouseDownX = x;
				LastMouseDownLinkNum = linknum;
				LastMouseDownLinkFromOrTo = from_or_to;
				DraggingState = dsMouseDown;
			}
			else
			{
/*
				if(FOnWaveLButtonDown)
				{
					FOnWaveLButtonDown(this, MouseXPosToSamplePos(x));
				}
*/
				CaretPos = MouseXPosToSamplePos(x);
			}
		}
		else
		{
/*
			if(FOnLinkLButtonDown)
			{
				FOnLinkLButtonDown(this, GetLinkAt(x, y));
			}
*/
			FocusedLink = GetLinkAt(x, y);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseMove(TShiftState shift, int x, int y)
{
	if(!FReader || !FReader->ReadDone)
	{
		Cursor = crDefault;
		return;
	}

	// mouse moved
	if(DraggingState != dsNone)
	{
		// dragging
		if(DraggingState == dsMouseDown)
		{
			if(LastMouseDownX != x)
			{
				// dragging started
				DraggingState = dsDragging;
				FocusedLink = LastMouseDownLinkNum;
			}
		}
		/* no else here */
		if(DraggingState == dsDragging)
		{
			// enable double buffering
			DoubleBuffered = true;

			// dragging
			std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

			tTVPWaveLoopLink &link = links[LastMouseDownLinkNum];

			InvalidateLink(LastMouseDownLinkNum);
			int pos = MouseXPosToSamplePos(x);
			if(!LastMouseDownLinkFromOrTo)
			{
				// 'from'
				link.From = pos;
			}
			else
			{
				// 'to'
				link.To = pos;
			}
			NotifyLinkChanged();
			InvalidateLink(LastMouseDownLinkNum);
		}
	}
	else
	{

		int foot_size = GetFootSize();
		int foot_start = ClientHeight - foot_size;

		if(y < foot_start)
		{
			int linknum = 0;
			bool from_or_to = false;
			if(GetLinkWaveMarkAt(x, linknum, from_or_to))
			{
				HoveredLink = linknum;
				Cursor = crSizeWE;
			}
			else
			{
				HoveredLink = -1;
				Cursor = crIBeam;
			}
		}
		else
		{
			HoveredLink = GetLinkAt(x, y);
			Cursor = crArrow;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseUp(TMouseButton button, TShiftState shift, int x, int y)
{
	// mouse up
	if(DraggingState != dsNone)
	{
		if(DraggingState == dsMouseDown)
		{
			// set caret to current from or to position
			std::vector<tTVPWaveLoopLink> & links = /**/ GetLinks(); /**/

			tTVPWaveLoopLink &link = links[LastMouseDownLinkNum];

			CaretPos = LastMouseDownLinkFromOrTo ? link.To : link.From;
		}
		DraggingState = dsNone;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseLeave()
{
	// mouse leaved
	HoveredLink = -1;
}
//---------------------------------------------------------------------------

