//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef TVP_ENVIRON
	#include <vcl.h>
	#pragma hdrstop
#else
	#include "tjsCommHead.h"
	#pragma hdrstop
#endif

#include "ConfSettingsUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ConfMainFrameUnit"
#pragma resource "*.dfm"
TConfSettingsForm *ConfSettingsForm;
//---------------------------------------------------------------------------
#define SPACER 6
__fastcall TConfSettingsForm::TConfSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
	// adjust components
	ConfMainFrame->Left = SPACER;
	ConfMainFrame->Top = SPACER;
	OKButton->Left = ClientWidth - (OKButton->Width + SPACER + CancelButton->Width
		+ SPACER);
	OKButton->Top = ClientHeight - OKButton->Height - SPACER;
	CancelButton->Top = ClientHeight - CancelButton->Height - SPACER;
	CancelButton->Left = ClientWidth - CancelButton->Width - SPACER;
	IconChangeWarnLabel->Top = CancelButton->Top;
	
	ConfMainFrame->Width = ClientWidth - SPACER * 2;
	ConfMainFrame->Height = ClientHeight - SPACER * 2 - SPACER - CancelButton->Height;
}
//---------------------------------------------------------------------------
bool __fastcall TConfSettingsForm::SelectFile()
{
	if(OpenDialog->Execute())
	{
		return ProcessFile(OpenDialog->FileName);
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TConfSettingsForm::ProcessFile(AnsiString filename)
{
	// ensure the program is TVP or TVP's descendant
	try
	{
		ConfMainFrame->SourceExe = filename;
		ConfMainFrame->FindOptionAreaOffset(ConfMainFrame->SourceExe);
	}
	catch(...)
	{
		Application->MessageBox(SpecifiedFileIsNotKrKrLabel->Caption.c_str(),
			Caption.c_str(), MB_OK|MB_ICONSTOP);
		return false;
	}

	// ensure the program is not runnning
	try
	{
		delete new TFileStream(filename, fmOpenReadWrite|
			fmShareDenyWrite);
	}
	catch(...)
	{
		Application->MessageBox(ProgramMayRunningLabel->Caption.c_str(),
			Caption.c_str(), MB_OK|MB_ICONSTOP);
		return false;
	}

	// load options and option information
	try
	{
		ConfMainFrame->LoadOptionsFromExe(1);
		ConfMainFrame->ReadOptionInfoFromExe();
		ConfMainFrame->LoadOptionsFromExe(0);
	}
	catch(...)
	{
		Application->MessageBox(SpecifiedFileIsNotKrKrLabel->Caption.c_str(),
			Caption.c_str(), MB_OK|MB_ICONSTOP);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TConfSettingsForm::OKButtonClick(TObject *Sender)
{
	if(ConfMainFrame->CheckOK())
	{
		ConfMainFrame->ModifyExe();
		Application->MessageBox(FileWroteLabel->Caption.c_str(), Caption.c_str(),
			MB_OK|MB_ICONINFORMATION);
		ModalResult = mrOk;
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfSettingsForm::ShowUserConfig(AnsiString exename)
{
	ConfMainFrame->IconGroupBox->Visible = false;
	ConfMainFrame->InvisibleCheckBox->Visible = false;
	IconChangeWarnLabel->Visible = false;
	ConfMainFrame->SetExcludeOptions();

	if(!ProcessFile(exename)) return;

	ShowModal();
}
//---------------------------------------------------------------------------
