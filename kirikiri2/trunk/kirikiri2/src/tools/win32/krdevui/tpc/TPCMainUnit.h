//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef TPCMainUnitH
#define TPCMainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TTPCMainForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TGroupBox *TranspOuputFormatGroupBox;
	TRadioButton *TranspBMPFormatRadioButton;
	TRadioButton *TranspPNGFormatRadioButton;
	TRadioButton *TranspSeparatedFormatRadioButton;
	TLabel *TranspMainFormatLabel;
	TLabel *TranspMaskFormatLabel;
	TComboBox *TranspMainFormatComboBox;
	TComboBox *TranspMaskFormatComboBox;
	TGroupBox *OutputFolderGroup;
	TRadioButton *SameFolderRadioButton;
	TRadioButton *SpecifyFolderRadioButton;
	TEdit *OutputFolderEdit;
	TButton *OutputFolderRefButton;
	TLabel *Label1;
	TButton *CloseButton;
	TCheckBox *OverwriteCheckBox;
	TButton *TranspMainJPEGOptionButton;
	TButton *TranspMaskJPEGOptionButton;
	TLabel *TranspFullTranspColorMethodLabel;
	TComboBox *TranspFullTranspColorMethodComboBox;
	TGroupBox *OpaqueOutputFormatGroupBox;
	TRadioButton *TranspTLG5FormatRadioButton;
	TRadioButton *OpaqueBMPFormatRadioButton;
	TRadioButton *OpaquePNGFormatRadioButton;
	TRadioButton *OpaqueTLG5FormatRadioButton;
	TRadioButton *OpaqueJPEGFormatRadioButton;
	TCheckBox *DoNotShowLogWhenNoErrorCheckBox;
	TRadioButton *OpaqueTLG6FormatRadioButton;
	TRadioButton *TranspTLG6FormatRadioButton;
	TCheckBox *ExpandButton;
	TButton *OpaqueJPEGOptionButton;
	TCheckBox *TranspOutputAddAlphaFormatCheckBox;
	TPanel *Bar1;
	TCheckBox *TranspAssumeInputIsAddAlphaCheckBox;
	void __fastcall TranspBMPFormatRadioButtonClick(TObject *Sender);
	void __fastcall TranspPNGFormatRadioButtonClick(TObject *Sender);
	void __fastcall TranspSeparatedFormatRadioButtonClick(TObject *Sender);
	void __fastcall SameFolderRadioButtonClick(TObject *Sender);
	void __fastcall SpecifyFolderRadioButtonClick(TObject *Sender);
	void __fastcall TranspMainFormatComboBoxChange(TObject *Sender);
	void __fastcall TranspMaskFormatComboBoxChange(TObject *Sender);
	void __fastcall TranspMainJPEGOptionButtonClick(TObject *Sender);
	void __fastcall TranspMaskJPEGOptionButtonClick(TObject *Sender);
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OutputFolderRefButtonClick(TObject *Sender);
	void __fastcall OpaqueJPEGOptionButtonClick(TObject *Sender);
	void __fastcall OpaqueBMPFormatRadioButtonClick(TObject *Sender);
	void __fastcall OpaquePNGFormatRadioButtonClick(TObject *Sender);
	void __fastcall OpaqueTLG5FormatRadioButtonClick(TObject *Sender);
	void __fastcall OpaqueJPEGFormatRadioButtonClick(TObject *Sender);
	void __fastcall TranspTLG5FormatRadioButtonClick(TObject *Sender);
	void __fastcall OpaqueTLG6FormatRadioButtonClick(TObject *Sender);
	void __fastcall TranspTLG6FormatRadioButtonClick(TObject *Sender);
	void __fastcall ExpandButtonClick(TObject *Sender);
	void __fastcall TranspOutputAddAlphaFormatCheckBoxClick(TObject *Sender);
	void __fastcall TranspAssumeInputIsAddAlphaCheckBoxClick(TObject *Sender);
private:	// ユーザー宣言
	int TranspMainJPEGQuality;
	int TranspMaskJPEGQuality;
	int OpaqueJPEGQuality;
	int OrgClientHeight;
	int OrgCloseButtonTop;

public:		// ユーザー宣言
	__fastcall TTPCMainForm(TComponent* Owner);

private:
	void __fastcall UpdateState();

	void __fastcall LoadFromIni();
	void __fastcall SaveToIni();
	TStringList * FileList;
	AnsiString Log;

protected:
BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER( WM_DROPFILES,  TMessage, WMDropFiles);
END_MESSAGE_MAP(TForm)
	void __fastcall WMDropFiles(TMessage &Msg);

public:
	bool __fastcall ProcessFile(AnsiString infile);

};
//---------------------------------------------------------------------------
extern PACKAGE TTPCMainForm *TPCMainForm;
//---------------------------------------------------------------------------
#endif
