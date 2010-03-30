//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef WaveViewH
#define WaveViewH

#include "WaveLoopManager.h"

#include <vector>
#include <deque>
//---------------------------------------------------------------------------
#define TVP_LGD_TIER_HEIGHT 12
//---------------------------------------------------------------------------
struct TTierMinMaxInfo
{
	// structure for bring back infomration from GetLinkMinMaxPixel
	int FromMin;
	int FromMax;
	int LinkMin;
	int LinkMax;
	int ToMin;
	int ToMax;
};
//---------------------------------------------------------------------------
class TWaveReader;
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TCaretStateChanged)(TObject *Sender, int caretpos, bool showcaret);
typedef TCaretStateChanged TMarkerStateChanged;
typedef void __fastcall (__closure *TWaveViewRangeChanged)(TObject *Sender, int viewstart, int viewlength);
typedef void __fastcall (__closure *TNotifyWavePosEvent)(TObject *Sender, int pos);
typedef void __fastcall (__closure *TNotifyPopupEvent)(TObject *Sender, AnsiString where, const TPoint & pos);
typedef void __fastcall (__closure *TShowCaretEvent)(TObject *Sender, int pos);
typedef void __fastcall (__closure *TLinkSelectedEvent)(TObject *Sender, int num, tTVPWaveLoopLink &link);
typedef void __fastcall (__closure *TLabelSelectedEvent)(TObject *Sender, int num, tTVPWaveLabel &label);
//---------------------------------------------------------------------------
class TWaveView : public TCustomControl
{
	typedef TCustomControl inherited;

//-- constructor and destructor
public:
	__fastcall TWaveView(Classes::TComponent* AOwner);
	__fastcall ~TWaveView();

	void __fastcall ClearAll();

//-- common stuff
private:
	TWaveReader * FReader;
	int GetHeadSize() const { return FShowRuler ? (FMinRulerMajorHeight + 1) : 0; }
	int GetFootSize() const { return FShowLinks ? (FLinkTierCount * TVP_LGD_TIER_HEIGHT + TVP_LGD_TIER_HEIGHT / 2) : 0; }
	void __fastcall Paint(void);
	void __fastcall SetReader(TWaveReader * reader);

public:
	void __fastcall ResetAll();

	__property TWaveReader * Reader = { read = FReader, write = SetReader };


//-- editing support
private:
	TNotifyPopupEvent FOnNotifyPopup;
	TShowCaretEvent FOnShowCaret;
	TLinkSelectedEvent FOnLinkSelected;
	TLabelSelectedEvent FOnLabelSelected;
	TNotifyEvent FOnSelectionLost;
	TNotifyEvent FOnLinkModified;
	bool FInOnLinkModified; // to prevent re-entrering
	TNotifyEvent FOnLabelModified;
	bool FInOnLabelModified; // to prevent re-entrering
	bool FModified;

	struct tHistoryInfo
	{
		std::vector<tTVPWaveLoopLink> Links;
		std::vector<tTVPWaveLabel> Labels;
	};
	std::deque<tHistoryInfo> FUndoStack;
	unsigned int FUndoLevel;

public:
	void __fastcall EraseRedo();
	void __fastcall PushUndo();
	void __fastcall PushFirstUndoState();

	void __fastcall Undo();
	void __fastcall Redo();
	bool __fastcall CanUndo() const;
	bool __fastcall CanRedo() const;

	void __fastcall DeleteItem();
	bool __fastcall CanDeleteItem() const;
	void __fastcall ClearAllLabels();

	__property TNotifyPopupEvent OnNotifyPopup =
		{ read = FOnNotifyPopup, write = FOnNotifyPopup };

	__property TShowCaretEvent OnShowCaret =
		{ read = FOnShowCaret, write = FOnShowCaret };

	__property TLinkSelectedEvent OnLinkSelected =
		{ read = FOnLinkSelected, write = FOnLinkSelected };

	__property TLabelSelectedEvent OnLabelSelected =
		{ read = FOnLabelSelected, write = FOnLabelSelected };

	__property TNotifyEvent OnSelectionLost =
		{ read = FOnSelectionLost, write = FOnSelectionLost };

	__property TNotifyEvent OnLinkModified =
		{ read = FOnLinkModified, write = FOnLinkModified };

	__property TNotifyEvent OnLabelModified =
		{ read = FOnLabelModified, write = FOnLabelModified };

	__property bool Modified =
		{ read = FModified, write = FModified };


//-- view control
private:
	int FMagnify; // magnification level (in logarithm based 2)
	int FStart; // start sample position at left edge of the client
	bool FDoubleBufferEnabled;
	bool FFollowingMarker;
	bool FWaitingMarker;
	int FMarkerPos; // marker position for current playing position, -1 for invisible
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);
	TNotifyEvent FOnStopFollowingMarker;
	TMarkerStateChanged FOnMarkerStateChanged;
	TWaveViewRangeChanged FOnRangeChanged;

	DWORD FSoftCenteringStartTick;
	int   FSoftCenteringPos;

	void __fastcall CreateParams(TCreateParams &params);
	DYNAMIC void __fastcall Resize();
	void __fastcall SetScrollBarRange();

