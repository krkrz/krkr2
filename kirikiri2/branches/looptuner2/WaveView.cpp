//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <math.h>
#include "WaveView.h"
#include "WaveReader.h"


//---------------------------------------------------------------------------
// Color schemes - currently fixed colors only
//---------------------------------------------------------------------------
const TColor C_DISABLEED_CLIENT	= clBtnFace;
const TColor C_CLIENT			= clWhite;
const TColor C_WAVE				= clBlack;
const TColor C_WAVE2			= clGray;
const TColor C_INF_LINE			= clGray;
const TColor C_DISABLED_TIME_CLIENT		= ((TColor)0xf0f0f0);
const TColor C_TIME_CLIENT		= clBtnFace;
const TColor C_TIME_COLOR		= clBlack;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TWaveDrawer::TWaveDrawer(Classes::TComponent* AOwner) :
	TGraphicControl(AOwner)
{
	FReader = NULL;
	FMagnify = -12;
	FStart = 0;
	FMinRulerMajorWidth = 0;
	FMinRulerMajorHeight = 0;
	FRulerUnit = 0;
	FDrawRuler = true;
	Canvas->Font->Height = -12;
}
//---------------------------------------------------------------------------
__fastcall TWaveDrawer::~TWaveDrawer()
{
}
//---------------------------------------------------------------------------
int __fastcall TWaveDrawer::PixelToSample(int pixel)
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
int __fastcall TWaveDrawer::SampleToPixel(int sample)
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
void __fastcall TWaveDrawer::SetReader(TWaveReader * reader)
{
	// set wave reader
	FReader = reader;
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TWaveDrawer::SetStart(int n)
{
	// set start position
	n = PixelToSample(SampleToPixel(n)); // quantize
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = ClientWidth;
	r.bottom = ClientHeight;
	ScrollWindowEx(Parent->Handle, SampleToPixel(FStart) - SampleToPixel(n), 0,
		&r, NULL, NULL, NULL, SW_INVALIDATE);

	FStart = n;
}
//---------------------------------------------------------------------------
void __fastcall TWaveDrawer::SetMagnify(int m)
{
	// set magnification
	if(m > 3) m = 3;
	if(m < -14) m = -14;
	if(FMagnify != m)
	{
		int center = FStart + PixelToSample(ClientWidth >> 1);
		FMagnify = m;
		int left = center - PixelToSample(ClientWidth >> 1);
		int view = PixelToSample(ClientWidth);
		if(left < 0) left = 0;
		if(left > FReader->NumSamples - view) left = FReader->NumSamples - view;
		if(left < 0) left = 0;

		FStart = PixelToSample(SampleToPixel(left)); // quantize
		FMinRulerMajorWidth = 0;
		FMinRulerMajorHeight = 0;
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDrawer::DrawWave(int start, bool clear)
{
	// draw waveform.
	// this does not clear the background unless 'clear' is true.
	// 'magnify' is magnify level in logarithm based 2;
	//  2 = x 4
	//  1 = x 2
	//  0 = x 1
	// -1 = x 1 / 2
	// -2 = x 1 / 4
	if(!FReader || !FReader->ReadDone)
	{
		TRect r;
		r.left = Canvas->ClipRect.left, r.top = 0, r.right = Canvas->ClipRect.right, r.bottom = ClientHeight;
		Canvas->Brush->Style = bsSolid;
		Canvas->Brush->Color = C_DISABLEED_CLIENT;
		Canvas->FillRect(r);
		return ; // reader not available
	}

	// calc minimum major ruler size
	if(FDrawRuler && FMinRulerMajorWidth == 0)
	{
		AnsiString zero("00:00:00.000");
		FMinRulerMajorWidth = Canvas->TextWidth(zero);
		FMinRulerMajorWidth += 3;
		FMinRulerMajorHeight = Canvas->TextHeight(zero);
		FMinRulerMajorHeight += FMinRulerMajorHeight / 3;

		// decice time unit
		double sample_per_pixel;
		if(FMagnify > 0)
			sample_per_pixel = 1.0 / (1<<FMagnify);
		else
			sample_per_pixel = 1.0 * (1<<(-FMagnify));

		double time_per_pixel = 10000.0 * // time in msec/10
			sample_per_pixel / FReader->Frequency;

		// crank up to upper reasonable time unit
		int n;
		int nn;
		for(n = 10000000; n >= 1; n /= 10)
		{
			double unit_size;
			nn = n * 1;
			unit_size = nn / time_per_pixel;
			if(unit_size <= FMinRulerMajorWidth) break;
		}
		FRulerUnit = nn;
	}

	// calc dimentions
	int xstep = FMagnify <= 0 ? 1 : (1<<FMagnify);
	int sstep = FMagnify <= 0 ? (1<<(-FMagnify)) : 1;

	int head_size = FDrawRuler ? (FMinRulerMajorHeight + 1) : 0;
	int one_height = ((Height - head_size) / FReader->Channels) & ~1;
	int one_height_half = one_height >> 1;
	int one_client_height = one_height - 2; // 2 : padding margin

	// calc start point
	int dest_left  = Canvas->ClipRect.left;
	int dest_right = Canvas->ClipRect.right;

	//- align destination start and end
	dest_left = (int)(dest_left / xstep) * xstep;
	dest_right = ((int)(dest_right / xstep) + 1) * xstep;

	//- calc pcm start point
	int pos = start + (int)(dest_left / xstep) * sstep;
	if(pos < 0)
	{
		// start position is negative
		dest_left += (-pos) * xstep;
		pos = 0;
		if(dest_left > dest_right) dest_left = dest_right;
	}


	//- calc pcm end point
	int vr = SampleToPixel(FReader->NumSamples - pos) + 1 + dest_left;
	if(vr < dest_right) dest_right = vr;

	// clear the background
	#define		CONV_Y(v, ch) ((((v) * one_client_height) /65536) + \
							one_height_half + (ch) * one_height + head_size)

	if(clear)
	{
		// negative
		if(Canvas->ClipRect.left < dest_left)
		{
			TRect r;
			r.left = Canvas->ClipRect.left, r.top = head_size, r.right = dest_left, r.bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_DISABLEED_CLIENT;
			Canvas->FillRect(r);
		}


		// main
		if(dest_left < dest_right)
		{
			TRect r;
			r.left = dest_left, r.top = head_size, r.right = dest_right, r.bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_CLIENT;
			Canvas->FillRect(r);
		}

		// draw -Inf line
		Canvas->Pen->Color = C_INF_LINE;
		for(int ch = 0; ch < FReader->Channels; ch ++)
		{
			Canvas->MoveTo(dest_left,  CONV_Y(0, ch));
			Canvas->LineTo(dest_right, CONV_Y(0, ch));
		}


		// over
		if(Canvas->ClipRect.right > dest_right)
		{
			TRect r;
			r.left = dest_right, r.top = head_size, r.right = Canvas->ClipRect.right, r.bottom = ClientHeight;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_DISABLEED_CLIENT;
			Canvas->FillRect(r);
		}

		// draw time base line
		if(FDrawRuler)
		{
			TRect r;
			r.left = dest_left, r.top = 0, r.right = dest_right, r.bottom = head_size;
			Canvas->Brush->Style = bsSolid;
			Canvas->Brush->Color = C_TIME_CLIENT;
			Canvas->FillRect(r);
			if(dest_right < Canvas->ClipRect.right)
			{
				r.left = dest_right;
				r.right = Canvas->ClipRect.right;
				Canvas->Brush->Color = C_DISABLED_TIME_CLIENT;
				Canvas->FillRect(r);
			}
			Canvas->Pen->Color = C_TIME_COLOR;
			Canvas->MoveTo(dest_left, head_size - 1);
			Canvas->LineTo(Canvas->ClipRect.right, head_size - 1);
		}

	}


	// main loop
	int rpos = pos;
	Canvas->Pen->Color = C_WAVE;
	if(dest_left >= dest_right) return; // no more samples
	for(int x = dest_left; x <= dest_right;
		x += xstep, pos += sstep)
	{
		for(int ch = 0; ch < FReader->Channels; ch ++)
		{
			// draw line to previous sample
			if(pos >= FReader->NumSamples) break;

			if(pos > 0)
			{
				int pv = FReader->GetSampleAt(pos-1, ch);
				Canvas->MoveTo(x-xstep, CONV_Y(pv, ch));
			}
			else
			{
				Canvas->MoveTo(x-xstep, CONV_Y(0, ch));
			}

			int cv;
			cv = FReader->GetSampleAt(pos, ch);
			Canvas->LineTo(x, CONV_Y(cv, ch));

			if(FMagnify < 0)
			{
				// shrink view
				// draw peaks
				int high, low;
				FReader->GetPeak(high, low, pos, ch, sstep);
				Canvas->MoveTo(x, CONV_Y(high, ch));
				Canvas->LineTo(x, CONV_Y(low , ch));
			}
		}
	}

	// draw time line
	if(FDrawRuler)
	{
		// find start ruler unit
		Canvas->Brush->Style = bsClear;
		Canvas->Pen->Color = C_TIME_COLOR;
		Canvas->Font->Color = C_TIME_COLOR;
		double sample_per_pixel;

		int time_index;

		if(FMagnify > 0)
			sample_per_pixel = 1.0 / (1<<FMagnify);
		else
			sample_per_pixel = 1.0 * (1<<(-FMagnify));

		double time_per_pixel = // time in msec/10
			sample_per_pixel / FReader->Frequency * 10000;

		double start_time = (double)rpos / (double)FReader->Frequency * 10000;
		time_index = (int)(start_time / FRulerUnit) - 10;
		start_time = (double)time_index * (double)FRulerUnit;

		double x_per_unit = FRulerUnit / time_per_pixel;

		double x = start_time / time_per_pixel - SampleToPixel(rpos);

		for(; x < dest_right; x += x_per_unit, time_index ++)
		{
			int ix = (int)floor(x + dest_left);
			int st;
			bool write_time = false;
			if(time_index % 10 == 0)
			{
				st = 0;
				write_time = true;
			}
			else if(time_index % 5 == 0)
			{
				if(FRulerUnit >= 5 && x_per_unit * 5 > FMinRulerMajorWidth)
				{
					st = 0;
					write_time = true;
				}
				else
				{
					st = (head_size >> 3) + (head_size >> 1);
				}
			}
			else
			{
				if(FRulerUnit >= 1 && x_per_unit > FMinRulerMajorWidth)
				{
					st = 0;
					write_time = true;
				}
				else
				{
					st = (head_size >> 2) + (head_size >> 1);
				}
			}
			Canvas->MoveTo(ix, st);
			Canvas->LineTo(ix, head_size - 1);
			if(write_time)
			{
				// write time
				int t = (int)((__int64)time_index * FRulerUnit / 10);
				char buf[100];
				if(FRulerUnit >= 5000)
				{
					sprintf(buf, "%02d:%02d:%02d",
						t / 1000 / 60 / 60,
						t / 1000 / 60 % 60,
						t / 1000 % 60);
				}
				else
				{
					sprintf(buf, "%02d:%02d:%02d.%03d",
						t / 1000 / 60 / 60,
						t / 1000 / 60 % 60,
						t / 1000 % 60,
						t % 1000);
				}
				Canvas->TextOut(ix+2, 0, buf);
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveDrawer::Paint(void)
{
	DrawWave(FStart, true);
}
//---------------------------------------------------------------------------

