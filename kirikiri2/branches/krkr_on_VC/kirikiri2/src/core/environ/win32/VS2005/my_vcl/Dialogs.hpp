#pragma once

#include "vcl_base.h"
#include "Component.h"

//----------------------------------------------------------------------------
class TCommonDialog : public TComponent
{
public:
	TCommonDialog(TComponent* owner);
};

class TOpenDialog : public TCommonDialog
{
public:
	TOpenDialog(TComponent* owner);
	PROPERTY_VAR0(AnsiString, FileName);
	virtual bool Execute(void);
};

class TSaveDialog : public TOpenDialog
{
public:
	TSaveDialog(TComponent* owner);
	virtual bool Execute(void);
};

