//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WaveDispUnit.h"
#include "WaveReaderUnit.h"

#define TIME_BAR_HEIGHT 16
#define TIME_BAR_CHAR_SIZE 12
#define ENTIRE_BAR_HEIGHT 12


//---------------------------------------------------------------------------
__fastcall TEntireBar::TEntireBar(TWinControl *owner) : TPanel(owner)
{
	FOwner = (TWaveDisplayer*)(owner);
	DoubleBuffered = true;
	Brush->Color = clWindow;
	Cursor = crArrow;
}
//---------------------------------------------------------------------------
__fastcall TEntireBar::~TEntireBar()
{

}
//---------------------------------------------------------------------------
void __fastcall TEntireBar::Paint(void)
{
	if(!FOwner->FWaveReader || !FOwner->FWaveReader->ReadDone)
	{
		// 読み込み終わっていない
		TRect r = Canvas->ClipRect;
		Canvas->Brush->Style = bsSolid;
		Canvas->Brush->Color = clAppWorkSpace;
		Canvas->FillRect(r);
		return;
	}

	// 背景
	TRect r;
	r.Left = 0;
	r.Top = 0;
	r.Right = ClientWidth;
	r.Bottom = ClientHeight;
	Canvas->Brush->Style = bsSolid;
	Canvas->Brush->Color = clWindow;
	Canvas->FillRect(r);


	// 選択範囲
	int s = FOwner->FSelStart;
	int e = FOwner->FSelEnd;
	if(s!=e)
	{
		s= (__int64)s * (__int64)ClientWidth / FOwner->FWaveReader->NumSamples;
		e= (__int64)e * (__int64)ClientWidth / FOwner->FWaveReader->NumSamples;

		r.Left = s;
		r.Top = 3;
		r.Right = e+1;
		r.Bottom = ClientHeight-4;
		Canvas->Brush->Color = clHighlight;
		Canvas->FillRect(r);

	}

	// マーカー
	Canvas->Pen->Color = clBtnText;
	int p = FOwner->Marker;
	if(p!=-1)
	{
		p = (__int64)p* (__int64)ClientWidth / FOwner->FWaveReader->NumSamples;
		r.left = p;
		r.top = 0;
		r.right = p+1;
		r.bottom = ClientHeight;

		InvertRect(Canvas->Handle, &r);
	}

	// ビュー範囲
	p = FOwner->GetViewOrigin();
	p = (__int64)p* (__int64)ClientWidth / FOwner->FWaveReader->NumSamples;
	r.left = p;
	r.top = 1;
	r.right = p+2;
	r.bottom = ClientHeight-2;
	InvertRect(Canvas->Handle, &r);
	r.left = p+1;
	r.top = 2;
	r.right = p+2;
	r.bottom = ClientHeight-3;
	InvertRect(Canvas->Handle, &r);

	p = FOwner->GetViewOrigin() + FOwner->GetViewRange();
	p = (__int64)p* (__int64)ClientWidth / FOwner->FWaveReader->NumSamples;
	p++;
	r.left = p-2;
	r.top = 1;
	r.right = p;
	r.bottom = ClientHeight-2;
	InvertRect(Canvas->Handle, &r);
	r.left = p-2;
	r.top = 2;
	r.right = p-1;
	r.bottom = ClientHeight-3;
	InvertRect(Canvas->Handle, &r);

	Canvas->Pen->Color = clWindowText;
	Canvas->MoveTo(0, ClientHeight-1);
	Canvas->LineTo(ClientWidth, ClientHeight-1);

}
//---------------------------------------------------------------------------
void __fastcall TEntireBar::MouseDown(TMouseButton button, TShiftState shift, int x, int y)
{
	if(FOwner->FWaveReader && FOwner->FWaveReader->ReadDone)
	{
		FOwner->ScrollToAndCancelFollow(
			(__int64)x*(__int64)FOwner->FWaveReader->NumSamples / ClientWidth);
	}

	TPanel::MouseDown(button, shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TEntireBar::MouseMove(TShiftState shift, int x, int y)
{
	if(FOwner->FWaveReader && FOwner->FWaveReader->ReadDone)
	{
		if(shift.Contains(ssLeft))
		{
			FOwner->ScrollToAndCancelFollow(
				(__int64)x*(__int64)FOwner->FWaveReader->NumSamples / ClientWidth);
		}
	}

	TPanel::MouseMove(shift, x, y);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TWaveDisplayer::TWaveDisplayer(TWinControl *owner)  : TCustomControl(owner)
{
	FFirstPaint = true;
	FMagnify = 512;
	FMouseDown = false;
	FMouseDrag = false;
	FShowTimeBar = false;
	FShowEntireBar = true;
	Brush->Style = bsClear;
	Brush->Color = clNone;
	FMarker = -1;
	FFollowMarker = true;
	FWaitingFollowing = true;
	LastMarker = -1;
	FOnFollowCanceled = NULL;
	FOnSelectionChanged = NULL;
	FOnMouseLeave = NULL;
	FWaveReader = NULL;
//	DoubleBuffered = true;

	EntireBar = new TEntireBar(this);
	EntireBar->Parent = this;
	EntireBar->Left = 0;
	EntireBar->Top = 0;
	EntireBar->Height=ENTIRE_BAR_HEIGHT;
	EntireBar->Align = alTop;
	EntireBar->Anchors = TAnchors() << akTop << akLeft << akRight;
	EntireBar->Visible = true;
}
//---------------------------------------------------------------------------
__fastcall TWaveDisplayer::~TWaveDisplayer()
{
	delete EntireBar;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::CreateParams(TCreateParams &params)
{
	TCustomControl::CreateParams(params);
	params.Style |= WS_HSCROLL;
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::ChannelFromMousePos(int y)
{
	if(!FWaveReader || !FWaveReader->ReadDone) return -1;
	int starty = (FShowTimeBar?TIME_BAR_HEIGHT:0) +
		(FShowEntireBar ? ENTIRE_BAR_HEIGHT:0);
	if(y < starty) return -1;
	y -= starty;
	y /= (ClientHeight - starty) / FWaveReader->Channels;
	if(y >= FWaveReader->Channels) y = -1;
	return y;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::Paint(void)
{
	if(FFirstPaint)
	{
		FFirstPaint = false;
		SetScrollRange();
	}

	if(!FWaveReader || !FWaveReader->ReadDone)
	{
		// 読み込み終わっていない
		TRect r = Canvas->ClipRect;
		Canvas->Brush->Style = bsSolid;
		Canvas->Brush->Color = clAppWorkSpace;
		Canvas->FillRect(r);
		return;
	}


	int sp = GetViewOrigin();

	if(FShowTimeBar)
	{
	/*
		// タイムバーを描画
		TRect r;
		r.Left = Canvas->ClipRect.Left;
		r.Top = 0;
		r.Right = Canvas->ClipRect.Right;
		r.Bottom = TIME_BAR_HEIGHT;
		Canvas->Brush->Color = clBtnFace;
		Canvas->Brush->Style = bsSolid;
		Canvas->FillRect(r);
		Canvas->Pen->Color = clBtnShadow;
		Canvas->MoveTo(Canvas->ClipRect.Left, TIME_BAR_HEIGHT-1);
		Canvas->LineTo(Canvas->ClipRect.Right, TIME_BAR_HEIGHT-1);

		Canvas->Pen->Color = clBtnText;

		int ms = 0;
		while(true)
		{
			int p = (__int64)ms * (__int64)FWaveReader->Frequency / 1000L; // sample pos

			if(p >= FWaveReader->NumSamples) break;

			p -= sp;
			p/=FMagnify;

			Canvas->MoveTo(p, TIME_BAR_HEIGHT-TIME_BAR_CHAR_SIZE);
			Canvas->LineTo(p, TIME_BAR_HEIGHT);

			ms+=1000;
		}
	*/
	}


	int starty = (FShowTimeBar?TIME_BAR_HEIGHT:0) +
		(FShowEntireBar ? ENTIRE_BAR_HEIGHT:0);

	if(FSelEnd - FSelStart == 0)
	{
		// 選択領域無し
		TRect r;
		r.Left = Canvas->ClipRect.Left;
		r.Top = starty;
		r.Right = Canvas->ClipRect.Right;
		r.Bottom = ClientHeight;
		Canvas->Brush->Color = clWindow;
		Canvas->Brush->Style = bsSolid;
		Canvas->FillRect(r);
	}
	else
	{
		// 選択領域あり
		int sl = (FSelStart - sp) / FMagnify;
		int sr = (FSelEnd - sp) / FMagnify;

		if(FSelEnd - FSelStart < FMagnify) sr++;

		if(sl < Canvas->ClipRect.Left) sl = Canvas->ClipRect.Left;
		if(sl >= Canvas->ClipRect.Right) sl = Canvas->ClipRect.Right;
		if(sr < Canvas->ClipRect.Left) sr = Canvas->ClipRect.Left;
		if(sr >= Canvas->ClipRect.Right) sr = Canvas->ClipRect.Right;

		TRect r;
		r.Top = starty;
		r.Bottom = ClientHeight;
		Canvas->Brush->Style = bsSolid;
		if(Canvas->ClipRect.Left < sl)
		{
			r.Left = Canvas->ClipRect.Left;
			r.Right = sl;
			Canvas->Brush->Color = clWindow;
			Canvas->FillRect(r);
		}
		if(sr > sl)
		{
			r.Left = sl;
			r.Right = sr;
			Canvas->Brush->Color = clHighlight;
			Canvas->FillRect(r);
		}
		if(sr < Canvas->ClipRect.Right)
		{
			r.Left = sr;
			r.Right = Canvas->ClipRect.Right;
			Canvas->Brush->Color = clWindow;
			Canvas->FillRect(r);
		}
	}

	int mp = (FMarker-sp) / FMagnify;

	int h[8];
	int y[8];
	int ch;
	for(ch=0; ch<FWaveReader->Channels; ch++)
	{
		h[ch] = (ClientHeight-starty) / FWaveReader->Channels;
		y[ch] = h[ch] * ch + starty;
	}

	TColor fg = clWindowText;
	Canvas->Pen->Color = fg;

	int x;
	int p;
	for(x=Canvas->ClipRect.Left; x<Canvas->ClipRect.Right; x++)
	{
		p = x*FMagnify + sp;
		if(p>=FWaveReader->NumSamples)
		{
			TRect r;
			r.Left = x;
			r.Top = y[0];
			r.Right = ClientWidth;
			r.Bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = clAppWorkSpace;
			Canvas->FillRect(r);
			break;
		}

		if(FSelStart <= p && p < FSelEnd)
		{
			if(fg != clHighlightText)
			{
				Canvas->Pen->Color = clHighlightText;
				fg = clHighlightText;
			}
		}
		else
		{
			if(fg != clWindowText)
			{
				Canvas->Pen->Color = clWindowText;
				fg = clWindowText;
			}
		}

		int ch;
		for(ch = 0; ch < FWaveReader->Channels; ch++)
		{
			int high, low;
			FWaveReader->GetPeak(high, low, p, ch, FMagnify);
			high *= h[0];
			high /= 65536;
			low *= h[0];
			low /= 65536;

			if(FMagnify > 16)
			{
				Canvas->MoveTo(x, low+ y[ch] + h[ch]/2);
				Canvas->LineTo(x, high+ y[ch] + h[ch]/2 +1);
			}
			else
			{
				if(low>0) low=0;
				if(high<0) high=0;
				Canvas->MoveTo(x, low+ y[ch] + h[ch]/2);
				Canvas->LineTo(x, high+ y[ch] + h[ch]/2);
			}
		}
		if(mp==x && FMarker != -1)
		{
			RECT r;
			r.left = x;
			r.top = y[0];
			r.right = x+1;
			r.bottom = ClientHeight;
			InvertRect(Canvas->Handle, &r);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetWaveReader(TWaveReader *wavereader)
{
	FWaveReader = wavereader;
	SetScrollRange();
	InvalidateRect(Handle, NULL, false);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetShowTimeBar(bool b)
{
	if(FShowTimeBar != b)
	{
		FShowTimeBar = b;
		InvalidateRect(Handle, NULL, false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetShowEntireBar(bool b)
{
	if(FShowEntireBar != b)
	{
		FShowEntireBar = b;
		EntireBar->Visible = FShowEntireBar;
		InvalidateRect(Handle, NULL, false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetScrollRange(void)
{
	if(!FWaveReader || !FWaveReader->ReadDone)
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
	si.nMax = FWaveReader->NumSamples;
	si.nPage = ClientWidth*FMagnify;
	si.nPos = 0;

	SetScrollInfo(Handle, SB_HORZ, &si, true);
	EnableScrollBar(Handle, SB_HORZ, ESB_ENABLE_BOTH);
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::GetViewOrigin(void)
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(Handle, SB_HORZ, &si);

	return (int)(si.nPos/FMagnify)*FMagnify;
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::GetViewRange(void)
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE;
	GetScrollInfo(Handle, SB_HORZ, &si);

	return (int)((si.nPage)/FMagnify)*FMagnify;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::WMHScroll(TWMHScroll &msg)
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
		si.nPos = si.nPos - ClientWidth*FMagnify/20;
		if(si.nPos<0) si.nPos=0;
	}
	else if(msg.ScrollCode == SB_LINERIGHT)
	{
		si.nPos = si.nPos + ClientWidth*FMagnify/20;
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

	if(FFollowMarker)
	{
		if(FOnFollowCanceled) FOnFollowCanceled(this);
		FFollowMarker = false;
		SetWaitingFollowing();
	}



	si.fMask = SIF_POS;

	SetScrollInfo(Handle, SB_HORZ, &si, true);
	RECT r;
	r.left = 0;
	r.top = FShowEntireBar ? ENTIRE_BAR_HEIGHT : 0;
	r.right = ClientWidth;
	r.bottom = ClientHeight;

	ScrollWindowEx(Handle, (int)(org / FMagnify)-(int)(si.nPos / FMagnify), 0,
		&r, NULL, NULL, NULL, SW_INVALIDATE);

	InvalidateRect(EntireBar->Handle, NULL, false);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::CMMouseLeave(TMessage &msg)
{
	if(FOnMouseLeave) FOnMouseLeave(this);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetMagnify(int n)
{
	int center = ClientWidth/2 * FMagnify + GetViewOrigin();
	FMagnify = n;
//	if(n<=16) DoubleBuffered = true; else DoubleBuffered = false;
	SetScrollRange();
	ScrollTo(center);
	InvalidateRect(EntireBar->Handle, NULL, false);
 	InvalidateRect(Handle, NULL, false);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetSelStart(int n)
{
	if(FSelStart != n)
	{
		FSelStart = n;
		InvalidateRect(EntireBar->Handle, NULL, false);
		InvalidateRect(Handle, NULL, false);
	}
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::GetSelStart()
{
	return FSelStart;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetSelLength(int n)
{
	if(FSelStart + n != FSelEnd)
	{
		FSelEnd = n + FSelStart;
		InvalidateRect(EntireBar->Handle, NULL, false);
		InvalidateRect(Handle, NULL, false);
	}
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::GetSelLength()
{
	return FSelEnd - FSelStart;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetSelection(int start, int end)
{
	if(start > end)
	{
		int t;
		t=start; start=end; end=t;
	}

	bool flag = false;

	if(start != FSelStart)
	{
		flag = true;
		int s = (FSelStart - GetViewOrigin()) / FMagnify;
		int e = (start - GetViewOrigin()) / FMagnify;
		if(e<s)
		{
			int t;
			t=s; s=e; e=t;
		}

		RECT r;
		r.left = s-1;
		r.top = FShowEntireBar ? ENTIRE_BAR_HEIGHT : 0;
		r.right = e+1;
		r.bottom = ClientHeight;

		FSelStart = start;
		InvalidateRect(Handle, &r, false);
		InvalidateRect(EntireBar->Handle, NULL, false);
	}

	if(end != FSelEnd)
	{
		flag = true;
		int s = (FSelEnd - GetViewOrigin()) / FMagnify;
		int e = (end - GetViewOrigin()) / FMagnify;
		if(e<s)
		{
			int t;
			t=s; s=e; e=t;
		}

		RECT r;
		r.left = s-1;
		r.top = FShowEntireBar ? ENTIRE_BAR_HEIGHT : 0;
		r.right = e+1;
		r.bottom = ClientHeight;

		FSelEnd = end;
		InvalidateRect(Handle, &r, false);
		InvalidateRect(EntireBar->Handle, NULL, false);
	}

	if(FSelEnd >= FWaveReader->NumSamples) FSelEnd = FWaveReader->NumSamples;

	if(flag)
	{
		if(FOnSelectionChanged)
			FOnSelectionChanged(this);
	}

}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetFollowMarker(bool b)
{
	if(FFollowMarker != b)
	{
		FFollowMarker = b;
		SetWaitingFollowing();

		if(FFollowMarker && FMarker!=-1)
		{
			int n = FMarker;
			n-=GetViewOrigin();
			n/=FMagnify;
			if(n < 0 || n >= ClientWidth)
			{
				int n = FMarker + ClientWidth*3/7 *FMagnify;
				ScrollTo(n<0?0:n);
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::SetMarker(int n)
{
	if(FMarker != n)
	{
		if(FWaveReader && FWaveReader->ReadDone)
		{
			if(n>=FWaveReader->NumSamples) n = -1;
		}

		if(n == -1)	SetWaitingFollowing();

		UpdateMarker(FMarker, n);
		if(FMarker > n) SetWaitingFollowing();

		FMarker = n;

		if(FFollowMarker && FMarker!=-1)
		{
			n-=GetViewOrigin();
			n/=FMagnify;
			if(FWaitingFollowing)
			{
				if(n > ClientWidth *6/7)
				{
					FWaitingFollowing = false;
					LastMarker = FMarker;
				}
			}

			if(n < 0 || n >= ClientWidth)
			{
				int n = FMarker + ClientWidth*3/7 *FMagnify;
				ScrollTo(n<0?0:n);
			}
			else
			{
				if(!FWaitingFollowing)
				{
					ScrollTo(FMarker, false);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::UpdateMarker(int op, int np)
{
	if(op!=-1)
	{
		op-=GetViewOrigin();
		op/=FMagnify;

		RECT r;
		r.left = op;  
		r.top = FShowEntireBar ? ENTIRE_BAR_HEIGHT : 0;
		r.right = op+1;
		r.bottom = ClientHeight;
		InvalidateRect(Handle, &r, false);
		InvalidateRect(EntireBar->Handle, NULL, false);
	}

	if(np!=-1)
	{
		np-=GetViewOrigin();
		np/=FMagnify;

		RECT r;
		r.left = np;
		r.top = FShowEntireBar ? ENTIRE_BAR_HEIGHT : 0;
		r.right = np+1;
		r.bottom = ClientHeight;
		InvalidateRect(Handle, &r, false);
		InvalidateRect(EntireBar->Handle, NULL, false);
	}

}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::GetDragPos(int x, int *ofs)
{
	if(FSelEnd - FSelStart  > FMagnify * 6)
	{
		int p = x * FMagnify + GetViewOrigin();

		if(p + FMagnify*3 >= FSelStart && p - FMagnify*3 <= FSelStart)
		{
			if(ofs) *ofs = p - FSelStart;
			return -1;
		}
		else if(p + FMagnify*3 >= FSelEnd && p - FMagnify*3 <= FSelEnd)
		{
			if(ofs) *ofs = p - FSelEnd;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::MouseDown(TMouseButton button, TShiftState shift,
	int x, int y)
{
	if(FWaveReader && FWaveReader->ReadDone)
	{

		if(button == mbLeft)
		{
			FMouseDown = true;
			if(FOnStartSelect) FOnStartSelect(this);
			FFirstMouseDownPos.x = x;
			FFirstMouseDownPos.y = y;
			FFirstDragPos = GetDragPos(x, &FMouseDragAdj);
		}
	}

	TCustomControl::MouseDown(button, shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::MouseUp(TMouseButton button, TShiftState shift,
	int x, int y)
{
	if(FWaveReader && FWaveReader->ReadDone)
	{
		if(button == mbLeft)
		{
			if(FMouseDrag)
			{
				if(FOnEndSelect) FOnEndSelect(this);
			}
			FMouseDown = false;
			FMouseDrag = false;
		}
	}

	TCustomControl::MouseUp(button, shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::MouseMove(TShiftState shift, int x, int y)
{
	if(FWaveReader && FWaveReader->ReadDone)
	{
		if(GetDragPos(x) != 0)
			Cursor = crHSplit;
		else
			Cursor = crIBeam;

		if(FMouseDown && !FMouseDrag)
		{
			int dx = 1; // GetSystemMetrics(SM_CXDRAG);
			int dy = 1; // GetSystemMetrics(SM_CYDRAG);

			POINT &p = FFirstMouseDownPos;
			if(p.x - dx >= x || p.x + dx <= x ||
			   p.y - dy >= y || p.y + dy <= y)
			{
				if(!FMouseDrag)
				{
					if(FFollowMarker)
					{
						if(FOnFollowCanceled) FOnFollowCanceled(this);
						FFollowMarker = false;
					}
				}

				FMouseDrag = true;
				FMouseSelStart = FFirstMouseDownPos.x * Magnify + GetViewOrigin();
			}
		}

		if(FMouseDrag)
		{
			if(FFirstDragPos == 0)
			{
				int start = FMouseSelStart;
				int p = x * FMagnify + GetViewOrigin();
				if(p<0) p = 0;
				if(p>=FWaveReader->NumSamples) p = FWaveReader->NumSamples;
				SetSelection(start, p);
			}
			else
			{
				int start = FFirstDragPos>0 ? FSelStart : FSelEnd;
				int p = x* FMagnify + GetViewOrigin() - FMouseDragAdj;
				if(p<0) p = 0;
				if(p>=FWaveReader->NumSamples) p = FWaveReader->NumSamples;
				if(FFirstDragPos >0)
				{
					if(p<start) FFirstDragPos = -1;
				}
				else
				{
					if(p>start) FFirstDragPos = 1;
				}

				SetSelection(start, p);
			}

			if(x <=0)
			{
				TWMHScroll msg;
				msg.ScrollCode = SB_LINELEFT;
				WMHScroll(msg);
			}

			if(x >= ClientWidth -1)
			{

				TWMHScroll msg;
				msg.ScrollCode = SB_LINERIGHT;
				WMHScroll(msg);
			}
		}
	}
	else
	{
		Cursor = crDefault;
	}
	TCustomControl::MouseMove(shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::Resize(void)
{
	SetScrollRange();

	TCustomControl::Resize();
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::MousePosToSamplePos(int x)
{
	if(!FWaveReader) return 0;
	int pos = x*FMagnify + GetViewOrigin();
	if(pos<0) pos = -1;
	if(pos>=FWaveReader->NumSamples) pos=-1;
	return pos;
}
//---------------------------------------------------------------------------
int __fastcall TWaveDisplayer::LastClickedMousePosToSamplePos()
{
	return MousePosToSamplePos(FFirstMouseDownPos.x);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::ScrollTo(int pos, bool force)
{
	// pos がみえる範囲に(中央に据える位置まで)スクロールする

	int cpos = ClientWidth/2 * FMagnify;
	int xpos = pos - cpos;

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

	int org = si.nPos;

	if(force)
	{
		si.nPos = xpos;
	}
	else
	{
		if(LastMarker == -1) { LastMarker = FMarker; return; }
		int dspeed = xpos - si.nPos;
		if(dspeed < 0) dspeed = -dspeed;
		dspeed = dspeed * 80 / (ClientWidth/2 * FMagnify) + 10;
		int diff = (FMarker - LastMarker) +
			(FMarker - LastMarker)* dspeed * FMagnify / WaveReader->Frequency;
		if(xpos > si.nPos)
		{
			si.nPos += diff;
			if(si.nPos > xpos) si.nPos = xpos;
		}
		else
		{
//			si.nPos -= diff;
//			if(si.nPos < xpos) si.nPos = xpos;
		}
		LastMarker = FMarker;
	}

	if(si.nPos<0) si.nPos=0;
	if((int)(si.nPos) >= (int)(si.nMax - si.nPage)) si.nPos=si.nMax- si.nPage;

	si.fMask = SIF_POS;
	SetScrollInfo(Handle, SB_HORZ, &si, true);
	RECT r;
	r.left = 0;
	r.top = FShowEntireBar ? ENTIRE_BAR_HEIGHT : 0;
	r.right = ClientWidth;
	r.bottom = ClientHeight;
	ScrollWindowEx(Handle, (int)(org / FMagnify)-(int)(si.nPos / FMagnify), 0,
		&r, NULL, NULL, NULL, SW_INVALIDATE);

	InvalidateRect(EntireBar->Handle, NULL, false);
}
//---------------------------------------------------------------------------
void __fastcall TWaveDisplayer::ScrollToAndCancelFollow(int pos)
{
	ScrollTo(pos);

	if(FFollowMarker)
	{
		if(FOnFollowCanceled) FOnFollowCanceled(this);
		FFollowMarker = false;
		SetWaitingFollowing();
	}
}
//---------------------------------------------------------------------------



#pragma package(smart_init)
