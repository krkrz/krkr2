//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <string.h>

//---------------------------------------------------------------------------
USERES("looptune.res");
USEFORM("MainUnit.cpp", TSSLoopTunerMainWindow);
USEUNIT("WaveReaderUnit.cpp");
USEUNIT("WaveDispUnit.cpp");
USEUNIT("DSoundUnit.cpp");
USEUNIT("tvpsnd.c");
USEFORM("WaveWriterUnit.cpp", WaveWriterForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HWND wnd = FindWindow("TTSSLoopTunerMainWindow", NULL);
	if(wnd)
	{
		char buf[256];
		GetWindowText(wnd, buf, 256-1);
		if(strstr(buf, "(//designing//)")==NULL)
		{
			PostMessage(wnd, WM_USER+ 199, 0, 0);
			int i;
			for(i=1; i<_argc; i++)
			{
				if(FileExists(_argv[i]))
				{
					AnsiString data = "open:" + AnsiString(_argv[i]);
					COPYDATASTRUCT cds;
					cds.dwData = 0x746f8ab2;
					cds.cbData = data.Length() + 1;
					cds.lpData = data.c_str();
					SendMessage(wnd, WM_COPYDATA, NULL, (LPARAM)&cds);
				}
			}
			return 0;
		}
	}

	try
	{
		Application->Initialize();
		Application->Title = "Loop Tuner";
		Application->CreateForm(__classid(TTSSLoopTunerMainWindow), &TSSLoopTunerMainWindow);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
