//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

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
	int paramcount = ParamCount();
	for(int i = 1; i <= paramcount; i++)
	{
		AnsiString param = ParamStr(i);
		if(param.c_str()[0] != '-') file = param;
	}

	TConfSettingsForm *form = new TConfSettingsForm(Application);

	if(file != "")
	{
		if(form->ProcessFile(file))
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
