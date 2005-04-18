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
public:
	__fastcall TWaveView(TWinControl *owner);
	__fastcall ~TWaveView();

private:
	void __fastcall CreateParams(TCreateParams &params);
	void __fastcall SetReader(TWaveReader * reader);
	int __fastcall GetMagnify();
	void __fastcall SetMagnify(int m);
	int __fastcall GetMarkerPos()  { return FDrawer->MarkerPos; }
	void __fastcall SetMarkerPos(int n)  { FDrawer->MarkerPos = n; }

	void __fastcall Paint(void);
	DYNAMIC void __fastcall Resize();

	void __fastcall SetScrollBarRange();

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_HSCROLL , TWMHScroll , WMHScroll)
END_MESSAGE_MAP(TCustomControl)
	void __fastcall WMHScroll(TWMHScroll &msg);

public:
	__property TWaveReader * Reader = { read = FReader, write = SetReader };
	__property int Magnify = { read = GetMagnify, write = SetMagnify };
	__property int MarkerPos = { read = GetMarkerPos, write = SetMarkerPos };

};
//---------------------------------------------------------------------------


#endif
