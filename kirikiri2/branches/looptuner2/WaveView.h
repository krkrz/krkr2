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
typedef void __fastcall (__closure *TNotifyWavePosEvent)(TObject *Sender, int pos);
//---------------------------------------------------------------------------
class TWaveView : public TCustomControl
{
//-- constructor and destructor
public:
	__fastcall TWaveView(Classes::TComponent* AOwner);
	__fastcall ~TWaveView();

//-- common stuff
private:
	TWaveReader * FReader;
	int FMagnify; // magnification level (in logarithm based 2)
	int FStart; // start sample position at left edge of the client
	int GetHeadSize() const { return FShowRuler ? (FMinRulerMajorHeight + 1) : 0; }
	int GetFootSize() const { return FShowLinks ? (FLinkTierCount * TVP_LGD_TIER_HEIGHT + TVP_LGD_TIER_HEIGHT / 2) : 0; }
	void __fastcall Paint(void);

public:
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);
	void __fastcall SetReader(TWaveReader * reader);

public:
	__property TWaveReader * Reader = { read = FReader, write = SetReader };
	__property int Start = { read = FStart, write = SetStart };
	__property int Magnify = { read = FMagnify, write = SetMagnify };


//-- editing support
private:
	struct tHistoryInfo
	{
		std::vector<tTVPWaveLoopLink> Links;
		std::vector<tTVPWaveLabel> Labels;
	};
	std::deque<tHistoryInfo> FUndoStack;
	unsigned int FUndoLevel;

private:
	void __fastcall PushUndo();

public:
	void __fastcall Undo();
	void __fastcall Redo();
	bool __fastcall CanUndo() const;
	bool __fastcall CanRedo() const;

	void __fastcall DeleteItem();
	bool __fastcall CanDeleteItem() const;

//-- view control
private:
	bool FDoubleBufferEnabled;
	bool FFollowingMarker;
	bool FWaitingMarker;
	bool FFocused;

	DWORD FSoftCenteringStartTick;
	int   FSoftCenteringPos;

	void __fastcall CreateParams(TCreateParams &params);
	DYNAMIC void __fastcall Resize();
	void __fastcall SetScrollBarRange();
	void __fastcall SetView(int n, int r = 5);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_HSCROLL , TWMHScroll , WMHScroll)
	VCL_MESSAGE_HANDLER(CM_MOUSELEAVE , TMessage , CMMouseLeave)
	VCL_MESSAGE_HANDLER(WM_SETFOCUS , TWMSetFocus , WMSetFocus)
	VCL_MESSAGE_HANDLER(WM_KILLFOCUS , TWMKillFocus , WMKillFocus)
END_MESSAGE_MAP(TCustomControl)
	void __fastcall WMHScroll(TWMHScroll &msg);
	void __fastcall CMMouseLeave(TMessage &msg);
	void __fastcall WMSetFocus(TWMSetFocus &msg);
	void __fastcall WMKillFocus(TWMKillFocus &msg);

//-- caret drawing stuff
	int FCaretPos; // caret position
	bool FShowCaret;
	bool FCaretVisiblePhase;
	TTimer * FBlinkTimer;

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


//-- wave drawing stuff
private:
	int FMinRulerMajorWidth;
	int FMinRulerMajorHeight;
	int FRulerUnit; // time in msec/10
	bool FShowRuler; // whether to draw time line
	int FMarkerPos; // marker position for current playing position, -1 for invisible

private:
	void __fastcall SetStart(int n);
	void __fastcall SetMagnify(int m);
	void __fastcall SetMarkerPos(int p);
public:
	void __fastcall ResetMarkerFollow();
private:
	void __fastcall DrawWave(int start, bool clear);

public:
	__property int MarkerPos = { read = FMarkerPos, write = SetMarkerPos };



//-- link drawing stuff
private:
	std::vector<tTVPWaveLoopLink> Links;
	bool FShowLinks;
	int FLinkTierCount;
	int FHoveredLink; // -1 for not hovered
	int FFocusedLink; // -1 for not focused

public:
	std::vector<tTVPWaveLoopLink> &  GetLinks();
	void __fastcall NotifyLinkChanged();

private:
	void __fastcall DrawLinkArrowAt(int tier, int where);
	void __fastcall DrawDirectionArrowAt(int tier, bool dir, int where);
	void __fastcall GetLinkMinMaxPixel(const tTVPWaveLoopLink & link,
		TTierMinMaxInfo & info);
	void __fastcall DrawLinkOf(const tTVPWaveLoopLink & link);
	void __fastcall DrawLinks(void);
	void __fastcall InvalidateLink(int linknum);
	void __fastcall SetHoveredLink(int l);
	void __fastcall SetFocusedLink(int l);
	bool __fastcall IsLinkAt(int linknum, int x, int y);
	int __fastcall GetLinkAt(int x, int y);
	int __fastcall IsLinkWaveMarkAt(int x, int linknum, bool &from_or_to);
	int __fastcall GetLinkWaveMarkAt(int x, int &linknum, bool &from_or_to);

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

private:
	void __fastcall GetLabelNameRect(const tTVPWaveLabel & label, TRect & rect);
	void __fastcall DrawLabelOf(const tTVPWaveLabel & label);
	void __fastcall DrawLabels();
	void __fastcall InvalidateLabel(int labelnum);
	void __fastcall SetHoveredLabel(int l);
	void __fastcall SetFocusedLabel(int l);
	bool __fastcall IsLabelAt(int labelnum, int x, int y);
	bool __fastcall IsLabelNameAt(int labelnum, int x, int y);
	int __fastcall GetLabelAt(int x, int y);
	int __fastcall IsLabelWaveMarkAt(int x, int labelnum);
	int __fastcall GetLabelWaveMarkAt(int x, int &labelnum);

public:
	std::vector<tTVPWaveLabel> &  GetLabels();
	__property int HoveredLabel = { read = FHoveredLabel, write = SetHoveredLabel };
	__property int FocusedLabel = { read = FFocusedLabel, write = SetFocusedLabel };

	void __fastcall CreateNewLabel();

//-- input
private:
	TNotifyWavePosEvent FOnDoubleClick;

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
	int LastMouseDownPosOffset;
	int LastMouseMoveX;

	int LastClickedPos[2]; // last clicked two positions; only used for new link creation

	TObjectInfo DraggingObjectInfo;
	TDraggingState DraggingState;

	void PushLastClickedPos(int pos);

	bool GetNearestObjectAt(int x, TObjectInfo & info);
	int MouseXPosToSamplePos(int x);

	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseUp(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall DblClick(void);
	void __fastcall MouseLeave();
	void __fastcall OnDragScrollTimer(TObject * sender);

public:
	__property TNotifyWavePosEvent OnDoubleClick = { read = FOnDoubleClick, write = FOnDoubleClick };
};
//---------------------------------------------------------------------------
#endif
