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
	FWaveView = NULL;
	InLoading = false;
}
//---------------------------------------------------------------------------
void __fastcall TEditLinkAttribFrame::SetLinkNum(int linknum)
{
	if(!FWaveView) return;
	FLinkNum = linknum;

	tTVPWaveLoopLink &link = FWaveView->GetLinks()[FLinkNum];

	InLoading = true;

	EnableConditionCheckBox->Checked = link.Condition != llcNone;
	CondVarComboBox->ItemIndex = (int)link.CondVar;
	CondRefValueEdit->Text = AnsiString(link.RefValue);
	ConditionComboBox->ItemIndex = (int)(link.Condition == 0 ? 0 : link.Condition - 1);
	SmoothCheckBox->Checked = link.Smooth;

	CondRefValueEdit->Modified = false;

	InLoading = false;

	AttribChanged();
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
	if(!FWaveView) return;
	if(InLoading) return;

	tTVPWaveLoopLink &link = FWaveView->GetLinks()[FLinkNum];

	FWaveView->InvalidateLink(FLinkNum);

	if(EnableConditionCheckBox->Checked)
	{
		link.CondVar = CondVarComboBox->ItemIndex;
		link.RefValue = CondRefValueEdit->Text.ToInt();
		link.Condition = (tTVPWaveLoopLinkCondition)(ConditionComboBox->ItemIndex + 1);
	}
	else
	{
		link.Condition = llcNone;
	}
	link.Smooth = SmoothCheckBox->Checked;

	FWaveView->NotifyLinkChanged();

	FWaveView->InvalidateLink(FLinkNum);

	FWaveView->PushUndo(); //==== push undo
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
void __fastcall TEditLinkAttribFrame::SmoothCheckBoxClick(TObject *Sender)
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
	if(FWaveView) FWaveView->EraseRedo();
}
//---------------------------------------------------------------------------

