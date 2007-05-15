#include "Registry.hpp"

//----------------------------------------------------------------------------
//class TRegistry
TRegistry::TRegistry()
{
	m_wxRegKey = NULL;
}

//----------------------------------------------------------------------------
TRegistry::~TRegistry()
{
	CloseKey();
}
//----------------------------------------------------------------------------
//PROPERTY( HKEY, RootKey );
//----------------------------------------------------------------------------
// 指定のキーをオープンする
// OpenKey メソッドを実行すると、指定のキーがカレントキーになる。
// CloseKey メソッドを実行せずに別のキーをオープンすると、直前にオープンしたキーをルートとして下層のキーを開く。
bool TRegistry::OpenKey(const AnsiString & Key, bool CanCreate)
{
	//HKEY_CLASSES_ROOT (HKCR)
	//HKEY_CURRENT_USER (HKCU)
	//HKEY_LOCAL_MACHINE (HKLM)
	//HKEY_CURRENT_CONFIG (HKCC)
	//HKEY_USERS (HKU)

	if ( m_wxRegKey == NULL )
	{
		if ( RootKey == HKEY_LOCAL_MACHINE )
			m_wxRegKey = new wxRegKey(wxRegKey::HKLM, Key.c_str());
		else
			m_wxRegKey = new wxRegKey(Key.c_str());
	}
	else
	{
		wxRegKey* oldRegKey = m_wxRegKey;
		m_wxRegKey = new wxRegKey(*oldRegKey, Key.c_str());
		delete oldRegKey;
	}
	if ( m_wxRegKey->Exists() == false )
	{
		if ( CanCreate )
			m_wxRegKey->Create();
		else
		{
			delete m_wxRegKey;
			return false;
		}
	}
	m_wxRegKey->Open();
	return true;
}
//----------------------------------------------------------------------------
// カレントキーの指定の値名のバイナリデータを取得する
size_t TRegistry::ReadBinaryData(const AnsiString & Name, void* Buffer, size_t BufLen)
{
	wxMemoryBuffer mem;
	m_wxRegKey->QueryValue( Name.c_str(), mem );

	size_t size = std::min( BufLen, mem.GetBufSize());
	memcpy(Buffer, mem.GetData(), size );
	return size;
}
//----------------------------------------------------------------------------
// カレントキーの指定の値名にバイナリデータを格納する
size_t TRegistry::WriteBinaryData(const AnsiString & Name, void* Buffer, size_t BufLen)
{
	wxMemoryBuffer mem;
	mem.AppendData(Buffer, BufLen);
	m_wxRegKey->SetValue( Name.c_str(), mem );
	return mem.GetBufSize();
}

//----------------------------------------------------------------------------
// カレントキーの指定の値名の文字列型データを取得する
AnsiString TRegistry::ReadString(const AnsiString & Name)
{
	// 未実装
	return "";
}

//----------------------------------------------------------------------------
// カレントキーの内容をレジストリに書き込み、キーをクローズする
void TRegistry::CloseKey(void)
{
	if ( m_wxRegKey )
	{
		m_wxRegKey->Close();
		delete m_wxRegKey;
		m_wxRegKey = NULL;
	}
}
