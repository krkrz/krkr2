//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <inifiles.hpp>
#include <mbstring.h>
#include "LoopTuneMainUnit.h"
#include "DSoundUnit.h"
#include "WaveWriterUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTSSLoopTunerMainWindow *TSSLoopTunerMainWindow;

//---------------------------------------------------------------------------
AnsiString __fastcall GetLongFileName(AnsiString fn)
{
	// fn をロングファイル名に変換して返す
	char buf[MAX_PATH];
	char *fnp=0;
	GetFullPathName(fn.c_str(),MAX_PATH,buf,&fnp);
	return AnsiString(buf);
}

//---------------------------------------------------------------------------
__fastcall TTSSLoopTunerMainWindow::TTSSLoopTunerMainWindow(TComponent* Owner)
	: TForm(Owner)
{
	Modified = false;

	InitDirectSound(Handle);

	WaveReader = new TWaveReader();
	WaveReader->OnReadProgress = WaveReaderProgress;
	MainDisplayer = new TWaveDisplayer(Panel);
	MainDisplayer->Left = 0;
	MainDisplayer->Top = 90;
	MainDisplayer->Width = 400;
	MainDisplayer->Height = 100;
	MainDisplayer->Parent = Panel;
	MainDisplayer->Visible = true;
	MainDisplayer->Align = alClient;
	MainDisplayer->PopupMenu = MainDisplayerPopupMenu;
	MainDisplayer->OnMouseDown = MainDisplayerMouseDown;
	MainDisplayer->OnMouseMove = MainDisplayerMouseMove;
	MainDisplayer->OnFollowCanceled = MainDisplayerFollowCanceled;
	MainDisplayer->OnEndSelect = MainDisplayerEndSelect;
	MainDisplayer->OnSelectionChanged = MainDisplayerSelectionChanged;
	MainDisplayer->OnDblClick = MainDisplayerDblClick;
	MainDisplayer->OnMouseLeave = MainDiaplayerMouseLeave;
	Application->OnHint = DisplayHint;
	Application->OnIdle = ApplicationIdle;

	LoadSettings();

	Clear();
	DisableAllAction();
	SetStatusBar();

	int i;
	int paramcount = ParamCount();
	for(i=1; i<=paramcount; i++)
	{
		if(FileExists(ParamStr(i)))
		{
			OpenDialog->FileName = GetLongFileName(ParamStr(i));
			Open();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::FormDestroy(TObject *Sender)
{
	FreeDirectSound();
	delete WaveReader;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::LoadSettings(void)
{
	TMemIniFile *ini;
	try
	{
		ini = new TMemIniFile(ChangeFileExt(ParamStr(0), ".ini"));
	}
	catch(...)
	{
		return;
	}

	Left = ini->ReadInteger("Window", "Left", Left);
	Top = ini->ReadInteger("Window", "Top", Top);
	if(Left > Screen->Width - 32) Left = Screen->Width - 32;
	if(Top > Screen->Height - 32) Top = Screen->Height - 32;
	Width = ini->ReadInteger("Window", "Width", Width);
	Height = ini->ReadInteger("Window", "Height", Height);

	OpenDialog->InitialDir = ini->ReadString("Folder", "InitialDir", "");
	SaveDialog->InitialDir = ini->ReadString("Folder", "ExportInitialDir", "");

	bool b;
	b = ini->ReadBool("Visibility", "ToolBar", true);
	ToolBar->Visible = b;
	ShowToolBarAction->Checked = b;
	b = ini->ReadBool("Visibility", "StatusBar", true);
	StatusBar->Visible = b;
	ShowStatusBarAction->Checked = b;

	b = ini->ReadBool("Appearance", "TimeBase", true);
	ShowTimeBaseAction->Checked = b;
	ShowSampleBaseAction->Checked = !b;

	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::SaveSettings(void)
{
	TMemIniFile *ini;
	try
	{
		ini = new TMemIniFile(ChangeFileExt(ParamStr(0), ".ini"));
	}
	catch(...)
	{
		return;
	}

	ini->WriteInteger("Window", "Left", Left);
	ini->WriteInteger("Window", "Top", Top);
	ini->WriteInteger("Window", "Width", Width);
	ini->WriteInteger("Window", "Height", Height);
	if(OpenDialog->FileName != "")
	{
		ini->WriteString("Folder", "InitialDir",ExtractFilePath(
			OpenDialog->FileName));
	}
	if(SaveDialog->FileName != "")
	{
		ini->WriteString("Folder", "ExportInitialDir", ExtractFilePath(
			SaveDialog->FileName));
	}
	ini->WriteBool("Visibility", "ToolBar", ShowToolBarAction->Checked);
	ini->WriteBool("Visibility", "StatusBar", ShowStatusBarAction->Checked);
	ini->WriteBool("Appearance", "TimeBase", ShowTimeBaseAction->Checked);

	try
	{
		ini->UpdateFile();
	}
	catch(...)
	{
		delete ini;
		return;
	}
	delete ini;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TTSSLoopTunerMainWindow::GetTimeString(int samplepos)
{
	if(!WaveReader->ReadDone) return "-";

	if(samplepos == -1) return "-";

	if(ShowTimeBaseAction->Checked)
	{
		DWORD time = WaveReader->SamplePosToTime(samplepos);
		int m = time/(1000L*60L);
		int s = (time/1000)%60;
		int ms = time%1000;
		return AnsiString().sprintf("%d:%02d.%03d", m, s, ms);
	}
	else
	{
		return CurrToStrF(samplepos, ffNumber, 0);
	}
}
//---------------------------------------------------------------------------
bool __fastcall TTSSLoopTunerMainWindow::Clear()
{
	if(Modified)
	{
		int r = MessageDlg(CurrentFileName +"\nはループ領域が変更されています。\n"
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

	Modified = false;
	StatusBar->Panels->Items[3]->Text = "停止";
	MainDisplayer->Marker = -1;
	Timer->Enabled = false;
	UncheckAllPlayAction();
	StopAction->Checked = true;
	StopPlay();
	ClearUndo();
	DisableAllAction();
	MainDisplayer->ScrollTo(0);
	MainDisplayer->SelLength = 0;
	WaveReader->Clear();
	MainDisplayer->WaveReader = NULL;
	SetStatusBar();

	Caption = "Loop Tuner";

	return true;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::ClearUndo(void)
{
	UndoPtr = RedoPtr = 0;
}
//---------------------------------------------------------------------------
bool __fastcall TTSSLoopTunerMainWindow::CanUndo(void)
{
	return UndoPtr >= 2;
}
//---------------------------------------------------------------------------
bool __fastcall TTSSLoopTunerMainWindow::CanRedo(void)
{
	return RedoPtr != UndoPtr;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::Undo(void)
{
	if(UndoPtr >= 2)
	{
		UndoPtr--;
		MainDisplayer->SelStart = UndoSelStart[UndoPtr];
		MainDisplayer->SelLength = UndoSelLength[UndoPtr];
		SetNewSelection();
	}
	UndoAction->Enabled = CanUndo();
	RedoAction->Enabled = CanRedo();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::Redo(void)
{
	if(UndoPtr != RedoPtr)
	{
		UndoPtr++;
		MainDisplayer->SelStart = UndoSelStart[UndoPtr];
		MainDisplayer->SelLength = UndoSelLength[UndoPtr];
		SetNewSelection();
	}
	UndoAction->Enabled = CanUndo();
	RedoAction->Enabled = CanRedo();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::PutUndo(void)
{
	if(UndoPtr == MAX_UNDO -1)
	{
		memmove(UndoSelStart , UndoSelStart +1, sizeof(UndoSelStart[0]) * (MAX_UNDO-1));
		memmove(UndoSelLength , UndoSelLength +1, sizeof(UndoSelLength[0]) * (MAX_UNDO-1));
		UndoPtr--;
	}

	UndoPtr++;
	UndoSelStart[UndoPtr] = MainDisplayer->SelStart;
	UndoSelLength[UndoPtr] = MainDisplayer->SelLength;
	RedoPtr = UndoPtr;
	Modified = true;

	UndoAction->Enabled = CanUndo();
	RedoAction->Enabled = CanRedo();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::DisplayHint(TObject *Sender)
{
	StatusBar->Panels->Items[5]->Text = GetLongHint(Application->Hint);
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::SetStatusBar(void)
{
	if(WaveReader->ReadDone)
		StatusBar->Panels->Items[0]->Text = "1/"+AnsiString(MainDisplayer->Magnify);
	else
		StatusBar->Panels->Items[0]->Text = "";

	if(WaveReader->ReadDone)
	{
		if(MainDisplayer->SelLength)
		{
			StatusBar->Panels->Items[4]->Text =
				GetTimeString(MainDisplayer->SelStart) +
				" ～ " +
				GetTimeString(MainDisplayer->SelStart +MainDisplayer->SelLength) +
				" ( "+
				GetTimeString(MainDisplayer->SelLength) + " )";
		}
		else
		{
			StatusBar->Panels->Items[4]->Text = "ループ領域は選択されていません";
		}
	}
	else
	{
		StatusBar->Panels->Items[4]->Text = "-";
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerMouseDown(TObject *Sender, TMouseButton button,
		TShiftState shift, int x, int y)
{
	if(!WaveReader->ReadDone) return;

	if(button == mbRight)
	{
		MainDisplayer->FollowMarker = false;
		FollowCurrentAction->Checked = false;
		MainDisplayerMouseDownPos = x;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerMouseMove(TObject *Sender, TShiftState Shift,
		int X, int Y)
{
	if(!WaveReader || !WaveReader->ReadDone) return;

	SetStatusBar();

	POINT pt;
	GetCursorPos(&pt);
	TPoint tpt;
	tpt = pt;
	tpt = MainDisplayer->ScreenToClient(tpt);
	int ch = MainDisplayer->ChannelFromMousePos(tpt.y);
	if(ch == -1 || tpt.x < 0 || tpt.y < 0 ||
		tpt.x >= MainDisplayer->Width || tpt.y >= MainDisplayer->Height)
	{
		StatusBar->Panels->Items[2]->Text = "-";
		StatusBar->Panels->Items[1]->Text = "-";
		return ;
	}

	if(WaveReader->ReadDone)
		StatusBar->Panels->Items[2]->Text = GetTimeString(MainDisplayer->MousePosToSamplePos(tpt.x));
	else
		StatusBar->Panels->Items[2]->Text = "-";

	if(ch == -1)
	{
		StatusBar->Panels->Items[1]->Text = "-";
	}
	else
	{
		StatusBar->Panels->Items[1]->Text = WaveReader->GetChannelLabel(ch);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDiaplayerMouseLeave(TObject *Sender)
{
	StatusBar->Panels->Items[2]->Text = "-";
	StatusBar->Panels->Items[1]->Text = "-";
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerDblClick(TObject *Sender)
{
	// ダブルクリック位置から再生開始
	if(!WaveReader->ReadDone) return;

	int pos = MainDisplayer->LastClickedMousePosToSamplePos();
	if(pos!=-1) Play(pos);
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerFollowCanceled(TObject *Sender)
{
	FollowCurrentAction->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::UncheckAllPlayAction(void)
{
	PlayNoLoopFromFirstAction->Checked = false;
	PlayLoopFromFirstAction->Checked = false;
	PlayLoopRegionAction->Checked = false;

	StopAction->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::DisableAllAction(void)
{
	SaveAction->Enabled = false;
	CloseAction->Enabled = false;
	PlayNoLoopFromFirstAction->Enabled = false;
	PlayLoopFromFirstAction->Enabled = false;
	FreeLoopAction->Enabled = false;
	PlayLoopRegionAction->Enabled = false;
	StopAction->Enabled = false;
	GotoFirstAction->Enabled = false;
	GotoLastAction->Enabled = false;
	StepFirstPrevAction->Enabled = false;
	StepFirstNextAction->Enabled = false;
	StepLastPrevAction->Enabled = false;
	StepLastNextAction->Enabled = false;
	SetSelectionAction->Enabled = false;
	FollowCurrentAction->Enabled = false;
	ZoomInAction->Enabled = false;
	ZoomOutAction->Enabled = false;
	UndoAction->Enabled = false;
	RedoAction->Enabled = false;
	ExportPreLoopAction->Enabled = false;
	ExportLoopAction->Enabled = false;
	ExportAllAction->Enabled = false;

	StopAction->Checked = true;
	FollowCurrentAction->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::EnableAllAction(void)
{
	SaveAction->Enabled = true;
	CloseAction->Enabled = true;
	PlayNoLoopFromFirstAction->Enabled = true;
//	PlayLoopFromFirstAction->Enabled = true;
//	PlayLoopRegionAction->Enabled = true;
	StopAction->Enabled = true;
//	GotoFirstAction->Enabled = true;
//	GotoLastAction->Enabled = true;
//	StepFirstPrevAction->Enabled = true;
//	StepFirstNextAction->Enabled = true;
//	StepLastPrevAction->Enabled = true;
//	StepLastNextAction->Enabled = true;
	SetSelectionAction->Enabled = true;
	FollowCurrentAction->Enabled = true;
	ZoomInAction->Enabled = true;
	ZoomOutAction->Enabled = true;
	ExportAllAction->Enabled = true;

	StopAction->Checked = true;
	FollowCurrentAction->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::WaveReaderProgress(TObject *Sender)
{
	if(WaveReader->ReadDone)
	{
		MainDisplayer->WaveReader = WaveReader;
		int m =  WaveReader->NumSamples / MainDisplayer->ClientWidth;
		int n=0;
		while(m) m>>=1, n++;
		if(n > 13) n = 13;

		MainDisplayer->Magnify = 1<<n;
		MainDisplayer->SelStart = OpeningLoopStart;
		MainDisplayer->SelLength = OpeningLoopLength;
		MainDisplayer->FollowMarker = true;
		MainDisplayerSelectionChanged(this);
		EnableAllAction();
		Caption = ExtractFileName(CurrentFileName);
		PutUndo();
		Modified = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::LoadLoopInformation(AnsiString filename)
{
	filename = filename + ".sli";

	OpeningLoopStart = 0;
	OpeningLoopLength = 0;

	if(!FileExists(filename)) return;

	TStringList *list = new TStringList();
	try
	{
		list->LoadFromFile(filename);
	}
	catch(...)
	{
		delete list;
		return;
	}

	int i;
	for(i=0; i<list->Count; i++)
	{
		AnsiString name = list->Names[i];
		if(name == "LoopStart")
		{
			AnsiString value = list->Values[name];
			OpeningLoopStart = atoi(value.c_str());
		}
		else if(name == "LoopLength")
		{
			AnsiString value = list->Values[name];
			OpeningLoopLength = atoi(value.c_str());
		}
	}

	delete list;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::Open(void)
{
	if(OpenDialog->FileName.Length() >= 4)
	{
		if(!_mbsicmp(OpenDialog->FileName.c_str() +
			(OpenDialog->FileName.Length() - 4), ".sli"))
		{
			AnsiString n = OpenDialog->FileName;
			n.SetLength(n.Length() - 4);
			OpenDialog->FileName = n;
		}
	}

	LoadLoopInformation(OpenDialog->FileName);
	WaveReader->LoadWave(OpenDialog->FileName);
	CurrentFileName = OpenDialog->FileName;
	SetStatusBar();

	Caption = "読み込み中 ... "+OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::OpenActionExecute(TObject *Sender)
{
	if(OpenDialog->Filter == "")
		OpenDialog->Filter = WaveReader->FilterString;


	if(!Clear()) return;

	if(OpenDialog->Execute())
	{
		Open();
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::WMDoOpen(TMessage &msg)
{
	if(Clear()) Open();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::WMDropFiles(TMessage &msg)
{
	HDROP hd=(HDROP)msg.WParam;

	char FileName[ MAX_PATH ];
	int FileCount=
		DragQueryFile(hd,0xFFFFFFFF,NULL,MAX_PATH);
			//いくつ落とされたか

	for(int i=FileCount-1;i>=0;i--)
	{
		DragQueryFile(hd, i, FileName, MAX_PATH );
		WIN32_FIND_DATA fd;
		HANDLE h;
		if((h = FindFirstFile(FileName,&fd))!=INVALID_HANDLE_VALUE)
		{
			FindClose(h);

			DragFinish(hd);

			OpenDialog->FileName = GetLongFileName(FileName);
			if(!Clear()) return;
			Open();

			return;
		}

	}
	DragFinish(hd);
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::WMCopyData(TWMCopyData &msg)
{
/*
	unsigned Msg;
	HWND From;
	tagCOPYDATASTRUCT *CopyDataStruct;
	int Result;
*/
	if(msg.CopyDataStruct->dwData == 0x746f8ab2)
	{
		if(msg.CopyDataStruct->lpData && msg.CopyDataStruct->cbData >= 6 &&
			!memcmp((const char*)msg.CopyDataStruct->lpData, "open:", 5))
		{
			// open action
			const char *filename = (const char*)(msg.CopyDataStruct->lpData) + 5;

			OpenDialog->FileName = GetLongFileName(filename);
			::PostMessage(Handle, WM_DOOPEN, 0, 0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::ZoomInActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->Magnify != 1)
	{
		MainDisplayer->Magnify = MainDisplayer->Magnify >> 1;
		SetStatusBar();
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::ZoomOutActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->Magnify != 8192)
	{
		MainDisplayer->Magnify = MainDisplayer->Magnify << 1;
		SetStatusBar();
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::PlayNoLoopFromFirstActionExecute(
	  TObject *Sender)
{
	if(!WaveReader->ReadDone) return;

	UncheckAllPlayAction();
	PlayNoLoopFromFirstAction->Checked = true;
	PlayingStartOfs = 0;
	StartPlay(WaveReader->GetFormat(), GetNoLoopFromFirstData);
	MainDisplayer->SetWaitingFollowing();
	Timer->Enabled = true;
}
//---------------------------------------------------------------------------
DWORD __fastcall TTSSLoopTunerMainWindow::GetNoLoopFromFirstData(__int16 *buf, DWORD ofs, DWORD num)
{
	if(!WaveReader->ReadDone) return 0;
	return WaveReader->GetData(buf, ofs + PlayingStartOfs, num);
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerSelectionChanged(TObject *Sender)
{
	bool b = MainDisplayer->SelLength;
	PlayLoopFromFirstAction->Enabled = b;
	PlayLoopRegionAction->Enabled = b;
	GotoFirstAction->Enabled = b;
	GotoLastAction->Enabled = b;
	StepFirstPrevAction->Enabled = b;
	StepFirstNextAction->Enabled = b;
	StepLastPrevAction->Enabled = b;
	StepLastNextAction->Enabled = b;
	ExportPreLoopAction->Enabled = b;
	ExportLoopAction->Enabled = b;
	FreeLoopAction->Enabled = b;
	SetStatusBar();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::SetNewSelection(void)
{
	if(!WaveReader->ReadDone) return;

	MainDisplayerSelectionChanged(this);
	if(PlayLoopFromFirstAction->Checked || PlayLoopRegionAction->Checked)
	{
		if(MainDisplayer->SelLength != 0)
		{
			StopPlay();
			int ofs = MainDisplayer->SelLength + MainDisplayer->SelStart;
			ofs -= WaveReader->Frequency * 2;
			if(ofs<MainDisplayer->SelStart) ofs=MainDisplayer->SelStart;
			PlayLoop(ofs);
		}
		else
		{
			StopAction->Execute();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerEndSelect(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;

	SetNewSelection();
	PutUndo();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::PlayLoop(int ofs)
{
	if(!WaveReader->ReadDone) return;

	if(MainDisplayer->SelLength != 0)
	{
		PlayingStartOfs = ofs;
		PlayingSelStart = MainDisplayer->SelStart;
		PlayingSelLength = MainDisplayer->SelLength;
		StartPlay(WaveReader->GetFormat(), GetLoopFromFirstData);
		MainDisplayer->SetWaitingFollowing();
		Timer->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::Play(int pos)
{
	if(!WaveReader->ReadDone) return;

	if(pos!=-1)
	{
		if(MainDisplayer->SelLength &&
			pos < MainDisplayer->SelStart + MainDisplayer->SelLength)
		{
			UncheckAllPlayAction();
			PlayLoopFromFirstAction->Checked = true;
			PlayLoop(pos);
		}
		else
		{
			UncheckAllPlayAction();
			PlayNoLoopFromFirstAction->Checked = true;
			PlayingStartOfs = pos;
			StartPlay(WaveReader->GetFormat(), GetNoLoopFromFirstData);
			MainDisplayer->SetWaitingFollowing();
		}
		Timer->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::PlayLoopFromFirstActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;

	if(MainDisplayer->SelLength != 0)
	{
		UncheckAllPlayAction();
		PlayLoopFromFirstAction->Checked = true;
		PlayLoop(0);
	}
}
//---------------------------------------------------------------------------
DWORD __fastcall TTSSLoopTunerMainWindow::GetLoopFromFirstData(__int16 *buf, DWORD ofs, DWORD num)
{
	if(!WaveReader->ReadDone) return 0;

	ofs += PlayingStartOfs;

	DWORD lim = PlayingSelStart + PlayingSelLength;

	if(ofs >= lim)
	{
		ofs -= PlayingSelStart;
		ofs = ofs % PlayingSelLength;
		ofs += PlayingSelStart;
	}

	int wp = 0;
	while(num)
	{
		int size = num < (lim-ofs) ? num : (lim-ofs);
		int wr = WaveReader->GetData(buf + wp*WaveReader->Channels, ofs, size);
//		if(wr <= 0) break;
		num -= wr;
		ofs += wr;
		wp += wr;
		if(ofs >= lim) ofs -= PlayingSelLength;
	}

	return wp;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::PlayLoopRegionActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;

	if(MainDisplayer->SelLength != 0)
	{
		UncheckAllPlayAction();
		PlayLoopRegionAction->Checked = true;
		int ofs = MainDisplayer->SelLength + MainDisplayer->SelStart;
		ofs -= WaveReader->Frequency * 2;
		if(ofs<MainDisplayer->SelStart) ofs=MainDisplayer->SelStart;
		PlayLoop(ofs);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::TimerTimer(TObject *Sender)
{
	if(WaveReader->ReadDone)
	{
		MainDisplayerMouseMove(this, TShiftState(), 0,0 );
		if(PlayNoLoopFromFirstAction->Checked)
		{
			int pos = GetCurrentPlayingPos() + PlayingStartOfs;
			if(!GetPlaying() || pos >= WaveReader->NumSamples)
			{
				UncheckAllPlayAction();
				StopAction->Checked=true;
				MainDisplayer->Marker = -1;
			}
			else
			{
				MainDisplayer->Marker = pos;

				StatusBar->Panels->Items[3]->Text = "再生中 "+GetTimeString(pos);
			}
			return;
		}
		else if(PlayLoopFromFirstAction->Checked || PlayLoopRegionAction->Checked)
		{
			if(!GetPlaying())
			{
				UncheckAllPlayAction();
				StopAction->Checked=true;
				MainDisplayer->Marker = -1;
			}
			else
			{
				DWORD lim = PlayingSelStart + PlayingSelLength;
				DWORD ofs = GetCurrentPlayingPos();

				ofs += PlayingStartOfs;

				if(ofs >= lim)
				{
					if(PlayLoopFromFirstAction->Checked)
					{
						UncheckAllPlayAction();
						PlayLoopRegionAction->Checked = true;
					}

					ofs -= PlayingSelStart;
					ofs = ofs % PlayingSelLength;
					ofs += PlayingSelStart;
//					MainDisplayer->SetWaitingFollowing();
				}

				MainDisplayer->Marker = ofs;

				StatusBar->Panels->Items[3]->Text = "再生中 "+GetTimeString(ofs);
			}
			return;
		}
	}

	StatusBar->Panels->Items[3]->Text = "停止";
	MainDisplayer->Marker = -1;
	Timer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::ApplicationIdle(TObject *Sender, bool &Done)
{
	if(StopAction->Checked || !WaveReader->ReadDone)
	{
		Done = true;
	}
	else
	{
		TimerTimer(this);
		Sleep(1);
		Done = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::FollowCurrentActionExecute(TObject *Sender)
{
	FollowCurrentAction->Checked = ! FollowCurrentAction->Checked;
	MainDisplayer->FollowMarker = FollowCurrentAction->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::StopActionExecute(TObject *Sender)
{
	StatusBar->Panels->Items[3]->Text = "停止";
	MainDisplayer->Marker = -1;
	Timer->Enabled = false;
	UncheckAllPlayAction();
	StopAction->Checked = true;
	StopPlay();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::GotoFirstActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->SelLength != 0)
	{
		MainDisplayer->FollowMarker = false;
		FollowCurrentAction->Checked = false;
		MainDisplayer->ScrollTo(MainDisplayer->SelStart);
	}
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::GotoLastActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->SelLength != 0)
	{
		MainDisplayer->FollowMarker = false;
		FollowCurrentAction->Checked = false;
		MainDisplayer->ScrollTo(MainDisplayer->SelStart+MainDisplayer->SelLength);
	}

}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::StepFirstPrevActionExecute(
	  TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->SelLength != 0 && MainDisplayer->SelStart > 0)
	{
		MainDisplayer->SetSelection(
			MainDisplayer->SelStart - 1,
			MainDisplayer->SelStart + MainDisplayer->SelLength
			);
	}
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::StepFirstNextActionExecute(
	  TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->SelLength > 2)
	{
		MainDisplayer->SetSelection(
			MainDisplayer->SelStart + 1,
			MainDisplayer->SelStart + MainDisplayer->SelLength
			);
	}
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::StepLastPrevActionExecute(
	  TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->SelStart + MainDisplayer->SelLength < WaveReader->NumSamples)
	{
		MainDisplayer->SetSelection(
			MainDisplayer->SelStart,
			MainDisplayer->SelStart + MainDisplayer->SelLength - 1
			);
	}
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::StepLastNextActionExecute(
	  TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	if(MainDisplayer->SelLength > 2)
	{
		MainDisplayer->SetSelection(
			MainDisplayer->SelStart,
			MainDisplayer->SelStart + MainDisplayer->SelLength + 1
			);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::MainDisplayerPopupMenuPopup(TObject *Sender)
{
	bool flag;
	if(WaveReader->ReadDone)
	{
		int pos = MainDisplayer->MousePosToSamplePos(MainDisplayerMouseDownPos);
		if(pos>=0 && pos<=WaveReader->NumSamples) flag=true; else flag=false;
	}
	else
	{
		flag = false;
	}

	PlayFromCurrentAction->Enabled = flag;
	SetRegionStartAction->Enabled = flag;
	SetRegionEndAction->Enabled = flag;
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::PlayFromCurrentActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	Play(MainDisplayer->MousePosToSamplePos(MainDisplayerMouseDownPos));
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::SetRegionStartActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	int pos = MainDisplayer->MousePosToSamplePos(MainDisplayerMouseDownPos);
	if(MainDisplayer->SelLength == 0 ||
		MainDisplayer->SelStart + MainDisplayer->SelLength <= pos)
	{
		MainDisplayer->SelStart = pos;
		MainDisplayer->SelLength = WaveReader->NumSamples - pos;
	}
	else
	{
		int end = MainDisplayer->SelStart + MainDisplayer->SelLength;
		MainDisplayer->SelStart = pos;
		MainDisplayer->SelLength = end-pos;
	}
	SetNewSelection();
	PutUndo();
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::SetRegionEndActionExecute(TObject *Sender)
{
	if(!WaveReader->ReadDone) return;
	int pos = MainDisplayer->MousePosToSamplePos(MainDisplayerMouseDownPos);
	if(MainDisplayer->SelLength == 0 ||
		MainDisplayer->SelStart > pos)
	{
		MainDisplayer->SelStart = 0;
		MainDisplayer->SelLength = pos;
	}
	else
	{
		int start = MainDisplayer->SelStart;
		MainDisplayer->SelStart = start;
		MainDisplayer->SelLength = pos-start;
	}
	SetNewSelection();
	PutUndo();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::SaveActionExecute(TObject *Sender)
{
	// ループ情報を保存

	AnsiString filename;

	filename = CurrentFileName +  ".sli";

	TStringList *list = new TStringList;
	list->Add("LoopStart=" + AnsiString((int)(MainDisplayer->SelLength ? MainDisplayer->SelStart : 0)));
	list->Add("LoopLength=" + AnsiString((int)(MainDisplayer->SelLength)));

	try
	{
		list->SaveToFile(filename);

		Modified = false;
	}
	__finally
	{
		delete list;
	}

}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ShowTimeBaseActionExecute(TObject *Sender)
{
	ShowSampleBaseAction->Checked = false;
	ShowTimeBaseAction->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ShowSampleBaseActionExecute(TObject *Sender)
{
	ShowSampleBaseAction->Checked = true;
	ShowTimeBaseAction->Checked = false;
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::FreeLoopActionExecute(TObject *Sender)
{
	if(WaveReader->ReadDone)
	{
		MainDisplayer->SelStart = 0;
		MainDisplayer->SelLength = 0;
		MainDisplayerEndSelect(this);
		SetStatusBar();
	}
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ShowToolBarActionExecute(TObject *Sender)
{
	ShowToolBarAction->Checked = !ShowToolBarAction->Checked;
	ToolBar->Visible = ShowToolBarAction->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ShowStatusBarActionExecute(TObject *Sender)
{
	ShowStatusBarAction->Checked = !ShowStatusBarAction->Checked;
	StatusBar->Visible = ShowStatusBarAction->Checked;

}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::UndoActionExecute(TObject *Sender)
{
	Undo();	
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::RedoActionExecute(TObject *Sender)
{
	Redo();
}
//---------------------------------------------------------------------------


void __fastcall TTSSLoopTunerMainWindow::CloseActionExecute(TObject *Sender)
{
	Clear();
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	CanClose = Clear();
	if(CanClose) SaveSettings();
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ExitActionExecute(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::WMShowFront(TMessage &msg)
{
	Application->Restore();
	Application->BringToFront();
	this->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::FormShow(TObject *Sender)
{
	DragAcceptFiles(Handle,true);
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::ExportWave(int start, int length)
{
	if(SaveDialog->Execute())
	{
		TWaveWriterForm * form = new TWaveWriterForm(this);
		form->WaveReader = WaveReader;
		form->Start = start;
		form->Length = length;
		form->FileName = SaveDialog->FileName;
		form->ShowModal();
	}

	if(SaveDialog->FileName != "")
	{
		SaveDialog->InitialDir = ExtractFileDir(SaveDialog->FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTSSLoopTunerMainWindow::ExportAllActionExecute(
	  TObject *Sender)
{
	SaveDialog->FileName =
		ChangeFileExt(ExtractFileName(CurrentFileName), ".wav");
	ExportWave(0, WaveReader->NumSamples);
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ExportPreLoopActionExecute(
	  TObject *Sender)
{
	SaveDialog->FileName =
		ChangeFileExt(ExtractFileName(CurrentFileName), "")+"_pre.wav";

	ExportWave(0, MainDisplayer->SelStart);
}
//---------------------------------------------------------------------------

void __fastcall TTSSLoopTunerMainWindow::ExportLoopActionExecute(
	  TObject *Sender)
{
	SaveDialog->FileName =
		ChangeFileExt(ExtractFileName(CurrentFileName), "")+"_loop.wav";

	ExportWave(MainDisplayer->SelStart, MainDisplayer->SelLength);
}
//---------------------------------------------------------------------------


void __fastcall TTSSLoopTunerMainWindow::ShowHelpActionExecute(
      TObject *Sender)
{
	ShellExecute(NULL,"open", (ExtractFilePath(ParamStr(0)) + "Help\\index.html").c_str(),
		NULL,"",SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------


void __fastcall TTSSLoopTunerMainWindow::VersionInfoActionExecute(
      TObject *Sender)
{
static VS_FIXEDFILEINFO *FixedFileInfo;
static BYTE VersionInfo[16384];
static AnsiString VersionString;
long VersionNumber;

	static bool DoGet=true;
	if(DoGet)
	{
		DoGet=false;

		UINT dum;

		char krdevui_path[MAX_PATH+1];
		GetModuleFileName(GetModuleHandle("krdevui.dll"), krdevui_path, MAX_PATH);

		GetFileVersionInfo(krdevui_path,0,16384,(void*)VersionInfo);
		VerQueryValue((void*)VersionInfo,"\\",(void**)(&FixedFileInfo),
			&dum);

		char *buf;
		VerQueryValue((void*)VersionInfo,
			"\\StringFileInfo\\041103A4\\FileVersion",
			(void**)&buf,&dum);
		VersionString=buf;
	}

	MessageDlg("Loop Tuner   version " + VersionString +
		"  (C) 2000-2004 W.Dee All rights reserved.\n"
		"各プラグインのバージョン情報は plugin フォルダの中の"
		"各 DLL のプロパティを参照してください",
			mtInformation, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
static int _UIExecLoopTuner(void)
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
			int paramcount = ParamCount();
			for(i=1; i<=paramcount; i++)
			{
				if(FileExists(ParamStr(i)))
				{
					AnsiString data = "open:" + AnsiString(ParamStr(i));
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
//		Application->Initialize();
		TTSSLoopTunerMainWindow *win= new TTSSLoopTunerMainWindow(Application);
//		Application->CreateForm(__classid(TTSSLoopTunerMainWindow), &TSSLoopTunerMainWindow);
//		Application->Run();
		win->ShowModal();
		delete win;
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
extern "C" int _export PASCAL UIExecLoopTuner(void)
{
	_UIExecLoopTuner();
	ExitProcess(0);
	return 0;
}
//---------------------------------------------------------------------------

