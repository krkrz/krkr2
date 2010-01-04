//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef RelSettingsUnitH
#define RelSettingsUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Mask.hpp>
#include <inifiles.hpp>
#include "ConfMainFrameUnit.h"
//---------------------------------------------------------------------------
class TRelSettingsForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TButton *OKButton;
	TButton *CancelButton;
	TButton *SelectFolderButton;
	TPageControl *PageControl;
	TTabSheet *OutputSheel;
	TGroupBox *OutputFileGroup;
	TEdit *OutputFileNameEdit;
	TButton *SetFileNameButton;
	TGroupBox *OutputFormatGroup;
	TRadioButton *XP3ArchiveRadio;
	TRadioButton *ExecutableRadio;
	TTabSheet *FileTypeSheet;
	TListBox *CompressExtList;
	TListBox *StoreExtList;
	TListBox *DiscardExtList;
	TTabSheet *OptionSheet;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TCheckBox *CompressLimitCheck;
	TLabel *CompressLimitSizeLabel;
	TCheckBox *ProtectCheck;
	TButton *SaveProfileButton;
	TButton *LoadProfileButton;
	TStaticText *StaticText1;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *NoExtNameLabel;
	TSaveDialog *SaveDialog;
	TTabSheet *ExeOptionsSheet;
	TEdit *CompressLimitSizeEdit;
	TSaveDialog *SaveProfileDialog;
	TOpenDialog *OpenProfileDialog;
	TLabel *InterruptNameLabel;
	TLabel *ExitNameLabel;
	TLabel *ArchiveCreatedNameLabel;
	TLabel *InterruptedNameLabel;
	TLabel *ProgressLabel;
	TLabel *SpecifyOutputNameLabel;
	TConfMainFrame *ConfMainFrame;
	TLabel *ConfirmOverwriteLabel;
	TCheckBox *OVBookShareCheck;
	TCheckBox *CompressIndexCheck;
	TLabel *MissingKrKrEXELabel;
	TLabel *ExecutableCreatedNameLabel;
	TLabel *ProtectWarnLabel;
	TLabel *ProtectWarnFileNameLabel1;
	TLabel *ProtectWarnFileNameLabel2;
	TCheckBox *UseXP3EncDLLCheck;
	TMemo *InputDirectoryMemo;
	TLabel *Label6;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall CompressExtListStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall CompressExtListDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
	void __fastcall CompressExtListDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall SetFileNameButtonClick(TObject *Sender);
	void __fastcall XP3ArchiveRadioClick(TObject *Sender);
	void __fastcall ExecutableRadioClick(TObject *Sender);
	void __fastcall CompressLimitCheckClick(TObject *Sender);
	void __fastcall CompressLimitSizeEditKeyPress(TObject *Sender, char &Key);
	void __fastcall CompressLimitSizeEditExit(TObject *Sender);
	void __fastcall SaveProfileButtonClick(TObject *Sender);
	void __fastcall LoadProfileButtonClick(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// ユーザー宣言
	void __fastcall SaveProfile(AnsiString name);
	void __fastcall SaveProfileToIni(TMemIniFile *ini);
public:
	void __fastcall LoadProfile(AnsiString name);
private:
	void __fastcall LoadProfileFromIni(TMemIniFile *ini);
	void __fastcall LoadExtList(AnsiString ext, TStrings *to, TStrings *f1, TStrings *f2);
	AnsiString ProjFolder;
	TStringList *FileList;
	void __fastcall MoveSelected(TListBox *dest, TListBox *src);

	void __fastcall _CreateArchive(void);
	void __fastcall CreateArchive(void);

	bool Aborted;

	bool GoImmediate;

	bool WriteDefaultRPF;

protected:
	void __fastcall WMStart(TMessage &msg);
	void __fastcall WMClose(TMessage &msg);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_USER + 0x10 , TMessage, WMStart);
	VCL_MESSAGE_HANDLER(WM_USER + 0x11 , TMessage, WMClose);
END_MESSAGE_MAP(TForm)

public:		// ユーザー宣言
	__fastcall TRelSettingsForm(TComponent* Owner);

	AnsiString __fastcall GetKrKrFileName();
	void __fastcall SetProjFolder(AnsiString folder);
	void __fastcall SetFileList(TStringList *list);
	void __fastcall SetExtList(TStringList *list);
	void __fastcall LoadDefaultProfile();

	void __fastcall SetGoImmediate(bool b = true) { GoImmediate = b; }
	void __fastcall SetWriteDefaultRPF(bool b = true) { WriteDefaultRPF = b; }
	void __fastcall SetOutputFilename(AnsiString s) { OutputFileNameEdit->Text = s; }
};
//---------------------------------------------------------------------------
extern PACKAGE TRelSettingsForm *RelSettingsForm;
//---------------------------------------------------------------------------
#endif
