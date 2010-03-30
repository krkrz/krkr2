//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "JPEGQualityUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TJPEGQualityForm *JPEGQualityForm;
//---------------------------------------------------------------------------
__fastcall TJPEGQualityForm::TJPEGQualityForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TJPEGQualityForm::TrackBarChange(TObject *Sender)
{
	QualityLabel->Caption = AnsiString(TrackBar->Position);
}
//---------------------------------------------------------------------------
void __fastcall TJPEGQualityForm::RestoreDefaultButtonClick(
	  TObject *Sender)
{
	TrackBar->Position = 90;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall ShowJPEGQualityForm(TForm *parent, int & quality)
{
	TJPEGQualityForm *form = new TJPEGQualityForm(parent);
	form->TrackBar->Position = quality;
	int mr = form->ShowModal();
	if(mr == mrOk)
		quality = form->TrackBar->Position;
	delete form;
}
//---------------------------------------------------------------------------


