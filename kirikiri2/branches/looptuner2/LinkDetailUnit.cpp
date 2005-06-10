//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DSound.h"
#include "LinkDetailUnit.h"
#include "LoopTunerMainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EditLinkAttribUnit"
#pragma resource "*.dfm"
#include "ColorScheme.h"


TLinkDetailForm *LinkDetailForm;
//---------------------------------------------------------------------------
__fastcall TLinkDetailForm::TLinkDetailForm(TComponent* Owner)
	: TForm(Owner)
{
	StopPlay();

	FReader = NULL;
	FMagnify = 0;
	FManager = NULL;
	LinkNum = 0;

	Dragging = false;
	BeforeOrAfter = false;
	FirstMouseDownPos = 0;
	FirstMouseDownX = 0;

	WaveAreaHasFocus = true;

	WaveAreaPanel->DoubleBuffered = true;
	PlayBeforePaintBoxPanel->DoubleBuffered = true;

	PlayBeforeLength = 0;
	PlayStartPos = 0;

	EditLinkAttribFrame->OnInfoChanged = EditLinkAttribFrameInfoChanged;
	EditLinkAttribFrame->OnEraseRedo   = EditLinkAttribFrameEraseRedo;

	BeforeLinkLabel->Font->Color = C_WAVE_B_FG_BASE;
	AfterLinkLabel ->Font->Color = C_WAVE_A_FG_BASE;

	UpdateLayout();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::FormDestroy(TObject *Sender)
{
	StopPlay();
	if(FManager) delete FManager, FManager = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	if(ModalResult == mrOk)
	{
		EditLinkAttribFrame->CheckUncommitted();
		EditLinkAttribFrame->SetLinkInfo(FLink);
		UpdateMainWindowParams(true);
	}
	else
	{
		FLink = FLinkOriginal;
		UpdateMainWindowParams(false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::OKButtonClick(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TLinkDetailForm::CancelButtonClick(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::SetReaderAndLink(TWaveReader * reader,
	tTVPWaveLoopLink link, int linknum)
{
	// set the reader and the link
	FReader = reader;
	FLink = FLinkOriginal = link;
	LinkNum = linknum;

	EditLinkAttribFrame->SetLink(FLink);

	if(FManager) delete FManager, FManager = NULL;
	FManager = new tTVPWaveLoopManager(FReader);

	// show link attributes
	SmoothAction->Checked = FLink.Smooth;
}
//---------------------------------------------------------------------------
int __fastcall TLinkDetailForm::PixelToSample(int pixel)
{
	if(FMagnify <= 0)
	{
		// shrink
		return pixel << (-FMagnify);
	}
	else
	{
		// expand
		return pixel >> FMagnify;
	}
}
//---------------------------------------------------------------------------
int __fastcall TLinkDetailForm::SampleToPixel(int sample)
{
	if(FMagnify <= 0)
	{
		// shrink
		return sample >> (-FMagnify);
	}
	else
	{
		// expand
		return sample << FMagnify;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::UpdateMainWindowParams(bool push_undo)
{
	// update main window's waveview link information
	TWaveView *waveview = ((TLoopTunerMainForm*)(Owner))->GetWaveView();

	tTVPWaveLoopLink &link = waveview->GetLinks()[LinkNum];

	waveview->InvalidateLink(LinkNum);

	link = FLink;

	waveview->NotifyLinkChanged();

	waveview->InvalidateLink(LinkNum);

	if(push_undo) waveview->PushUndo(); //==== push undo
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::UpdateDisplay()
{
	// update labels
	MagnifyLabel->Caption = "/" + AnsiString(1<<-FMagnify);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::UpdateLayout()
{
	PosAdjustToolBar->Left = (ToolBarPanel->Width - PosAdjustToolBar->Width) /2 + 1;
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::PlayLink(int before, int after)
{
	// play the link

	// stop current playing sound
	StopPlay();

	std::vector<tTVPWaveLoopLink> links;
	tTVPWaveLoopLink link = FLink;
	link.Condition = llcNone; // disable the condition
	links.push_back(link);
	FManager->SetLinks(links); // set new links to the manager

	const WAVEFORMATEXTENSIBLE * wfx;
	wfx = FReader->GetWindowsFormat();
	int before_samples = FReader->Frequency * before / 1000;
	PlayStartPos = PlayLastPos = FLink.From - before_samples < 0 ? 0 : FLink.From - before_samples;
	PlayBeforeLength = FLink.From - PlayStartPos;

	FManager->SetPosition(PlayStartPos);
	StartPlay(wfx, FManager);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::FormResize(TObject *Sender)
{
	UpdateLayout();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WavePaintBoxPaint(TObject *Sender)
{
	// draw waveform

	// check reader validity
	if(!FReader || !FReader->ReadDone)
	{
		TRect r;
		r.left = WavePaintBox->Canvas->ClipRect.left, r.top = 0,
			r.right = WavePaintBox->Canvas->ClipRect.right,
			r.bottom = WavePaintBox->Height;
		WavePaintBox->Canvas->Brush->Style = bsSolid;
		WavePaintBox->Canvas->Brush->Color = C_DISABLEED_CLIENT;
		WavePaintBox->Canvas->FillRect(r);
		return ; // reader not available
	}


	// calc dimentions
	int xstep = 1<<(-FMagnify);
	const int sstep = 1;

	int one_height = ((WavePaintBox->Height) / FReader->Channels) & ~1;
	int one_height_half = one_height >> 1;
	int one_client_height = one_height - 2; // 2 : padding margin

	int center = WavePaintBox->Width / 2; // center line

	// calc start point
	int dest_left  = WavePaintBox->Canvas->ClipRect.left;
	int dest_right = WavePaintBox->Canvas->ClipRect.right;

	int before_pos = PixelToSample(dest_left - center) + FLink.From;
	int after_pos =  PixelToSample(dest_left - center) + FLink.To;


	// clear the background
	#define		CONV_Y(v, ch) ((((v) * one_client_height) / -65536) + \
							one_height_half + (ch) * one_height)

	//- main
	if(dest_left < dest_right)
	{
		TRect r;
		r.left = dest_left, r.top = 0, r.right = dest_right, r.bottom =
			WavePaintBox->Height;
		WavePaintBox->Canvas->Brush->Style = bsSolid;
		WavePaintBox->Canvas->Brush->Color = C_CLIENT;
		WavePaintBox->Canvas->FillRect(r);
	}

	//- draw -Inf line
	WavePaintBox->Canvas->Pen->Color = C_INF_LINE;
	for(int ch = 0; ch < FReader->Channels; ch ++)
	{
		WavePaintBox->Canvas->MoveTo(dest_left,  CONV_Y(0, ch));
		WavePaintBox->Canvas->LineTo(dest_right, CONV_Y(0, ch));
	}

	// draw waveform
	for(int x = dest_left; x < dest_right; x++, before_pos += xstep, after_pos += xstep)
	{
		for(int ch = 0; ch < FReader->Channels; ch ++)
		{
			// get "before" min max
			int before_high, before_low;
			if(before_pos >= 0 && before_pos + xstep <= FReader->NumSamples)
				FReader->GetPeak(before_high, before_low, before_pos, ch, sstep);
			else
				before_high = before_low = 0;

			if(before_high > 0 && before_low > 0)
				before_low = 0;
			if(before_high < 0 && before_low < 0)
				before_high = 0;

			before_high = CONV_Y(before_high, ch);
			before_low  = CONV_Y(before_low , ch);

			// get "after" min max
			int after_high, after_low;
			if(after_pos >= 0 && after_pos + xstep <= FReader->NumSamples)
				FReader->GetPeak(after_high, after_low, after_pos, ch, sstep);
			else
				after_high = after_low = 0;

			if(after_high > 0 && after_low > 0)
				after_low = 0;
			if(after_high < 0 && after_low < 0)
				after_high = 0;

			after_high = CONV_Y(after_high, ch);
			after_low  = CONV_Y(after_low , ch);

			// detect bg and fg
			int bg_high, bg_low;
			int fg_high, fg_low;
			if(x < center)
			{
				bg_high = after_high;
				bg_low  = after_low;
				fg_high = before_high;
				fg_low  = before_low;
			}
			else
			{
				bg_high = before_high;
				bg_low  = before_low;
				fg_high = after_high;
				fg_low  = after_low;
			}

			// draw waveform
			int fg_bg_high;
			int fg_bg_low;
			if(bg_high < fg_high)
			{
				// draw bg only
				WavePaintBox->Canvas->Pen->Color = x < center ? C_WAVE_B_BG : C_WAVE_A_BG;
				WavePaintBox->Canvas->MoveTo(x, bg_high);
				WavePaintBox->Canvas->LineTo(x, fg_high);
				fg_bg_high = fg_high;

			}
			else
			{
				// draw fg only
				WavePaintBox->Canvas->Pen->Color = x < center ? C_WAVE_B_FG : C_WAVE_A_FG;
				WavePaintBox->Canvas->MoveTo(x, fg_high);
				WavePaintBox->Canvas->LineTo(x, bg_high);
				fg_bg_high = bg_high;
			}

			if(bg_low < fg_low)
			{
				// draw fg only
				WavePaintBox->Canvas->Pen->Color = x < center ? C_WAVE_B_FG : C_WAVE_A_FG;
				WavePaintBox->Canvas->MoveTo(x, bg_low);
				WavePaintBox->Canvas->LineTo(x, fg_low);
				fg_bg_low = bg_low;
			}
			else
			{
				// draw bg only
				WavePaintBox->Canvas->Pen->Color = x < center ? C_WAVE_B_BG : C_WAVE_A_BG;
				WavePaintBox->Canvas->MoveTo(x, fg_low);
				WavePaintBox->Canvas->LineTo(x, bg_low);
				fg_bg_low = fg_low;
			}

			// draw fg-bg
			WavePaintBox->Canvas->Pen->Color = x < center ? C_WAVE_B_FG_BG : C_WAVE_A_FG_BG;
			WavePaintBox->Canvas->MoveTo(x, fg_bg_high);
			WavePaintBox->Canvas->LineTo(x, fg_bg_low);
		}
	}

	// draw center split
	WavePaintBox->Canvas->Pen->Color = C_WAVE_CENTER_SPLIT;
	WavePaintBox->Canvas->MoveTo(center, 0);
	WavePaintBox->Canvas->LineTo(center, WavePaintBox->Height);

	// draw focus frame
	if(WaveAreaHasFocus)
	{
		WavePaintBox->Canvas->Pen->Color = (TColor)((~C_CLIENT)&0xffffff);
		WavePaintBox->Canvas->MoveTo(0, 0);
		WavePaintBox->Canvas->LineTo(WavePaintBox->Width-1, 0);
		WavePaintBox->Canvas->LineTo(WavePaintBox->Width-1, WavePaintBox->Height-1);
		WavePaintBox->Canvas->LineTo(0, WavePaintBox->Height-1);
		WavePaintBox->Canvas->LineTo(0, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WaveAreaPanelEnter(TObject *Sender)
{
	WaveAreaHasFocus = true;
	WaveAreaPanel->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WaveAreaPanelExit(TObject *Sender)
{
	WaveAreaHasFocus = false;
	WaveAreaPanel->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WavePaintBoxMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	WaveAreaPanel->SetFocus();
	if(Button == mbLeft)
	{
		Dragging = true;
		int center = WavePaintBox->Width / 2;
		BeforeOrAfter = X < center;
		FirstMouseDownPos =
			(BeforeOrAfter ? FLink.From : FLink.To);
		FirstMouseDownX = X;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WavePaintBoxMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
	if(Dragging)
	{
		if(BeforeOrAfter)
		{
			FLink.From = FirstMouseDownPos - PixelToSample(X - FirstMouseDownX);
			if(FLink.From < 0) FLink.From = 0;
			if(FLink.From >= FReader->NumSamples) FLink.From = FReader->NumSamples - 1;
		}
		else
		{
			FLink.To   = FirstMouseDownPos - PixelToSample(X - FirstMouseDownX);
			if(FLink.To < 0) FLink.To = 0;
			if(FLink.To >= FReader->NumSamples) FLink.To = FReader->NumSamples - 1;
		}
		WavePaintBox->Invalidate();
		UpdateMainWindowParams();
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WavePaintBoxMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Dragging)
	{
		Dragging = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::WavePaintBoxDblClick(TObject *Sender)
{
	PlayLink();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::ZoomInActionExecute(TObject *Sender)
{
	if(FMagnify < 0) FMagnify ++;
	WavePaintBox->Invalidate();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::ZoomOutActionExecute(TObject *Sender)
{
	if(FMagnify > -10) FMagnify --;
	WavePaintBox->Invalidate();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::SmoothActionExecute(TObject *Sender)
{
	SmoothAction->Checked = ! SmoothAction->Checked;
	FLink.Smooth = SmoothAction->Checked;
	UpdateMainWindowParams();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::BeforePrevCrossActionExecute(
	  TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::BeforePrevFastActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify) * 20;
	if(FLink.From > step)
		FLink.From -= step;
	else
		FLink.From = 0;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::BeforePrevStepActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify);
	if(FLink.From > step)
		FLink.From -= step;
	else
		FLink.From = 0;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::BeforeNextStepActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify);
	if((FReader->NumSamples - 1) - FLink.From > step )
		FLink.From += step;
	else
		FLink.From = FReader->NumSamples - 1;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::BeforeNextFastActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify) * 20;
	if((FReader->NumSamples - 1) - FLink.From > step )
		FLink.From += step;
	else
		FLink.From = FReader->NumSamples - 1;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::BeforeNextCrossActionExecute(
      TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::AfterPrevCrossActionExecute(
	  TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::AfterPrevFastActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify) * 20;
	if(FLink.To > step)
		FLink.To -= step;
	else
		FLink.To = 0;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::AfterPrevStepActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify);
	if(FLink.To > step)
		FLink.To -= step;
	else
		FLink.To = 0;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::AfterNextStepActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify);
	if((FReader->NumSamples - 1) - FLink.To > step )
		FLink.To += step;
	else
		FLink.To = FReader->NumSamples - 1;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::AfterNextFastActionExecute(
	  TObject *Sender)
{
	int step = (1<<-FMagnify) * 20;
	if((FReader->NumSamples - 1) - FLink.To > step )
		FLink.To += step;
	else
		FLink.To = FReader->NumSamples - 1;
	WavePaintBox->Invalidate();
	UpdateMainWindowParams();
	UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::AfterNextCrossActionExecute(
	  TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::PlayLink()
{
	int ms = 500;
	if     (PlayHalfSecToolButton->Marked)			ms = 500;
	else if(Play1SecToolButton->Marked)				ms = 1000;
	else if(Play2SecToolButton->Marked)				ms = 2000;
	else if(Play3SecToolButton->Marked)				ms = 3000;
	else if(Play5SecToolButton->Marked)				ms = 5000;
	PlayLink(ms);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::MarkPlayButton(TObject * button)
{
	PlayHalfSecToolButton->Marked = PlayHalfSecToolButton == button;
	Play1SecToolButton->Marked    = Play1SecToolButton == button;
	Play2SecToolButton->Marked    = Play2SecToolButton == button;
	Play3SecToolButton->Marked    = Play3SecToolButton == button;
	Play5SecToolButton->Marked    = Play5SecToolButton == button;
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::StopPlayActionExecute(TObject *Sender)
{
	StopPlay();
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::PlayHalfSecActionExecute(TObject *Sender)
{
	MarkPlayButton(PlayHalfSecToolButton);
	PlayLink(500);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::Play1SecActionExecute(TObject *Sender)
{
	MarkPlayButton(Play1SecToolButton);
	PlayLink(1000);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::Play2SecActionExecute(TObject *Sender)
{
	MarkPlayButton(Play2SecToolButton);
	PlayLink(2000);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::Play3SecActionExecute(TObject *Sender)
{
	MarkPlayButton(Play3SecToolButton);
	PlayLink(3000);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::Play5SecActionExecute(TObject *Sender)
{
	MarkPlayButton(Play5SecToolButton);
	PlayLink(5000);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::ApplicationEventsIdle(TObject *Sender,
	  bool &Done)
{
	int pos = (int)GetCurrentPlayingPos();

	if(pos != -1)
	{
		Sleep(1); // this will make the cpu usage low
		Done = false;

		PlayBeforePaintBox->Invalidate();
	}
	else
	{
		if(PlayBeforeLength)
		{
			PlayBeforeLength = 0;
			PlayBeforePaintBox->Invalidate();
		}
		Done = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::PlayBeforePaintBoxPaint(TObject *Sender)
{
	if(PlayBeforeLength)
	{
		int current_pos = GetCurrentPlayingPos();
		int elapsed = current_pos - PlayStartPos;
		if(elapsed < 0 || elapsed >= PlayBeforeLength || current_pos < PlayLastPos)
		{
			PlayBeforeLength = 0;
		}

		if(PlayBeforeLength == 0)
		{
			PlayBeforeLabel->Visible = false;
			return; // do nothing
		}

		PlayLastPos = current_pos;

		TRect r;
		r.left = 0;
		r.top = 0;
		r.right = PlayBeforePaintBox->Width *  elapsed / PlayBeforeLength;
		r.bottom = PlayBeforePaintBox->Height;
		PlayBeforePaintBox->Canvas->Brush->Color  = clGreen;
		PlayBeforePaintBox->Canvas->FillRect(r);

		r.left = r.right;
		r.right = PlayBeforePaintBox->Width;
		PlayBeforePaintBox->Canvas->Brush->Color  = clYellow;
		PlayBeforePaintBox->Canvas->FillRect(r);

		AnsiString str;
		str.sprintf(PlayBeforeLabel->Hint.c_str(),
			(double)(PlayBeforeLength - elapsed) / FReader->Frequency);
		PlayBeforeLabel->Caption = str;
		PlayBeforeLabel->Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::EditLinkAttribFrameInfoChanged(TObject * Sender)
{
	EditLinkAttribFrame->SetLinkInfo(FLink);
	UpdateMainWindowParams(false);
}
//---------------------------------------------------------------------------
void __fastcall TLinkDetailForm::EditLinkAttribFrameEraseRedo(TObject * Sender)
{
	// do nothing
}
//---------------------------------------------------------------------------






void __fastcall TLinkDetailForm::ZoomInMenuItemClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

