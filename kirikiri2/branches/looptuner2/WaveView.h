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
typedef void __fastcall (__closure *TNotifyWavePosEvent)(TObject *Sender, int pos);
class TWaveDrawer : public TGraphicControl
{
//-- constructor and destructor
public:
	__fastcall TWaveDrawer(Classes::TComponent* AOwner);
	__fastcall ~TWaveDrawer();

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

public:
	void __fastcall NotifyLinkChanged();

private:
	void __fastcall DrawLinkArrowAt(int tier, int where);
	void __fastcall DrawDirectionArrowAt(int tier, bool dir, int where);
	void __fastcall GetLinkMinMaxPixel(const tTVPWaveLoopLink & link,
		TTierMinMaxInfo & info);
	void __fastcall DrawLink(void);


//-- input
private:
	TNotifyWavePosEvent FOnLButtonDown;

	int MouseXPosToSamplePos(int x);

	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);


public:
	__property TNotifyWavePosEvent OnLButtonDown = { read = FOnLButtonDown, write = FOnLButtonDown };

};
//---------------------------------------------------------------------------
#endif
