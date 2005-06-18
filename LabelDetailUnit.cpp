//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LabelDetailUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EditLabelAttribUnit"
#pragma resource "*.dfm"
TLabelDetailForm *LabelDetailForm;
//---------------------------------------------------------------------------
__fastcall TLabelDetailForm::TLabelDetailForm(TComponent* Owner)
	: TForm(Owner)
{
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

void __fastcall TLabelDetailForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	if(ModalResult == mrOk)
	{
		EditLabelAttribFrame->CheckUncommitted();
		EditLabelAttribFrame->SetLabelInfo(FLabel);
	}
}
//---------------------------------------------------------------------------

