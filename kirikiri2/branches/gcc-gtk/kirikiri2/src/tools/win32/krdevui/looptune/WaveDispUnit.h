//---------------------------------------------------------------------------

#ifndef WaveDispUnitH
#define WaveDispUnitH
class TWaveReader;
class TWaveDisplayer;
//---------------------------------------------------------------------------
class TEntireBar : public TPanel
{
	TWaveDisplayer * FOwner;
public:
	__fastcall TEntireBar(TWinControl *owner);
	__fastcall ~TEntireBar();

protected:
	void __fastcall Paint(void);


	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);

};
//---------------------------------------------------------------------------
class TWaveDisplayer : public TCustomControl
{
	friend class TEntireBar;

	TWaveReader *FWaveReader;

public:
	__fastcall TWaveDisplayer(TWinControl *owner);
	__fastcall ~TWaveDisplayer();

protected:
	TNotifyEvent FOnFollowCanceled;
	TNotifyEvent FOnSelectionChanged;
	TNotifyEvent FOnStartSelect;
	TNotifyEvent FOnEndSelect;
	TNotifyEvent FOnMouseLeave;
	bool FShowTimeBar;
	bool FShowEntireBar;
	bool FFirstPaint;
	int FMagnify;
	int FSelStart;
	int FSelEnd;
	bool FMouseDown;
	POINT FFirstMouseDownPos;
	int FFirstDragPos;
	int FMouseDragAdj;
	int FMouseSelStart;
	bool FMouseDrag;
	bool FFollowMarker;
	int FMarker;
	int LastMarker;
	bool FWaitingFollowing; // true = カーソルが右端に行くまで待ち
	TPanel *EntireBar;

	void __fastcall CreateParams(TCreateParams &params);
	void __fastcall Paint(void);
	void __fastcall SetWaveReader(TWaveReader *wavereader);

	void __fastcall SetShowTimeBar(bool b);
	void __fastcall SetShowEntireBar(bool b);

	void __fastcall SetScrollRange(void);

	int __fastcall GetViewOrigin(void);
	int __fastcall GetViewRange(void);

	void __fastcall WMHScroll(TWMHScroll &msg);

	void __fastcall CMMouseLeave(TMessage &msg);

	void __fastcall SetMagnify(int n);

	void __fastcall SetSelStart(int n);
	int __fastcall GetSelStart();
	void __fastcall SetSelLength(int n);
	int __fastcall GetSelLength();


	void __fastcall SetFollowMarker(bool b);
	void __fastcall SetMarker(int n);

	void __fastcall UpdateMarker(int op, int np);

	int __fastcall GetDragPos(int x, int *ofs = NULL);
	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseUp(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
	DYNAMIC void __fastcall Resize(void);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_HSCROLL , TWMHScroll , WMHScroll)
	VCL_MESSAGE_HANDLER(CM_MOUSELEAVE, TMessage, CMMouseLeave);
END_MESSAGE_MAP(TCustomControl)

public:
	void __fastcall SetSelection(int start, int end);

	int __fastcall ChannelFromMousePos(int y);

	int __fastcall MousePosToSamplePos(int x);
	int __fastcall LastClickedMousePosToSamplePos();

	void __fastcall ScrollTo(int pos, bool force = true);
	void __fastcall ScrollToAndCancelFollow(int pos);

	void __fastcall SetWaitingFollowing() { FWaitingFollowing = true; LastMarker = -1; }

	__property TWaveReader * WaveReader = {read=FWaveReader, write=SetWaveReader};
	__property bool ShowTimeBar = {read = FShowTimeBar , write=SetShowTimeBar};
	__property bool ShowEntireBar = {read = FShowEntireBar , write=SetShowEntireBar};
	__property int Magnify = {read=FMagnify, write=SetMagnify};
	__property int SelStart = {read=GetSelStart, write=SetSelStart};
	__property int SelLength = {read=GetSelLength, write=SetSelLength};
	__property bool FollowMarker = {read=FFollowMarker, write=SetFollowMarker};
	__property int Marker = {read=FMarker, write=SetMarker};
	__property TNotifyEvent OnFollowCanceled = { read=FOnFollowCanceled,
		write = FOnFollowCanceled};
	__property TNotifyEvent OnSelectionChanged = { read= FOnSelectionChanged,
		write = FOnSelectionChanged};
	__property TNotifyEvent OnStartSelect = { read = FOnStartSelect,
		write = FOnStartSelect};
	__property TNotifyEvent OnEndSelect = { read = FOnEndSelect,
		write = FOnEndSelect};
	__property TNotifyEvent OnMouseLeave = {read = FOnMouseLeave, write=FOnMouseLeave};


	__property OnMouseMove;
	__property OnMouseDown;
	__property OnMouseUp;
	__property OnClick;
	__property OnDblClick;
	__property PopupMenu;
};
//---------------------------------------------------------------------------
#endif
