//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DSound.h"
#include "LoopTunerMainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoopTunerMainForm *LoopTunerMainForm;
//---------------------------------------------------------------------------
__fastcall TLoopTunerMainForm::TLoopTunerMainForm(TComponent* Owner)
	: TForm(Owner)
{
	Reader = new TWaveReader();
	Reader->OnReadProgress = OnReaderProgress;
	WaveView = new TWaveView(this);
	WaveView->Parent = this;
	WaveView->Align = alClient;
	WaveView->Reader = Reader;
	InitDirectSound(Application->Handle);
	Manager = NULL;
	Application->OnIdle = OnApplicationIdle;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FormDestroy(TObject *Sender)
{
	StopPlay();
	FreeDirectSound();
	delete WaveView;
	if(Manager) delete Manager, Manager = NULL;
	delete Reader;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnReaderProgress(TObject *sender)
{
	int pct = Reader->SamplesRead / (Reader->NumSamples / 100);
	if(pct > 100) pct = 100;
	Caption = AnsiString(pct);
	if(Reader->ReadDone)
	{
		WaveView->Reader = Reader; // reset the reader
		WaveView->Invalidate();
		if(Manager) delete Manager, Manager = NULL;
		Manager = new tTVPWaveLoopManager(Reader);
	}
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::OpenActionExecute(TObject *Sender)
{
	OpenDialog->Filter = Reader->FilterString;
	if(OpenDialog->Execute())
	{
		Reader->LoadWave(OpenDialog->FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::ZoomInActionExecute(TObject *Sender)
{
	WaveView->Magnify ++;
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::ZoomOutActionExecute(TObject *Sender)
{
	WaveView->Magnify --;
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::PlayFromStartActionExecute(TObject *Sender)
{
	if(Reader->ReadDone)
	{
		const WAVEFORMATEXTENSIBLE * wfx;
		wfx = Reader->GetWindowsFormat();
		StartPlay(wfx, Manager);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnApplicationIdle(TObject *sender, bool &done)
{
	int pos = (int)GetCurrentPlayingPos();

	if(pos != -1)
	{
		done = false;

		WaveView->MarkerPos = pos;

		Sleep(1); // this will make the cpu usage low
	}
	else
	{
		done = true;
	}
}
//---------------------------------------------------------------------------

