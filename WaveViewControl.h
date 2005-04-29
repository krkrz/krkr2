#ifndef WAVEVIEWCLIENT_H
#define WAVEVIEWCLIENT_H

#include "WaveView.h"

//---------------------------------------------------------------------------
class TWaveReader;
class TWaveDrawer;
class TWaveView : public TCustomControl
{
	TWaveReader *FReader;
	TWaveDrawer *FDrawer;

	bool FFollowingMarker;
	bool FWaitingMarker;

	DWORD FSoftCenteringStartTick;
	int   FSoftCenteringPos;

public:
	__fastcall TWaveView(TWinControl *owner);
	__fastcall ~TWaveView();

private:
	void __fastcall CreateParams(TCreateParams &params);
	void __fastcall SetReader(TWaveReader * reader);
	int __fastcall GetMagnify();
	void __fastcall SetMagnify(int m);
	int __fastcall GetMarkerPos();
	void __fastcall SetMarkerPos(int n);
	TNotifyWavePosEvent __fastcall GetOnWaveLButtonDown() { return FDrawer->OnWaveLButtonDown; }
	void __fastcall SetOnWaveLButtonDown(TNotifyWavePosEvent ev) { FDrawer->OnWaveLButtonDown = ev; }
	TNotifyLinkEvent __fastcall GetOnLinkLButtonDown() { return FDrawer->OnLinkLButtonDown; }
	void __fastcall SetOnLinkLButtonDown(TNotifyLinkEvent ev) { FDrawer->OnLinkLButtonDown = ev; }
	int __fastcall GetCaretPos() { return FDrawer->CaretPos; }
	void __fastcall SetCaretPos(int v) { FDrawer->CaretPos = v; }
	bool __fastcall GetDrawCaret() { return FDrawer->DrawCaret; }
	void __fastcall SetDrawCaret(bool v) { FDrawer->DrawCaret = v; }
	int __fastcall GetFocusedLink() { return FDrawer->FocusedLink; }
	void __fastcall SetFocusedLink(int v) { FDrawer->FocusedLink = v; }



	void __fastcall Paint(void);
	DYNAMIC void __fastcall Resize();
	void __fastcall SetScrollBarRange();
	void __fastcall SetView(int n, int r = 5);


BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_HSCROLL , TWMHScroll , WMHScroll)
	VCL_MESSAGE_HANDLER(CM_MOUSELEAVE , TMessage , CMMouseLeave)
END_MESSAGE_MAP(TCustomControl)
	void __fastcall WMHScroll(TWMHScroll &msg);
	void __fastcall CMMouseLeave(TMessage &msg);

public:
	__property TWaveReader * Reader = { read = FReader, write = SetReader };
	__property int Magnify = { read = GetMagnify, write = SetMagnify };
	__property int MarkerPos = { read = GetMarkerPos, write = SetMarkerPos };
	__property TNotifyWavePosEvent OnWaveLButtonDown = { read = GetOnWaveLButtonDown, write = SetOnWaveLButtonDown };
	__property TNotifyLinkEvent OnLinkLButtonDown = { read = GetOnLinkLButtonDown, write = SetOnLinkLButtonDown };
	__property int CaretPos = { read = GetCaretPos, write = SetCaretPos };
	__property bool DrawCaret = { read = GetDrawCaret, write = SetDrawCaret };
	__property int FocusedLink = { read = GetFocusedLink, write = SetFocusedLink };

};
//---------------------------------------------------------------------------


#endif
