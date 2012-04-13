//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LabelDetailUnit.h"
#include "WaveLoopManager.h"
#include "looptune.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EditLabelAttribUnit"
#pragma resource "*.dfm"
TLabelDetailForm *LabelDetailForm;
//---------------------------------------------------------------------------
__fastcall TLabelDetailForm::TLabelDetailForm(TComponent* Owner)
	: TForm(Owner)
{
	
	// window position and size
	ReadWindowBasicInformationFromIniFile("LabelDetail", this);
}
//---------------------------------------------------------------------------

void __fastcall TLabelDetailForm::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	// write information to ini file
	WriteWindowBasicInformationToIniFile("LabelDetail", this);
}
//---------------------------------------------------------------------------
void __fastcall TLabelDetailForm::SetLabel(const tTVPWaveLabel & label)
{
	FLabel = label;
	EditLabelAttribFrame->SetLabel(label);
}
//---------------------------------------------------------------------------
void __fastcall TLabelDetailForm::EditLabelAttribFrameInfoChanged(TObject * Sender)
{
	EditLabelAttribFrame->SetLabelInfo(FLabel);
}
//---------------------------------------------------------------------------
void __fastcall TLabelDetailForm::OKButtonClick(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TLabelDetailForm::CancelButtonClick(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


void __fastcall TLabelDetailForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{

	if(ModalResult == mrOk)
	{
		EditLabelAttribFrame->CheckUncommitted();
		EditLabelAttribFrame->SetLabelInfo(FLabel);
		if(FLabel.Name.c_str()[0] == ':' && !tTVPWaveLoopManager::GetLabelExpression(FLabel.Name))
		{
			Application->MessageBox("式の文法が間違っています", "エラー", MB_OK|MB_ICONEXCLAMATION);
			CanClose = false;
		}
		if(FLabel.Name.AnsiPos("'") != 0)
		{
			Application->MessageBox("ラベル名にアポストロフィー (') を含める事はできません", "エラー", MB_OK|MB_ICONEXCLAMATION);
			CanClose = false;
		}
	}
}
//---------------------------------------------------------------------------

