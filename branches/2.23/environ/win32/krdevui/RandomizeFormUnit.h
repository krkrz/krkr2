//---------------------------------------------------------------------------

#ifndef RandomizeFormUnitH
#define RandomizeFormUnitH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <tomcrypt/mycrypt.h>
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TRandomizeForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TButton *CancelButton;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label5;
	TLabel *Label7;
	TTimer *Timer;
	void __fastcall TimerTimer(TObject *Sender);
private:	// ユーザー宣言
	prng_state *State;
	POINT PrevMousePos;
	__int64 Distance;
	DWORD StartTick;
	
public:		// ユーザー宣言
	__fastcall TRandomizeForm(TComponent* Owner, prng_state *state);
};
//---------------------------------------------------------------------------
extern PACKAGE TRandomizeForm *RandomizeForm;
//---------------------------------------------------------------------------
bool RandomizePRNG(prng_state *state);
bool RandomizePRNGSimple(prng_state *state);
//---------------------------------------------------------------------------

#endif
