//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef LogViewerUnitH
#define LogViewerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TLogViewerForm : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TPanel *Panel1;
	TButton *CloseButton;
	TRichEdit *LogEdit;
	void __fastcall Panel1Resize(TObject *Sender);
private:	// ���[�U�[�錾
public:		// ���[�U�[�錾
	__fastcall TLogViewerForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLogViewerForm *LogViewerForm;
//---------------------------------------------------------------------------
#endif
