//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditLabelAttribUnit.h"
#include "WaveLoopManager.h"
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
	LabelNameEdit->Modified = false;

	InLoading = false;

	AttribChanged();
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::SetLabelInfo(tTVPWaveLabel &label)
{
	label.Name = FLabel.Name;
}
//---------------------------------------------------------------------------
void __fastcall TEditLabelAttribFrame::CheckUncommitted()
{
	LabelNameEditExit(this);
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
	if(LabelNameEdit->Modified)
	{
		AttribChanged();
		CommitChanges();
        LabelNameEdit->Modified = false;
	}
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

