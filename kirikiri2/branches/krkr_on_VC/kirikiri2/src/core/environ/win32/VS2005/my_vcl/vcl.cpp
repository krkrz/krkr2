#include "vcl.h"
#include "Forms.hpp"


//----------------------------------------------------------------------------
// 本来の戻り値はShortString。めんどいので、AnsiStringに。あと、typeid演算子を使う都合、staticにできなかった
AnsiString System::TObject::ClassName() const
{
	return AnsiString( typeid(*this).name() );
}

//----------------------------------------------------------------------------
int getHintHidePause()
{
	return 250;
}
void setHintHidePause(int v)
{
}

//----------------------------------------------------------------------------
bool getShowHint()
{
	return false;
}
void setShowHint(bool v)
{
}
//----------------------------------------------------------------------------
bool getShowMainForm()
{
	return true;
}
//----------------------------------------------------------------------------
void setShowMainForm(bool v)
{
}


//----------------------------------------------------------------------------
// System.hpp
PACKAGE AnsiString __fastcall ParamStr(int Index)
{
	return AnsiString(_argv[Index]);
}



//----------------------------------------------------------------------------
int FindFirst( const AnsiString & Path, int Attr, TSearchRec & F )
{
	return 0;
}
//----------------------------------------------------------------------------
int FindNext( TSearchRec &F )
{
	return 0;
}
//----------------------------------------------------------------------------
void FindClose( TSearchRec &F )
{
	return;
}
