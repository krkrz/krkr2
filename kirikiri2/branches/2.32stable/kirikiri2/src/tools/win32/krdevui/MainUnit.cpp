//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop




#pragma argsused
//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	Application->Initialize();

	return 1;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
extern "C" void _export PASCAL UIGetVersion(DWORD *hi, DWORD *low)
{
	static VS_FIXEDFILEINFO *FixedFileInfo;
	static BYTE VersionInfo[16384];

	static bool DoGet=true;
	if(DoGet)
	{
		DoGet=false;

		UINT dum;

		char path[MAX_PATH+1];
		GetModuleFileName(GetModuleHandle("krdevui.dll"), path, MAX_PATH);

		GetFileVersionInfo(path,0,16384,(void*)VersionInfo);
		VerQueryValue((void*)VersionInfo,"\\",(void**)(&FixedFileInfo),
			&dum);

	}

	if(FixedFileInfo)
	{
		if(hi) *hi = FixedFileInfo->dwFileVersionMS;
		if(low) *low = FixedFileInfo->dwFileVersionLS;
	}
	else
	{
		if(hi) *hi = 0;
		if(low) *low = 0;
	}
}
//---------------------------------------------------------------------------




