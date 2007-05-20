#include "vcl.h"
#include "IniFiles.hpp"

//----------------------------------------------------------------------------
// IniFiles.hpp
//class TCustomIniFile : public System::TObject
//----------------------------------------------------------------------------
TCustomIniFile::TCustomIniFile()
{
}

//----------------------------------------------------------------------------
// 指定した値名が存在しないとかのエラーが起きた場合は例外が送出される
bool TCustomIniFile::ReadBool(const AnsiString & Section, const AnsiString & Ident, bool Default)
{
	return (bool)( ReadInteger(Section, Ident, (int) Default) != 0);
}

//----------------------------------------------------------------------------
void TCustomIniFile::WriteBool(const AnsiString & Section, const AnsiString & Ident, bool Value)
{
	WriteInteger(Section, Ident, (int) Value);
}

//----------------------------------------------------------------------------
int TCustomIniFile::ReadInteger(const AnsiString & Section, const AnsiString & Ident, int Default)
{
	AnsiString str = ReadString(Section, Ident, "");
	if ( str.Length() == 0 )
		return Default;
	return atoi(str.c_str());
}

//----------------------------------------------------------------------------
void TCustomIniFile::WriteInteger(const AnsiString & Section, const AnsiString & Ident, int Value)
{
	WriteString(Section, Ident, Value);
}


//----------------------------------------------------------------------------
//class TMemIniFile : public TCustomIniFile
//----------------------------------------------------------------------------
// TIniFileのバグ回避版らしい・・・が、TIniFileと同じことをやる。ドン
TMemIniFile::TMemIniFile(const AnsiString& str)
{
	if ( str.AnsiPos("\\\\?\\") )
		m_strIniFile = str.SubString( 4, str.Length() - 4).c_str();
	else
		m_strIniFile = str.c_str();
//	m_pStream = new wxFileInputStream( m_strIniFile );
//	m_pConfig = new wxFileConfig(*m_pStream);

//	m_pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, m_strIniFile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	m_pConfig = new wxFileConfig(wxT("krkr"), wxT("ym"), m_strIniFile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
}

//----------------------------------------------------------------------------
TMemIniFile::~TMemIniFile()
{
	delete m_pConfig;
	delete m_pStream;
}
//----------------------------------------------------------------------------
AnsiString TMemIniFile::ReadString(const AnsiString & Section, const AnsiString & Ident, const AnsiString & Default)
{
	wxString str;
	m_pConfig->SetPath( ("/" + Section).c_str() );
	m_pConfig->Read(Ident.c_str(), &str, Default.c_str());
	return str.c_str();
}
//----------------------------------------------------------------------------
void TMemIniFile::WriteString(const AnsiString & Section, const AnsiString & Ident, const AnsiString & Value)
{
	m_pConfig->SetPath( ("/" + Section).c_str() );
	m_pConfig->Write(Ident.c_str(), Value.c_str());
}
//----------------------------------------------------------------------------
void TMemIniFile::UpdateFile(void)
{
	wxFileOutputStream os(m_strIniFile);
	m_pConfig->Save(os);
}

//----------------------------------------------------------------------------
void TMemIniFile::EraseSection(const AnsiString & Section)
{
	// 未実装
}

//----------------------------------------------------------------------------
void TMemIniFile::ReadSectionValues(const AnsiString & Section, TStrings* Strings)
{
	// 未実装
}