public:
	void __fastcall SetView(int n, int r = 50);

private:
BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_HSCROLL , TWMHScroll , WMHScroll)
	VCL_MESSAGE_HANDLER(CM_MOUSELEAVE , TMessage , CMMouseLeave)
	VCL_MESSAGE_HANDLER(CM_WANTSPECIALKEY , TCMWantSpecialKey, CMWantSpecialKey)
	VCL_MESSAGE_HANDLER(WM_SETFOCUS , TWMSetFocus , WMSetFocus)
	VCL_MESSAGE_HANDLER(WM_KILLFOCUS , TWMKillFocus , WMKillFocus)
END_MESSAGE_MAP(TCustomControl)
	void __fastcall WMHScroll(TWMHScroll &msg);
	void __fastcall WMSetFocus(TWMSetFocus &msg);
	void __fastcall WMKillFocus(TWMKillFocus &msg);
	void __fastcall SetMarkerPos(int p);
	void __fastcall SetFollowingMarker(bool b);
public:
	void __fastcall ResetMarkerFollow();
private:
	void __fastcall FireRangeChanged();
	void __fastcall SetStart(int n);
	void __fastcall SetMagnify(int m);

public:
	void __fastcall SetInitialMagnify();
	AnsiString __fastcall GetMagnifyString();
	void __fastcall EnsureView(int p, int length = 0);

public:
	void __fastcall GetPositionStringAt(int vx, int vy, AnsiString &pos, AnsiString &channel);

	__property int Start = { read = FStart, write = SetStart };
	__property int Magnify = { read = FMagnify, write = SetMagnify };
	__property bool FollowingMarker =
		{ read = FFollowingMarker, write = SetFollowingMarker };
	__property TNotifyEvent OnStopFollowingMarker =
		{ read = FOnStopFollowingMarker, write = FOnStopFollowingMarker };
	__property int MarkerPos = { read = FMarkerPos, write = SetMarkerPos };
	__property TWaveViewRangeChanged OnRangeChanged =
		{ read = FOnRangeChanged, write = FOnRangeChanged };
	__property TMarkerStateChanged OnMarkerStateChanged =
		{ read = FOnMarkerStateChanged, write = FOnMarkerStateChanged };

//-- caret drawing stuff
	int FCaretPos; // caret position
	bool FShowCaret;
	bool FCaretVisiblePhase;
	TTimer * FBlinkTimer;
	TCaretStateChanged FOnCaretStateChanged;

	void __fastcall InvalidateCaret(int pos);

	void __fastcall SetCaretPos(int pos);
	void __fastcall SetShowCaret(bool b);

	void __fastcall OnBlinkTimer(TObject * sender);
	void __fastcall ResetCaretFocusState();

	void __fastcall DrawCaret();

	int __fastcall GetAttentionPos();

public:
	__property int CaretPos = { read = FCaretPos, write = SetCaretPos };
	__property bool ShowCaret = { read = FShowCaret, write = SetShowCaret };
	__property TCaretStateChanged OnCaretStateChanged =
		{ read = FOnCaretStateChanged, write = FOnCaretStateChanged };

//-- wave drawing stuff
private:
	int FMinRulerMajorWidth;
	int FMinRulerMajorHeight;
	int FRulerUnit; // time in msec/10
	bool FShowRuler; // whether to draw time line

private:
	void __fastcall DrawWave(int start, bool clear);




//-- link drawing stuff
private:
	std::vector<tTVPWaveLoopLink> Links;
	bool FShowLinks;
	int FLinkTierCount;
	int FHoveredLink; // -1 for not hovered
	int FFocusedLink; // -1 for not focused

public:
	void __fastcall NotifyLinkChanged();
	void __fastcall SetLinks(const std::vector<tTVPWaveLoopLink> & links);
	const std::vector<tTVPWaveLoopLink> & GetLinks() const { return Links; }
	std::vector<tTVPWaveLoopLink> & GetLinks() { return Links; }

private:
	void __fastcall DrawLinkArrowAt(int tier, int where);
	void __fastcall DrawDirectionArrowAt(int tier, bool dir, int where);
	void __fastcall GetLinkMinMaxPixel(const tTVPWaveLoopLink & link,
		TTierMinMaxInfo & info);
	void __fastcall DrawLinkOf(const tTVPWaveLoopLink & link);
	void __fastcall DrawLinks(void);
public:
	void __fastcall InvalidateLink(int linknum);
