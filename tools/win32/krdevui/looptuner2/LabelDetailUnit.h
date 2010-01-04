//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef LabelDetailUnitH
#define LabelDetailUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "EditLabelAttribUnit.h"
//---------------------------------------------------------------------------
class TLabelDetailForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TEditLabelAttribFrame *EditLabelAttribFrame;
	TButton *OKButton;
	TButton *CancelButton;
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// ユーザー宣言
	int LabelNum; // label number
	tTVPWaveLabel FLabel; // current editting label



public:		// ユーザー宣言
	__fastcall TLabelDetailForm(TComponent* Owner);

	void __fastcall SetLabel(const tTVPWaveLabel & label);

	const tTVPWaveLabel & GetLabel() const { return FLabel ; }

private:
	void __fastcall EditLabelAttribFrameInfoChanged(TObject * Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TLabelDetailForm *LabelDetailForm;
//---------------------------------------------------------------------------
#endif
