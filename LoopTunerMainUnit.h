//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "WaveReader.h"
#include "WaveViewControl.h"
#include "WaveLoopManager.h"
#include <ToolWin.hpp>
#include "EditLabelAttribUnit.h"
#include "EditLinkAttribUnit.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TLoopTunerMainForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TMainMenu *MainMenu;
	TActionList *ActionList;
	TMenuItem *FileMenu;
	TAction *OpenAction;
	TImageList *ImageList;
	TMenuItem *OpenMenuItem;
	TStatusBar *StatusBar;
	TOpenDialog *OpenDialog;
	TToolBar *ToolBar1;
	TToolButton *OpenToolButton;
	TToolButton *ToolButton2;
	TAction *ZoomInAction;
	TAction *ZoomOutAction;
	TMenuItem *ViewMenu;
	TMenuItem *ZoomInMenuItem;
	TMenuItem *ZoomOutMenuItem;
	TToolButton *ZoomInToolButton;
	TToolButton *ZoomOutToolButton;
	TToolButton *ToolButton1;
	TToolButton *PlayToolButton;
	TAction *PlayFromStartAction;
	TToolButton *StopToolButton;
	TAction *StopPlayAction;
	TToolButton *PlayFromCursorToolButton;
	TAction *PlayFromCaretAction;
	TAction *SaveAction;
	TToolButton *SaveToolButton;
	TAction *DeleteAction;
	TMenuItem *EditMenu;
	TMenuItem *DeleteMenuItem;
	TAction *UndoAction;
	TAction *RedoAction;
	TMenuItem *UndoMenuItem;
	TMenuItem *RedoMenuItem;
	TToolButton *UndoToolButton;
	TToolButton *RedoToolButton;
	TToolButton *ToolButton3;
	TToolButton *DeleteToolButton;
	TToolButton *ToolButton5;
	TAction *NewLinkAction;
	TAction *NewLabelAction;
	TMenuItem *NewLinkMenuItem;
	TMenuItem *NewLabelMenuItem;
	TToolButton *NewLinkToolButton;
	TToolButton *NewLabelToolButton;
	TToolButton *ToolButton4;
	TPopupMenu *ForLabelPopupMenu;
	TMenuItem *ForLink_DeleteMenuItem;
	TPopupMenu *ForLinkPopupMenu;
	TMenuItem *MenuItem1;
	TPopupMenu *ForWavePopupMenu;
	TMenuItem *ForWave_PlayFromCursorMenuItem;
	TMenuItem *ForWave_NewLinkMenuItem;
	TMenuItem *ForWave_NewLabelMenuItem;
	TMenuItem *ForWave_ZoomInMenuItem;
	TMenuItem *ForWave_ZoomOutMenuItem;
	TAction *ShowEditAttribAction;
	TToolButton *ShowEditAttribToolButton;
	TToolButton *ToolButton7;
	TPanel *EditAttribPanel;
	TBevel *EditLabelAttribBevel;
	TPanel *EmptyEditAttribFrame;
	TEditLinkAttribFrame *EditLinkAttribFrame;
	TEditLabelAttribFrame *EditLabelAttribFrame;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall OpenActionExecute(TObject *Sender);
	void __fastcall ZoomInActionExecute(TObject *Sender);
	void __fastcall ZoomOutActionExecute(TObject *Sender);
	void __fastcall PlayFromStartActionExecute(TObject *Sender);
	void __fastcall StopPlayActionExecute(TObject *Sender);
	void __fastcall PlayFromCaretActionExecute(TObject *Sender);
	void __fastcall SaveActionExecute(TObject *Sender);
	void __fastcall UndoActionExecute(TObject *Sender);
	void __fastcall RedoActionExecute(TObject *Sender);
	void __fastcall DeleteActionExecute(TObject *Sender);
	void __fastcall NewLabelActionExecute(TObject *Sender);
	void __fastcall NewLinkActionExecute(TObject *Sender);
	void __fastcall UndoActionUpdate(TObject *Sender);
	void __fastcall RedoActionUpdate(TObject *Sender);
	void __fastcall DeleteActionUpdate(TObject *Sender);
	void __fastcall ShowEditAttribActionExecute(TObject *Sender);
private:	// ユーザー宣言
	TWaveReader * Reader;
	TWaveView *WaveView;
	tTVPWaveLoopManager *Manager;
	AnsiString FileName; // current filename

public:		// ユーザー宣言
	__fastcall TLoopTunerMainForm(TComponent* Owner);
private:
	void __fastcall OnReaderProgress(TObject *sender);
	void __fastcall PlayFrom(int pos);
	void __fastcall OnApplicationIdle(TObject *sender, bool &done);
	void __fastcall OnWaveViewDoubleClick(TObject *Sender, int pos);
	void __fastcall OnWaveViewNotifyPopup(TObject *Sender, AnsiString type);
	void __fastcall OnWaveViewShowCaret(TObject *Sender, int pos);
	void __fastcall OnWaveViewLinkSelected(TObject *Sender, int num, tTVPWaveLoopLink &link);
	void __fastcall OnWaveViewLabelSelected(TObject *Sender, int num, tTVPWaveLabel &label);
	void __fastcall OnWaveViewSelectionLost(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoopTunerMainForm *LoopTunerMainForm;
//---------------------------------------------------------------------------
#endif
