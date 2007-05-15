#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include <wx/config.h>

class TRegistry : public System::TObject
{
protected:
	wxRegKey* m_wxRegKey;
public:
	TRegistry();
	virtual ~TRegistry();
	PROPERTY_VAR0( HKEY, RootKey );
	bool OpenKey(const AnsiString &, bool );

	size_t ReadBinaryData(const AnsiString & Name, void*, size_t );
	size_t WriteBinaryData(const AnsiString & Name, void*, size_t );
	AnsiString ReadString(const AnsiString & Name );
	void CloseKey(void);
};