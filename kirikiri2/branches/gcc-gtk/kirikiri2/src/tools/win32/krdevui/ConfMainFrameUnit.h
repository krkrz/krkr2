//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------


#ifndef ConfMainFrameUnitH
#define ConfMainFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <inifiles.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TConfMainFrame : public TFrame
{
__published:	// IDE 管理のコンポーネント
	TGroupBox *IconGroupBox;
	TCheckBox *ChangeIconCheck;
	TButton *RefIconButton;
	TPanel *IconPanel;
	TImage *IconImage;
	TGroupBox *OptionsGroupBox;
	TOpenPictureDialog *OpenPictureDialog;
	TTreeView *OptionsTreeView;
	TLabel *Label1;
	TComboBox *OptionValueComboBox;
	TLabel *Label2;
	TMemo *OptionDescMemo;
	TLabel *OptionsReadFailedLabel;
	TLabel *ReadingOptionsLabel;
	TLabel *Label3;
	TButton *RestoreDefaultButton;
	TPanel *Panel1;
	TCheckBox *InvisibleCheckBox;
	TEdit *OptionValueEdit;
	void __fastcall RefIconButtonClick(TObject *Sender);
	void __fastcall OptionsTreeViewChange(TObject *Sender, TTreeNode *Node);
	void __fastcall OptionValueComboBoxChange(TObject *Sender);
	void __fastcall RestoreDefaultButtonClick(TObject *Sender);
	void __fastcall InvisibleCheckBoxClick(TObject *Sender);
	void __fastcall OptionValueEditChange(TObject *Sender);
private:	// ユーザー宣言
	bool DescGot;

public:		// ユーザー宣言
	AnsiString SourceExe;

	__fastcall TConfMainFrame(TComponent* Owner);
	__fastcall ~TConfMainFrame();

protected:

	AnsiString OptionsFileName;
	AnsiString OriginalOptionsGroupBoxLabel;
	AnsiString OrgExcludeOptions;
	TStringList *UnrecognizedOptions;
	bool ExcludeInvisibleOptions;

	TStringList *ExcludeOptions;

	static AnsiString EncodeString(AnsiString str);
	static AnsiString DecodeString(AnsiString str);

	AnsiString __fastcall GetPluginOptions(AnsiString module);
	AnsiString __fastcall SearchPluginOptions(AnsiString path, AnsiString ext);
	void __fastcall ParseData();
	void __fastcall ParseOptionsData(TStrings *options);
	void __fastcall FreeOptionsData();
	void __fastcall GetValueCaptionAndValue(AnsiString str,
		AnsiString &valuecaption, AnsiString &value);
	void __fastcall SetOptionValue(AnsiString name, AnsiString value);
	void __fastcall SetNodeOptionValue(TTreeNode *node, AnsiString valuecaption,
		AnsiString value);
	void __fastcall SetNodeInvisibleState(TTreeNode *node, bool invisible);
	void __fastcall SetNodeDefaultOptionValue(TTreeNode *node);

public:
	bool __fastcall CheckOK();
	void __fastcall CopyExe(AnsiString to);
	void __fastcall ModifyExe();

	void __fastcall SetOptionsFromString(AnsiString str);
	AnsiString __fastcall GetOptionString();
	void __fastcall SetExcludeOptionsFromString(AnsiString str);
	AnsiString __fastcall GetExcludeOptionString();
	void __fastcall LoadProfileFromIni(TMemIniFile *ini);
	void __fastcall SaveProfileToIni(TMemIniFile *ini);

	void __fastcall ReadOptionInfoFromExe();
	void __fastcall LoadOptionsFromExe(int mode);
	void __fastcall SaveOptionsToExe(AnsiString target);
	static unsigned int __fastcall FindOptionAreaOffset(AnsiString target);

	void __fastcall SetExcludeOptions() { ExcludeInvisibleOptions = true; }
};
//---------------------------------------------------------------------------
extern PACKAGE TConfMainFrame *ConfMainFrame;
//---------------------------------------------------------------------------
#endif
