//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DSound.h"
#include "LoopTunerMainUnit.h"
#include "LinkDetailUnit.h"
#include "LabelDetailUnit.h"
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
	InitDirectSound(Application->Handle);
	Manager = NULL;
	ActiveControl = WaveView;
	EditAttribPanel->Height = EmptyEditAttribFrame->Height + EditLabelAttribBevel->Height;
	CreateWaveView();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FormDestroy(TObject *Sender)
{
	StopPlay();
	FreeDirectSound();
	if(WaveView) delete WaveView;
	if(Manager) delete Manager, Manager = NULL;
	delete Reader;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::CreateWaveView()
{
	if(WaveView) delete WaveView, WaveView = NULL;
	WaveView = new TWaveView(this);
	WaveView->Parent = this;
	WaveView->Align = alClient;
	WaveView->Reader = NULL;
	WaveView->OnStopFollowingMarker = WaveViewStopFollowingMarker;
	WaveView->OnWaveDoubleClick		= WaveViewWaveDoubleClick;
	WaveView->OnLinkDoubleClick		= WaveViewLinkDoubleClick;
	WaveView->OnLabelDoubleClick	= WaveViewLabelDoubleClick;
	WaveView->OnNotifyPopup			= WaveViewNotifyPopup;
	WaveView->OnShowCaret			= WaveViewShowCaret;
	WaveView->OnLinkSelected		= WaveViewLinkSelected;
	WaveView->OnLabelSelected		= WaveViewLabelSelected;
	WaveView->OnSelectionLost		= WaveViewSelectionLost;
	WaveView->OnLinkModified		= WaveViewLinkModified;
	WaveView->OnLabelModified		= WaveViewLabelModified;
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

			// note that below two wiil call OnLinkModified/OnLabelModified event.
			// but it's ok because the handler will write back the same data to the waveview.
			WaveView->SetLinks(Manager->GetLinks());
			WaveView->SetLabels(Manager->GetLabels());
			WaveView->PushFirstUndoState();
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

		// stop playing
		StopPlay();

		// reset all
		CreateWaveView();
		FollowMarkerAction->Checked = true;

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
void __fastcall TLoopTunerMainForm::EditLinkDetailActionExecute(
	  TObject *Sender)
{
	WaveView->PerformLinkDoubleClick();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::EditLabelDetailActionExecute(
	  TObject *Sender)
{
	WaveView->PerformLabelDoubleClick();
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
void __fastcall TLoopTunerMainForm::FollowMarkerActionExecute(
	  TObject *Sender)
{
	FollowMarkerAction->Checked = !FollowMarkerAction->Checked;
	WaveView->FollowingMarker = FollowMarkerAction->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewStopFollowingMarker(TObject *Sender)
{
	FollowMarkerAction->Checked = false;
	WaveView->FollowingMarker = FollowMarkerAction->Checked;
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

	UndoAction->Enabled = WaveView->CanUndo();
	RedoAction->Enabled = WaveView->CanRedo();
	DeleteAction->Enabled = WaveView->CanDeleteItem();
	EditLinkDetailAction->Enabled = WaveView->FocusedLink != -1;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewWaveDoubleClick(TObject *Sender, int pos)
{
	PlayFrom(pos);
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLinkDoubleClick(TObject *Sender, int num, tTVPWaveLoopLink &link)
{
	bool org_waveview_followingmarker =  WaveView->FollowingMarker;
	WaveView->FollowingMarker = true; // temporarily enable the marker

	TLinkDetailForm * ldf = new TLinkDetailForm(this);
	try
	{
		ldf->SetReaderAndLink(Reader, link, num);
		ldf->ShowModal();
	}
	catch(...)
	{
		WaveView->FollowingMarker = org_waveview_followingmarker;
		delete ldf;
		throw;
	}
	WaveView->FollowingMarker = org_waveview_followingmarker;
	delete ldf;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLabelDoubleClick(TObject *Sender,
	int num, tTVPWaveLabel &label)
{
	TLabelDetailForm * ldf = new TLabelDetailForm(this);
	try
	{
		ldf->SetLabel(label);
		if(ldf->ShowModal() == mrOk)
		{
			WaveView->InvalidateLabel(num);
			label = ldf->GetLabel();
			WaveView->NotifyLabelChanged();
			WaveView->InvalidateLabel(num);
			WaveView->PushUndo(); //==== push undo
		}
	}
	catch(...)
	{
		delete ldf;
		throw;
	}
	delete ldf;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewNotifyPopup(TObject *Sender, AnsiString type)
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
void __fastcall TLoopTunerMainForm::WaveViewShowCaret(TObject *Sender, int pos)
{
	EditLabelAttribBevel->Visible = false;

	EmptyEditAttribFrame->Visible = true;
	EditLinkAttribFrame->Visible  = false;
	EditLabelAttribFrame->Visible = false;

	EditLabelAttribBevel->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLinkSelected(TObject *Sender, int num, tTVPWaveLoopLink &link)
{
	EditLabelAttribBevel->Visible = false;

	EditLinkAttribFrame->OnInfoChanged = EditLinkAttribFrameInfoChanged;
	EditLinkAttribFrame->OnEraseRedo = EditLinkAttribFrameEraseRedo;
	EditLinkAttribFrame->Tag  = num;
	EditLinkAttribFrame->SetLink(link);

	EmptyEditAttribFrame->Visible = false;
	EditLinkAttribFrame->Visible  = true;
	EditLabelAttribFrame->Visible = false;

	EditLabelAttribBevel->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLabelSelected(TObject *Sender, int num, tTVPWaveLabel &label)
{
	EditLabelAttribBevel->Visible  = false;

	EditLabelAttribFrame->OnInfoChanged = EditLabelAttribFrameInfoChanged;
	EditLabelAttribFrame->OnEraseRedo = EditLabelAttribFrameEraseRedo;
	EditLabelAttribFrame->Tag  = num;
	EditLabelAttribFrame->SetLabel(label);

	EmptyEditAttribFrame->Visible  = false;
	EditLinkAttribFrame->Visible   = false;
	EditLabelAttribFrame->Visible  = true;

	EditLabelAttribBevel->Visible  = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewSelectionLost(TObject *Sender)
{
	EditLabelAttribBevel->Visible = false;

	EmptyEditAttribFrame->Visible = true;
	EditLinkAttribFrame->Visible  = false;
	EditLabelAttribFrame->Visible = false;

	EditLabelAttribBevel->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLinkModified(TObject *Sender)
{
	Manager->SetLinks(WaveView->GetLinks());
	if(EditAttribPanel->Visible && EditLinkAttribFrame->Visible)
	{
		tTVPWaveLoopLink &link = WaveView->GetLinks()[EditLinkAttribFrame->Tag];
		EditLinkAttribFrame->SetLink(link);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLabelModified(TObject *Sender)
{
	Manager->SetLabels(WaveView->GetLabels());
	if(EditAttribPanel->Visible && EditLabelAttribFrame->Visible)
	{
		tTVPWaveLabel &label = WaveView->GetLabels()[EditLabelAttribFrame->Tag];
		EditLabelAttribFrame->SetLabel(label);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::EditLinkAttribFrameInfoChanged(TObject * Sender)
{
	tTVPWaveLoopLink &link = WaveView->GetLinks()[EditLinkAttribFrame->Tag];
	WaveView->InvalidateLink(EditLinkAttribFrame->Tag);
	EditLinkAttribFrame->SetLinkInfo(link);
	WaveView->NotifyLinkChanged();
	WaveView->InvalidateLink(EditLinkAttribFrame->Tag);
	WaveView->PushUndo(); //==== push undo
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::EditLinkAttribFrameEraseRedo(TObject * Sender)
{
	WaveView->EraseRedo();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::EditLabelAttribFrameInfoChanged(TObject * Sender)
{
	tTVPWaveLabel &label = WaveView->GetLabels()[EditLabelAttribFrame->Tag];
	WaveView->InvalidateLabel(EditLabelAttribFrame->Tag);
	EditLabelAttribFrame->SetLabelInfo(label);
	WaveView->NotifyLabelChanged();
	WaveView->InvalidateLabel(EditLabelAttribFrame->Tag);
	WaveView->PushUndo(); //==== push undo
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::EditLabelAttribFrameEraseRedo(TObject * Sender)
{
	WaveView->EraseRedo();
}
//---------------------------------------------------------------------------
























