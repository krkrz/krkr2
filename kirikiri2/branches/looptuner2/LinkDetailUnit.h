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
//---------------------------------------------------------------------------
class TLinkDetailForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPanel *ToolBarPanel;
	TToolBar *BeforeToolBar;
	TToolBar *AfterToolBar;
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
	TCheckBox *SmoothCheckBox;
	TToolButton *ToolButton15;
	TToolButton *PlayHalfSecToolButton;
	TToolButton *Play1SecToolButton;
	TToolButton *Play2SecToolButton;
	TToolButton *Play3SecToolButton;
	TToolButton *Play5SecToolButton;
	void __fastcall WavePaintBoxPaint(TObject *Sender);
private:	// ユーザー宣言
	TWaveReader * FReader; // wave reader
	tTVPWaveLoopLink FLink; // current editting link
	int FMagnify; // magnification level (in logarithm based 2)

public:		// ユーザー宣言
	__fastcall TLinkDetailForm(TComponent* Owner);
	void __fastcall SetReaderAndLink(TWaveReader * reader, tTVPWaveLoopLink link);

public:
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);

};
//---------------------------------------------------------------------------
extern PACKAGE TLinkDetailForm *LinkDetailForm;
//---------------------------------------------------------------------------
#endif
