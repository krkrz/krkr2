//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------


#include <inifiles.hpp>

//---------------------------------------------------------------------------
// global supporting functions
//---------------------------------------------------------------------------
TMemIniFile *  __fastcall GetIniFile();
void __fastcall CommitIniFile();
void __fastcall CommitAndCloseIniFile();
void __fastcall ReadWindowBasicInformationFromIniFile(const char * section, TForm * form);
void __fastcall WriteWindowBasicInformationToIniFile(const char * section, TForm * form);
//---------------------------------------------------------------------------

