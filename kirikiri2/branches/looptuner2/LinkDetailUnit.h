//---------------------------------------------------------------------------

#ifndef LinkDetailUnitH
#define LinkDetailUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
#include "WaveReader.h"
#include "WaveLoopManager.h"
#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TLinkDetailForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPanel *ToolBarPanel;
	TToolBar *PosAdjustToolBar;
	TToolButton *BeforePrevCrossToolButton;
	TToolButton *BeforePrevFastToolButton;
	TToolButton *BeforePrevStepToolButton;
	TToolButton *BeforeNextStepToolButton;
	TToolButton *BeforeNextFastToolButton;
	TToolButton *BeforeNextCrossToolButton;
	TToolButton *AfterPrevCrossToolButton;
	TToolButton *AfterPrevFastToolButton;
	TToolButton *AfterPrevStepToolButton;
	TToolButton *AfterNextStepToolButton;
	TToolButton *AfterNextFastToolButton;
	TToolButton *AfterNextCrossToolButton;
	TPanel *BottomPanel;
	TToolBar *BottomToolBar;
	TToolButton *ZoomInToolButton;
	TToolButton *ZoomOutToolButton;
	TButton *OKButton;
	TButton *CancelButton;
	TPanel *WaveAreaPanel;
	TPaintBox *WavePaintBox;
	TToolButton *ToolButton17;
	TToolButton *ToolButton15;
	TToolButton *PlayHalfSecToolButton;
	TToolButton *Play1SecToolButton;
	TToolButton *Play2SecToolButton;
	TToolButton *Play3SecToolButton;
	TToolButton *Play5SecToolButton;
	TLabel *MagnifyLabel;
	TActionList *ActionList;
	TAction *ZoomInAction;
	TAction *ZoomOutAction;
	TAction *PlayHalfSecAction;
	TAction *Play1SecAction;
	TAction *Play2SecAction;
	TAction *Play3SecAction;
	TAction *Play5SecAction;
	TAction *SmoothAction;
	TToolButton *ToolButton1;
	TAction *BeforePrevCrossAction;
	TAction *BeforePrevFastAction;
	TAction *BeforePrevStepAction;
	TAction *BeforeNextStepAction;
	TAction *BeforeNextFastAction;
	TAction *BeforeNextCrossAction;
	TToolButton *ToolButton2;
	void __fastcall WavePaintBoxPaint(TObject *Sender);
	void __fastcall ZoomInActionExecute(TObject *Sender);
	void __fastcall ZoomOutActionExecute(TObject *Sender);
	void __fastcall BeforePrevCrossActionExecute(TObject *Sender);
	void __fastcall BeforePrevFastActionExecute(TObject *Sender);
	void __fastcall BeforePrevStepActionExecute(TObject *Sender);
	void __fastcall BeforeNextStepActionExecute(TObject *Sender);
	void __fastcall BeforeNextFastActionExecute(TObject *Sender);
	void __fastcall BeforeNextCrossActionExecute(TObject *Sender);
	void __fastcall AfterPrevCrossActionExecute(TObject *Sender);
	void __fastcall AfterPrevFastActionExecute(TObject *Sender);
	void __fastcall AfterPrevStepActionExecute(TObject *Sender);
	void __fastcall AfterNextStepActionExecute(TObject *Sender);
	void __fastcall AfterNextFastActionExecute(TObject *Sender);
	void __fastcall AfterNextCrossActionExecute(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall WavePaintBoxMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall WavePaintBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall WavePaintBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// ユーザー宣言
	TWaveReader * FReader; // wave reader
	tTVPWaveLoopLink FLink; // current editting link
	int FMagnify; // magnification level (in logarithm based 2)

	bool Dragging;
	bool BeforeOrAfter;
	int FirstMouseDownPos;
	int FirstMouseDownX;

public:		// ユーザー宣言
	__fastcall TLinkDetailForm(TComponent* Owner);
	void __fastcall SetReaderAndLink(TWaveReader * reader, tTVPWaveLoopLink link);

public:
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);

private:
	void __fastcall UpdateDisplay();
	void __fastcall UpdateLayout();

};
//---------------------------------------------------------------------------
extern PACKAGE TLinkDetailForm *LinkDetailForm;
//---------------------------------------------------------------------------
#endif
