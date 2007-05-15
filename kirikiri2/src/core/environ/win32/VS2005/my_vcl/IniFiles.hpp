#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "SysUtils.hpp"

#include <wx/fileconf.h>

//----------------------------------------------------------------------------
class TCustomIniFile : public System::TObject
{
public:
	TCustomIniFile();
	virtual AnsiString ReadString(const AnsiString & Section, const AnsiString & Ident, const AnsiString & Default) = 0;
	virtual void WriteString(const AnsiString & Section, const AnsiString & Ident, const AnsiString & Value) = 0;

	virtual bool ReadBool(const AnsiString & Section, const AnsiString & Ident, bool Default);
	virtual void WriteBool(const AnsiString & Section, const AnsiString & Ident, bool Value);

	virtual int ReadInteger(const AnsiString & Section, const AnsiString & Ident, int Default);
	virtual void WriteInteger(const AnsiString & Section, const AnsiString & Ident, int Value);
	virtual void UpdateFile(void) = 0;

	virtual void EraseSection(const AnsiString & Section) = 0;
	virtual void ReadSectionValues(const AnsiString & Section, TStrings* Strings) = 0;
};
class TMemIniFile : public TCustomIniFile
{
protected:
	wxString m_strIniFile;
	wxFileInputStream* m_pStream;
	wxFileConfig* m_pConfig;
public:
	TMemIniFile(const AnsiString& str);
	virtual ~TMemIniFile();
	virtual AnsiString ReadString(const AnsiString & Section, const AnsiString & Ident, const AnsiString & Default);
	virtual void WriteString(const AnsiString & Section, const AnsiString & Ident, const AnsiString & Value);
	virtual void UpdateFile(void);

	virtual void EraseSection(const AnsiString & Section);
	virtual void ReadSectionValues(const AnsiString & Section, TStrings* Strings);
};

