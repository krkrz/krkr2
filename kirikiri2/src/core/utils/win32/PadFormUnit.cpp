//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Context Debugger Pad
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "PadFormUnit.h"
#include "MainFormUnit.h"
#include "ScriptMgnIntf.h"
#include "SysInitIntf.h"
#include <clipbrd.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTVPPadForm *TVPPadForm;
//---------------------------------------------------------------------------
__fastcall TTVPPadForm::TTVPPadForm(TComponent* Owner, bool ismain)
	: TForm(Owner)
{
	// adjust components
	ToolBar->Left = 0;
	ToolBar->Top = ClientHeight - ToolBar->Height;
	StatusBar->Left = ToolBar->Width;
	StatusBar->Top = ClientHeight - StatusBar->Height;
	StatusBar->Width = ClientWidth - ToolBar->Width;
	Memo->Left = 0;
	Memo->Top = 0;
	Memo->Width = ClientWidth;
	Memo->Height = ClientHeight-StatusBar->Height;


	// set hot keys
	ShowControllerMenuItem->ShortCut = TVPMainForm->ShowControllerMenuItem->ShortCut;
	ShowScriptEditorMenuItem->ShortCut = TVPMainForm->ShowScriptEditorMenuItem->ShortCut;
	ShowWatchMenuItem->ShortCut = TVPMainForm->ShowWatchMenuItem->ShortCut;
	ShowConsoleMenuItem->ShortCut = TVPMainForm->ShowConsoleMenuItem->ShortCut;
	ShowAboutMenuItem->ShortCut = TVPMainForm->ShowAboutMenuItem->ShortCut;
	CopyImportantLogMenuItem->ShortCut = TVPMainForm->CopyImportantLogMenuItem->ShortCut;

	// initialize
	FOnExecute = DefaultCDPExecute;
	FIsMainCDP = ismain;
	UpdatePosition();

	if(FIsMainCDP)
	{
		tTVPProfileHolder * prof = TVPGetEnvironProfile();
		TVPEnvironProfileAddRef();
		static AnsiString section("script editor");
		int n;
		n = prof->ReadInteger(section, "left", -1);
		if(n != -1) Left = n;
		n = prof->ReadInteger(section, "top", -1);
		if(n != -1) Top = n;
		n = prof->ReadInteger(section, "width", -1);
		if(n != -1) Width = n;
		n = prof->ReadInteger(section, "height", -1);
		if(n != -1) Height = n;
		n = prof->ReadInteger(section, "stayontop", 0);
		FormStyle = n ? fsStayOnTop : fsNormal;

		if(Left > Screen->Width) Left = Screen->Width - Width;
		if(Top > Screen->Height) Top = Screen->Height - Height;

		prof->ReadStrings(section, "content", Memo->Lines);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::FormDestroy(TObject *Sender)
{
	if(FIsMainCDP)
	{
		WriteProfile();
		TVPEnvironProfileRelease();
	}
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(FIsMainCDP)
	{
		// write profile to disk
		WriteProfile();
		TVPWriteEnvironProfile();
	}

	// hide
	if(FFreeOnTerminate)
		Action = caFree;
	else
		Action = caHide;

	if(FIsMainCDP) TVPMainForm->NotifyScriptEditorHiding();
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::WriteProfile()
{
	tTVPProfileHolder * prof = TVPGetEnvironProfile();
	static AnsiString section("script editor");
	prof->WriteInteger(section, "left", Left);
	prof->WriteInteger(section, "top", Top);
	prof->WriteInteger(section, "width", Width);
	prof->WriteInteger(section, "height", Height);
	prof->WriteStrings(section, "content", Memo->Lines);
	prof->WriteInteger(section, "stayontop", FormStyle == fsStayOnTop);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::DefaultCDPExecute(TObject *Sender, const ttstr &content)
{
	try
	{
		try
		{
			TVPExecuteScript(content);
		}
		TJS_CONVERT_TO_TJS_EXCEPTION
	}
	TVP_CATCH_AND_SHOW_SCRIPT_EXCEPTION_FORCE_SHOW_EXCEPTION(TJS_W("Script Editor"));
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ExecuteButtonClick(TObject *Sender)
{
	if(FIsMainCDP)
	{
		// write profile to disk
		WriteProfile();
		TVPWriteEnvironProfile();
	}

	ttstr content(Memo->Text.c_str());
	if(FOnExecute) FOnExecute(this, content);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::MemoPopupMenuPopup(TObject *Sender)
{
	ShowOnTopMenuItem->Checked = FormStyle == fsStayOnTop;

	ShowControllerMenuItem->Checked = TVPMainForm->Visible;
	ShowWatchMenuItem->Checked = TVPMainForm->GetWatchVisible();
	ShowConsoleMenuItem->Checked = TVPMainForm->GetConsoleVisible();

	CutMenuItem->Enabled = Memo->SelLength;
	CopyMenuItem->Enabled = Memo->SelLength;
	PasteMenuItem->Enabled = Clipboard()->HasFormat(CF_TEXT);
	UndoMenuItem->Enabled = Memo->CanUndo;
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::CutMenuItemClick(TObject *Sender)
{
	Memo->CutToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::CopyMenuItemClick(TObject *Sender)
{
	Memo->CopyToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::PasteMenuItemClick(TObject *Sender)
{
	if(Clipboard()->HasFormat(CF_TEXT))
	{
		AnsiString text = Clipboard()->AsText;
		Memo->SelText = text;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::UndoMenuItemClick(TObject *Sender)
{
	Memo->Undo();
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::FormShow(TObject *Sender)
{
	TVPMainForm->SmallIconImageList->GetIcon(2,Icon);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::GoToLine(int line)
{
	int y = Memo->Perform( EM_LINEFROMCHAR, Memo->SelStart, 0 );
	Memo->Perform( EM_LINESCROLL, 0, line - 1 - y );
	Memo->SelStart = Memo->Perform( EM_LINEINDEX, line - 1, 0 );
	Memo->SelLength = 0;
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::SetLines(const ttstr & lines)
{
	// unify \n to \r\n
	int newlen = 0;
	const char *p, *op;
	AnsiString in = lines.AsAnsiString();
	p = op = in.c_str();
	while(*p)
	{
		if(*p == '\n' && (p != op && p[-1] != '\r' || p == op))
			newlen += 2;
		else
			newlen ++;

		p++;
	}

	char *tmp = new char[newlen + 1];
	char *tp = tmp;
	p = op = in.c_str();
	while(*p)
	{
		if(*p == '\n' && (p != op && p[-1] != '\r' || p == op))
		{
			*(tp++) = '\r';
			*(tp++) = '\n';
		}
		else
		{
			*(tp++) = *p;
		}
		p++;
	}
	*tp = 0;

	in = tmp;
	delete [] tmp;

	Memo->Text = in;
}
//---------------------------------------------------------------------------
ttstr __fastcall TTVPPadForm::GetLines() const
{
	return Memo->Text;
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::SetEditColor(tjs_uint32 color)
{
	Memo->Color = (TColor)ColorToRGB((TColor)color);
}
//---------------------------------------------------------------------------
tjs_uint32 TTVPPadForm::GetEditColor() const
{
	return (tjs_uint32)Memo->Color;
}
//---------------------------------------------------------------------------
void TTVPPadForm::SetFileName(const ttstr &name)
{
	// name must be a local file name
	SaveDialog->FileName = name.AsAnsiString();
}
//---------------------------------------------------------------------------
ttstr TTVPPadForm::GetFileName() const
{
	return ttstr(SaveDialog->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::UpdatePosition(void)
{
	if(Memo->SelLength==0)
	{
		int x,y;
		y=Memo->Perform(EM_LINEFROMCHAR,Memo->SelStart,0);
		x=Memo->Perform(EM_LINEINDEX,-1,0);
		x=Memo->SelStart-x;
		StatusBar->Panels->Items[0]->Text=AnsiString(y+1) +" : "+AnsiString(x+1);
	}
	else
	{
		int x,y;
		AnsiString a;
		y=Memo->Perform(EM_LINEFROMCHAR,Memo->SelStart,0);
		x=Memo->Perform(EM_LINEINDEX,y,0);
		x=Memo->SelStart-x;
		a=AnsiString(y+1) +":"+AnsiString(x+1)+"-";

		y=Memo->Perform(EM_LINEFROMCHAR,Memo->SelStart+Memo->SelLength,0);
		x=Memo->Perform(EM_LINEINDEX,y,0);
		x=Memo->SelStart+Memo->SelLength-x;
		a+=AnsiString(y+1) +":"+AnsiString(x+1);

		StatusBar->Panels->Items[0]->Text=a;
	}
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::MemoKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	UpdatePosition();
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::MemoMouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
	UpdatePosition();
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::MemoMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	UpdatePosition();
}
//---------------------------------------------------------------------------

void __fastcall TTVPPadForm::MemoSelectionChange(TObject *Sender)
{
	UpdatePosition();
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::SetStatusText(const ttstr & text)
{
	StatusBar->Panels->Items[1]->Text = text.AsAnsiString();
}
//---------------------------------------------------------------------------
ttstr __fastcall TTVPPadForm::GetStatusText(void)
{
	return (ttstr)(StatusBar->Panels->Items[1]->Text);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::SetExecButtonEnabled(bool en)
{
	ExecuteButton->Enabled = en;
	ExecuteMenuItem->Enabled = en;
}
//---------------------------------------------------------------------------
bool __fastcall TTVPPadForm::GetExecButtonEnabled()
{
	return ExecuteButton->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::SetReadOnly(bool st)
{
	Memo->ReadOnly = st;
}
//---------------------------------------------------------------------------
bool __fastcall TTVPPadForm::GetReadOnly()
{
	return Memo->ReadOnly;
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ShowWatchMenuItemClick(TObject *Sender)
{
	TVPMainForm->ShowWatchButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ShowConsoleMenuItemClick(TObject *Sender)
{
	TVPMainForm->ShowConsoleButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ShowAboutMenuItemClick(TObject *Sender)
{
	TVPMainForm->ShowAboutMenuItemClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::CopyImportantLogMenuItemClick(TObject *Sender)
{
	TVPMainForm->CopyImportantLogMenuItemClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ShowControllerMenuItemClick(TObject *Sender)
{
	TVPMainForm->ShowController();
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ShowScriptEditorMenuItemClick(TObject *Sender)
{
	TVPMainForm->ShowScriptEditorButtonClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::SaveMenuItemClick(TObject *Sender)
{
	if(SaveDialog->Execute())
	{
		Memo->Lines->SaveToFile(SaveDialog->FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTVPPadForm::ShowOnTopMenuItemClick(TObject *Sender)
{
	FormStyle = FormStyle == fsStayOnTop ? fsNormal : fsStayOnTop;
}
//---------------------------------------------------------------------------


