//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// System Initialization and Uninitialization
//---------------------------------------------------------------------------
#ifndef SysInitImplH
#define SysInitImplH

//---------------------------------------------------------------------------
extern void TVPDumpHWException();

extern void TVPInitializeBaseSystems();

extern AnsiString TVPNativeProjectDir;
extern bool TVPProjectDirSelected;
extern bool TVPSystemIsBasedOnNT;
extern bool TVPCheckCmdDescription(void);


extern bool TVPExecuteUserConfig1();
extern bool TVPExecuteUserConfig2();
extern bool TVPExecuteUserConfig3();


//---------------------------------------------------------------------------


#include "SysInitIntf.h"

#endif
