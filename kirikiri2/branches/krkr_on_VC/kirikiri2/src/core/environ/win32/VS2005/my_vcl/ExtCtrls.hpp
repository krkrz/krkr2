#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "Controls.hpp"
#include "Forms.hpp"
#include "Messages.hpp"

//----------------------------------------------------------------------------
class TTimer : public TComponent
{
public:
	TTimer(TComponent* Owner);
	virtual ~TTimer();
	PROPERTY_VAR0(int, Interval);
	PROPERTY_VAR0(bool,Enabled);
	boost::function1<void, System::TObject*> OnTimer;
};
