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
__published:	// IDE 管理のコンポーネント
	TCheckBox *EnableConditionCheckBox;
	TComboBox *CondVarComboBox;
	TLabel *Label1;
	TEdit *CondRefValueEdit;
	TComboBox *ConditionComboBox;
	TCheckBox *SmoothCheckBox;
	void __fastcall EnableConditionCheckBoxClick(TObject *Sender);
	void __fastcall CondVarComboBoxChange(TObject *Sender);
	void __fastcall ConditionComboBoxChange(TObject *Sender);
	void __fastcall SmoothCheckBoxClick(TObject *Sender);
	void __fastcall CondRefValueEditExit(TObject *Sender);
	void __fastcall ConditionComboBoxKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall CondRefValueEditChange(TObject *Sender);
private:	// ユーザー宣言
	TWaveView * FWaveView;
	int FLinkNum;
	bool InLoading;

	void __fastcall SetLinkNum(int num);

	void __fastcall AttribChanged();
	void __fastcall CommitChanges();
public:		// ユーザー宣言
	__fastcall TEditLinkAttribFrame(TComponent* Owner);

	__property TWaveView * WaveView = { read = FWaveView, write = FWaveView };
	__property int LinkNum = { read = FLinkNum, write = SetLinkNum };
};
//---------------------------------------------------------------------------
extern PACKAGE TEditLinkAttribFrame *EditLinkAttribFrame;
//---------------------------------------------------------------------------
#endif
