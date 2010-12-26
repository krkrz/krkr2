//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef WaveWriterUnitH
#define WaveWriterUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TWaveReader;
class TWaveWriterForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TProgressBar *ProgressBar;
	TButton *CancelButton;
	TLabel *Label;
	TTimer *Timer;
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// ユーザー宣言
	bool CloseLock;
	bool UserBreak;
	void __fastcall WriteWaveData(TWaveReader *reader, int start, int length,
		AnsiString filename);


public:		// ユーザー宣言
	TWaveReader * WaveReader;
	int Start;
	int Length;
	AnsiString FileName;
	__fastcall TWaveWriterForm(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TWaveWriterForm *WaveWriterForm;
//---------------------------------------------------------------------------
#endif
