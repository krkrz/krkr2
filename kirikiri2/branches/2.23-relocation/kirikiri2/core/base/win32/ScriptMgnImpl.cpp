//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS2 Script Managing
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop


#include "MainFormUnit.h"
#include "WindowIntf.h"
#include "ScriptMgnImpl.h"
#include "MsgIntf.h"
#include "PadFormUnit.h"
#include "tjsScriptBlock.h"
#include "EventIntf.h"
#include "SysInitImpl.h"
#include "SysInitIntf.h"
#include "DebugIntf.h"



//---------------------------------------------------------------------------
// TVPBeforeProcessUnhandledException
//---------------------------------------------------------------------------
void TVPBeforeProcessUnhandledException()
{
	TVPDumpHWException();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPShowScriptException
//---------------------------------------------------------------------------
/*
	These functions display the error location, reason, etc.
	And disable the script event dispatching to avoid massive occurrence of
	errors.
*/
//---------------------------------------------------------------------------
void TVPShowScriptException(eTJS &e)
{
	TVPSetSystemEventDisabledState(true);
	TVPOnError();

	if(!TVPSystemUninitCalled)
	{
		if(TVPMainForm) TVPMainForm->Visible = true;
		ttstr errstr = (ttstr(TVPScriptExceptionRaised) + TJS_W("\n") + e.GetMessage());
		TVPAddLog(ttstr(TVPScriptExceptionRaised) + TJS_W("\n") + e.GetMessage());
		MessageDlg(errstr.AsAnsiString(), mtError, TMsgDlgButtons() << mbOK, 0);
	}
//	throw EAbort("Script Error Abortion");
}
//---------------------------------------------------------------------------
void TVPShowScriptException(eTJSScriptError &e)
{
	TVPSetSystemEventDisabledState(true);
	TVPOnError();

	if(!TVPSystemUninitCalled)
	{

		if(TVPMainForm) TVPMainForm->Visible = true;
		TTVPPadForm *pad = new TTVPPadForm(Application);
		pad->FreeOnTerminate = true;
		pad->ExecButtonEnabled = false;
		pad->SetLines(e.GetBlockNoAddRef()->GetScript());
		pad->GoToLine(1+e.GetBlockNoAddRef()->SrcPosToLine(e.GetPosition() )
			- e.GetBlockNoAddRef()->GetLineOffset());
		pad->ReadOnly = true;
		pad->StatusText = e.GetMessage();
		pad->Caption = ttstr(TVPExceptionCDPName).AsAnsiString();
		pad->Visible = true;

		ttstr errstr = (ttstr(TVPScriptExceptionRaised) + TJS_W("\n") + e.GetMessage());
		TVPAddLog(ttstr(TVPScriptExceptionRaised) + TJS_W("\n") + e.GetMessage());
		if(e.GetTrace().GetLen() != 0)
			TVPAddLog(ttstr(TJS_W("trace : ")) + e.GetTrace());
		Application->MessageBox(errstr.AsAnsiString().c_str(),
			Application->Title.c_str(), MB_OK|MB_ICONSTOP);
		//	throw EAbort("Script Error Abortion");
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPInitializeStartupScript
//---------------------------------------------------------------------------
void TVPInitializeStartupScript()
{
	TVPExecuteStartupScript();
	if(TVPGetWindowCount() == 0 && (TVPMainForm && !TVPMainForm->Visible))
	{
		// no window is created and main window is invisible
		Application->Terminate();
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCreateNativeClass_Scripts
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Scripts()
{
	tTJSNC_Scripts *cls = new tTJSNC_Scripts();

	// setup some platform-specific members

//----------------------------------------------------------------------

// currently none

//----------------------------------------------------------------------
	return cls;
}
//---------------------------------------------------------------------------

