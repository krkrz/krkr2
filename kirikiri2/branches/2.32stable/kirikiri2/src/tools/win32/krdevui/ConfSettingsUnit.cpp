//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef TVP_ENVIRON
	#include <vcl.h>
	#pragma hdrstop
#else
	#include "tjsCommHead.h"
#endif

#include "ConfSettingsUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ConfMainFrameUnit"
#pragma resource "*.dfm"
TConfSettingsForm *ConfSettingsForm;
//---------------------------------------------------------------------------
#define SPACER 6
__fastcall TConfSettingsForm::TConfSettingsForm(TComponent* Owner, bool userconf)
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

	if(userconf)
	{
		ConfMainFrame->SetUserConfMode();
		IconChangeWarnLabel->Visible = false;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TConfSettingsForm::SelectFile()
{
	if(OpenDialog->Execute())
	{
		return InitializeConfig(OpenDialog->FileName);
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TConfSettingsForm::InitializeConfig(AnsiString filename)
{
	// set source and target exe (will be the same)
	ConfMainFrame->SetSourceAndTargetFileName(filename, filename);

	// ensure the program is TVP or TVP's descendant
	try
	{
		ConfMainFrame->FindOptionAreaOffset(filename);
	}
	catch(...)
	{
		Application->MessageBox(SpecifiedFileIsNotKrKrLabel->Caption.c_str(),
			Caption.c_str(), MB_OK|MB_ICONSTOP);
		return false;
	}

	if(!ConfMainFrame->GetUserConfMode())
	{
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
	}

	// load options and option information
	try
	{
		ConfMainFrame->ReadOptions();
	}
	catch(const Exception & e)
	{
		Application->MessageBox((SpecifiedFileIsNotKrKrLabel->Caption + "\r\n" + e.Message).c_str(),
			Caption.c_str(), MB_OK|MB_ICONSTOP);
		return false;
	}

	// apply read data to the ui
	ConfMainFrame->ApplyReadDataToUI();

	return true;
}
//---------------------------------------------------------------------------
void __fastcall TConfSettingsForm::OKButtonClick(TObject *Sender)
{
	try
	{
		if(ConfMainFrame->CheckOK())
		{
			ConfMainFrame->WriteOptions();
			Application->MessageBox(FileWroteLabel->Caption.c_str(), Caption.c_str(),
				MB_OK|MB_ICONINFORMATION);
			ModalResult = mrOk;
		}
	}
	catch(const Exception & e)
	{
		Application->MessageBox(e.Message.c_str(),
			Caption.c_str(), MB_OK|MB_ICONSTOP);
	}
}
//---------------------------------------------------------------------------
