//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "LogViewerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLogViewerForm *LogViewerForm;
//---------------------------------------------------------------------------
__fastcall TLogViewerForm::TLogViewerForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLogViewerForm::Panel1Resize(TObject *Sender)
{
	CloseButton->Left = (Panel1->Width - CloseButton->Width) >> 1;	
}
//---------------------------------------------------------------------------
