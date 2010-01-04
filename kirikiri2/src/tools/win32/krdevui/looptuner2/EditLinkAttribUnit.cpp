//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditLinkAttribUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditLinkAttribFrame *EditLinkAttribFrame;
//---------------------------------------------------------------------------
__fastcall TEditLinkAttribFrame::TEditLinkAttribFrame(TComponent* Owner)
	: TFrame(Owner)
{
	FOnInfoChanged = NULL;
	FOnEraseRedo = NULL;
	InLoading = false;
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::SetLink(const tTVPWaveLoopLink &link)
{
	InLoading = true;

	FLink = link;

	EnableConditionCheckBox->Checked = FLink.Condition != llcNone;
	CondVarComboBox->ItemIndex = (int)FLink.CondVar;
	CondRefValueEdit->Text = AnsiString(FLink.RefValue);
	ConditionComboBox->ItemIndex = (int)(FLink.Condition == 0 ? 0 : FLink.Condition - 1);

	CondRefValueEdit->Modified = false;

	InLoading = false;

	AttribChanged();
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::SetLinkInfo(tTVPWaveLoopLink &link)
{
	link.CondVar   = FLink.CondVar;
	link.RefValue  = FLink.RefValue;
	link.Condition = FLink.Condition;
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::CheckUncommitted()
{
	CondRefValueEditExit(this);
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::AttribChanged()
{
	bool b = EnableConditionCheckBox->Checked;
	TColor fc = b ? clWindowText : clGrayText;
	CondVarComboBox->Enabled = b;
	CondRefValueEdit->Enabled = b;
	ConditionComboBox->Enabled = b;
	CondVarComboBox->Font->Color = fc;
	Label1->Font->Color = fc;
	CondRefValueEdit->Font->Color = fc;
	ConditionComboBox->Font->Color = fc;
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::CommitChanges()
{
	if(InLoading) return;

	if(EnableConditionCheckBox->Checked)
	{
		FLink.CondVar = CondVarComboBox->ItemIndex;
		FLink.RefValue = CondRefValueEdit->Text.ToInt();
		FLink.Condition = (tTVPWaveLoopLinkCondition)(ConditionComboBox->ItemIndex + 1);
	}
	else
	{
		FLink.Condition = llcNone;
	}

	if(OnInfoChanged) OnInfoChanged(this);
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::EnableConditionCheckBoxClick(
	  TObject *Sender)
{
	AttribChanged();
	CommitChanges();
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::CondVarComboBoxChange(
	  TObject *Sender)
{
	AttribChanged();
	CommitChanges();
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::ConditionComboBoxChange(
	  TObject *Sender)
{
	AttribChanged();
	CommitChanges();
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::CondRefValueEditExit(TObject *Sender)
{
	if(CondRefValueEdit->Modified)
	{
		AttribChanged();
		CommitChanges();
		CondRefValueEdit->Modified = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TEditLinkAttribFrame::ConditionComboBoxKeyDown(
      TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(Key == VK_RETURN)
	{
		AttribChanged();
		CommitChanges();
	}
}
//---------------------------------------------------------------------------

void __fastcall TEditLinkAttribFrame::CondRefValueEditChange(
      TObject *Sender)
{
	if(InLoading) return;

	TEdit *edit = dynamic_cast<TEdit*>(Sender);
	if(edit)
	{
		if(edit->Text == "")
		{
			edit->Text = "0";
			edit->SelStart = 1;
			edit->Modified = true;
		}

		if(edit->Text != "0" && edit->Text.c_str()[0] == '0')
		{
			edit->Text = AnsiString(edit->Text.ToInt());
			edit->SelStart = edit->Text.Length();
			edit->Modified = true;
		}
	}

	if(FOnEraseRedo) FOnEraseRedo(this);
}
//---------------------------------------------------------------------------

void __fastcall TEditLinkAttribFrame::CondRefValueEditKeyPress(
      TObject *Sender, char &Key)
{
	if(!(Key >= '0' && Key <= '9' || Key < 0x20)) Key = 0;
}
//---------------------------------------------------------------------------

