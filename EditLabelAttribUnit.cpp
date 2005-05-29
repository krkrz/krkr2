//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditLabelAttribUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditLabelAttribFrame *EditLabelAttribFrame;
//---------------------------------------------------------------------------
__fastcall TEditLabelAttribFrame::TEditLabelAttribFrame(TComponent* Owner)
	: TFrame(Owner)
{
	FWaveView = NULL;
	InLoading = false;
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::SetLabelNum(int num)
{
	if(!FWaveView) return;
	FLabelNum = num;

	tTVPWaveLabel &label = FWaveView->GetLabels()[FLabelNum];

	InLoading = true;

	LabelNameEdit->Text = label.Name;

	InLoading = false;

	AttribChanged();
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::AttribChanged()
{
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::CommitChanges()
{
	if(!FWaveView) return;
	if(InLoading) return;

	tTVPWaveLabel &label = FWaveView->GetLabels()[FLabelNum];

	FWaveView->InvalidateLabel(FLabelNum);

	label.Name = LabelNameEdit->Text;

	FWaveView->NotifyLabelChanged();

	FWaveView->InvalidateLabel(FLabelNum);

	FWaveView->PushUndo(); //==== push undo
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::LabelNameEditExit(TObject *Sender)
{
	AttribChanged();
	CommitChanges();
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::LabelNameEditKeyDown(
      TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(Key == VK_RETURN)
	{
		AttribChanged();
		CommitChanges();
	}
}
//---------------------------------------------------------------------------

void __fastcall TEditLabelAttribFrame::LabelNameEditChange(TObject *Sender)
{
	if(InLoading) return;
	if(FWaveView) FWaveView->EraseRedo();
}
//---------------------------------------------------------------------------

