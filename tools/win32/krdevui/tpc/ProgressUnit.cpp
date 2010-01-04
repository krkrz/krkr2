//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ProgressUnit.h"
#include "TPCMainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProgressForm *ProgressForm;
//---------------------------------------------------------------------------
__fastcall TProgressForm::TProgressForm(TComponent* Owner, TStringList *list)
	: TForm(Owner)
{
	Interrupted = false;
	Shutdown = false;
	MainForm = (TMainForm*)Owner;
	FileList = list;

	AllSucceeded = true;

}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::Process()
{
	//
	for(int i = 0; i < FileList->Count; i++)
	{
		CurrentFileLabel->Caption = ExtractFileName(FileList->Strings[i]);
		ProgressBar->Position = i * 100 / FileList->Count;

		Application->ProcessMessages();

		bool success = TPCMainForm->ProcessFile(FileList->Strings[i]);

		if(!success) AllSucceeded = false;

		if(Interrupted) break;
	}

    Shutdown = true;
	ModalResult = mrOk;

}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::WMStartProcess(TMessage &Msg)
{
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	CanClose = Shutdown;
	Interrupted = true;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::CancelButtonClick(TObject *Sender)
{
	Interrupted = true;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::TimerTimer(TObject *Sender)
{
	Timer->Enabled = false;
	Process();
}
//---------------------------------------------------------------------------

