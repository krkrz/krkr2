//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <math.h>
#include "TotalView.h"
#include "WaveReader.h"
#include "ColorScheme.h"

//---------------------------------------------------------------------------
// constants
//---------------------------------------------------------------------------
const int CONTROL_HEIGHT = 17;
const int CONTROL_WAVE_HEIGHT = 11;
const int CONTROL_WAVE_CENTER = 7;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// MakeGradationPalette
//---------------------------------------------------------------------------
#define G_COLOR_BLEND_8(x, y, ratio)  (((x) * (ratio) + (y) * (255-(ratio))) / 255)
#define G_BLEND_RGB(x, y, ratio) (\
			(G_COLOR_BLEND_8(BYTEOF(x, 0), BYTEOF(y, 0), (ratio))    )+ \
			(G_COLOR_BLEND_8(BYTEOF(x, 1), BYTEOF(y, 1), (ratio))<<8 )+ \
			(G_COLOR_BLEND_8(BYTEOF(x, 2), BYTEOF(y, 2), (ratio))<<16) )
static void MakeGradationPalette(Graphics::TBitmap *bmp, TColor fromcolor, TColor tocolor)
{
	int i;

#pragma pack(push,1)
	struct tagpal
	{
		WORD palversion;
		WORD numentries;
		PALETTEENTRY entry[256];
	} pal;
#pragma pack(pop)
	pal.palversion = 0x300;
	pal.numentries = 256;

	for(i=0;i<256;i++)
	{
		TColor c = (TColor)G_BLEND_RGB((int)tocolor, (int)fromcolor, i);
		pal.entry[i].peRed		= c & 0xff;
		pal.entry[i].peGreen	= (c & 0xff00) >> 8;
		pal.entry[i].peBlue		= (c & 0xff0000) >> 16;
		pal.entry[i].peFlags = 0;
	}
	bmp->Palette = CreatePalette((LOGPALETTE*)&pal);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TThumbnailThread
// thread to create thumbnailed waveform graph
//---------------------------------------------------------------------------
class TThumbnailThread : public TThread
{
	TWaveReader * Reader;
	Graphics::TBitmap *Bitmap;
	TWinControl *Owner;

public:
	__fastcall TThumbnailThread(TWinControl *owner, TWaveReader * reader,
			Graphics::TBitmap *bmp) : TThread(true)
	{
		Owner = owner;
		Reader = reader;
		Bitmap = bmp;
		Priority = tpIdle;
		Suspended = false;
	}

	__fastcall ~TThumbnailThread(void)
	{
		Priority = tpNormal;
		Suspended = false;
		Terminate();
		WaitFor();
	}


protected:

	void __fastcall SyncFunc()
	{
		Owner->Invalidate();
	}


	void __fastcall Execute(void)
	{
		int x = 0;
		int w = Bitmap->Width;
		double ratio = (double)Reader->NumSamples / w;

		DWORD tick = GetTickCount();

		while(!Terminated && x < w)
		{
			// calculate range
			int start = ratio * x;
			int end = ratio * (x+1);
			if(end > Reader->NumSamples) end = Reader->NumSamples;
			if(end - start < 1) end ++;
			if(end > Reader->NumSamples) break;

			// calculate histgram
			int hist[CONTROL_HEIGHT];
			for(int i = 0; i < CONTROL_HEIGHT; i++) hist[i] = 0;

			for(int p = start; p < end; p++)
			{
				for(int ch = 0; ch < Reader->Channels; ch++)
				{
					int smp = Reader->GetSampleAt(p, ch);
					int n = smp  * -CONTROL_WAVE_HEIGHT / 65536 + CONTROL_WAVE_CENTER;
					int z = CONTROL_WAVE_CENTER;
					if(n < z)
						for(int i = n; i <= z; i++) hist[i]++;
					else
						for(int i = z; i <= n; i++) hist[i]++;
				}
			}

			// draw histgram graph as pixel
			float  idiv = 1.0 / ((end - start) * Reader->Channels);
			for(int y = 0; y < CONTROL_HEIGHT; y++)
			{
				float p = 1.0 - (float) hist[y] * idiv;
				p = 1.0 - p * p;
				int ip = (int)(p * 1024);
				if(ip > 255) ip = 255;
				((unsigned char *)Bitmap->ScanLine[y])[x] = ip;
			}

			x++;

			DWORD now = GetTickCount();
			if(now - tick > 250)
			{
				Synchronize(SyncFunc);
				tick = now;
			}

		}
		Synchronize(SyncFunc);
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TTotalView
//---------------------------------------------------------------------------
__fastcall TTotalView::TTotalView(Classes::TComponent* AOwner) :
	TCustomControl(AOwner)
{
	FThumbnailer = NULL;
	FReader = NULL;
	WaveCache = NULL;
	Height = CONTROL_HEIGHT;
	FMouseDowned = false;
	FOnPoint = NULL;
	FOnDoubleClick = NULL;
	DoubleBuffered = true;
	ResetAll();
}
//---------------------------------------------------------------------------
__fastcall TTotalView::~TTotalView()
{
	if(FThumbnailer) delete FThumbnailer;
	if(WaveCache) delete WaveCache;
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::ResetAll()
{
	if(FThumbnailer) delete FThumbnailer, FThumbnailer = NULL;
	FViewStart = 0;
	Reader = NULL;
	FMagnify = 1.0;
	FViewLength = 0;
	FCaretPos = -1;
	FShowCaret = false;
	FMarkerPos = -1;
	FShowMarker = false;
	Labels.clear();
	Links.clear();
	FShowCaret = false;
	if(WaveCache) RecreateWaveCacheBitmap();
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::Paint(void)
{
	if(!FReader) return;

	if(WaveCache) Canvas->Draw(0, 0, WaveCache);

	DrawViewRange(); // draw current view range
	DrawLinksAndLabels(); // draw links and labels
	DrawCaret(); // draw caret
	DrawMarker(); // draw caret

	// draw bottom line
	Canvas->Pen->Color = clBtnShadow;
	Canvas->MoveTo(-1, ClientHeight - 1);
	Canvas->LineTo(ClientWidth, ClientHeight - 1);
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::SetReader(TWaveReader * reader)
{
	// set wave reader
	if(FThumbnailer) delete FThumbnailer, FThumbnailer = NULL;
	FReader = reader;
	RecreateWaveCache();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::Resize()
{
	if(!WaveCache) return;
	if(WaveCache->Width != ClientWidth) RecreateWaveCache();
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::RecreateWaveCache()
{
	if(!FReader) return;

	// delete current thumbnailer
	if(FThumbnailer) delete FThumbnailer, FThumbnailer = NULL;

	// caluclate magnification level
	FMagnify = (double)FReader->NumSamples / (ClientWidth ? ClientWidth : 0);

	// recreate bitmap
	RecreateWaveCacheBitmap();

	// begin thumbnailing
	FThumbnailer = new  TThumbnailThread(this, FReader, WaveCache);

}
//---------------------------------------------------------------------------
void __fastcall TTotalView::RecreateWaveCacheBitmap()
{
	// recreate bitmap
	if(WaveCache) delete WaveCache, WaveCache = NULL;
	WaveCache = new Graphics::TBitmap();
	WaveCache->Width = ClientWidth;
	WaveCache->Height = ClientHeight;
	WaveCache->PixelFormat = pf8bit;
	MakeGradationPalette(WaveCache, C_CLIENT, C_WAVE);
}
//---------------------------------------------------------------------------
void TTotalView::DrawViewRange()
{
	int start = FViewStart / FMagnify;
	int length = FViewLength / FMagnify;

	RECT r;
	r.top = 0;
	r.bottom = 3;
	r.left = start;
	r.right = start + length;
	InvertRect(Canvas->Handle, &r);

	r.top = ClientHeight - 5;
	r.bottom = ClientHeight - 2;
	r.left = start;
	r.right = start + length;
	InvertRect(Canvas->Handle, &r);

	r.top = 1;
	r.bottom = 3;
	r.left = start + 1;
	r.right = start + length - 1;
	InvertRect(Canvas->Handle, &r);

	r.top = ClientHeight - 5;
	r.bottom = ClientHeight - 3;
	r.left = start + 1;
	r.right = start + length - 1;
	InvertRect(Canvas->Handle, &r);
}
//---------------------------------------------------------------------------
void TTotalView::InvalidateViewRange()
{
	int start = FViewStart / FMagnify;
	int length = FViewLength / FMagnify;
	RECT r;
	r.top = 0;
	r.bottom = ClientHeight - 1;
	r.left = start;
	r.right = start + length;
	::InvalidateRect(Handle, &r, FALSE);
}
//---------------------------------------------------------------------------
void TTotalView::SetViewRange(int start, int length)
{
	if(FViewStart != start || FViewLength != length)
	{
		InvalidateViewRange();

		FViewStart = start;
		FViewLength = length;

		InvalidateViewRange();
	}
}
//---------------------------------------------------------------------------
void TTotalView::DrawCaret()
{
	if(FShowCaret)
	{
		int pos = FCaretPos / FMagnify;
		RECT r;
		r.top = 0;
		r.bottom = ClientHeight - 2;
		r.left = pos;
		r.right = pos + 1;
		InvertRect(Canvas->Handle, &r);
	}
}
//---------------------------------------------------------------------------
void TTotalView::InvalidateCaret()
{
	if(FShowCaret)
	{
		int pos = FCaretPos / FMagnify;
		RECT r;
		r.top = 0;
		r.bottom = ClientHeight - 2;
		r.left = pos;
		r.right = pos + 1;
		::InvalidateRect(Handle, &r, FALSE);
	}
}
//---------------------------------------------------------------------------
void TTotalView::SetCaretState(int caretpos, bool showcaret)
{
	if(FCaretPos != caretpos || FShowCaret != showcaret)
	{
		InvalidateCaret();

		FCaretPos = caretpos;
		FShowCaret = showcaret;

		InvalidateCaret();
	}
}
//---------------------------------------------------------------------------
void TTotalView::DrawMarker()
{
	if(FShowMarker)
	{
		int pos = FMarkerPos / FMagnify;
		RECT r;
		r.top = 0;
		r.bottom = ClientHeight - 2;
		r.left = pos;
		r.right = pos + 1;
		InvertRect(Canvas->Handle, &r);
	}
}
//---------------------------------------------------------------------------
void TTotalView::InvalidateMarker()
{
	if(FShowMarker)
	{
		int pos = FMarkerPos / FMagnify;
		RECT r;
		r.top = 0;
		r.bottom = ClientHeight - 2;
		r.left = pos;
		r.right = pos + 1;
		::InvalidateRect(Handle, &r, FALSE);
	}
}
//---------------------------------------------------------------------------
void TTotalView::SetMarkerState(int markerpos, bool showmarker)
{
	if(FMarkerPos != markerpos || FShowMarker != showmarker)
	{
		InvalidateMarker();

		FMarkerPos = markerpos;
		FShowMarker = showmarker;

		InvalidateMarker();
	}
}
//---------------------------------------------------------------------------
void TTotalView::DrawLinksAndLabels()
{
	// draw links and labels

	// draw labels
	Canvas->Pen->Color = C_WAVETHUMB_LABELS;
	for(unsigned int i = 0; i < Labels.size(); i++)
	{
		int pos = Labels[i].Position / FMagnify;
		Canvas->MoveTo(pos, 0);
		Canvas->LineTo(pos, ClientHeight - 2);
	}

	// draw links
	Canvas->Pen->Color = C_WAVETHUMB_LINKS;
	for(unsigned int i = 0; i < Links.size(); i++)
	{
		int from = Links[i].From / FMagnify;
		int to = Links[i].To / FMagnify;
		Canvas->MoveTo(from, CONTROL_WAVE_CENTER);
		Canvas->LineTo(to  , CONTROL_WAVE_CENTER);
		Canvas->MoveTo(from, CONTROL_WAVE_CENTER - 2);
		Canvas->LineTo(from, CONTROL_WAVE_CENTER + 3);
		Canvas->MoveTo(to,   CONTROL_WAVE_CENTER - 2);
		Canvas->LineTo(to,   CONTROL_WAVE_CENTER + 3);
	}

}
//---------------------------------------------------------------------------
void TTotalView::SetLinks(const std::vector<tTVPWaveLoopLink> & links)
{
	Links = links;
	Invalidate();
}
//---------------------------------------------------------------------------
void TTotalView::SetLabels(const std::vector<tTVPWaveLabel> & labels)
{
	Labels = labels;
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::MouseDown(TMouseButton button, TShiftState shift, int x, int y)
{
	if(!FReader) return;
	if(!FMouseDowned)
	{
		FMouseDowned = true;
		LastClickedPos = x * FMagnify;
		if(FOnPoint) FOnPoint(this, x * FMagnify);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::MouseMove(TShiftState shift, int x, int y)
{
	if(!FReader) return;
	if(FMouseDowned) if(FOnPoint) FOnPoint(this, x * FMagnify);
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::MouseUp(TMouseButton button, TShiftState shift, int x, int y)
{
	if(!FReader) return;
	FMouseDowned = false;
}
//---------------------------------------------------------------------------
void __fastcall TTotalView::DblClick(void)
{
	if(!FReader) return;
	FMouseDowned = true;
	if(FOnDoubleClick) FOnDoubleClick(this, LastClickedPos);
}
//---------------------------------------------------------------------------


