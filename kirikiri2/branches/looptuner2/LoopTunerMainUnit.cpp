//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DSound.h"
#include "LoopTunerMainUnit.h"
#include "LinkDetailUnit.h"
#include "LabelDetailUnit.h"
#include "looptune.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EditLabelAttribUnit"
#pragma link "EditLinkAttribUnit"
#pragma resource "*.dfm"
TLoopTunerMainForm *LoopTunerMainForm;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
__fastcall TLoopTunerMainForm::TLoopTunerMainForm(TComponent* Owner)
	: TForm(Owner)
{
	Reader = new TWaveReader();
	Reader->OnReadProgress = OnReaderProgress;
	InitDirectSound(Application->Handle);
	Manager = NULL;
	ActiveControl = WaveView;
	CreateWaveView();
	ResettingFlags = false;

	Application->HintHidePause = 24*60*60*1000;
		// not to hide tool tip hint immediately

	ReadFromIni();
}
//---------------------------------------------------------------------------
__fastcall TLoopTunerMainForm::~TLoopTunerMainForm()
{
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::ReadFromIni()
{
	// read information from ini file
	const char * section = "Main";

	// window position and size
	ReadWindowBasicInformationFromIniFile(section, this);

	bool b = GetIniFile()->ReadBool(section, "ShowEditFlags", false);
	if(b) ShowEditFlagsAction->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WriteToIni()
{
	// write information to ini file
	const char * section = "Main";
	WriteWindowBasicInformationToIniFile(section, this);
	GetIniFile()->WriteBool(section, "ShowEditFlags", ShowEditFlagsAction->Checked);
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
bool __fastcall TLoopTunerMainForm::GetCanClose()
{
	// get whether the document can be closed
	if(!Manager) return true;
	if(WaveView->CanUndo())
	{
		int r = MessageDlg("ファイル " + FileName + ".sli は変更されています。\n"
			"変更を保存しますか？", mtWarning, TMsgDlgButtons() << mbYes << mbNo << mbCancel, 0);
		if(r == mrYes)
		{
			SaveAction->Execute();
		}
		else if(r == mrCancel)
		{
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	WriteToIni();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	CanClose =  GetCanClose();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::CreateWaveView()
{
	if(!WaveView)
	{
		WaveView = new TWaveView(this);
		WaveView->Parent = this;
		WaveView->Align = alClient;
	}

	WaveView->ClearAll();
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

	WaveView->PopupMenu = WaveViewPopupMenu;

	ActiveControl  = WaveView;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::ApplicationEventsHint(TObject *Sender)
{
	StatusBar->Panels->Items[4]->Text =   GetLongHint(Application->Hint);
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
		Manager->SetDecoder(Reader);

		// note that below two wiil call OnLinkModified/OnLabelModified event.
		// but it's ok because the handler will write back the same data to the waveview.
		WaveView->SetLinks(Manager->GetLinks());
		WaveView->SetLabels(Manager->GetLabels());
		WaveView->PushFirstUndoState();

		WaveView->SetInitialMagnify();
		StatusBar->Panels->Items[0]->Text = WaveView->GetMagnifyString();
	}
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::OpenActionExecute(TObject *Sender)
{
	if(!GetCanClose()) return;
	OpenDialog->Filter = Reader->FilterString;
	if(OpenDialog->Execute())
	{
		FileName = OpenDialog->FileName;

		// stop playing
		StopPlay();

		// reset all
		CreateWaveView();
		FollowMarkerAction->Checked = true;

		// create manager
		if(Manager) delete Manager, Manager = NULL;
		Manager = new tTVPWaveLoopManager();
		IgnoreLinksAction->Checked = false;
		FlagsClearSpeedButtonClick(this);

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
	StatusBar->Panels->Items[0]->Text = WaveView->GetMagnifyString();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::ZoomOutActionExecute(TObject *Sender)
{
	WaveView->Magnify --;
	StatusBar->Panels->Items[0]->Text = WaveView->GetMagnifyString();
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
void __fastcall TLoopTunerMainForm::ShowEditFlagsActionExecute(
	  TObject *Sender)
{
	ShowEditFlagsAction->Checked = !ShowEditFlagsAction->Checked;
	FlagsPanel->Visible = ShowEditFlagsAction->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::IgnoreLinksActionExecute(
	  TObject *Sender)
{
	IgnoreLinksAction->Checked = !IgnoreLinksAction->Checked;
 	if(Manager) Manager->SetIgnoreLinks(IgnoreLinksAction->Checked);
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
void __fastcall TLoopTunerMainForm::GotoLinkFromActionExecute(
	  TObject *Sender)
{
	if(WaveView->FocusedLink != -1)
	{
		WaveView->CaretPos = WaveView->GetLinks()[WaveView->FocusedLink].From;
		WaveView->EnsureView(WaveView->CaretPos);
		WaveView->ShowCaret = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TLoopTunerMainForm::GotoLinkToActionExecute(
	  TObject *Sender)
{
	if(WaveView->FocusedLink != -1)
	{
		WaveView->CaretPos = WaveView->GetLinks()[WaveView->FocusedLink].To;
		WaveView->EnsureView(WaveView->CaretPos);
		WaveView->ShowCaret = true;
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

		StatusBar->Panels->Items[3]->Text = "再生中 " + Reader->SamplePosToTimeString(pos);
	}
	else
	{
		Done = true;
		WaveView->MarkerPos = -1;

		StatusBar->Panels->Items[3]->Text = "停止";
	}

	UndoAction->Enabled = WaveView->CanUndo();
	RedoAction->Enabled = WaveView->CanRedo();
	DeleteAction->Enabled = WaveView->CanDeleteItem();
	EditLinkDetailAction->Enabled = WaveView->FocusedLink != -1;
	EditLabelDetailAction->Enabled = WaveView->FocusedLabel != -1;
	GotoLinkFromAction->Enabled = WaveView->FocusedLink != -1;
	GotoLinkToAction->Enabled = WaveView->FocusedLink != -1;

	if(Manager && Manager->GetFlagsModifiedByLabelExpression()) ResetFlagsEditFromLoopManager();


	POINT mouse_pt;
	GetCursorPos(&mouse_pt);
	mouse_pt = WaveView->ScreenToClient(*(TPoint*)&mouse_pt);
	AnsiString cursor_pos, cursor_ch;
	WaveView->GetPositionStringAt(mouse_pt.x, mouse_pt.y, cursor_pos, cursor_ch);

	StatusBar->Panels->Items[1]->Text = cursor_ch;
	StatusBar->Panels->Items[2]->Text = cursor_pos;
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
void __fastcall TLoopTunerMainForm::WaveViewNotifyPopup(TObject *Sender, AnsiString type,
	const TPoint &point)
{
	if(type == "Link")
	{
		ForLinkPopupMenu->Popup(point.x, point.y);
	}
	else if(type == "Label")
	{
		ForLabelPopupMenu->Popup(point.x, point.y);
	}
	else if(type == "Wave")
	{
		ForWavePopupMenu->Popup(point.x, point.y);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewShowCaret(TObject *Sender, int pos)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLinkSelected(TObject *Sender, int num, tTVPWaveLoopLink &link)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLabelSelected(TObject *Sender, int num, tTVPWaveLabel &label)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewSelectionLost(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLinkModified(TObject *Sender)
{
	Manager->SetLinks(WaveView->GetLinks());
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::WaveViewLabelModified(TObject *Sender)
{
	Manager->SetLabels(WaveView->GetLabels());
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagsEditToggleMenuItemClick(
	  TObject *Sender)
{
	TEdit *edit = dynamic_cast<TEdit*>(ActiveControl);
	if(edit)
	{
		if(edit->Text.ToInt())
			edit->Text = "0";
		else
			edit->Text = "1";
		edit->SelStart = 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagEdit0DblClick(TObject *Sender)
{
	TEdit *edit = dynamic_cast<TEdit*>(ActiveControl);
	if(edit)
	{
		if(edit->Text.ToInt())
			edit->Text = "0";
		else
			edit->Text = "1";
		edit->SelStart = 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagsEditZeroMenuItemClick(
	  TObject *Sender)
{
	TEdit *edit = dynamic_cast<TEdit*>(ActiveControl);
	if(edit)
		edit->Text = "0";
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlasgEditOneMenuItemClick(TObject *Sender)
{
	TEdit *edit = dynamic_cast<TEdit*>(ActiveControl);
	if(edit)
		edit->Text = "1";
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagEdit0MouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TWinControl *control = dynamic_cast<TWinControl*>(Sender);
	if(control) control->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagEdit0Change(TObject *Sender)
{
	if(ResettingFlags) return;

	TEdit *edit = dynamic_cast<TEdit*>(Sender);
	if(edit)
	{
		if(edit->Text == "")
		{
			edit->Text = "0";
			edit->SelStart = 1;
		}

		if(edit->Text != "0" && edit->Text[1] == '0')
		{
			edit->Text = AnsiString(edit->Text.ToInt());
			edit->SelStart = edit->Text.Length();
		}

		if(edit->Text.ToInt() > TVP_WL_MAX_FLAG_VALUE)
			edit->Text = AnsiString(TVP_WL_MAX_FLAG_VALUE);
	}

	// refrect all controls
	if(Manager)
	{
		int count = FlagsPanel->ControlCount;
		for(int i = 0; i < count ; i++)
		{
			TControl * comp = FlagsPanel->Controls[i];
			TEdit * edit = dynamic_cast<TEdit *>(comp);
			if(edit)
			{
				if(edit->Name.AnsiPos("FlagEdit") == 1)
				{
					int num = atoi(edit->Name.c_str()  + 8);
					Manager->SetFlag(num, edit->Text.ToInt());
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagEdit0KeyPress(TObject *Sender,
	  char &Key)
{
	if(!(Key >= '0' && Key <= '9' || Key < 0x20)) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::ResetFlagsEditFromLoopManager()
{
	// display flags from waveloopmanager
	if(!Manager) return;

	ResettingFlags = true;
	tjs_int flags[TVP_WL_MAX_FLAGS];
	Manager->CopyFlags(flags);

	int count = FlagsPanel->ControlCount;
	for(int i = 0; i < count ; i++)
	{
		TControl * comp = FlagsPanel->Controls[i];
		TEdit * edit = dynamic_cast<TEdit *>(comp);
		if(edit)
		{
			if(edit->Name.AnsiPos("FlagEdit") == 1)
			{
				int num = atoi(edit->Name.c_str()  + 8);
				edit->Text = AnsiString(flags[num]);
			}
		}
	}
	ResettingFlags = false;
}
//---------------------------------------------------------------------------
void __fastcall TLoopTunerMainForm::FlagsClearSpeedButtonClick(
	  TObject *Sender)
{
	ResettingFlags = true;

	int count = FlagsPanel->ControlCount;
	for(int i = 0; i < count ; i++)
	{
		TControl * comp = FlagsPanel->Controls[i];
		TEdit * edit = dynamic_cast<TEdit *>(comp);
		if(edit)
		{
			if(edit->Name.AnsiPos("FlagEdit") == 1)
			{
				edit->Text = "0";
			}
		}
	}

	if(Manager) Manager->ClearFlags();

	ResettingFlags = false;
}
//---------------------------------------------------------------------------
































