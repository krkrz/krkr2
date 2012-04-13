//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------


#ifndef EditLabelAttribUnitH
#define EditLabelAttribUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "WaveView.h"
//---------------------------------------------------------------------------
class TEditLabelAttribFrame : public TFrame
{
__published:	// IDE 管理のコンポーネント
	TLabel *Label1;
	TEdit *LabelNameEdit;
	void __fastcall LabelNameEditExit(TObject *Sender);
	void __fastcall LabelNameEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall LabelNameEditChange(TObject *Sender);
private:	// ユーザー宣言
	tTVPWaveLabel FLabel;
	bool InLoading;
	TNotifyEvent FOnInfoChanged;
	TNotifyEvent FOnEraseRedo;

	void __fastcall AttribChanged();
	void __fastcall CommitChanges();
public:		// ユーザー宣言
	__fastcall TEditLabelAttribFrame(TComponent* Owner);
	void __fastcall SetLabel(const tTVPWaveLabel &label);
	void __fastcall SetLabelInfo(tTVPWaveLabel &label);
	void __fastcall CheckUncommitted();

	__property TNotifyEvent OnInfoChanged = { read = FOnInfoChanged, write = FOnInfoChanged };
	__property TNotifyEvent OnEraseRedo = { read = FOnEraseRedo, write = FOnEraseRedo };

};
//---------------------------------------------------------------------------
extern PACKAGE TEditLabelAttribFrame *EditLabelAttribFrame;
//---------------------------------------------------------------------------
#endif
