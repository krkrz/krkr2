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
	FOnInfoChanged = NULL;
	FOnEraseRedo = NULL;
	InLoading = false;
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::SetLabel(const tTVPWaveLabel &label)
{
	FLabel = label;

	InLoading = true;

	LabelNameEdit->Text = FLabel.Name;

	InLoading = false;

	AttribChanged();
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::SetLabelInfo(tTVPWaveLabel &label)
{
	label.Name = FLabel.Name;
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::AttribChanged()
{
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::CommitChanges()
{
	if(InLoading) return;

	FLabel.Name = LabelNameEdit->Text;

	if(OnInfoChanged) OnInfoChanged(this);
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
	if(OnEraseRedo) OnEraseRedo(this);
}
//---------------------------------------------------------------------------

