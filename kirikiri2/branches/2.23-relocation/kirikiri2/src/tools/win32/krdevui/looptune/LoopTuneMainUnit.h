//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WaveDispUnit.h"
#include "WaveReaderUnit.h"
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ActnList.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#define MAX_UNDO 100
#define WM_SHOWFRONT (WM_USER+199)
#define WM_DOOPEN (WM_USER+200)
//---------------------------------------------------------------------------
class TTSSLoopTunerMainWindow : public TForm
{
__published:	// IDE 管理のコンポーネント
	TMainMenu *MainMenu;
	TMenuItem *FileGroupMenuItem;
	TMenuItem *ExitMenuItem;
	TMenuItem *RegionGroupMenuItem;
	TMenuItem *ViewGroupMenuItem;
	TMenuItem *HelpGroupMenuItem;
	TMenuItem *PlayGroupMenuItem;
	TMenuItem *OpenMenuItem;
	TMenuItem *SaveMenuItem;
	TMenuItem *CloseMenuItem;
	TMenuItem *N1;
	TActionList *ActionList;
	TAction *OpenAction;
	TAction *SaveAction;
	TAction *CloseAction;
	TAction *ExitAction;
	TAction *GotoFirstAction;
	TMenuItem *GotoFirstMenuItem;
	TAction *GotoLastAction;
	TMenuItem *GotoLastMenuItem;
	TAction *SetSelectionAction;
	TMenuItem *SetSelectionMenuItem;
	TAction *PlayNoLoopFromFirstAction;
	TMenuItem *PlayNoLoopFromFirstMenuItem;
	TAction *PlayLoopFromFirstAction;
	TMenuItem *PlayLoopFromFirstMenuItem;
	TAction *StopAction;
	TMenuItem *StopMenuItem;
	TAction *ZoomInAction;
	TAction *ZoomOutAction;
	TMenuItem *ZoomInMenuItem;
	TMenuItem *ZoomOutMenuItem;
	TMenuItem *N2;
	TAction *ShowToolBarAction;
	TMenuItem *ShowToolBarMenuItem;
	TAction *ShowStatusBarAction;
	TMenuItem *ShowStatusBarMenuItem;
	TMenuItem *N4;
	TAction *ShowTimeBaseAction;
	TAction *ShowSampleBaseAction;
	TMenuItem *ShowTimeBaseMenuItem;
	TMenuItem *ShowSampleBaseMenuItem;
	TStatusBar *StatusBar;
	TTimer *Timer;
	TAction *FollowCurrentAction;
	TMenuItem *N5;
	TMenuItem *FollowCurrentMenuItem;
	TAction *PlayLoopRegionAction;
	TMenuItem *PlayLoopRegionMenuItem;
	TImageList *IconImageList;
	TToolBar *ToolBar;
	TToolButton *PlayNoLoopFromFirstButton;
	TToolButton *PlayLoopFromFirstButton;
	TToolButton *PlayLoopRegionButton;
	TToolButton *StopButton;
	TAction *ExportPreLoopAction;
	TAction *ExportLoopAction;
	TMenuItem *N3;
	TMenuItem *ExportPreLoopMenuItem;
	TMenuItem *ExportLoopMenuItem;
	TToolButton *OpenButton;
	TToolButton *SaveButton;
	TToolButton *ToolButton3;
	TToolButton *GotoFirstButton;
	TToolButton *GotoLastButton;
	TToolButton *ToolButton4;
	TToolButton *ToolButton1;
	TToolButton *ZoomInButton;
	TToolButton *ZoomOutButton;
	TToolButton *FollowCurrentButton;
	TToolButton *ToolButton7;
	TOpenDialog *OpenDialog;
	TPopupMenu *MainDisplayerPopupMenu;
	TAction *SetRegionStartAction;
	TAction *SetRegionEndAction;
	TAction *PlayFromCurrentAction;
	TMenuItem *PlayFromCurrentMenuAction;
	TMenuItem *N6;
	TMenuItem *SetRegionStartMenuItem;
	TMenuItem *SetRegionEndMenuItem;
	TAction *FreeLoopAction;
	TMenuItem *FreeLoopMenuItem;
	TAction *UndoAction;
	TAction *RedoAction;
	TMenuItem *N7;
	TMenuItem *UndoMenuItem;
	TMenuItem *RedoMenuItem;
	TToolButton *UndoButton;
	TToolButton *ToolButton5;
	TPanel *Panel;
	TAction *ExportAllAction;
	TMenuItem *ExportAllMenuItem;
	TSaveDialog *SaveDialog;
	TAction *ShowHelpAction;
	TAction *VersionInfoAction;
	TMenuItem *ShowHelpMenuItem;
	TMenuItem *VersionInfoMenuItem;
	TMenuItem *StepFirstPrevMenuItem;
	TMenuItem *StepFirstNextMenuItem;
	TMenuItem *StepLastPrevMenuItem;
	TMenuItem *StepLastNextMenuItem;
	TMenuItem *N8;
	TAction *StepFirstPrevAction;
	TAction *StepFirstNextAction;
	TAction *StepLastPrevAction;
	TAction *StepLastNextAction;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall OpenActionExecute(TObject *Sender);
	void __fastcall ZoomInActionExecute(TObject *Sender);
	void __fastcall ZoomOutActionExecute(TObject *Sender);
	void __fastcall PlayNoLoopFromFirstActionExecute(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FollowCurrentActionExecute(TObject *Sender);
	void __fastcall PlayLoopFromFirstActionExecute(TObject *Sender);
	void __fastcall StopActionExecute(TObject *Sender);
	void __fastcall GotoFirstActionExecute(TObject *Sender);
	void __fastcall GotoLastActionExecute(TObject *Sender);
	void __fastcall PlayLoopRegionActionExecute(TObject *Sender);
	void __fastcall MainDisplayerPopupMenuPopup(TObject *Sender);
	void __fastcall PlayFromCurrentActionExecute(TObject *Sender);
	void __fastcall SetRegionStartActionExecute(TObject *Sender);
	void __fastcall SetRegionEndActionExecute(TObject *Sender);
	void __fastcall SaveActionExecute(TObject *Sender);
	void __fastcall ShowTimeBaseActionExecute(TObject *Sender);
	void __fastcall ShowSampleBaseActionExecute(TObject *Sender);
	void __fastcall FreeLoopActionExecute(TObject *Sender);
	void __fastcall ShowToolBarActionExecute(TObject *Sender);
	void __fastcall ShowStatusBarActionExecute(TObject *Sender);
	void __fastcall UndoActionExecute(TObject *Sender);
	void __fastcall RedoActionExecute(TObject *Sender);
	void __fastcall CloseActionExecute(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ExitActionExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ExportAllActionExecute(TObject *Sender);
	void __fastcall ExportPreLoopActionExecute(TObject *Sender);
	void __fastcall ExportLoopActionExecute(TObject *Sender);
	void __fastcall ShowHelpActionExecute(TObject *Sender);
	void __fastcall VersionInfoActionExecute(TObject *Sender);
	void __fastcall StepFirstPrevActionExecute(TObject *Sender);
	void __fastcall StepFirstNextActionExecute(TObject *Sender);
	void __fastcall StepLastPrevActionExecute(TObject *Sender);
	void __fastcall StepLastNextActionExecute(TObject *Sender);
private:	// ユーザー宣言
protected:
	void __fastcall WMShowFront(TMessage &msg);
	void __fastcall WMDoOpen(TMessage &msg);
	void __fastcall WMDropFiles(TMessage &msg);
	void __fastcall WMCopyData(TWMCopyData &msg);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_SHOWFRONT , TMessage , WMShowFront);
	VCL_MESSAGE_HANDLER(WM_DOOPEN, TMessage, WMDoOpen);
	VCL_MESSAGE_HANDLER(WM_DROPFILES,TMessage,WMDropFiles);
	VCL_MESSAGE_HANDLER(WM_COPYDATA,TWMCopyData,WMCopyData);
END_MESSAGE_MAP(TForm)

private:	// ユーザー宣言
	AnsiString CurrentFileName;

	void __fastcall LoadSettings(void);
	void __fastcall SaveSettings(void);

	AnsiString __fastcall GetTimeString(int samplepos);

	bool Modified;
	bool __fastcall Clear();

	void __fastcall Open();

	int UndoSelStart[MAX_UNDO];
	int UndoSelLength[MAX_UNDO];
	int UndoPtr;
	int RedoPtr;
	void __fastcall ClearUndo(void);
	bool __fastcall CanUndo(void);
	bool __fastcall CanRedo(void);
	void __fastcall Undo(void);
	void __fastcall Redo(void);
	void __fastcall PutUndo(void);

	int PlayingStartOfs;
	int PlayingSelStart;
	int PlayingSelLength;

	int OpeningLoopStart;
	int OpeningLoopLength;
	void __fastcall LoadLoopInformation(AnsiString name);

	void __fastcall DisableAllAction(void);
	void __fastcall EnableAllAction(void);
	void __fastcall UncheckAllPlayAction(void);
	DWORD __fastcall GetNoLoopFromFirstData(__int16 *buf, DWORD ofs, DWORD num);
	DWORD __fastcall GetLoopFromFirstData(__int16 *buf, DWORD ofs, DWORD num);
	void __fastcall DisplayHint(TObject *Sender);
	void __fastcall SetStatusBar(void);
	void __fastcall PlayLoop(int ofs);
	void __fastcall Play(int pos);
	int MainDisplayerMouseDownPos;
	void __fastcall MainDisplayerMouseDown(TObject *Sender, TMouseButton button,
		TShiftState shift, int x, int y);
	void __fastcall MainDisplayerMouseMove(TObject *Sender, TShiftState Shift,
		int X, int Y);
	void __fastcall MainDiaplayerMouseLeave(TObject *Sender);
	void __fastcall MainDisplayerDblClick(TObject *Sender);
	void __fastcall MainDisplayerFollowCanceled(TObject *Sender);
	void __fastcall MainDisplayerSelectionChanged(TObject *Sender);
	void __fastcall SetNewSelection(void);
	void __fastcall MainDisplayerEndSelect(TObject *Sender);


	void __fastcall ApplicationIdle(TObject *Sender, bool &Done);

	void __fastcall ExportWave(int start, int length);
public:		// ユーザー宣言
	void __fastcall WaveReaderProgress(TObject *Sender);
	TWaveDisplayer *MainDisplayer;
	TWaveReader *WaveReader;
	__fastcall TTSSLoopTunerMainWindow(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTSSLoopTunerMainWindow *TSSLoopTunerMainWindow;
//---------------------------------------------------------------------------
#endif
