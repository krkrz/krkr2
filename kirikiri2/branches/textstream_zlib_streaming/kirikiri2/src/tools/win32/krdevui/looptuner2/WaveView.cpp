//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <math.h>
#include "WaveView.h"
#include "WaveReader.h"
#include "ColorScheme.h"

//---------------------------------------------------------------------------
// constants
//---------------------------------------------------------------------------
const AnsiString C_NEW_LABEL_BASE_NAME = "ラベル";
const AnsiString C_LINK_HINT =
	"リンク元: %s\n"	// link from
	"リンク先: %s\n"	// link to
	"距離 : %s\n"		// distance
	"条件 : %s"   		// condition
	;
const AnsiString C_LINK_COND_CODES[] = {
	"条件無し",
	"%d 番のフラグが %d と同じとき",
	"%d 番のフラグが %d でないとき",
	"%d 番のフラグが %d より大きいとき",
	"%d 番のフラグが %d 以上のとき",
	"%d 番のフラグが %d より小さいとき",
	"%d 番のフラグが %d 以下のとき"
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const int LinkArrowSize = 4;
const int LinkDirectionArrowWidth = 10;
const int LinkDirectionInterval = 200;
//---------------------------------------------------------------------------
const int MaxUndoLevel = 20;
const int MaxMagnify = 3;
const int MinMagnify = -16;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TWaveView
//---------------------------------------------------------------------------
__fastcall TWaveView::TWaveView(Classes::TComponent* AOwner) :
	TCustomControl(AOwner)
{
	FBlinkTimer = new TTimer(this);
	FBlinkTimer->OnTimer = OnBlinkTimer;
	FBlinkTimer->Interval = GetCaretBlinkTime();
	FBlinkTimer->Enabled = true;

	DragScrollTimer = new TTimer(this);
	DragScrollTimer->OnTimer = OnDragScrollTimer;
	DragScrollTimer->Interval = 100;
	DragScrollTimer->Enabled = false;

	ClearAll();
}
//---------------------------------------------------------------------------
__fastcall TWaveView::~TWaveView()
{
	delete DragScrollTimer;
	delete FBlinkTimer;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::ClearAll()
{
	Color = C_CLIENT;

	FReader = NULL;
	FMagnify = -12;
	FStart = 0;

	FDoubleBufferEnabled = true;
	FFollowingMarker = true;
	FWaitingMarker = true;
	FSoftCenteringStartTick = 0;
	FSoftCenteringPos = 0;
	FOnStopFollowingMarker = NULL;
	FOnRangeChanged = NULL;
	FOnMarkerStateChanged = NULL;

	FCaretPos = 0;
	FShowCaret = false;
	FCaretVisiblePhase = true;
	FOnCaretStateChanged = NULL;

	FMinRulerMajorWidth = 0;
	FMinRulerMajorHeight = 0;
	FRulerUnit = 0;
	FShowRuler = true;
	FMarkerPos = -1;
	Canvas->Font->Height = -12;

	FOnNotifyPopup = NULL;
	FOnShowCaret = NULL;
	FOnLinkSelected = NULL;
	FOnLabelSelected = NULL;
	FOnSelectionLost = NULL;
	FOnLinkModified = NULL;
	FInOnLinkModified = false; // to prevent re-entrering
	FOnLabelModified = NULL;
	FInOnLabelModified = false; // to prevent re-entrering

	FUndoStack.clear();
	FUndoLevel = 0;

	Links.clear();
	FShowLinks = true;
	FLinkTierCount = 0;
	FHoveredLink = -1; // -1 for not hovered
	FFocusedLink = -1; // -1 for not focused
	NotifyLinkChanged();

	Labels.clear();
	FShowLabels = true;
	FHoveredLabel = -1; // -1 for not hovered
	FFocusedLabel = -1; // -1 for not focused
	LabelTextHeight = -1;
	NotifyLabelChanged();

	FOnWaveDoubleClick = NULL;
	FOnLinkDoubleClick = NULL;
	FOnLabelDoubleClick = NULL;

	LastMouseDownX = -1;
	LastMouseDownY = -1;
	LastMouseDownPosOffset = 0;
	LastMouseMoveX = -1;
	PopupType = "";
	DraggingState = dsNone;
	DisableNextMouseDown = false;

	LastClickedPos[0] = LastClickedPos[1] = 0;

	Cursor = crIBeam;

	ShowHint = true;

	FModified = false;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::Paint(void)
{
	DWORD start = GetTickCount();

	DrawWave(FStart, true);
	DrawLinks();
	DrawLabels();
	DrawCaret();

	DWORD end = GetTickCount();
	if(FDoubleBufferEnabled && end - start > 100)
	{
		// too heavy to double buffering
		// disable double buffering
		FDoubleBufferEnabled = false;
		DoubleBuffered = false;
	}
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
void __fastcall TWaveView::ResetAll()
{
	FReader = NULL;
	FUndoStack.clear();
	FMagnify = -12;
	FStart = 0;
	FHoveredLink = -1; // -1 for not hovered
	FFocusedLink = -1; // -1 for not focused
	FHoveredLabel = -1; // -1 for not hovered
	FFocusedLabel = -1; // -1 for not focused
	SetScrollBarRange();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::EraseRedo()
{
	// erase redo data
	if(CanRedo())
	{
		FUndoStack.erase(FUndoStack.begin() + FUndoLevel + 1, FUndoStack.end());
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::PushUndo()
{
	// erase redo
	EraseRedo();

	// remove old data
	if(FUndoStack.size() == (MaxUndoLevel+1))
	{
		FUndoStack.pop_front();
		FUndoLevel --;
	}

	// append new data
	tHistoryInfo info;
	info.Links = Links;
	info.Labels = Labels;
	FUndoStack.push_back(info);
	FUndoLevel ++;

	// set modified flag
	FModified = true;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::PushFirstUndoState()
{
	FUndoStack.clear();
	PushUndo();
	FUndoLevel = 0;
	FModified = false;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::Undo()
{
	// do undo
	if(CanUndo())
	{
		int linknum = FFocusedLink;
		int labelnum = FFocusedLabel;
		FocusedLink = -1;
		FocusedLabel = -1;
		HoveredLink = -1;
		HoveredLabel = -1;

		if(CanUndo())
		{
			FUndoLevel --;
			Links = FUndoStack[FUndoLevel].Links;
			Labels = FUndoStack[FUndoLevel].Labels;
		}

		if((int)Links.size() <= linknum) linknum = Links.size() - 1;
		FocusedLink = linknum;
		if((int)Labels.size() <= labelnum) labelnum = Labels.size() - 1;
		FocusedLabel = labelnum;

		NotifyLinkChanged();
		NotifyLabelChanged();
		DoubleBuffered = FDoubleBufferEnabled;
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::Redo()
{
	if(CanRedo())
	{
		int linknum = FFocusedLink;
		int labelnum = FFocusedLabel;
		FocusedLink = -1;
		FocusedLabel = -1;
		HoveredLink = -1;
		HoveredLabel = -1;

		if(CanRedo())
		{
			FUndoLevel ++;
			Links = FUndoStack[FUndoLevel].Links;
			Labels = FUndoStack[FUndoLevel].Labels;
		}

		if((int)Links.size() <= linknum) linknum = Links.size() - 1;
		FocusedLink = linknum;
		if((int)Labels.size() <= labelnum) labelnum = Labels.size() - 1;
		FocusedLabel = labelnum;

		NotifyLinkChanged();
		NotifyLabelChanged();
		DoubleBuffered = FDoubleBufferEnabled;
		Invalidate();
	}
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::CanUndo() const
{
	return FUndoLevel != 0;
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::CanRedo() const
{
	return FUndoStack.size() - FUndoLevel > 1;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DeleteItem()
{
	// delete current focused item
	if(FFocusedLink != -1)
	{
		int num = FocusedLink;
		FocusedLink = -1;
		HoveredLink = -1;
		Links.erase(Links.begin() + num);
		NotifyLinkChanged();
		DoubleBuffered = FDoubleBufferEnabled;
		Invalidate();
		PushUndo(); //==== push undo
	}
	else if(FFocusedLabel != -1)
	{
		int num = FocusedLabel;
		FocusedLabel = -1;
		HoveredLabel = -1;
//		InvalidateLabel(num);
		Labels.erase(Labels.begin() + num);
		NotifyLabelChanged();
		DoubleBuffered = false;
		Invalidate();
		PushUndo(); //==== push undo
	}
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::CanDeleteItem() const
{
	return FFocusedLink != -1 || FFocusedLabel != -1;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::ClearAllLabels()
{
	// delete all labels
	FocusedLabel = -1;
	HoveredLabel = -1;
	Labels.clear();
	NotifyLabelChanged();
	DoubleBuffered = false;
	Invalidate();
	PushUndo();
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

	FDoubleBufferEnabled = true;
	DoubleBuffered = FDoubleBufferEnabled;

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
	clwp = clwp / 100 * r;
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

	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_HORZ, &si);

	SetStart(si.nPos);

	if(FOnStopFollowingMarker) FOnStopFollowingMarker(this);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::WMSetFocus(TWMSetFocus &msg)
{
	::SetFocus(Handle);
	ResetCaretFocusState();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::WMKillFocus(TWMKillFocus &msg)
{
	ResetCaretFocusState();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::InvalidateCaret(int pos)
{
	DoubleBuffered = false;
	RECT r;
	r.top = GetHeadSize();
	r.bottom = ClientHeight - GetFootSize();
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
	if(pos < 0) pos = 0;
	if(pos >= FReader->NumSamples) pos = FReader->NumSamples - 1;

	if(pos != FCaretPos)
	{
		// invalidate new position and old position
		if(FShowCaret)
		{
			InvalidateCaret(FCaretPos);
			InvalidateCaret(pos);
		}

		if(pos >= 0)
		{
			FCaretVisiblePhase = true;
			FBlinkTimer->Enabled = false;
			FBlinkTimer->Enabled = FShowCaret;
		}
		FCaretPos = pos;
		if(FOnCaretStateChanged)
			FOnCaretStateChanged(this, FCaretPos, FShowCaret && FCaretVisiblePhase && Focused());
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetShowCaret(bool b)
{
	if(b != FShowCaret)
	{
		FShowCaret = b;

		InvalidateCaret(FCaretPos);
		if(FCaretPos >= 0)
		{
			FCaretVisiblePhase = true;
			FBlinkTimer->Enabled = b;
		}

		if(FShowCaret && FCaretPos >= 0)
		{
			FocusedLink = -1;
			FocusedLabel = -1; // these are exclusive
			Hint = "";
		}
		else
		{
			if(FOnSelectionLost) FOnSelectionLost(this);
		}

		if(FShowCaret && FOnShowCaret && FCaretPos >= 0)
		{
			FOnShowCaret(this, FCaretPos);
		}
		if(FOnCaretStateChanged)
			FOnCaretStateChanged(this, FCaretPos, FShowCaret && FCaretVisiblePhase && Focused());
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::OnBlinkTimer(TObject * sender)
{
	FCaretVisiblePhase = !FCaretVisiblePhase;
	InvalidateCaret(FCaretPos);
	if(FOnCaretStateChanged)
		FOnCaretStateChanged(this, FCaretPos, FShowCaret && FCaretVisiblePhase && Focused());
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::ResetCaretFocusState()
{
	InvalidateCaret(FCaretPos);
	FBlinkTimer->Enabled = Focused();
	if(FOnCaretStateChanged)
		FOnCaretStateChanged(this, FCaretPos, FShowCaret && FCaretVisiblePhase && Focused());
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawCaret()
{
	// draw caret
	if(!FReader || !FReader->ReadDone) return;

	if(FShowCaret && FCaretVisiblePhase && Focused())
	{
		int p_pos = SampleToPixel(FCaretPos - FStart);
		if(p_pos >= 0 && p_pos < ClientWidth)
		{
			RECT r;
			r.top = GetHeadSize();
			r.bottom = ClientHeight - GetFootSize();
			r.left = p_pos;
			r.right = p_pos + 1;
			InvertRect(Canvas->Handle, &r);
		}
	}

}
//---------------------------------------------------------------------------
int __fastcall TWaveView::GetAttentionPos()
{
	int center;
	int c_pos = SampleToPixel(FCaretPos - FStart);
	if(c_pos >= 0 && c_pos < ClientWidth)
		center = FCaretPos;
	else
		center = FStart + PixelToSample(ClientWidth >> 1);
	return center;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::FireRangeChanged()
{
	if(FOnRangeChanged && FReader)
	{
		int length;
		length = PixelToSample(ClientWidth);
		if(length >= FReader->NumSamples)
			length = FReader->NumSamples;
		FOnRangeChanged(this, FStart, length);
	}
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
	FireRangeChanged();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetMagnify(int m)
{
	// set magnification
	if(!FReader || !FReader->ReadDone) return;

	if(m > MaxMagnify) m = MaxMagnify;
	if(m < MinMagnify) m = MinMagnify;
	if(FMagnify != m)
	{
		int center = GetAttentionPos();
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
		NotifyLabelChanged();
		FireRangeChanged();
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
void __fastcall TWaveView::SetInitialMagnify()
{
	// set initial view magnify, which fits the current view width
	int m;
	for(m = MaxMagnify; m >= MinMagnify; m--)
	{
		int w;
		if(m <= 0)
		{
			// shrink
			w = FReader->NumSamples >> (-m);
		}
		else
		{
			// expand
			w = FReader->NumSamples << m;
		}
		if(w < ClientWidth) break;
	}

	SetMagnify(m);
}
//---------------------------------------------------------------------------
AnsiString __fastcall TWaveView::GetMagnifyString()
{
	AnsiString ret;
	if(FMagnify >= 0)
	{
		ret.sprintf("x%d", 1 << FMagnify);
		return ret;
	}
	else
	{
		ret.sprintf("/%d", 1 << (-FMagnify));
		return ret;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::EnsureView(int p, int length)
{
	// scroll the view (if needed) to ensure the point p .. (p+length) is visible
	int lim = PixelToSample(ClientWidth-1);

	if(lim * 0.9 < std::abs(length))
	{
		// the client width is less than the length
		if(length > 0)
			SetView(p, 1);
		else
			SetView(p, 99);
	}
	else
	{
		int lofs = p - Start;
		int rofs = p + length - Start;
		if(rofs < lofs) std::swap(rofs, lofs);

		if(lofs <= 0)
		{
			SetView(lofs + Start, 5);
		}
		else if(rofs >= lim)
		{
			SetView(rofs + Start, 95);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::GetPositionStringAt(int vx, int vy,
	AnsiString &pos, AnsiString &channel)
{
	// vx vy are in client coodinates
	if(!FReader || !FReader->ReadDone ||
		vx < 0 || vy < 0 || vx >= ClientWidth || vy >= ClientHeight)
	{
		pos = "-"; channel = "-";
		return;
	}

	int last = SampleToPixel(FReader->NumSamples - FStart);
	if(last <= vx)
	{
		pos = "-"; channel = "-";
		return;
	}

	pos = FReader->SamplePosToTimeString(PixelToSample(vx) + FStart);

	int head_size = GetHeadSize();
	int foot_size = GetFootSize();

	int one_height = ((ClientHeight - head_size - foot_size) / FReader->Channels) & ~1;

	int ch = (vy - head_size) / one_height;

	channel = FReader->GetChannelLabel(ch);
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
		if(FOnMarkerStateChanged) FOnMarkerStateChanged(this, FMarkerPos, FMarkerPos != -1);
	}

	// marker following
	if(FReader && FReader->ReadDone && FFollowingMarker && n != -1)
	{
		__int64 viewsamples = PixelToSample(ClientWidth);
		if(viewsamples * 1000 / FReader->Frequency < 500)
		{
			// too fast to track
			SetView(n);
			FWaitingMarker = false;
			DoubleBuffered = FDoubleBufferEnabled;
		}
		else
		{
			DoubleBuffered = false;

			int px = SampleToPixel(n - FStart);
			if(px < 0 || px >= ClientWidth)
			{
				SetView(n, 10);
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
void __fastcall TWaveView::SetFollowingMarker(bool b)
{
	if(FFollowingMarker != b)
	{
		FFollowingMarker = b;
		if(FFollowingMarker)
		{
			ResetMarkerFollow();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::ResetMarkerFollow()
{
	FWaitingMarker = true;
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
		i = j;
		j = t;
	}

	return
		a >= i && a <= j ||
		b >= i && b <= j ||
		i >= a && i <= b ||
		j >= a && j <= b;
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
	if(FShowRuler && FMinRulerMajorWidth == 0)
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
	#define		CONV_Y(v, ch) ((((v) * one_client_height) / -65536) + \
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
		if(FShowRuler)
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
	if(FShowRuler && Canvas->ClipRect.top < head_size)
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
						t / (1000 * 60 * 60),
						t / (1000 * 60) % 60,
						t / 1000 % 60);
				}
				else
				{
					sprintf(buf, "%02d:%02d:%02d.%03d",
						t / (1000 * 60 * 60),
						t / (1000 * 60) % 60,
						t / 1000 % 60,
						t % 1000);
				}
				Canvas->TextOut(ix+2, 0, buf);
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::NotifyLinkChanged()
{
	// re-check tier

	// re-check focused or hovered state
	if(FFocusedLink >= (int)Links.size()) FFocusedLink = -1;
	if(FHoveredLink >= (int)Links.size()) FHoveredLink = -1;
	Hint = "";

	// arrange link lines to ensure visibility and discrimination.

	//- mark indices
	for(unsigned int i = 0; i < Links.size(); i++) Links[i].Index = i;

	//- sort by link distance
	std::stable_sort(Links.begin(), Links.end(), tTVPWaveLoopLink::tSortByDistanceFuncObj());

	//- arrange Links by finding line intersections.
	int tier_count = 0;
	std::vector<std::vector<std::pair<int, int> > > tiers;
	for(unsigned int i = 0; i < Links.size(); i++)
	{
		// search free tier
		TTierMinMaxInfo cinfo;
		GetLinkMinMaxPixel(Links[i], cinfo);

		int found_link_tier = -1;
		int found_from_tier = 0;
		int found_to_tier   = 0;

		for(unsigned int j = 0; j < i; j++)
		{
			TTierMinMaxInfo jinfo;
			GetLinkMinMaxPixel(Links[j], jinfo);

			// for 'from'
			if(	PartIntersect(jinfo.ToMin, jinfo.ToMax, cinfo.FromMin, cinfo.FromMax) )
			{
				// intersection found
				found_from_tier = Links[j].ToTier + 1;
			}
			if( PartIntersect(jinfo.FromMin, jinfo.FromMax, cinfo.FromMin, cinfo.FromMax))
			{
				// intersection found
				found_from_tier = Links[j].FromTier + 1;
			}

			// for 'to'
			if( PartIntersect(jinfo.ToMin, jinfo.ToMax, cinfo.ToMin, cinfo.ToMax))
			{
				// intersection found
				found_to_tier = Links[j].ToTier + 1;
			}
			if(	PartIntersect(jinfo.FromMin, jinfo.FromMax, cinfo.ToMin, cinfo.ToMax))
			{
				// intersection found
				found_to_tier = Links[j].FromTier + 1;
			}
		}

		for(unsigned int i =
			found_from_tier > found_to_tier ?
				found_from_tier : found_to_tier; i < tiers.size(); i++)
		{
			// for link
			bool conflicted = false;
			std::vector<std::pair<int, int> > & tier = tiers[i];
			for(std::vector<std::pair<int, int> >::iterator j = tier.begin();
				j != tier.end(); j++)
			{
				if(PartIntersect(j->first, j->second, cinfo.LinkMin, cinfo.LinkMax))
				{
					// intersection found
					conflicted = true;
					break;
				}
			}
			if(!conflicted)
			{
				// intersection not found
				found_link_tier = i;
				break;
			}
		}

		if(found_link_tier == -1)
		{
			// create new tier
			found_link_tier = tiers.size();
			tiers.push_back(std::vector<std::pair<int, int> >() );
		}

		Links[i].LinkTier = found_link_tier;
		Links[i].FromTier = found_from_tier;
		Links[i].ToTier   = found_to_tier;

		tiers[found_link_tier].push_back(std::pair<int, int>(cinfo.LinkMin, cinfo.LinkMax));

		if(tier_count < found_link_tier + 1) tier_count = found_link_tier + 1;
	}

	if(FLinkTierCount != tier_count)
		Invalidate();
	FLinkTierCount = tier_count;

	//- sort by Index
	std::stable_sort(Links.begin(), Links.end(), tTVPWaveLoopLink::tSortByIndexFuncObj());

	// call notify handler
	if(FOnLinkModified && !FInOnLinkModified)
	{
		FInOnLinkModified = true;
		try
		{
			FOnLinkModified(this);
		}
		catch(...)
		{
			FInOnLinkModified = false;
			throw;
		}
		FInOnLinkModified  = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetLinks(const std::vector<tTVPWaveLoopLink> & links)
{
	// set new link information
	// note that this will call OnLinkModified
	Links = links;
	NotifyLinkChanged();
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

	r.left = where - 2;
	r.top = y;
	r.right = where + 6;
	r.bottom = y + 1;
	Canvas->FillRect(r);

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

	if(link.Condition != llcNone)
		Canvas->Pen->Style = psDot;
	else
		Canvas->Pen->Style = psSolid;

	// for from line
	x_from = SampleToPixel(link.From - Start);
	if(x_from >= dest_left || x_from < dest_right)
	{
		int y0 = y_start + link.FromTier * TVP_LGD_TIER_HEIGHT;
		Canvas->Brush->Style = bsClear;
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
		Canvas->Brush->Style = bsClear;
		Canvas->MoveTo(x_to, y0);
		Canvas->LineTo(x_to, y_link);
	}

	// for horizontal link line
	if(PartIntersect(x_from, x_to, dest_left, dest_right-1))
	{
		Canvas->Brush->Style = bsClear;
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

	Canvas->Pen->Style = psSolid;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLinks(void)
{
	if(!FReader || !FReader->ReadDone) return;
	if(!FShowLinks) return;

	// calc start point
	int dest_left  = Canvas->ClipRect.left;
	int dest_right = Canvas->ClipRect.right;
	int head_size = GetHeadSize();
	int foot_size = GetFootSize();
	int y_start = ClientHeight - foot_size;

	// draw separator
	Canvas->Pen->Color = C_LINK_SEPARETOR;
	Canvas->MoveTo(dest_left,  y_start);
	Canvas->LineTo(dest_right, y_start);

	if(PartIntersect(Canvas->ClipRect.top, Canvas->ClipRect.bottom -1,
		y_start, ClientHeight - 1))
	{
		// draw each link (for vertical weak line)
		Canvas->Pen->Color = C_LINK_WEAK_LINE;

		for(unsigned int i = 0; i < Links.size(); i++)
		{
			const tTVPWaveLoopLink & link = Links[i];
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
		for(unsigned int i = 0; i < Links.size(); i++)
		{
			if((int)i == FFocusedLink)
				continue;
			else if((int)i == FHoveredLink)
				continue;
			else
				Canvas->Pen->Color = C_LINK_LINE;

			const tTVPWaveLoopLink & link = Links[i];

			DrawLinkOf(link);
		}
		if(FHoveredLink != -1)
		{
			Canvas->Pen->Color = C_LINK_HOVER;
			const tTVPWaveLoopLink & link = Links[FHoveredLink];
			DrawLinkOf(link);
		}

		if(FFocusedLink != -1)
		{
			Canvas->Pen->Width = 2;
			Canvas->Pen->Color = C_LINK_FOCUS;
			const tTVPWaveLoopLink & link = Links[FFocusedLink];
			DrawLinkOf(link);
			Canvas->Pen->Width = 1;
		}
	}


	if(PartIntersect(Canvas->ClipRect.top, Canvas->ClipRect.bottom -1,
		head_size, y_start - 1))
	{
		// draw link 'from' and 'to' lines
		Canvas->Pen->Color = C_LINK_WAVE_MARK;
		Canvas->Pen->Style = psDashDot;
		Canvas->Brush->Style = bsClear;
		for(unsigned int i = 0; i < Links.size(); i++)
		{
			const tTVPWaveLoopLink & link = Links[i];
			int x;

			x = SampleToPixel(link.From - Start);
			Canvas->MoveTo(x, head_size);
			Canvas->LineTo(x, y_start);

			x = SampleToPixel(link.To - Start);
			Canvas->MoveTo(x, head_size);
			Canvas->LineTo(x, y_start);
		}
		Canvas->Pen->Style = psSolid;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::InvalidateLink(int linknum)
{
	// invalidate displaying area of link 'linknum'.
	// currently not completely implemented.

	if(linknum < 0 || linknum >= (int)Links.size()) return;

	const tTVPWaveLoopLink & link = Links[linknum];

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
		DoubleBuffered = FDoubleBufferEnabled;
		InvalidateLink(FHoveredLink);
		FHoveredLink = l;
		InvalidateLink(FHoveredLink);

		// generate link hint
		AnsiString hint;
		if(FHoveredLink != -1)
		{
			tTVPWaveLoopLink & link = Links[FHoveredLink];
			AnsiString cond;
			if(link.Condition == llcNone)
			{
				cond = C_LINK_COND_CODES[0];
			}
			else
			{
				cond.sprintf(C_LINK_COND_CODES[(int)link.Condition].c_str(),
					link.CondVar, link.RefValue);
			}

			hint.sprintf(C_LINK_HINT.c_str(),
				FReader->SamplePosToTimeString(link.From),
				FReader->SamplePosToTimeString(link.To),
				FReader->SamplePosToTimeString(std::abs(link.To - link.From)),
				cond.c_str());
		}
		Application->CancelHint();
		Hint = hint;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetFocusedLink(int l)
{
	if(FFocusedLink != l)
	{
		DoubleBuffered = FDoubleBufferEnabled;
		InvalidateLink(FFocusedLink);
		FFocusedLink = l;
		InvalidateLink(FFocusedLink);
		if(l != -1)
		{
			ShowCaret = false; // link, label and caret are exclusive
			FocusedLabel = -1;
			Hint = "";
			if(FOnLinkSelected)
			{
				FOnLinkSelected(this, l, Links[l]);
			}
		}
		else
		{
			if(FOnSelectionLost) FOnSelectionLost(this);
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::IsLinkAt(int linknum, int x, int y)
{
	if(linknum < 0 || linknum >= (int)Links.size()) return false;

	const tTVPWaveLoopLink & link = Links[linknum];
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
	for(unsigned int i = 0; i < Links.size(); i++)
	{
		if((int)i != FFocusedLink && IsLinkAt(i, x, y)) return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
#define MARK_GRIP_MARGIN 2
int __fastcall TWaveView::IsLinkWaveMarkAt(int x, int linknum, bool &from_or_to)
{
	const tTVPWaveLoopLink & link = Links[linknum];

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
		return from_or_to ? link.To : link.From;
	}

	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::GetLinkWaveMarkAt(int x, int &linknum, bool &from_or_to)
{
	// give priority to focused link
	int pos;
	if(FFocusedLink != -1)
	{
		pos = IsLinkWaveMarkAt(x, FFocusedLink, from_or_to);

		if(pos != -1)
		{
			linknum = FFocusedLink;
			return pos;
		}
	}

	// for each links
	for(unsigned int i = 0; i < Links.size(); i++)
	{
		if((int)i != FFocusedLink)
		{
			pos = IsLinkWaveMarkAt(x, i, from_or_to);

			if(pos != -1)
			{
				linknum = i;
				return pos;
			}
		}
	}

	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::SelectLink(int tier, int spos, int mode)
{
	// select link at sample position "spos", at tier "tier".
	// mode =  0 : nearest link
	// mode =  1 : upper link
	// mode =  2 : lower link
	// mode =  3 : left link
	// mode =  4 : right link

	// search the nearest link at spos
	int found_link = -1;
	int min_dist = -1;
	for(unsigned int i = 0; i < Links.size(); i++)
	{
		if(Links[i].LinkTier != tier) continue;
		int dist = std::abs((Links[i].From + Links[i].To)/2 - spos);
		if(min_dist == -1 || min_dist > dist)
			min_dist = dist, found_link = i;
	}

	if(found_link == -1) return -1; // link not found at tier
	if(mode == 0) return found_link; // the nearest link found

	// search most nearest left or right siblings
	const tTVPWaveLoopLink & reflink = Links[found_link];
	int reflink_num = found_link;
	found_link = -1;
	min_dist = -1;
	for(unsigned int i = 0; i < Links.size(); i++)
	{
		if((int)i == reflink_num) continue;

		int rx, ry = reflink.LinkTier * TVP_LGD_TIER_HEIGHT;
		int cx, cy = Links[i].LinkTier * TVP_LGD_TIER_HEIGHT;
		// find link's nearest position
		if(Links[i].LinkTier == reflink.LinkTier ||
			!PartIntersect(Links[i].From, Links[i].To, reflink.From, reflink.To))
		{
			// the same link pos or does not have link intersection
			if(Links[i].From < reflink.From)
			{
				rx = SampleToPixel(std::min(reflink.From, reflink.To));
				cx = SampleToPixel(std::max(Links[i].From, Links[i].To));
			}
			else
			{
				rx = SampleToPixel(std::max(reflink.From, reflink.To));
				cx = SampleToPixel(std::min(Links[i].From, Links[i].To));
			}
		}
		else
		{
			// different link tier but have intersection
			rx = SampleToPixel((reflink.From + reflink.To)/2);
			cx = rx;
		}

		if(mode == 1 && !(cy <= ry && std::abs(cx - rx) < ry - cy)) continue;
		if(mode == 2 && !(cy >= ry && std::abs(cx - rx) < cy - ry)) continue;
		if(mode == 3 && !(cx <= rx && std::abs(cy - ry) < rx - cx)) continue;
		if(mode == 4 && !(cx >= rx && std::abs(cy - ry) < cx - rx)) continue;

		int dist = (cx-rx)*(cx-rx) + (cy-ry)*(cy-ry);
		if(min_dist == -1 || min_dist > dist)
			min_dist = dist, found_link = i;
	}

	return found_link;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::FocusLinkAt(int tier, int spos, int mode)
{
	// focus the nearest link
	int found = SelectLink(tier, spos, mode);
	if(found != -1)
	{
		FocusedLink = found;
		EnsureView(Links[found].From, Links[found].To - Links[found].From);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CreateNewLink()
{
	// create a new link
	if(!FReader || !FReader->ReadDone) return;

	int from, to;
	if(LastClickedPos[0] == LastClickedPos[1])
	{
		// 'from' and the 'to' is the same
		from = LastClickedPos[0];
		to = LastClickedPos[0] + 500000;
	}
	else
	{
		from = LastClickedPos[0];
		to = LastClickedPos[1];
	}

	if(from < 0) from = 0;
	if(from > FReader->NumSamples) from = FReader->NumSamples;

	if(to < 0) to = 0;
	if(to > FReader->NumSamples) to = FReader->NumSamples;


	tTVPWaveLoopLink link;

	link.From = from;
	link.To = to;
	link.Smooth = false;
	link.Condition = llcNone;
	link.CondVar = 0;
	Links.push_back(link);

	NotifyLinkChanged();

	FocusedLink = Links.size() - 1; // focus last

	DoubleBuffered = FDoubleBufferEnabled;
	Invalidate();
	PushUndo(); //==== push undo
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CreateNewLabel(int pos)
{
	// create new label at current caret position
	if(!FReader || !FReader->ReadDone) return;

	if(pos == -1) pos = GetAttentionPos();
	tTVPWaveLabel label;
	label.Position = pos;

	// search free label name
	int last = 0;
	for(std::vector<tTVPWaveLabel>::iterator i = Labels.begin(); i != Labels.end();
		i++)
	{
		if(i->Name.AnsiPos(C_NEW_LABEL_BASE_NAME) == 1)
		{
			int n = atoi(i->Name.c_str() + C_NEW_LABEL_BASE_NAME.Length());
			if(n >= last) last = n + 1;
		}
	}

	label.Name = C_NEW_LABEL_BASE_NAME + AnsiString(last);
	label.NameWidth = -1;

	Labels.push_back(label);

	NotifyLabelChanged();

	FocusedLabel = Labels.size() - 1;

	DoubleBuffered = FDoubleBufferEnabled;
	Invalidate();
	PushUndo(); //==== push undo
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::NotifyLabelChanged()
{
	// notify the label information has changed
	if(FFocusedLabel >= (int)Labels.size()) FFocusedLabel = -1;
	if(FHoveredLabel >= (int)Labels.size()) FHoveredLabel = -1;


	for(std::vector<tTVPWaveLabel>::iterator i = Labels.begin(); i != Labels.end();
		i++)
	{
		// invalidate all cache information
		i->NameWidth = -1;
	}

	// call notify handler
	if(FOnLabelModified && ! FInOnLabelModified)
	{
		FInOnLabelModified = true;
		try
		{
			FOnLabelModified(this);
		}
		catch(...)
		{
			FInOnLabelModified = false;
			throw;
		}
		FInOnLabelModified  = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetLabels(const std::vector<tTVPWaveLabel> & labels)
{
	// set new label information
	// note that this will call OnLabelModified
	Labels = labels;
	NotifyLabelChanged();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::GetLabelNameRect(tTVPWaveLabel & label, TRect & rect)
{
	// get text height
	if(LabelTextHeight == -1)
		LabelTextHeight = Canvas->TextHeight("|");

	// get label name rectangle
	int x = SampleToPixel(label.Position - Start);
	int head_size = GetHeadSize();
	Canvas->Font->Style = Canvas->Font->Style << fsBold;

	// decide the label position
	int text_width;

	if(label.NameWidth == -1)
		label.NameWidth = Canvas->TextWidth(label.Name);

	text_width = label.NameWidth + 10;

	// check right free space
	int right_space;
	if(PixelToSample(ClientWidth) > FReader->NumSamples)
		right_space = ClientWidth - x;
	else
		right_space = SampleToPixel(FReader->NumSamples - label.Position);

	// check left free space
	int left_space;
	left_space = SampleToPixel(label.Position);

	// decide position
	int text_x;
	if(right_space < text_width && left_space >= text_width)
		text_x = x - text_width;
	else
		text_x = x + 10;

	int text_y = head_size - LabelTextHeight - 2;

	// return a rectangle
	rect.left = text_x - 2;
	rect.top = text_y - 1;
	rect.right = text_x + text_width - 10 + 2;
	rect.bottom = head_size;

	Canvas->Font->Style = Canvas->Font->Style >> fsBold;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLabelMarkOf(tTVPWaveLabel & label)
{
	int head_size = GetHeadSize();
	int foot_size = GetFootSize();
	int bottom_limit = ClientHeight - foot_size;

	// draw label mark
	int x;
	x = SampleToPixel(label.Position - Start);

	Canvas->Brush->Color = C_LABEL_MARK;
	Canvas->Brush->Style = bsSolid;
	TPoint points[3];
	points[0].x = x;
	points[0].y = head_size - 2;
	points[1].x = x - 4;
	points[1].y = head_size - 10;
	points[2].x = x + 4;
	points[2].y = head_size - 10;

	Canvas->Polygon(points, 2);

	Canvas->Pen->Width = 1;
	Canvas->Brush->Style = bsClear;
	Canvas->Pen->Color = C_LABEL_WAVE_MARK;
	Canvas->Pen->Style = psDashDot;
	Canvas->MoveTo(x, head_size);
	Canvas->LineTo(x, bottom_limit);
	Canvas->Pen->Style = psSolid;

}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLabelNameOf(tTVPWaveLabel & label)
{
	// draw label name
	TRect rect;
	GetLabelNameRect(label, rect);

	if(
		PartIntersect(rect.left, rect.right-1, Canvas->ClipRect.left, Canvas->ClipRect.right-1) &&
		PartIntersect(rect.top, rect.bottom-1, Canvas->ClipRect.top, Canvas->ClipRect.bottom-1) )
	{

		int text_x = rect.left + 2;
		int text_y = rect.top + 1;

		Canvas->Font->Style = Canvas->Font->Style << fsBold;
		TColor color_save = Canvas->Font->Color;
		Canvas->Font->Color = C_LABEL_TEXT_BG;
		Canvas->TextOut(text_x-2 , text_y   , label.Name);
		Canvas->TextOut(text_x+2 , text_y   , label.Name);
		Canvas->TextOut(text_x   , text_y-1 , label.Name);
		Canvas->TextOut(text_x   , text_y+1 , label.Name);
		Canvas->Font->Color = color_save;
		Canvas->TextOut(text_x   , text_y   , label.Name);
		Canvas->Font->Style = Canvas->Font->Style >> fsBold;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DrawLabels()
{
	if(!FReader || !FReader->ReadDone) return;
	if(!FShowLabels) return;

	// draw each label mark
	for(unsigned int i = 0; i < Labels.size(); i++)
	{
		if((int)i == FFocusedLabel)
			continue;
		else if((int)i == FHoveredLabel)
			continue;
		else
		{
			Canvas->Pen->Color = C_LABEL_MARK_LINE;
			Canvas->Font->Color = C_LABEL_TEXT;
		}

		tTVPWaveLabel & label = Labels[i];

		DrawLabelMarkOf(label);
	}
	if(FFocusedLabel != -1)
	{
		Canvas->Pen->Width = 2;
		Canvas->Pen->Color = C_LABEL_MARK_FOCUS;
		Canvas->Font->Color = C_LABEL_TEXT_FOCUS;
		tTVPWaveLabel & label = Labels[FFocusedLabel];
		DrawLabelMarkOf(label);
		Canvas->Pen->Width = 1;
	}
	if(FHoveredLabel != -1 && FHoveredLabel != FFocusedLabel)
	{
		Canvas->Pen->Color = C_LABEL_MARK_HOVER;
		Canvas->Font->Color = C_LABEL_TEXT_HOVER;
		tTVPWaveLabel & label = Labels[FHoveredLabel];
		DrawLabelMarkOf(label);
	}

	// draw each label name
	for(unsigned int i = 0; i < Labels.size(); i++)
	{
		if((int)i == FFocusedLabel)
			continue;
		else if((int)i == FHoveredLabel)
			continue;
		else
		{
			Canvas->Pen->Color = C_LABEL_MARK_LINE;
			Canvas->Font->Color = C_LABEL_TEXT;
		}

		tTVPWaveLabel & label = Labels[i];

		DrawLabelNameOf(label);
	}
	if(FFocusedLabel != -1)
	{
		Canvas->Pen->Width = 2;
		Canvas->Pen->Color = C_LABEL_MARK_FOCUS;
		Canvas->Font->Color = C_LABEL_TEXT_FOCUS;
		tTVPWaveLabel & label = Labels[FFocusedLabel];
		DrawLabelNameOf(label);
		Canvas->Pen->Width = 1;
	}
	if(FHoveredLabel != -1 && FHoveredLabel != FFocusedLabel)
	{
		Canvas->Pen->Color = C_LABEL_MARK_HOVER;
		Canvas->Font->Color = C_LABEL_TEXT_HOVER;
		tTVPWaveLabel & label = Labels[FHoveredLabel];
		DrawLabelNameOf(label);
	}


}
//---------------------------------------------------------------------------
void __fastcall TWaveView::InvalidateLabel(int labelnum)
{
	// invalidate label labelnum
	if(labelnum < 0 || labelnum >= (int)Labels.size()) return;

	int head_size = GetHeadSize();
	int foot_size = GetFootSize();
	int bottom_limit = ClientHeight - foot_size;

	// invalidate marker triangle
	tTVPWaveLabel & label = Labels[labelnum];
	int x = SampleToPixel(label.Position - Start);

	// invalidate marker triangle
	RECT r;
	r.left   = x - 5;
	r.top    = head_size - 11;
	r.right  = x + 6;
	r.bottom = head_size;
	InvalidateRect(Handle, &r, FALSE);

	// invalidate wave mark
	r.left   = x;
	r.top    = head_size;
	r.right  = x + 1;
	r.bottom = bottom_limit;
	InvalidateRect(Handle, &r, FALSE);

	// invalidate label name
	TRect rect;
	GetLabelNameRect(label, rect);
	InvalidateRect(Handle, (RECT*)&rect, FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetHoveredLabel(int l)
{
	if(FHoveredLabel != l)
	{
		DoubleBuffered = FDoubleBufferEnabled;
		InvalidateLabel(FHoveredLabel);
		FHoveredLabel = l;
		InvalidateLabel(FHoveredLabel);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::SetFocusedLabel(int l)
{
	if(FFocusedLabel != l)
	{
		DoubleBuffered = FDoubleBufferEnabled;
		InvalidateLabel(FFocusedLabel);
		FFocusedLabel = l;
		InvalidateLabel(FFocusedLabel);
		if(l != -1)
		{
			ShowCaret = false; // link, label and caret are exclusive
			FocusedLink = -1; // link, label and caret are exclusive
			Hint = "";
			if(FOnLabelSelected)
			{
				FOnLabelSelected(this, l, Labels[l]);
			}
		}
		else
		{
			if(FOnSelectionLost) FOnSelectionLost(this);
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::IsLabelAt(int labelnum, int x, int y)
{
	if(labelnum < 0 || labelnum >= (int)Labels.size()) return false;

	const tTVPWaveLabel & label = Labels[labelnum];
	int lx = SampleToPixel(label.Position - Start);

	int head_size = GetHeadSize();
	return y >= 0 && y < head_size && x >= lx - 4 && x <= lx + 4;
}
//---------------------------------------------------------------------------
bool __fastcall TWaveView::IsLabelNameAt(int labelnum, int x, int y)
{
	if(labelnum < 0 || labelnum >= (int)Labels.size()) return false;

	tTVPWaveLabel & label = Labels[labelnum];
	TRect rect;
	GetLabelNameRect(label, rect);

	return x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::GetLabelAt(int x, int y)
{
	// give priority to focused label
	if(FFocusedLabel != -1)
	{
		if(IsLabelAt(FFocusedLabel, x, y))
			return FFocusedLabel;
	}

	// for each labels
	for(unsigned int i = 0; i < Labels.size(); i++)
	{
		if((int)i != FFocusedLabel)
		{
			if(IsLabelAt(i, x, y))
			{
				return i;
			}
		}
	}

	// give priority to focused label
	if(FFocusedLabel != -1)
	{
		if(IsLabelNameAt(FFocusedLabel, x, y))
			return FFocusedLabel;
	}

	// for each labels
	for(unsigned int i = 0; i < Labels.size(); i++)
	{
		if((int)i != FFocusedLabel)
		{
			if(IsLabelNameAt(i, x, y))
			{
				return i;
			}
		}
	}

	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::IsLabelWaveMarkAt(int x, int labelnum)
{
	if(labelnum < 0 || labelnum >= (int)Labels.size()) return false;

	const tTVPWaveLabel & label = Labels[labelnum];
	int lx = SampleToPixel(label.Position - FStart);

	return (lx - MARK_GRIP_MARGIN <= x && x <= lx + MARK_GRIP_MARGIN) ?
		label.Position  : -1;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::GetLabelWaveMarkAt(int x, int &labelnum)
{
	// give priority to focused label
	int pos;

	if(FFocusedLabel != -1)
	{
		pos = IsLabelWaveMarkAt(x, FFocusedLabel);

		if(pos != -1)
		{
			labelnum = FFocusedLabel;
			return pos;
		}
	}

	// for each labels
	for(unsigned int i = 0; i < Labels.size(); i++)
	{
		if((int)i != FFocusedLabel)
		{
			pos = IsLabelWaveMarkAt(x, i);

			if(pos != -1)
			{
				labelnum = i;
				return pos;
			}
		}
	}

	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TWaveView::SelectLabel(int spos, int mode)
{
	// select one label at sample position "spos".
	// mode = -1 : search left label of spos
	// mode = 0  : search the nearest label at spos
	// mode = +1 : search right label of spos

	// mark label index
	for(unsigned int i = 0; i < Labels.size(); i++) Labels[i].Index = i;

	// sort labels by the position
	std::sort(Labels.begin(), Labels.end(), tTVPWaveLabel::tSortByPositionFuncObj());

	// find the nearest label at spos
	int found_label = -1;
	do // dummy loop
	{
		int mindist = -1;
		int minlabel = -1;
		for(unsigned int i = 0; i < Labels.size(); i++)
		{
			int dist = std::abs(Labels[i].Position - spos);
			if(mindist == -1 || dist < mindist)
				mindist = dist, minlabel = i;
		}
		if(minlabel == -1)
		{
			found_label = -1;  // no labels found
			break;
		}

		if(mode == 0) return minlabel; // nearest label found

		// skip labels which have the same position
		minlabel += mode;
		if(minlabel < 0  || minlabel >= (int)Labels.size())
		{
			found_label = -1; // out of range
			break;
		}
		while(minlabel >= 0 && minlabel < (int)Labels.size() && Labels[minlabel].Position == spos)
			minlabel += mode;
		if(minlabel < 0  || minlabel >= (int)Labels.size())
		{
			found_label = -1; // out of range
			break;
		}

		found_label = minlabel; // label found
	} while(false);

	if(found_label != -1)
	{
		// retrieve the original label order
		found_label = Labels[found_label].Index;
	}

	// sort labels by the index
	std::sort(Labels.begin(), Labels.end(), tTVPWaveLabel::tSortByIndexFuncObj());

	return found_label;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::FocusLabelAt(int spos, int mode)
{
	int num = SelectLabel(spos, mode);
	if(num != -1)
	{
		TRect rect;
		GetLabelNameRect(Labels[num], rect);
		EnsureView(Labels[num].Position, PixelToSample(rect.right - rect.left));
		FocusedLabel = num;
	}
}
//---------------------------------------------------------------------------
void TWaveView::PushLastClickedPos(int pos)
{
	LastClickedPos[0] = LastClickedPos[1];
	LastClickedPos[1] = pos;
}
//---------------------------------------------------------------------------
bool TWaveView::GetNearestObjectAt(int x, TObjectInfo & info)
{
	int num = 0;
	bool from_or_to = false;
	int pos;

	// give priority to currently focused object (link or label)
	int prio_order[2];
	if(FFocusedLink != -1)
		prio_order[0] = 0, prio_order[1] = 1;
	else if(FFocusedLabel != -1)
		prio_order[0] = 1, prio_order[1] = 0;
	else
		prio_order[0] = 0, prio_order[1] = 1;


	for(int p = 0; p < sizeof(prio_order) / sizeof(prio_order[0]); p++)
	{
		if(prio_order[p] == 0)
		{
			pos = GetLinkWaveMarkAt(x, num, from_or_to);

			if(pos != -1)
			{
				info.Kind = okLink;
				info.Num  = num;
				info.FromOrTo = from_or_to;
				info.Position = pos;
				return true;
			}
		}
		else if(prio_order[p] == 1)
		{
			pos = GetLabelWaveMarkAt(x, num);

			if(pos != -1)
			{
				info.Kind = okLabel;
				info.Num = num;
				info.Position = pos;
				HoveredLink = -1;
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
bool __fastcall TWaveView::DoMouseWheel(Classes::TShiftState Shift, int WheelDelta,
		const Windows::TPoint &MousePos)
{
	WheelDelta /= 120; // is 120 minimum unit?

	if(Shift.Contains(ssCtrl))
	{
		// zoom up/down
		if(WheelDelta > 0)	Magnify ++;
		else				Magnify --;
	}
	else
	{
		// scroll
		// generate fake TWMHScroll structure and call WMHScroll
		TWMHScroll msg;
		ZeroMemory(&msg, sizeof(msg));
		msg.ScrollCode = WheelDelta  > 0 ? SB_LINELEFT : SB_LINERIGHT;
		WMHScroll(msg);
	}

	return true;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CMMouseLeave(TMessage &msg)
{
	MouseLeave();
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseDown(TMouseButton button, TShiftState shift, int x, int y)
{
	if(!FReader || !FReader->ReadDone) return;

	// mouse downed
	if(DisableNextMouseDown)
	{
		DisableNextMouseDown = false;
		return;
	}

	SetFocus();
	PopupType = "";
	int head_size = GetHeadSize();
	int foot_size = GetFootSize();
	int foot_start = ClientHeight - foot_size;

	LastMouseDownY = y;
	LastMouseDownX = x;

	if(button == mbLeft)
	{
		if(y < head_size)
		{
			int fl = GetLabelAt(x, y);
			if(fl != -1)
			{
				FocusedLabel = fl;
				// generate dragging information
				tTVPWaveLabel &label = Labels[fl];
				DraggingObjectInfo.Kind = okLabel;
				DraggingObjectInfo.Num = fl;
				DraggingObjectInfo.Position = label.Position;
				DraggingState = dsMouseDown;
				LastMouseDownPosOffset = MouseXPosToSamplePos(x) - DraggingObjectInfo.Position;
				if(FOnStopFollowingMarker) FOnStopFollowingMarker(this);
			}
		}
		else if(y < foot_start)
		{
			TObjectInfo info;
			if(GetNearestObjectAt(x, info))
			{
				DraggingObjectInfo = info;
				DraggingState = dsMouseDown;
				LastMouseDownPosOffset = MouseXPosToSamplePos(x) - DraggingObjectInfo.Position;
			}
			else
			{
				int pos = MouseXPosToSamplePos(x);
				CaretPos = pos;
				ShowCaret = true;
				PushLastClickedPos(pos);
			}

		}
		else
		{
			HoveredLabel = -1;
			int fl = GetLinkAt(x, y);
			if(fl != -1)
			{
				FocusedLink = fl;
				if(FOnStopFollowingMarker) FOnStopFollowingMarker(this);
			}
		}
	}
	else if(button == mbRight)
	{
		if(y < head_size)
		{
			int fl = GetLabelAt(x, y);
			if(fl != -1)
				FocusedLabel = fl;
			else
				CaretPos = MouseXPosToSamplePos(x);
			PopupType = "Label";
		}
		else if(y < foot_start)
		{
			int pos;
			TObjectInfo info;
			if(GetNearestObjectAt(x, info))
				pos = info.Position;
			else
				pos = MouseXPosToSamplePos(x);
			CaretPos = pos;
			ShowCaret = true;
			PushLastClickedPos(pos);
			PopupType = "Wave";
		}
		else
		{
			HoveredLabel = -1;
			int fl = GetLinkAt(x, y);
			if(fl != -1)
			{
				FocusedLink = fl;
			}
			else
			{
				int pos = MouseXPosToSamplePos(x);
				CaretPos = pos;
				PushLastClickedPos(pos);
			}
			PopupType = "Link";
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
		LastMouseMoveX = x;

		// dragging
		if(DraggingState == dsMouseDown)
		{
			if(LastMouseDownX != x)
			{
				// dragging started
				DraggingState = dsDragging;
				if(DraggingObjectInfo.Kind == okLink)
					FocusedLink = DraggingObjectInfo.Num;
				else if(DraggingObjectInfo.Kind == okLabel)
					FocusedLabel = DraggingObjectInfo.Num;
			}
		}
		/* no else here */
		if(DraggingState == dsDragging)
		{
			// enable double buffering
			DoubleBuffered = FDoubleBufferEnabled;

			int pos = MouseXPosToSamplePos(x) - LastMouseDownPosOffset;
			if(pos < 0) pos = 0;
			if(pos > FReader->NumSamples) pos = FReader->NumSamples;

			// dragging
			if(DraggingObjectInfo.Kind == okLink)
			{
				tTVPWaveLoopLink &link = Links[DraggingObjectInfo.Num];

				InvalidateLink(DraggingObjectInfo.Num);
				if(!DraggingObjectInfo.FromOrTo)
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
				InvalidateLink(DraggingObjectInfo.Num);
				CaretPos = pos; // also set the caret position
			}
			else if(DraggingObjectInfo.Kind == okLabel)
			{
				tTVPWaveLabel &label = Labels[DraggingObjectInfo.Num];

				InvalidateLabel(DraggingObjectInfo.Num);
				label.Position = pos;
				NotifyLabelChanged();
				InvalidateLabel(DraggingObjectInfo.Num);
				CaretPos = pos; // also set the caret position
			}

			// scroll
			if(x < 10 || x >= ClientWidth - 10)
				DragScrollTimer->Enabled = true;
			else
				DragScrollTimer->Enabled = false;
		}
	}
	else
	{
		int head_size = GetHeadSize();
		int foot_size = GetFootSize();
		int foot_start = ClientHeight - foot_size;

		if(y < head_size)
		{
			HoveredLink = -1;
			HoveredLabel = GetLabelAt(x, y);
			Cursor = crArrow;
		}
		else if(y < foot_start)
		{
			TObjectInfo info;
			if(GetNearestObjectAt(x, info))
			{
				if(info.Kind == okLink)
				{
					HoveredLabel = -1;
					HoveredLink = info.Num;
					Cursor = crSizeWE;
				}
				else if(info.Kind == okLabel)
				{
					HoveredLink = -1;
					HoveredLabel = info.Num;
					Cursor = crSizeWE;
				}
			}
			else
			{
				HoveredLink  = -1;
				HoveredLabel = -1;
				Cursor = crIBeam;
			}
		}
		else
		{
			HoveredLabel = -1;
			HoveredLink = GetLinkAt(x, y);
			Cursor = crArrow;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseUp(TMouseButton button, TShiftState shift, int x, int y)
{
	// mouse up
	if(!FReader || !FReader->ReadDone)
	{
		return;
	}

	if(DraggingState != dsNone)
	{
		if(DraggingState == dsMouseDown)
		{
			// set caret to current from or to position
			if(DraggingObjectInfo.Kind == okLink)
			{
				CaretPos = DraggingObjectInfo.Position;
				ShowCaret = true;
				PushLastClickedPos(DraggingObjectInfo.Position);
			}
			else if(DraggingObjectInfo.Kind == okLabel)
			{
				if(y >= GetHeadSize())
				{
					CaretPos = DraggingObjectInfo.Position;
					ShowCaret = true;
					PushLastClickedPos(DraggingObjectInfo.Position);
				}
			}
		}
		else
		{
			// commit current state
			PushUndo(); //==== push undo
		}
		DraggingState = dsNone;
		DragScrollTimer->Enabled = false;
	}
	else
	{
		PopupType = "";
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DblClick(void)
{
	// on double click
	if(!FReader || !FReader->ReadDone)
	{
		return;
	}

	int head_size = GetHeadSize();
	int foot_size = GetFootSize();
	int foot_start = ClientHeight - foot_size;

	if(LastMouseDownY < head_size)
	{
		// label
		if(FOnLabelDoubleClick)
		{
			int labelnum = GetLabelAt(LastMouseDownX, LastMouseDownY);
			if(labelnum != -1)
			{
				// cancel dragging
				DraggingState = dsNone;
				DragScrollTimer->Enabled = false;

				// call double click handler
				FOnLabelDoubleClick(this, labelnum, Labels[labelnum]);

				DisableNextMouseDown = true; // to avoid dragging
			}
		}
	}
	else if(LastMouseDownY < foot_start)
	{
		// wave
		if(FOnWaveDoubleClick)
		{
			int pos = MouseXPosToSamplePos(LastMouseDownX);
			if(pos >= 0 && pos < FReader->NumSamples)
			{
				FOnWaveDoubleClick(this, pos);
			}
		}
	}
	else
	{
		// link
		if(FOnLinkDoubleClick)
		{
			int linknum = GetLinkAt(LastMouseDownX, LastMouseDownY);
			if(linknum != -1)
				FOnLinkDoubleClick(this, linknum, Links[linknum]);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::DoContextPopup(const Windows::TPoint & MousePos, bool &Handled)
{
	if(FOnNotifyPopup)
	{
		TPoint mpos = MousePos;
		if(mpos.x == -1 && mpos.y == -1)
		{
			// point not specified; decide popup point
			if(FocusedLabel != -1)
			{
				mpos.x = SampleToPixel(Labels[FocusedLabel].Position - FStart);
				mpos.y = GetHeadSize();
			}
			else if(FocusedLink != -1)
			{
				int f = SampleToPixel(Links[FocusedLink].From - FStart);
				int t = SampleToPixel(Links[FocusedLink].To   - FStart);
				if(f >= 0 && f <= ClientWidth)
					mpos.x = f;
				else if(t >= 0 && t <= ClientWidth)
					mpos.x = t;
				else
				{
					if(f >= ClientWidth)
						mpos.x = ClientWidth;
					else
						mpos.x = 0;
				}
				int y_start = ClientHeight - GetFootSize();
				int y = y_start +
					Links[FocusedLink].LinkTier * TVP_LGD_TIER_HEIGHT +
						TVP_LGD_TIER_HEIGHT - 3;
				mpos.y = y + 3;
			}
			else if(ShowCaret)
			{
				mpos.x = SampleToPixel(CaretPos - FStart);
				mpos.y = ClientHeight - GetFootSize();
			}
		}
		mpos = ClientToScreen(mpos);

		if(PopupType != "")
		{
			// popup by mouse
			FOnNotifyPopup(this, PopupType, mpos);
		}
		else
		{
			// popup by the kerboard
			// decide popup type by currently focused item
			if(FocusedLabel != -1)
				PopupType = "Label";
			else if(FocusedLink != -1)
				PopupType = "Link";
			else if(ShowCaret)
				PopupType = "Wave";
			FOnNotifyPopup(this, PopupType, mpos);
			PopupType = "";
		}
	}

	Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::PerformLinkDoubleClick()
{
	if(FocusedLink != -1)
	{
		FOnLinkDoubleClick(this, FocusedLink, Links[FocusedLink]);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::PerformLabelDoubleClick()
{
	if(FocusedLabel != -1)
	{
		FOnLabelDoubleClick(this, FocusedLabel, Labels[FocusedLabel]);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::MouseLeave()
{
	// mouse leaved
	HoveredLink = -1;
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::OnDragScrollTimer(TObject * sender)
{
	TWMHScroll wmscroll;
	ZeroMemory(&wmscroll, sizeof(TWMHScroll));
	wmscroll.ScrollCode = LastMouseMoveX > ClientWidth / 2 ? SB_LINERIGHT : SB_LINELEFT;
	WMHScroll(wmscroll);
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::CMWantSpecialKey(TCMWantSpecialKey &message)
{
	switch(message.CharCode)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		if(FOnStopFollowingMarker) FOnStopFollowingMarker(this);
		message.Result = 1;
		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveView::KeyDown(Word &Key, Classes::TShiftState Shift)
{
	if(!FReader || !FReader->ReadDone)
	{
		return;
	}

	// key action is to be changed by which item is currently focused
	if(FocusedLabel != -1)
	{
		// currently the label is focued
		if(Key == VK_LEFT)
		{
			// select left label
			FocusLabelAt(Labels[FocusedLabel].Position, -1);
		}
		else if(Key == VK_RIGHT)
		{
			// select right label
			FocusLabelAt(Labels[FocusedLabel].Position, 1);
		}
		else if(Key == VK_DOWN)
		{
			// show caret
			CaretPos = Labels[FocusedLabel].Position;
			ShowCaret = true;
		}
		else if(Key == VK_RETURN)
		{
			// enter key pressed
			PerformLabelDoubleClick();
		}
	}
	else if(FocusedLink != -1)
	{
		// currently the link is focused
		// mode =  0 : nearest link
		// mode =  1 : upper link
		// mode =  2 : lower link
		// mode =  3 : left link
		// mode =  4 : right link
		if(Key == VK_LEFT)
		{
			// select left link
			FocusLinkAt(Links[FocusedLink].LinkTier,
				(Links[FocusedLink].From + Links[FocusedLink].To)/2, 3);
		}
		else if(Key == VK_RIGHT)
		{
			// select right link
			FocusLinkAt(Links[FocusedLink].LinkTier,
				(Links[FocusedLink].From + Links[FocusedLink].To)/2,  4);
		}
		else if(Key == VK_UP)
		{
			// select upper tier or wave view
			if(Links[FocusedLink].LinkTier == 0)
			{
				CaretPos = (Links[FocusedLink].From + Links[FocusedLink].To)/2;
				EnsureView(CaretPos);
				ShowCaret = true;
			}
			else
			{
				FocusLinkAt(Links[FocusedLink].LinkTier,
					(Links[FocusedLink].From + Links[FocusedLink].To)/2, 1);
			}
		}
		else if(Key == VK_DOWN)
		{
			// select lower tier
			FocusLinkAt(Links[FocusedLink].LinkTier,
				(Links[FocusedLink].From + Links[FocusedLink].To)/2, 2);
		}
		else if(Key == VK_RETURN)
		{
			// enter key pressed
			PerformLinkDoubleClick();
		}
	}
	else/* if(ShowCaret) */
	{
		// no item is selected or currently the caret is showing
		ShowCaret = true;
		if(Key == VK_LEFT)
		{
			// move the caret left
			int xstep = FMagnify <= 0 ? 1 : (1<<FMagnify);
			int cp = SampleToPixel(CaretPos);
			cp -= xstep * ((Shift.Contains(ssShift)) ? 10 : 1);
			cp = PixelToSample(cp);
			if(cp < 0) cp = 0;
			EnsureView(cp);
			CaretPos = cp;
		}
		else if(Key == VK_RIGHT)
		{
			// move the caret right
			int xstep = FMagnify <= 0 ? 1 : (1<<FMagnify);
			int cp = SampleToPixel(CaretPos);
			cp += xstep * ((Shift.Contains(ssShift)) ? 10 : 1);
			cp = PixelToSample(cp);
			if(cp >= FReader->NumSamples) cp = FReader->NumSamples - 1;
			cp = PixelToSample(SampleToPixel(cp));
			EnsureView(cp);
			CaretPos = cp;
		}
		else if(Key == VK_UP)
		{
			// select label
			FocusLabelAt(CaretPos, 0);
		}
		else if(Key == VK_DOWN)
		{
			// select link
			FocusLinkAt(0, CaretPos, 0);
		}
	}
}
//---------------------------------------------------------------------------

