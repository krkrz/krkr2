//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TVP Win32 Project File
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "SysInitIntf.h"
#include "SysInitImpl.h"
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "ScriptMgnIntf.h"
#include "ScriptMgnImpl.h"
#include "tjsError.h"
#include "PluginImpl.h"
#include "SystemIntf.h"

#define TVP_LOCALE_FIX

#ifdef TVP_LOCALE_FIX
extern "C"
{
struct tagLocale {
	long            codepage;     // Code page of the locale
	LCID            handle;       // LCID of the locale
	BOOL            isCLocale;    // is that locale "C" locale ?
};
extern __cdecl tagLocale *__locale; // locale structure in RTL
}
#endif

//---------------------------------------------------------------------------
bool TVPCheckCmdDescription(void);
bool TVPCheckAbout(void);
void TVPOnError();
//---------------------------------------------------------------------------
USEFORM("..\HaltWarnFormUnit.cpp", TVPHaltWarnForm);
USEFORM("..\..\..\utils\win32\ConsoleFormUnit.cpp", TVPConsoleForm);
USEFORM("..\..\..\utils\win32\PadFormUnit.cpp", TVPPadForm);
USEFORM("..\MainFormUnit.cpp", TVPMainForm);
USEFORM("..\WatchFormUnit.cpp", TVPWatchForm);
USEFORM("..\..\..\visual\win32\WindowFormUnit.cpp", TVPWindowForm);
USEFORM("..\..\..\visual\win32\MenuContainerFormUnit.cpp", TVPMenuContainerForm);
USEFORM("..\..\..\visual\win32\FontSelectFormUnit.cpp", TVPFontSelectForm);
USEFORM("..\VersionFormUnit.cpp", TVPVersionForm);
USEFORM("..\..\..\..\tools\win32\krdevui\ConfMainFrameUnit.cpp", ConfMainFrame); /* TFrame: File Type */
//---------------------------------------------------------------------------
#ifdef TVP_SUPPORT_ERI
#	pragma link "../../../../Lib/liberina.lib"
#endif
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// setup locale information
#ifdef TVP_LOCALE_FIX
	/* This avoids RTL locale code page bug which occurs in WinME. */
	/* Here is the code that rewrites RTL's management area directly, yes, of cource,
	   we must be aware of thus nasty thing. */
	__locale->codepage  = GetACP();
	__locale->isCLocale = FALSE;
#else
	setlocale(LC_ALL, "");
#endif
	// try starting the program!
	try
	{
		TVPInitScriptEngine();

		// banner
		TVPAddImportantLog(TJS_W("Program started on ") + TVPGetOSName() +
			TJS_W(" (") + TVPGetPlatformName() + TJS_W(")"));

		// TVPInitializeBaseSystems
		TVPInitializeBaseSystems();

		Application->Initialize();

		if(TVPCheckCmdDescription()) return 0;
		if(TVPExecuteUserConfig1()) return 0; // userconf stage 1
		if(TVPExecuteUserConfig2()) return 0; // userconf stage 2
		if(TVPExecuteUserConfig3()) return 0; // userconf stage 3

		TVPSystemInit();

		if(TVPCheckAbout()) return 0; // version information dialog box;

		Application->Title = "‹g—¢‹g—¢";
		Application->CreateForm(__classid(TTVPMainForm), &TVPMainForm);
		TVPLoadPluigins(); // load plugin module *.tpm
		if(TVPProjectDirSelected) TVPInitializeStartupScript();

		Application->Run();

		TVPSystemUninit();
	}
	catch (EAbort &e)
	{
		// nothing to do
	}
	catch (Exception &exception)
	{
		TVPOnError();
		if(!TVPSystemUninitCalled)
			Application->ShowException(&exception);
	}
	catch (eTJSScriptError &e)
	{
		TVPOnError();
		if(!TVPSystemUninitCalled)
			Application->ShowException(&Exception(e.GetMessage().AsAnsiString()));
	}
	catch (eTJS &e)
	{
		TVPOnError();
		if(!TVPSystemUninitCalled)
			Application->ShowException(&Exception(e.GetMessage().AsAnsiString()));
	}
	catch(...)
	{
		Application->ShowException(&Exception("Unknown error!"));
	}

	TVPUninitScriptEngine();
	return 0;
}
//---------------------------------------------------------------------------





