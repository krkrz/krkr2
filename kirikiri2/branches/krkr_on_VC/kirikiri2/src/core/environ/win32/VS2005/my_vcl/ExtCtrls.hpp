#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "Controls.hpp"
#include "Forms.hpp"
#include "Messages.hpp"

#include <wx/timer.h>
//----------------------------------------------------------------------------
class TTimer : public TComponent
{
private:
	wxTimer* m_wxTimer;
public:
	TTimer(TComponent* Owner);
	virtual ~TTimer();
	PROPERTY_VAR1(int, Interval);
	PROPERTY(bool,Enabled);
	boost::function1<void, System::TObject*> OnTimer;
};
