//---------------------------------------------------------------------------

#ifndef WaveViewH
#define WaveViewH

#include "WaveLoopManager.h"

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
/*
typedef void __fastcall (__closure *TNotifyWavePosEvent)(TObject *Sender, int pos);
typedef void __fastcall (__closure *TNotifyLinkEvent)(TObject *Sender, int link);
typedef void __fastcall (__closure *TNotifyLinkDragStartEvent)(
	TObject *Sender, int link, bool from_or_to, bool &enable_drag);
typedef void __fastcall (__closure *TNotifyLinkDragOverEvent)(
	TObject *Sender, int link, bool from_or_to, int pos);
typedef void __fastcall (__closure *TNotifyLinkDragDropEvent)(
	TObject *Sender, int link, bool from_or_to, int pos);
*/
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
	int GetHeadSize() const { return FDrawRuler ? (FMinRulerMajorHeight + 1) : 0; }
	int GetFootSize() const { return FDrawLinks ? (FLinkTierCount * TVP_LGD_TIER_HEIGHT) : 0; }
	void __fastcall Paint(void);

public:
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);
	void __fastcall SetReader(TWaveReader * reader);

public:
	__property TWaveReader * Reader = { read = FReader, write = SetReader };
	__property int Start = { read = FStart, write = SetStart };
	__property int Magnify = { read = FMagnify, write = SetMagnify };

//-- view control
private:
	bool FFollowingMarker;
	bool FWaitingMarker;

	DWORD FSoftCenteringStartTick;
	int   FSoftCenteringPos;

	void __fastcall CreateParams(TCreateParams &params);
	DYNAMIC void __fastcall Resize();
	void __fastcall SetScrollBarRange();
	void __fastcall SetView(int n, int r = 5);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_HSCROLL , TWMHScroll , WMHScroll)
	VCL_MESSAGE_HANDLER(CM_MOUSELEAVE , TMessage , CMMouseLeave)
END_MESSAGE_MAP(TCustomControl)
	void __fastcall WMHScroll(TWMHScroll &msg);
	void __fastcall CMMouseLeave(TMessage &msg);

//-- caret drawing stuff
	int FCaretPos; // caret position
	bool FDrawCaret;
	bool FCaretVisiblePhase;
	TTimer * FBlinkTimer;

	void __fastcall InvalidateCaret(int pos);

	void __fastcall SetCaretPos(int pos);
	void __fastcall SetDrawCaret(bool b);

	void __fastcall OnBlinkTimer(TObject * sender);

public:
	__property int CaretPos = { read = FCaretPos, write = SetCaretPos };
	__property bool DrawCaret = { read = FDrawCaret, write = SetDrawCaret };

//-- wave drawing stuff
private:
	int FMinRulerMajorWidth;
	int FMinRulerMajorHeight;
	int FRulerUnit; // time in msec/10
	bool FDrawRuler; // whether to draw time line
	int FMarkerPos; // marker position for current playing position, -1 for invisible

private:
	void __fastcall SetStart(int n);
	void __fastcall SetMagnify(int m);
	void __fastcall SetMarkerPos(int p);

	void __fastcall DrawWave(int start, bool clear);

public:
	__property int MarkerPos = { read = FMarkerPos, write = SetMarkerPos };



//-- link drawing stuff
private:
	bool FDrawLinks;
	int FLinkTierCount;
	int FHoveredLink; // -1 for non visible
	int FFocusedLink; // -1 for not focused

public:
	void __fastcall NotifyLinkChanged();

private:
	void __fastcall DrawLinkArrowAt(int tier, int where);
	void __fastcall DrawDirectionArrowAt(int tier, bool dir, int where);
	void __fastcall GetLinkMinMaxPixel(const tTVPWaveLoopLink & link,
		TTierMinMaxInfo & info);
	void __fastcall DrawLinkOf(const tTVPWaveLoopLink & link);
	void __fastcall DrawLink(void);
	void __fastcall InvalidateLink(int linknum);
	void __fastcall SetHoveredLink(int l);
	void __fastcall SetFocusedLink(int l);
	bool __fastcall IsLinkAt(int linknum, int x, int y);
	int __fastcall GetLinkAt(int x, int y);
	bool __fastcall IsLinkWaveMarkAt(int x, int linknum, bool &from_or_to);
	bool __fastcall GetLinkWaveMarkAt(int x, int &linknum, bool &from_or_to);

public:
	__property int HoveredLink = { read = FHoveredLink, write = SetHoveredLink };
	__property int FocusedLink = { read = FFocusedLink, write = SetFocusedLink };

//-- input
private:
/*
	TNotifyWavePosEvent FOnWaveLButtonDown;
	TNotifyLinkEvent FOnLinkLButtonDown;
	TNotifyLinkDragStartEvent FOnLinkDragStart;
	TNotifyLinkDragOverEvent  FOnLinkDragOver;
	TNotifyLinkDragDropEvent  FOnLinkDragDrop;
*/
	int LastMouseDownX;
	int LastMouseDownLinkNum;
	bool LastMouseDownLinkFromOrTo;
	enum { dsNone, dsMouseDown, dsDragging } DraggingState;

	int MouseXPosToSamplePos(int x);


	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseUp(TMouseButton button, TShiftState shift, int x, int y);

public:
	void __fastcall MouseLeave();

public:
/*
	__property TNotifyWavePosEvent OnWaveLButtonDown = { read = FOnWaveLButtonDown, write = FOnWaveLButtonDown };
	__property TNotifyLinkEvent OnLinkLButtonDown = { read = FOnLinkLButtonDown, write = FOnLinkLButtonDown };
	__property TNotifyLinkDragStartEvent OnLinkDragStart = { read = FOnLinkDragStart, write = FOnLinkDragStart };
	__property TNotifyLinkDragOverEvent  OnLinkDragOver  = { read = FOnLinkDragOver , write = FOnLinkDragOver  };
	__property TNotifyLinkDragDropEvent  OnLinkDragDrop  = { read = FOnLinkDragDrop , write = FOnLinkDragDrop  };
*/
};
//---------------------------------------------------------------------------
#endif
