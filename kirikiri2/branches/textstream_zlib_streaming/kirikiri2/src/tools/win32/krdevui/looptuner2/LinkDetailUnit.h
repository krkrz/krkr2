//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
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
#include <AppEvnts.hpp>
#include "EditLinkAttribUnit.h"
#include <Menus.hpp>
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
	TToolButton *SmoothToolButton;
	TAction *BeforePrevCrossAction;
	TAction *BeforePrevFastAction;
	TAction *BeforePrevStepAction;
	TAction *BeforeNextStepAction;
	TAction *BeforeNextFastAction;
	TAction *BeforeNextCrossAction;
	TToolButton *ToolButton2;
	TApplicationEvents *ApplicationEvents;
	TPanel *PlayBeforePaintBoxPanel;
	TPaintBox *PlayBeforePaintBox;
	TLabel *PlayBeforeLabel;
	TEditLinkAttribFrame *EditLinkAttribFrame;
	TToolButton *StopPlayToolButton;
	TAction *StopPlayAction;
	TLabel *AfterLinkLabel;
	TLabel *BeforeLinkLabel;
	TPopupMenu *WaveAreaPopupMenu;
	TMenuItem *ZoomInMenuItem;
	TMenuItem *ZoomOutMenuItem;
	TMenuItem *N1;
	TMenuItem *StopPlayMenuItem;
	TMenuItem *BeforePrevCrossMenuItem;
	TMenuItem *BeforePrevFastMenuItem;
	TMenuItem *PlayMenuItem;
	TAction *PlayAction;
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
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall PlayHalfSecActionExecute(TObject *Sender);
	void __fastcall Play1SecActionExecute(TObject *Sender);
	void __fastcall Play2SecActionExecute(TObject *Sender);
	void __fastcall Play3SecActionExecute(TObject *Sender);
	void __fastcall Play5SecActionExecute(TObject *Sender);
	void __fastcall ApplicationEventsIdle(TObject *Sender, bool &Done);
	void __fastcall PlayBeforePaintBoxPaint(TObject *Sender);
	void __fastcall SmoothActionExecute(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall StopPlayActionExecute(TObject *Sender);
	void __fastcall WavePaintBoxDblClick(TObject *Sender);
	void __fastcall WaveAreaPanelEnter(TObject *Sender);
	void __fastcall WaveAreaPanelExit(TObject *Sender);
	void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
	void __fastcall PlayActionExecute(TObject *Sender);
	void __fastcall PlayMenuItemClick(TObject *Sender);
private:	// ユーザー宣言
	TWaveReader * FReader; // wave reader
	tTVPWaveLoopManager * FManager; // wave loop manager
	int LinkNum; // link number
	tTVPWaveLoopLink FLink; // current editting link
	tTVPWaveLoopLink FLinkOriginal; // the original information of editting link
	int FMagnify; // magnification level (in logarithm based 2)

	bool WaveAreaHasFocus;

	bool Dragging;
	bool BeforeOrAfter;
	int FirstMouseDownPos;
	int FirstMouseDownX;

	int PlayBeforeLength;
	int PlayStartPos;
	int PlayLastPos;

public:		// ユーザー宣言
	__fastcall TLinkDetailForm(TComponent* Owner);
	void __fastcall SetReaderAndLink(TWaveReader * reader, tTVPWaveLoopLink link, int linknum);

public:
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);

private:
	void __fastcall UpdateMainWindowParams(bool push_undo = false);
	void __fastcall UpdateDisplay();
	void __fastcall UpdateLayout();
	int __fastcall SearchCrossingPoint(int from, int direction);
	void __fastcall StepBefore(int step);
	void __fastcall StepAfter(int step);
	void __fastcall PlayLink(int before, int after = 5000);
	void __fastcall PlayLink();
	void __fastcall MarkPlayButton(TObject * button);
	void __fastcall EditLinkAttribFrameInfoChanged(TObject * Sender);
	void __fastcall EditLinkAttribFrameEraseRedo(TObject * Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TLinkDetailForm *LinkDetailForm;
//---------------------------------------------------------------------------
#endif
