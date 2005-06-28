


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

