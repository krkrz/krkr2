#pragma once

#include "vcl_base.h"
#include "Component.h"

//----------------------------------------------------------------------------
class TForm;
class TScreen : public TComponent
{
private:
	std::vector<TForm*> m_Forms;
	std::vector<HCURSOR> m_Cursors;
public:
	PROPERTY_R(int, Width);
	PROPERTY_R(int, Height);
	PROPERTY_R(int, FormCount);
	PROPERTY_VAR0(TCursor, Cursor);

	PROPERTY_ARY_R(TForm*, Forms);
	PROPERTY_ARY(HCURSOR, Cursors);
};
extern TScreen* Screen;

