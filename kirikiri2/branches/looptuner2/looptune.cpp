//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("looptune.res");
USEUNIT("WaveReader.cpp");
USEUNIT("RIFFWaveContext.cpp");
USEUNIT("TSSWaveContext.cpp");
USEUNIT("tvpsnd.c");
USEUNIT("WaveView.cpp");
USEUNIT("WaveLoopManager.cpp");
USEUNIT("DSound.cpp");
USEFORM("LoopTunerMainUnit.cpp", LoopTunerMainForm);
USEUNIT("CharacterSet.cpp");
USEFORM("EditLabelAttribUnit.cpp", EditLabelAttribFrame); /* TFrame: File Type */
USEFORM("EditLinkAttribUnit.cpp", EditLinkAttribFrame); /* TFrame: File Type */
USEFORM("LinkDetailUnit.cpp", LinkDetailForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TLoopTunerMainForm), &LoopTunerMainForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
