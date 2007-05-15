#pragma once

#include "vcl_base.h"

//----------------------------------------------------------------------------
class Exception : public System::TObject
{
private:
public:
	AnsiString Message;

	Exception();
	Exception(const char* sz) { Message = AnsiString(sz); }
	Exception(const wchar_t* sz) { }
	Exception(const AnsiString& str) { Message = str; }
};

//----------------------------------------------------------------------------
class EAbort : public Exception
{
public:
	EAbort();
	EAbort(const char* sz) : Exception(sz) {}
};

//----------------------------------------------------------------------------
class EAccessViolation : public Exception
{
public:
	EAccessViolation();
};

extern PACKAGE AnsiString __fastcall ChangeFileExt(const AnsiString & FileName, const AnsiString & Extension);
extern PACKAGE AnsiString __fastcall ExtractFileDir(const AnsiString & FileName);
extern PACKAGE AnsiString __fastcall ExtractFileName(const AnsiString & FileName);
extern PACKAGE AnsiString __fastcall ExtractFilePath(const AnsiString & FileName);
extern PACKAGE AnsiString __fastcall ExcludeTrailingBackslash(const AnsiString & FileName);
extern PACKAGE AnsiString __fastcall IncludeTrailingBackslash(const AnsiString & FileName);
extern PACKAGE AnsiString __fastcall ExpandUNCFileName(const AnsiString & FileName);


extern bool DirectoryExists(const AnsiString&);
extern bool FileExists(const AnsiString&);
extern bool DeleteFile(const AnsiString&);
