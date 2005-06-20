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
#include <AppEvnts.hpp>
#include <Buttons.hpp>
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
	TToolBar *ToolBar;
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
	TMenuItem *ForLink_DeleteItemMenuItem;
	TPopupMenu *ForWavePopupMenu;
	TMenuItem *ForWave_PlayFromCursorMenuItem;
	TMenuItem *ForWave_NewLinkMenuItem;
	TMenuItem *ForWave_NewLabelMenuItem;
	TMenuItem *ForWave_ZoomInMenuItem;
	TMenuItem *ForWave_ZoomOutMenuItem;
	TToolButton *ToolButton7;
	TApplicationEvents *ApplicationEvents;
	TToolButton *ToolButton6;
	TToolButton *FollowMarkerToolButton;
	TAction *FollowMarkerAction;
	TMenuItem *SaveMenuItem;
	TMenuItem *FollowMarkerMenuItem;
	TMenuItem *PlayMenu;
	TMenuItem *StopPlayMenuItem;
	TMenuItem *PlayFromCaretMenuItem;
	TMenuItem *PlayFromStartMenuItem;
	TAction *EditLinkDetailAction;
	TMenuItem *ForLink_EditLinkDetailMenuItem;
	TMenuItem *EditLinkDetailMenuItem;
	TMenuItem *ForLAbel_EditLabelDetailMenuItem;
	TAction *EditLabelDetailAction;
	TMenuItem *ForLabel_NewLabelMenuItem;
	TMenuItem *ForLink_NewLinkMenuItem;
	TPopupMenu *WaveViewPopupMenu;
	TMenuItem *WaveView_ZoomInMenuItem;
	TMenuItem *WaveView_ZoomOutMenuItem;
	TMenuItem *WaveView_FollowMarkerMenuItem;
	TMenuItem *WaveView_StopPlayMenuItem;
	TMenuItem *WaveView_PlayFromCaretMenuItem;
	TMenuItem *WaveView_PlayFromStartMenuItem;
	TMenuItem *WaveView_UndoMenuItem;
	TMenuItem *WaveView_DeleteMenuItem;
	TAction *GotoLinkFromAction;
	TAction *GotoLinkToAction;
	TMenuItem *ForLink_GotoLinkFromMenuItem;
	TMenuItem *ForLink_GotoLinkToAction;
	TMenuItem *WaveView_GotoLinkFromMenuItem;
	TMenuItem *WaveView_GotoLinkToMenuItem;
	TPanel *FlagsPanel;
	TBevel *Bevel1;
	TLabel *FlagLabel0;
	TEdit *FlagEdit0;
	TPopupMenu *FlagsEditPopupMenu;
	TMenuItem *FlagsEditZeroMenuItem;
	TMenuItem *FlasgEditOneMenuItem;
	TMenuItem *FlagsEditToggleMenuItem;
	TAction *ShowEditFlagsAction;
	TToolButton *ShowEditFlagToolButton;
	TSpeedButton *FlagsClearSpeedButton;
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
	void __fastcall ApplicationEventsIdle(TObject *Sender, bool &Done);
	void __fastcall FollowMarkerActionExecute(TObject *Sender);
	void __fastcall EditLinkDetailActionExecute(TObject *Sender);
	void __fastcall EditLabelDetailActionExecute(TObject *Sender);
	void __fastcall GotoLinkFromActionExecute(TObject *Sender);
	void __fastcall GotoLinkToActionExecute(TObject *Sender);
	void __fastcall FlagsEditZeroMenuItemClick(TObject *Sender);
	void __fastcall FlasgEditOneMenuItemClick(TObject *Sender);
	void __fastcall FlagEdit0MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FlagsEditToggleMenuItemClick(TObject *Sender);
	void __fastcall FlagEdit0Change(TObject *Sender);
	void __fastcall FlagEdit0KeyPress(TObject *Sender, char &Key);
	void __fastcall ShowEditFlagsActionExecute(TObject *Sender);
	void __fastcall FlagEdit0DblClick(TObject *Sender);
	void __fastcall FlagsClearSpeedButtonClick(TObject *Sender);
private:	// ユーザー宣言
	TWaveReader * Reader;
	TWaveView *WaveView;
	tTVPWaveLoopManager *Manager;
	AnsiString FileName; // current filename

	bool ResettingFlags;

public:		// ユーザー宣言
	__fastcall TLoopTunerMainForm(TComponent* Owner);
	TWaveView * GetWaveView() { return WaveView; }
private:
	void __fastcall CreateWaveView();
	void __fastcall OnReaderProgress(TObject *sender);
	void __fastcall PlayFrom(int pos);
	void __fastcall WaveViewStopFollowingMarker(TObject *Sender);
	void __fastcall WaveViewWaveDoubleClick(TObject *Sender, int pos);
	void __fastcall WaveViewLinkDoubleClick(TObject *Sender, int num, tTVPWaveLoopLink &link);
	void __fastcall WaveViewLabelDoubleClick(TObject *Sender, int num, tTVPWaveLabel &label);
	void __fastcall WaveViewNotifyPopup(TObject *Sender, AnsiString type, const TPoint &point);
	void __fastcall WaveViewShowCaret(TObject *Sender, int pos);
	void __fastcall WaveViewLinkSelected(TObject *Sender, int num, tTVPWaveLoopLink &link);
	void __fastcall WaveViewLabelSelected(TObject *Sender, int num, tTVPWaveLabel &label);
	void __fastcall WaveViewSelectionLost(TObject *Sender);
	void __fastcall WaveViewLinkModified(TObject *Sender);
	void __fastcall WaveViewLabelModified(TObject *Sender);

	void __fastcall ResetFlagsEditFromLoopManager();
};
//---------------------------------------------------------------------------
extern PACKAGE TLoopTunerMainForm *LoopTunerMainForm;
//---------------------------------------------------------------------------
#endif
