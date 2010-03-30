//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ConfMain.h"
#include "ConfSettingsUnit.h"

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
extern "C" void _export PASCAL UIExecConfig()
{
	AnsiString file;
	bool userconf = false;
	int paramcount = ParamCount();
	for(int i = 1; i <= paramcount; i++)
	{
		AnsiString param = ParamStr(i);
		if(param.c_str()[0] != '-')
			file = param;
		else if(param == "-userconf")
			userconf = true;
	}

	TConfSettingsForm *form = new TConfSettingsForm(Application, userconf);

	if(file != "")
	{
		if(form->InitializeConfig(file))
		{
			form->ShowModal();
		}
	}
	else
	{
		if(form->SelectFile())
		{
			form->ShowModal();
		}
	}
	delete form;
	ExitProcess(0);
}
//---------------------------------------------------------------------------


#pragma package(smart_init)
