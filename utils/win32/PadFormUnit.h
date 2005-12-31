//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Context Debugger Pad
//---------------------------------------------------------------------------

#ifndef PadFormUnitH
#define PadFormUnitH
//---------------------------------------------------------------------------
#include "tjs.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
typedef void __fastcall (__closure *TCDPExecEvent)(System::TObject* Sender,
	const ttstr & content);
//---------------------------------------------------------------------------
class TTVPPadForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TStatusBar *StatusBar;
	TToolBar *ToolBar;
	TToolButton *ExecuteButton;
	TPopupMenu *MemoPopupMenu;
	TMenuItem *CutMenuItem;
	TMenuItem *CopyMenuItem;
	TMenuItem *PasteMenuItem;
	TMenuItem *N1;
	TMenuItem *ShowConsoleMenuItem;
	TMenuItem *ShowAboutMenuItem;
	TMenuItem *N2;
	TMenuItem *ExecuteMenuItem;
	TMenuItem *ShowControllerMenuItem;
	TRichEdit *Memo;
	TMenuItem *ShowWatchMenuItem;
	TMenuItem *N3;
	TMenuItem *UndoMenuItem;
	TMenuItem *SaveMenuItem;
	TSaveDialog *SaveDialog;
	TMenuItem *CopyImportantLogMenuItem;
	TMenuItem *N4;
	TMenuItem *ShowOnTopMenuItem;
	TMenuItem *ShowScriptEditorMenuItem;
	void __fastcall ExecuteButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall MemoPopupMenuPopup(TObject *Sender);
	void __fastcall CutMenuItemClick(TObject *Sender);
	void __fastcall CopyMenuItemClick(TObject *Sender);
	void __fastcall PasteMenuItemClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall MemoKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall MemoMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall MemoMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall MemoSelectionChange(TObject *Sender);
	void __fastcall ShowConsoleMenuItemClick(TObject *Sender);
	void __fastcall ShowAboutMenuItemClick(TObject *Sender);
	void __fastcall ShowControllerMenuItemClick(TObject *Sender);
	void __fastcall ShowWatchMenuItemClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall UndoMenuItemClick(TObject *Sender);
	void __fastcall SaveMenuItemClick(TObject *Sender);
	void __fastcall CopyImportantLogMenuItemClick(TObject *Sender);
	void __fastcall ShowOnTopMenuItemClick(TObject *Sender);
	void __fastcall ShowScriptEditorMenuItemClick(TObject *Sender);
private:	// ユーザー宣言
	TCDPExecEvent FOnExecute;
	bool FFreeOnTerminate;
	bool FIsMainCDP;

public:		// ユーザー宣言
	__fastcall TTVPPadForm(TComponent* Owner, bool ismain = false);

	void __fastcall GoToLine(int line);
	void __fastcall SetLines(const ttstr & lines);
	ttstr __fastcall GetLines() const;
	__property TCDPExecEvent OnExecute = { read = FOnExecute, write = FOnExecute };
	__property bool FreeOnTerminate = { read = FFreeOnTerminate, write = FFreeOnTerminate};
	__property ttstr StatusText = { read=GetStatusText, write=SetStatusText };
	__property bool ExecButtonEnabled ={ read=GetExecButtonEnabled, write=
		SetExecButtonEnabled };
	__property bool ReadOnly = { read=GetReadOnly, write = SetReadOnly };
	__property bool IsMainCDP = { read=IsMainCDP };

	void SetEditColor(tjs_uint32 color);
	tjs_uint32 GetEditColor() const;
	void SetFileName(const ttstr &name); // name must be a local file name
	ttstr GetFileName() const;

	void __fastcall DefaultCDPExecute(TObject *Sender, const ttstr & content);
	void __fastcall UpdatePosition(void);
	void __fastcall SetStatusText(const ttstr & text);
	ttstr __fastcall GetStatusText(void);
	void __fastcall SetExecButtonEnabled(bool en);
	bool __fastcall GetExecButtonEnabled();
	void __fastcall SetReadOnly(bool st);
	bool __fastcall GetReadOnly();

private:
	void __fastcall WriteProfile();
};
//---------------------------------------------------------------------------
extern PACKAGE TTVPPadForm *TVPPadForm;
//---------------------------------------------------------------------------
#endif
