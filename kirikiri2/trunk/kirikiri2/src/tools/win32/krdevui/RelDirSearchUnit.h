//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef RelDirSearchUnitH
#define RelDirSearchUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TRelDirSearchForm : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TButton *AbortButton;
	TLabel *Label1;
	TLabel *CurrentLabel;
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall AbortButtonClick(TObject *Sender);
private:	// ���[�U�[�錾
	bool Aborted;
	void __fastcall GetFileList(AnsiString path, int baselen);
public:		// ���[�U�[�錾
	__fastcall TRelDirSearchForm(TComponent* Owner);
	TStringList *FileList;
	TStringList *ExtList;
	bool __fastcall GetFileList(AnsiString folder);
};
//---------------------------------------------------------------------------
extern PACKAGE TRelDirSearchForm *RelDirSearchForm;
//---------------------------------------------------------------------------
#endif
