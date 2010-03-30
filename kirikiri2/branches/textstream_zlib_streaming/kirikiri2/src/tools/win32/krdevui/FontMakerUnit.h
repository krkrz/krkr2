//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef FontMakerUnitH
#define FontMakerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "FontPreviewUnit.h"
//---------------------------------------------------------------------------
extern DWORD WINAPI (*procGetFontUnicodeRanges)(HDC, LPGLYPHSET);
extern DWORD WINAPI (*procGetGlyphOutlineW)(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2 *);
extern BOOL  WINAPI (*procTextOutW)(HDC, int, int, LPCWSTR, int);
extern BOOL  APIENTRY (*procGetTextExtentPoint32W)(HDC, LPCWSTR, int, LPSIZE);
//---------------------------------------------------------------------------
class TPrivRefControl : public TCustomControl
{
	// a class to provide Canvas and window handle
public:
	__fastcall TPrivRefControl(TComponent *AOwner) : TCustomControl(AOwner) {;}
	__fastcall ~TPrivRefControl() {;}

public:
	__property Canvas;
};
//---------------------------------------------------------------------------
class TFontMakerForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TGroupBox *FontGroupBox;
	TLabel *Label1;
	TComboBox *FontSelectComboBox;
	TLabel *Label2;
	TEdit *SizeEdit;
	TUpDown *SizeUpDown;
	TCheckBox *BoldCheckBox;
	TButton *ShowSampleButton;
	TGroupBox *OutputGroupBox;
	TEdit *OutputEdit;
	TButton *OutputRefButton;
	TLabel *Label3;
	TButton *CreateButton;
	TButton *CancelButton;
	TCheckBox *IncludeAllFontCharsCheckBox;
	TCheckBox *AntialiasedCheckBox;
	TSaveDialog *SaveDialog;
	TCheckBox *ShowAllFontsCheckBox;
	TCheckBox *IncludePrivateCharsCheckBox;
	void __fastcall OutputRefButtonClick(TObject *Sender);
	void __fastcall CreateButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall ShowAllFontsCheckBoxClick(TObject *Sender);
	void __fastcall ShowSampleButtonClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FontSelectComboBoxChange(TObject *Sender);
	void __fastcall SizeEditChange(TObject *Sender);
	void __fastcall BoldCheckBoxClick(TObject *Sender);
	void __fastcall AntialiasedCheckBoxClick(TObject *Sender);
	void __fastcall IncludeAllFontCharsCheckBoxClick(TObject *Sender);
	void __fastcall IncludePrivateCharsCheckBoxClick(TObject *Sender);
private:	// ユーザー宣言
	TFontPreviewForm *FontPreviewForm;
	void __fastcall EnumFonts();
	bool CharacterListInvalidated;
	wchar_t * CharacterList;
	int CharacterListCount;
	TPrivRefControl * PrivRefControl;


public:		// ユーザー宣言
	__fastcall TFontMakerForm(TComponent* Owner);
private:
	void __fastcall SetFontToCanvas(LOGFONT & font);
	void __fastcall SetPrivFontToPrivCanvas();
	void __fastcall NotifyFontChanged();

	static int __cdecl CodeComparison(const void *s1, const void *s2);
	void __fastcall PushDBCSCharacter(int code);
	void __fastcall PushCharacterRange(int base, int count);
	void __fastcall CreateCharacterList();
public:
	int __fastcall GetCharacterListCount();
	const wchar_t * __fastcall GetCharacterList();
};
//---------------------------------------------------------------------------
extern PACKAGE TFontMakerForm *FontMakerForm;
//---------------------------------------------------------------------------
#endif
