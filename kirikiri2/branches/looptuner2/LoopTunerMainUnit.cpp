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
	WaveView->OnWaveLButtonDown = OnWaveViewWaveLButtonDown;
	WaveView->OnLinkLButtonDown = OnWaveViewLinkLButtonDown;
	InitDirectSound(Application->Handle);
	Manager = NULL;
	Application->OnIdle = OnApplicationIdle;
	Application->OnActivate = FormActivate;
	Application->OnDeactivate = FormDeactivate;
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
		StopPlay();
		const WAVEFORMATEXTENSIBLE * wfx;
		wfx = Reader->GetWindowsFormat();
		Manager->SetPosition(0);
		StartPlay(wfx, Manager);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnApplicationIdle(TObject *sender, bool &done)
{
	int pos = (int)GetCurrentPlayingPos();

	if(pos != -1)
	{
		Sleep(1); // this will make the cpu usage low
		done = false;

		WaveView->MarkerPos = pos;

	}
	else
	{
		done = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewWaveLButtonDown(TObject *sender, int pos)
{
	WaveView->CaretPos = pos;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewLinkLButtonDown(TObject *sender, int link)
{
	WaveView->FocusedLink = link;
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::FormActivate(TObject *Sender)
{
	WaveView->DrawCaret = true;	
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::FormDeactivate(TObject *Sender)
{
	WaveView->DrawCaret = false;	
}
//---------------------------------------------------------------------------