private:
	void __fastcall SetHoveredLink(int l);
	void __fastcall SetFocusedLink(int l);
	bool __fastcall IsLinkAt(int linknum, int x, int y);
	int __fastcall GetLinkAt(int x, int y);
	int __fastcall IsLinkWaveMarkAt(int x, int linknum, bool &from_or_to);
	int __fastcall GetLinkWaveMarkAt(int x, int &linknum, bool &from_or_to);
	int __fastcall SelectLink(int tier, int spos, int mode);
	void __fastcall FocusLinkAt(int tier, int spos, int mode);

public:
	__property int HoveredLink = { read = FHoveredLink, write = SetHoveredLink };
	__property int FocusedLink = { read = FFocusedLink, write = SetFocusedLink };

	void __fastcall CreateNewLink();

//-- label drawing stuff
private:
	std::vector<tTVPWaveLabel> Labels;
	bool FShowLabels;
	int FHoveredLabel; // -1 for not hovered
	int FFocusedLabel; // -1 for not focused
	int LabelTextHeight; // label's text height

public:
	void __fastcall NotifyLabelChanged();
	void __fastcall SetLabels(const std::vector<tTVPWaveLabel> & labels);
	const std::vector<tTVPWaveLabel> & GetLabels() const { return Labels; }
	std::vector<tTVPWaveLabel> & GetLabels() { return Labels; }

private:
	void __fastcall GetLabelNameRect(tTVPWaveLabel & label, TRect & rect);
	void __fastcall DrawLabelMarkOf(tTVPWaveLabel & label);
	void __fastcall DrawLabelNameOf(tTVPWaveLabel & label);
	void __fastcall DrawLabels();
public:
	void __fastcall InvalidateLabel(int labelnum);
private:
	void __fastcall SetHoveredLabel(int l);
	void __fastcall SetFocusedLabel(int l);
	bool __fastcall IsLabelAt(int labelnum, int x, int y);
	bool __fastcall IsLabelNameAt(int labelnum, int x, int y);
	int __fastcall GetLabelAt(int x, int y);
	int __fastcall IsLabelWaveMarkAt(int x, int labelnum);
	int __fastcall GetLabelWaveMarkAt(int x, int &labelnum);
	int __fastcall SelectLabel(int spos, int mode);
	void __fastcall FocusLabelAt(int spos, int mode);

public:
	__property int HoveredLabel = { read = FHoveredLabel, write = SetHoveredLabel };
	__property int FocusedLabel = { read = FFocusedLabel, write = SetFocusedLabel };

	void __fastcall CreateNewLabel(int pos = -1);

//-- mouse input
private:
	TNotifyWavePosEvent FOnWaveDoubleClick;
	TLinkSelectedEvent FOnLinkDoubleClick;
	TLabelSelectedEvent FOnLabelDoubleClick;

	enum TObjectKind { okLink, okLabel };
	enum TDraggingState { dsNone, dsMouseDown, dsDragging };

	struct TObjectInfo
	{
		TObjectKind Kind; // object kind
		int Num; // object number
		bool FromOrTo; // 'from' or 'to' (for link only)
		int Position; // object position
	};

	TTimer * DragScrollTimer;
	int LastMouseDownX;
	int LastMouseDownY;
	int LastMouseDownPosOffset;
	int LastMouseMoveX;
	AnsiString PopupType;

	bool DisableNextMouseDown;

	int LastClickedPos[2]; // last clicked two positions; only used for new link creation

	TObjectInfo DraggingObjectInfo;
	TDraggingState DraggingState;

	void PushLastClickedPos(int pos);

	bool GetNearestObjectAt(int x, TObjectInfo & info);
	int MouseXPosToSamplePos(int x);

	DYNAMIC bool __fastcall DoMouseWheel(Classes::TShiftState Shift, int WheelDelta,
		const Windows::TPoint &MousePos);
	void __fastcall CMMouseLeave(TMessage &msg);
	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseUp(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall DblClick(void);
	DYNAMIC void __fastcall DoContextPopup(const Windows::TPoint & MousePos, bool &Handled);

public:
	void __fastcall PerformLinkDoubleClick();
	void __fastcall PerformLabelDoubleClick();
private:
	void __fastcall MouseLeave();
	void __fastcall OnDragScrollTimer(TObject * sender);

public:
	__property TNotifyWavePosEvent OnWaveDoubleClick = { read = FOnWaveDoubleClick, write = FOnWaveDoubleClick };
	__property TLinkSelectedEvent OnLinkDoubleClick = { read = FOnLinkDoubleClick, write = FOnLinkDoubleClick };
	__property TLabelSelectedEvent OnLabelDoubleClick = { read = FOnLabelDoubleClick, write = FOnLabelDoubleClick };

	__property PopupMenu;

//-- keyboad input
	void __fastcall CMWantSpecialKey(TCMWantSpecialKey &message);
	DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);

};
//---------------------------------------------------------------------------
#endif
