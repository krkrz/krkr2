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
	WaveView->OnDoubleClick = OnWaveViewDoubleClick;
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
		if(Manager) delete Manager, Manager = NULL;
		Manager = new tTVPWaveLoopManager(Reader);

		// load link and label information
		if(FileExists(FileName + ".sli"))
		{
			char *mem = NULL;
			TFileStream *fs = new TFileStream(FileName + ".sli", fmShareDenyWrite | fmOpenRead);
			try
			{
				try
				{
					// load into memory
					int size = fs->Size;
					mem = new char [size + 1];
					fs->Read(mem, size);
					mem[size] = '\0';
				}
				catch(...)
				{
					delete fs;
					throw;
				}
				delete fs;

				// read from the memory
				if(!Manager->ReadInformation(mem))// note that this method can destory the buffer
				{
					throw Exception(FileName + ".sli は文法が間違っているか、"
						"不正な形式のため、読み込むことができません");
				}
			}
			catch(...)
			{
				if(mem) delete [] mem;
				throw;
			}
			if(mem) delete [] mem;
		}
		else
		{
			// TODO: clear the links and labels
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::OpenActionExecute(TObject *Sender)
{
	OpenDialog->Filter = Reader->FilterString;
	if(OpenDialog->Execute())
	{
		FileName = OpenDialog->FileName;

		// load wave
		Reader->LoadWave(FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::SaveActionExecute(TObject *Sender)
{
	// save current loop information
	TFileStream * fs = new TFileStream(FileName  + ".sli" ,  fmShareDenyWrite | fmCreate);
	try
	{
		Manager->SetLinks(WaveView->GetLinks());
		Manager->SetLabels(WaveView->GetLabels());
		AnsiString str;
		Manager->WriteInformation(str);
		Manager->ClearLinksAndLabels();
		fs->Write(str.c_str(), str.Length());
	}
	catch(...)
	{
		delete fs;
		throw;
	}
	delete fs;
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

void __fastcall TLoopTunerMainForm::UndoActionExecute(TObject *Sender)
{
	WaveView->Undo();
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::RedoActionExecute(TObject *Sender)
{
	WaveView->Redo();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::DeleteActionExecute(TObject *Sender)
{
	WaveView->DeleteItem();
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::ActionListUpdate(TBasicAction *Action,
	  bool &Handled)
{
	// check enable/disable state
	UndoAction->Enabled = WaveView->CanUndo();
	RedoAction->Enabled = WaveView->CanRedo();
	DeleteAction->Enabled = WaveView->CanDeleteItem();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::PlayFromStartActionExecute(TObject *Sender)
{
	PlayFrom(0);
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::PlayFromCaretActionExecute(
	  TObject *Sender)
{
	PlayFrom(WaveView->CaretPos);
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::StopPlayActionExecute(TObject *Sender)
{
	StopPlay();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::PlayFrom(int pos)
{
	if(Reader->ReadDone)
	{
		StopPlay();
		const WAVEFORMATEXTENSIBLE * wfx;
		wfx = Reader->GetWindowsFormat();
		Manager->SetPosition(pos);
		WaveView->ResetMarkerFollow();
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
		WaveView->MarkerPos = -1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewDoubleClick(TObject *Sender, int pos)
{
	PlayFrom(pos);
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::FormActivate(TObject *Sender)
{
	WaveView->ShowCaret = true;
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::FormDeactivate(TObject *Sender)
{
	WaveView->ShowCaret = false;	
}
//---------------------------------------------------------------------------








