//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------


#ifndef EditLinkAttribUnitH
#define EditLinkAttribUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "WaveView.h"
//---------------------------------------------------------------------------
class TEditLinkAttribFrame : public TFrame
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TCheckBox *EnableConditionCheckBox;
	TComboBox *CondVarComboBox;
	TLabel *Label1;
	TEdit *CondRefValueEdit;
	TComboBox *ConditionComboBox;
	void __fastcall EnableConditionCheckBoxClick(TObject *Sender);
	void __fastcall CondVarComboBoxChange(TObject *Sender);
	void __fastcall ConditionComboBoxChange(TObject *Sender);
	void __fastcall CondRefValueEditExit(TObject *Sender);
	void __fastcall ConditionComboBoxKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall CondRefValueEditChange(TObject *Sender);
	void __fastcall CondRefValueEditKeyPress(TObject *Sender, char &Key);
private:	// ���[�U�[�錾
	tTVPWaveLoopLink FLink;
	bool InLoading;

	void __fastcall SetLinkNum(int num);

	void __fastcall AttribChanged();
	void __fastcall CommitChanges();

	TNotifyEvent FOnInfoChanged;
	TNotifyEvent FOnEraseRedo;

public:		// ���[�U�[�錾
	__fastcall TEditLinkAttribFrame(TComponent* Owner);

	void __fastcall SetLink(const tTVPWaveLoopLink &link);
	void __fastcall SetLinkInfo(tTVPWaveLoopLink &link);

	void __fastcall CheckUncommitted();

	__property TNotifyEvent OnInfoChanged = { read = FOnInfoChanged, write = FOnInfoChanged };
	__property TNotifyEvent OnEraseRedo = { read = FOnEraseRedo, write = FOnEraseRedo };
};
//---------------------------------------------------------------------------
extern PACKAGE TEditLinkAttribFrame *EditLinkAttribFrame;
//---------------------------------------------------------------------------
#endif
