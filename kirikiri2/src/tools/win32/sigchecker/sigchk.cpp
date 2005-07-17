//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("sigchk.res");
USEFORM("MainUnit.cpp", MainForm);
USEUNIT("krkrsiglib\KrkrExecutableUnit.cpp");
USEUNIT("krkrsiglib\KrkrSignatureUnit.cpp");
USELIB("..\..\..\..\..\..\..\Lib\libtomcrypt1.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
