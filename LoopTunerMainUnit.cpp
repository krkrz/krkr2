//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DSound.h"
#include "LoopTunerMainUnit.h"
#include "LinkDetailUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EditLabelAttribUnit"
#pragma link "EditLinkAttribUnit"
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
	WaveView->OnWaveDoubleClick = OnWaveViewWaveDoubleClick;
	WaveView->OnLinkDoubleClick = OnWaveViewLinkDoubleClick;
	WaveView->OnNotifyPopup = OnWaveViewNotifyPopup;
	WaveView->OnShowCaret = OnWaveViewShowCaret;
	WaveView->OnLinkSelected = OnWaveViewLinkSelected;
	WaveView->OnLabelSelected = OnWaveViewLabelSelected;
	WaveView->OnSelectionLost = OnWaveViewSelectionLost;
	InitDirectSound(Application->Handle);
	Manager = NULL;
	ActiveControl = WaveView;
	EditAttribPanel->Height = EmptyEditAttribFrame->Height + EditLabelAttribBevel->Height;
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
void __fastcall TLoopTunerMainForm::UndoActionUpdate(TObject *Sender)
{
	UndoAction->Enabled = WaveView->CanUndo();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::RedoActionExecute(TObject *Sender)
{
	WaveView->Redo();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::RedoActionUpdate(TObject *Sender)
{
	RedoAction->Enabled = WaveView->CanRedo();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::DeleteActionExecute(TObject *Sender)
{
	WaveView->DeleteItem();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::DeleteActionUpdate(TObject *Sender)
{
	DeleteAction->Enabled = WaveView->CanDeleteItem();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::NewLinkActionExecute(TObject *Sender)
{
	WaveView->CreateNewLink();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::NewLabelActionExecute(TObject *Sender)
{
	WaveView->CreateNewLabel();
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
void __fastcall TLoopTunerMainForm::ShowEditAttribActionExecute(
	  TObject *Sender)
{
	ShowEditAttribAction->Checked = !ShowEditAttribAction->Checked;
	EditAttribPanel->Visible = ShowEditAttribAction->Checked;
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
void __fastcall TLoopTunerMainForm::ApplicationEventsIdle(TObject *Sender,
	  bool &Done)
{
	int pos = (int)GetCurrentPlayingPos();

	if(pos != -1)
	{
		Sleep(1); // this will make the cpu usage low
		Done = false;

		WaveView->MarkerPos = pos;

	}
	else
	{
		Done = true;
		WaveView->MarkerPos = -1;
	}

}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewWaveDoubleClick(TObject *Sender, int pos)
{
	PlayFrom(pos);
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewLinkDoubleClick(TObject *Sender, int num, tTVPWaveLoopLink &link)
{
	TLinkDetailForm * ldf = new TLinkDetailForm(this);
	try
	{
		ldf->SetReaderAndLink(Reader, link);
		ldf->ShowModal();
	}
	catch(...)
	{
		delete ldf;
		throw;
	}
	delete ldf;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewNotifyPopup(TObject *Sender, AnsiString type)
{
	POINT pt;
	::GetCursorPos(&pt);
	if(type == "Link")
	{
		ForLinkPopupMenu->Popup(pt.x, pt.y);
	}
	else if(type == "Label")
	{
		ForLabelPopupMenu->Popup(pt.x, pt.y);
	}
	else if(type == "Wave")
	{
		ForWavePopupMenu->Popup(pt.x, pt.y);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewShowCaret(TObject *Sender, int pos)
{
	EditLabelAttribBevel->Visible = false;

	EmptyEditAttribFrame->Visible = true;
	EditLinkAttribFrame->Visible  = false;
	EditLabelAttribFrame->Visible = false;

	EditLabelAttribBevel->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewLinkSelected(TObject *Sender, int num, tTVPWaveLoopLink &link)
{
	EditLabelAttribBevel->Visible = false;

	EditLinkAttribFrame->WaveView = WaveView;
	EditLinkAttribFrame->LinkNum  = num;

	EmptyEditAttribFrame->Visible = false;
	EditLinkAttribFrame->Visible  = true;
	EditLabelAttribFrame->Visible = false;

	EditLabelAttribBevel->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewLabelSelected(TObject *Sender, int num, tTVPWaveLabel &label)
{
	EditLabelAttribBevel->Visible  = false;

	EditLabelAttribFrame->WaveView = WaveView;
	EditLabelAttribFrame->LabelNum = num;

	EmptyEditAttribFrame->Visible  = false;
	EditLinkAttribFrame->Visible   = false;
	EditLabelAttribFrame->Visible  = true;

	EditLabelAttribBevel->Visible  = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::OnWaveViewSelectionLost(TObject *Sender)
{
	EditLabelAttribBevel->Visible = false;

	EmptyEditAttribFrame->Visible = true;
	EditLinkAttribFrame->Visible  = false;
	EditLabelAttribFrame->Visible = false;

	EditLabelAttribBevel->Visible = true;
}
//---------------------------------------------------------------------------




















