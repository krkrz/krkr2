//---------------------------------------------------------------------------
#ifndef KrkrSignatureUnitH
#define KrkrSignatureUnitH
//---------------------------------------------------------------------------
extern bool CheckExeHasSignature(AnsiString fn);
typedef bool __fastcall (__closure *TSigProgressNotifyEvent)(int percent);
extern bool CheckSignature(AnsiString fn, AnsiString publickey,
	TSigProgressNotifyEvent notify);
//---------------------------------------------------------------------------

#endif
