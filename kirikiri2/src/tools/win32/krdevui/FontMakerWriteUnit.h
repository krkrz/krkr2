//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef FontMakerWriteUnitH
#define FontMakerWriteUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
#pragma pack(push, 1)
struct TCharacterItem
{
	wchar_t Code;
	unsigned __int32 Offset;
	unsigned __int16 Width;
	unsigned __int16 Height;
	__int16 OriginX;
	__int16 OriginY;
	__int16 IncX;
	__int16 IncY;
	__int16 Inc;
	__int16 Reserved;
};
#pragma pack(pop)
inline bool operator <(const TCharacterItem &x, const TCharacterItem &y)
{ return x.Code< y.Code; }
//---------------------------------------------------------------------------
class TFontMakerWriteForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TProgressBar *ProgressBar;
	TButton *CancelButton;
	TTimer *Timer;
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// ユーザー宣言
	AnsiString FileName;
	bool Canceled;
	TCanvas *TargetCanvas;
	TCanvas *PrivTargetCanvas;
	const wchar_t *CharacterList;
	int CharacterListCount;
	bool Antialiased;
	bool Vertical;
	TStream * OutStream;
	std::vector<TCharacterItem> Items;
public:		// ユーザー宣言
	__fastcall TFontMakerWriteForm(TComponent* Owner,
		AnsiString outfile, TCanvas * font, TCanvas * privfont,
		bool antialiased, bool vertical, const wchar_t *list, int listcount);
};
//---------------------------------------------------------------------------
extern PACKAGE TFontMakerWriteForm *FontMakerWriteForm;
//---------------------------------------------------------------------------
#endif
