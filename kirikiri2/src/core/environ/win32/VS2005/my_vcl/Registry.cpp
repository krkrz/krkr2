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
	struct {
		HKEY hKey0;
		wxRegKey::StdKey hKey1;
	} table[] = {
		{ HKEY_CLASSES_ROOT,     wxRegKey::HKCR },
		{ HKEY_CURRENT_USER,     wxRegKey::HKCU },
		{ HKEY_LOCAL_MACHINE,    wxRegKey::HKLM },
		{ HKEY_USERS,            wxRegKey::HKUSR },
		{ HKEY_PERFORMANCE_DATA, wxRegKey::HKPD },
		{ HKEY_CURRENT_CONFIG,   wxRegKey::HKCC },
		{ HKEY_DYN_DATA,         wxRegKey::HKDD } };

	if ( m_wxRegKey == NULL )
	{
		int i;
		for(i=0; i < 7; i++)
		{
			if ( RootKey == table[i].hKey0 )
			{
				m_wxRegKey = new wxRegKey(wxRegKey::HKCR, Key.c_str());
				break;
			}
		}
		if ( i == 7 )
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
			m_wxRegKey = NULL;
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
		if ( m_wxRegKey->IsOpened()	)
			m_wxRegKey->Close();
		delete m_wxRegKey;
		m_wxRegKey = NULL;
	}
}
