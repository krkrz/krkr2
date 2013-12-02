//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
class TLayeredOption
{
	TLayeredOption * Parent; // parent option
	TStringList * Strings; // option strings

public:
	TLayeredOption();
	virtual ~TLayeredOption();

	void Clear();

	TLayeredOption * GetParent() const { return Parent; }
	void SetParent(TLayeredOption * parent) { Parent = parent; }

	void FromString(const AnsiString & content);
	void LoadFromIni(TMemIniFile * ini);
	void LoadFromFile(const AnsiString & filename);
	AnsiString ToString() const;
	void SaveToFile(const AnsiString & filename) const;
	void SaveToIni(TMemIniFile * ini) const;

	bool GetOptionValue(const AnsiString & key, AnsiString & value) const;
	void SetOptionValue(const AnsiString & key, const AnsiString & value);

private:
	void InternalListOptionsNames(TStringList * dest) const;
public:
	void ListOptionNames(TStringList * dest) const;
};
//---------------------------------------------------------------------------



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
	TCheckBox *InvisibleCheckBox;
	TEdit *OptionValueEdit;
	TGroupBox *ReleaseOptionGroupBox;
	TPanel *Panel1;
	TPanel *Panel2;
	TCheckBox *DisableMessageMapCheckBox;
	TCheckBox *ForceDataXP3CheckBox;
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
	AnsiString TargetExe;

	__fastcall TConfMainFrame(TComponent* Owner);
	__fastcall ~TConfMainFrame();

	void __fastcall SetUserConfMode();
	bool __fastcall GetUserConfMode() const { return UserConfMode; }

protected:

	AnsiString OptionsFileName;
	AnsiString OriginalOptionsGroupBoxLabel;
	AnsiString OrgExcludeOptions;
	bool UserConfMode;

public:
	bool __fastcall CheckOK();
protected:
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
	void __fastcall ReadOptionInfoFromExe(AnsiString exename);

public: // load and store
	void __fastcall CopyExe();

//--
	// options data
protected:
	TLayeredOption UserConfigData;
	TLayeredOption ConfigData;
	TLayeredOption EmbeddedConfigData;
	TLayeredOption DefaultConfigData;
	TLayeredOption * TargetConfigData;
	TStringList * ExcludeOptions;

public:
	// data mgn.
	void InitConfigData();
	void FreeConfigData();

	// data exchange between data and ui
	void ApplyReadDataToUI();
	void ApplyUIDataToConfigData();

	// filenames
	void SetSourceAndTargetFileName(AnsiString source, AnsiString target);

	static AnsiString GetConfigFileName(AnsiString exename);
	static AnsiString GetDataPathDirectory(AnsiString datapath, AnsiString exename);
	static AnsiString GetUserConfigFileName(AnsiString datapath, AnsiString exename);

	// misc
	unsigned int __fastcall FindOptionAreaOffset(AnsiString target);

	AnsiString ReadOptionSectionFromExe(AnsiString filename, int section);
	void WriteOptionSectionToExe(AnsiString filename, int section, AnsiString content);

	// read stuff
	void ReadOptionsFromExe(TLayeredOption * dest, AnsiString filename);
	void ReadOptionsFromConfFile(TLayeredOption * dest, AnsiString filename);
	void ReadOptionsFromIni(TLayeredOption * dest, TMemIniFile * ini);
	void ReadExcludeOptionsFromString(AnsiString str);
	void ReadExcludeOptionsFromExe(AnsiString filename);
	void ReadExcludeOptionsFromIni(TMemIniFile * ini);
	static int GetSecurityOptionItem(const char *options, const char *name);
	void ReadSecurityOptionsFromExe(AnsiString filename);
	void ReadSecurityOptionsFromIni(TMemIniFile * ini);
	void ReadOptions(AnsiString exename);
	void ReadOptions() { ReadOptions(SourceExe); }
	void ReadFromIni(TMemIniFile *ini);

	// write stuff
	void WriteOptionsToExe(const TLayeredOption * src, AnsiString filename);
	void WriteOptionsToConfFile(const TLayeredOption * src, AnsiString filename);
	void WriteOptionsToIni(const TLayeredOption * src, TMemIniFile * ini);
	AnsiString ConvertExcludeOptionsToString();
	void WriteExcludeOptionsToExe(AnsiString filename);
	void WriteExcludeOptionsToIni(TMemIniFile * ini);
	static void SetSecurityOptionItem(char *options, const char *name, int value);
	void WriteSecurityOptionsToExe(AnsiString filename);
	void WriteSecurityOptionsToIni(TMemIniFile * ini);
	void WriteOptions(AnsiString exename);
	void WriteOptions() { WriteOptions(TargetExe); }
	void WriteToIni(TMemIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfMainFrame *ConfMainFrame;
//---------------------------------------------------------------------------
#endif
