//---------------------------------------------------------------------------

#ifndef SignUnitH
#define SignUnitH
//---------------------------------------------------------------------------
extern bool SignFile(AnsiString privkey, AnsiString signfn, int ignorestart, int ignoreend, int ofs);
extern bool CheckSignatureOfFile(AnsiString pubkey, AnsiString signfn, int ignorestart, int ignoreend, int ofs);
//---------------------------------------------------------------------------


#endif
