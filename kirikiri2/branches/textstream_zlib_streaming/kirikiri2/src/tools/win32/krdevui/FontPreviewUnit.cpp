//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FontPreviewUnit.h"
#include "FontMakerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFontPreviewForm *FontPreviewForm;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void __fastcall SetGrayscalePalette(Graphics::TBitmap *dib)
{
	int psize;
	int icol;

	psize = 256;

	icol = 255/(psize-1);

	int i;
	int c = 0;

#pragma pack(push, 1)

	struct
	{
		WORD v;
		WORD n;
		PALETTEENTRY entry[256];
	} paldata;

#pragma pack(pop)

	paldata.v = 0x300;
	paldata.n = 256;

	for(i = 0; i < psize; i++)
	{
		if(c >= 256) c = 255;
		paldata.entry[i].peRed = paldata.entry[i].peGreen = paldata.entry[i].peBlue = (BYTE)c;
		paldata.entry[i].peFlags = 0;
		c += icol;
	}
	dib->Palette = CreatePalette((const LOGPALETTE*)&paldata);
}
//---------------------------------------------------------------------------
static Graphics::TBitmap * GetGlyphBitmap(wchar_t code, TCanvas *refcanvas,
	bool antialiased)
{
	// retrieve character metrics
	GLYPHMETRICS gm;
	ZeroMemory(&gm, sizeof(gm));
	static MAT2 no_transform_affin_matrix = { {0,1}, {0,0}, {0,0}, {0,1} };

	UINT format = antialiased ? GGO_GRAY8_BITMAP : GGO_BITMAP;
	int size;
	int pitch;

	int ncode = code;

	if(!procGetGlyphOutlineW)
	{
		unsigned char temp[4];
		int nch = WideCharToMultiByte(CP_ACP, 0, &code, 1, (char*)temp, 4, 0, NULL);
		if(nch == 2) ncode = temp[1] + (temp[0] << 8); else ncode = temp[0];
	}

	if(procGetGlyphOutlineW)
		size = procGetGlyphOutlineW(refcanvas->Handle, ncode, format,
			&gm, 0, NULL, &no_transform_affin_matrix);
	else
		size = GetGlyphOutline(refcanvas->Handle, ncode, format,
			&gm, 0, NULL, &no_transform_affin_matrix);

	if(!size || !gm.gmBlackBoxX || !gm.gmBlackBoxY) return NULL;

	// create bitmap
	Graphics::TBitmap *bmp = new Graphics::TBitmap();
	bmp->PixelFormat = pf8bit;
	bmp->Width = gm.gmBlackBoxX;
	bmp->Height = gm.gmBlackBoxY;
	SetGrayscalePalette(bmp);

	if(antialiased)
		pitch = (size / gm.gmBlackBoxY) & ~0x03; // data is aligned to DWORD
	else
		pitch = (((gm.gmBlackBoxX -1)>>5)+1)<<2; // data is aligned to DWORD

	// retrieve font data
	unsigned char *buf = new unsigned char[size];

	if(procGetGlyphOutlineW)
		procGetGlyphOutlineW(refcanvas->Handle, ncode, format,
			&gm, size, buf, &no_transform_affin_matrix);
	else
		GetGlyphOutline(refcanvas->Handle, ncode, format,
			&gm, size, buf, &no_transform_affin_matrix);


	// expand
	if(!antialiased)
	{
		unsigned char *p = buf;
		for(unsigned int y = 0; y < gm.gmBlackBoxY; y++)
		{
			unsigned char *d = (unsigned char *)bmp->ScanLine[y];
			for(unsigned int x = 0; x < gm.gmBlackBoxX; x++)
			{
				d[x] = (p[x >> 3]& (0x80 >> (x & 0x07))) ? 0 : 255;
			}
			p += pitch;
		}
	}
	else
	{
		unsigned char *p = buf;
		for(unsigned int y = 0; y < gm.gmBlackBoxY; y++)
		{
			unsigned char *d = (unsigned char *)bmp->ScanLine[y];
			for(unsigned int x = 0; x < gm.gmBlackBoxX; x++)
			{
				d[x] = 255 - (p[x] == 64 ? 255 : p[x] * 4);
			}
			p += pitch;
		}
	}

	delete [] buf;

	return bmp;
}

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
struct TRedrawData
{
	int Index;
};
//---------------------------------------------------------------------------
__fastcall TFontPreviewBox::TFontPreviewBox(TWinControl *Owner)
	: TCustomControl(Owner)
{
	Form = (TFontPreviewForm*)Owner;
	Cols = 0;
	Brush->Style = bsClear;
//	Brush->Color = clWindow;

	RedrawList = new TList();
	Application->OnIdle = ApplicationIdle;
}
//---------------------------------------------------------------------------
__fastcall TFontPreviewBox::~TFontPreviewBox()
{
	for(int i = 0; i < RedrawList->Count; i++)
		delete (TRedrawData*)(RedrawList->Items[i]);
	delete RedrawList;
	Application->OnIdle = NULL;
}
//---------------------------------------------------------------------------
int __fastcall TFontPreviewBox::GetOneSize()
{
	int onesize = Form->GetRefCanvas()->Font->Height;
	if(onesize < 0) onesize = -onesize;
	onesize *= Form->GetZoom();
	onesize += 4;
	return onesize;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::CreateParams(TCreateParams &params)
{
	inherited::CreateParams(params);

	params.Style |= WS_VSCROLL;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::Paint(void)
{
	// paint the surface
	if(!Cols) return;

	int onesize = GetOneSize();
	const wchar_t * charlist = Form->GetCharacterList();
	int charlistcount = Form->GetCharacterListCount();

	TRect r;
	r = Canvas->ClipRect;

	Canvas->Brush->Color = clWindow;
	Canvas->FillRect(r);
	Canvas->Font->Color = clGrayText;

	int topindex = GetTopIndex();

	int xstart = r.left / onesize;
	int ystart = r.top / onesize;
	int xlim = r.right / onesize;
	int ylim = r.bottom / onesize;

	for(int y = ylim; y >= ystart; y--)
	{
		for(int x = xlim; x >= xstart; x--)
		{
			int index = topindex + x + y*Cols;

			if(index >= charlistcount) continue;

			int i;
			int listcount = RedrawList->Count;
			for(i = 0; i < listcount; i++)
			{
				if(((TRedrawData*)(RedrawList->Items[i]))->Index == index) break;
			}

			if(i == listcount)
			{
				TRedrawData *redraw = new TRedrawData;
				redraw->Index = index;
				RedrawList->Add(redraw);
			}

			TRect r;
			r.left = ((index - topindex) % Cols) * onesize;
			r.right = r.left + onesize;
			r.top = ((index - topindex) / Cols) * onesize;
			r.bottom = r.top + onesize;

			SIZE sz;
			procGetTextExtentPoint32W(Canvas->Handle, charlist + index, 1, &sz);
			procTextOutW(Canvas->Handle,  ((r.right - r.left - sz.cx) >>1) + r.left,
				((r.bottom - r.top - sz.cy) >> 1) + r.top, charlist + index, 1);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::RedrawGlyphs()
{
	int onesize = GetOneSize();
	int topindex = GetTopIndex();
	int charlistcount = Form->GetCharacterListCount();
	const wchar_t * charlist = Form->GetCharacterList();

	for(int i = RedrawList->Count - 1; i >= 0 ; i--)
	{
		TRedrawData *red = (TRedrawData*)(RedrawList->Items[i]);
		if(!red) continue;

		int index = red->Index;

		TRect r;
		r.left = ((index - topindex) % Cols) * onesize;
		r.right = r.left + onesize;
		r.top = ((index - topindex) / Cols) * onesize;
		r.bottom = r.top + onesize;

		if(index >= charlistcount ||
			r.Right < 0 || r.Top < 0 || r.Left > ClientWidth || r.Top > ClientHeight)
		{
			delete red;
			RedrawList->Items[i] = NULL;
			continue;
		}

		Canvas->Brush->Color = clWindow;
		Canvas->FillRect(r);

		Graphics::TBitmap *bmp;
		wchar_t ch = charlist[index];

		bmp = GetGlyphBitmap(ch, 
 			(ch >= 0xe000 && ch <= 0xf8ff) ? Form->GetPrivRefCanvas() : Form->GetRefCanvas(),
			Form->GetAntialiased());

		if(bmp)
		{
			int w = bmp->Width;
			int h = bmp->Height;
			int zoom = Form->GetZoom();
			w *= zoom;
			h *= zoom;
			TRect dr;
			dr.left = (r.right - r.left - w)/2 + r.left;
			dr.right = dr.left + w;
			dr.top = (r.bottom - r.top - h)/2 + r.top;
			dr.bottom = dr.top + h;
			Canvas->StretchDraw(dr, bmp);
			delete bmp;
		}

		RedrawList->Items[i] = NULL;

		break;
	}
	RedrawList->Pack();
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::ApplicationIdle(System::TObject* Sender, bool &Done)
{
	RedrawGlyphs();
	if(RedrawList->Count) Done = false; else Done = true;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::SetScrollRange(void)
{
	int charlistcount = Form->GetCharacterListCount();

	int onesize = GetOneSize();
	int cols = ClientWidth / onesize;
	if(cols <= 0) cols = 1;
	Cols = cols;
	int max = ((charlistcount - 1) / cols) + 1 ;
	int page = ClientHeight / onesize;
	if(page <= 0) page = 1;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_RANGE/* | SIF_DISABLENOSCROLL*/;
	si.nMin = 0;
	si.nMax = max;
	si.nPage = page;
	si.nPos = 0;

	SetScrollInfo(Handle, SB_VERT, &si, true);
	EnableScrollBar(Handle, SB_VERT, ESB_ENABLE_BOTH);
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::WMVScroll(TWMHScroll &msg)
{
	// window is to be scrolled

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_VERT, &si);

	int org = si.nPos;

	if(msg.ScrollCode == SB_THUMBTRACK)
	{
		si.nPos = si.nTrackPos;
	}
	else if(msg.ScrollCode == SB_LINEUP)
	{
		si.nPos--;
		if(si.nPos<0) si.nPos=0;
	}
	else if(msg.ScrollCode == SB_LINEDOWN)
	{
		si.nPos++;
		if((int)(si.nPos) >= (int)(si.nMax - si.nPage)) si.nPos=si.nMax- si.nPage + 1;
	}
	else if(msg.ScrollCode == SB_PAGEUP)
	{
		si.nPos = si.nPos - si.nPage;
		if(si.nPos<0) si.nPos=0;
	}
	else if(msg.ScrollCode == SB_PAGEDOWN)
	{
		si.nPos = si.nPos + si.nPage;
		if(si.nPos >=si.nMax) si.nPos=si.nMax-1;
	}

	si.fMask = SIF_POS;

	SetScrollInfo(Handle, SB_VERT, &si, true);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = ClientWidth;
	r.bottom = ClientHeight;

	int onesize = GetOneSize();

	ScrollWindowEx(Handle, 0, (int)(org - si.nPos) * onesize,
		&r, NULL, NULL, NULL, SW_INVALIDATE);

}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::WMMouseMove(TWMMouseMove &msg)
{
	int topindex = GetTopIndex();
	int onesize = GetOneSize();
	int x = msg.XPos / onesize;
	int y = msg.YPos / onesize;
	int index = topindex + x + y * Cols;
	if(index < Form->GetCharacterListCount())
	{
		wchar_t ch = Form->GetCharacterList()[index];
		Form->StatusBar->Panels->Items[0]->Text = "UNICODE " + IntToHex((int)ch, 4);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::Resize(void)
{
	SetScrollRange();

	inherited::Resize();
}
//---------------------------------------------------------------------------
int __fastcall TFontPreviewBox::GetTopIndex()
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(Handle, SB_VERT, &si);

	return (int)(si.nPos*Cols);
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewBox::NotifyFontChanged()
{
	SetScrollRange();
	Invalidate();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
__fastcall TFontPreviewForm::TFontPreviewForm(TComponent* Owner,
	TCanvas *refcanvas, TCanvas *privrefcanvas)
	: TForm(Owner)
{
	RefCanvas = refcanvas;
	PrivRefCanvas = privrefcanvas;
	PreviewBox = NULL;

	Icon->Assign(((TFontMakerForm*)Owner)->Icon);
	ZoomComboBox->ItemIndex = 0;

	PreviewBox = new TFontPreviewBox(this);
	PreviewBox->Parent = PreviewPanel;
	PreviewBox->Align = alClient;
	PreviewBox->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewForm::FormDestroy(TObject *Sender)
{
	delete PreviewBox;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caHide;	
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewForm::ZoomComboBoxChange(TObject *Sender)
{
	if(PreviewBox) PreviewBox->NotifyFontChanged();
}
//---------------------------------------------------------------------------

void __fastcall TFontPreviewForm::CloseButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewForm::FormMouseWheelDown(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled)
{
	::PostMessage(PreviewBox->Handle, WM_VSCROLL, SB_PAGEDOWN, 0);
	Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewForm::FormMouseWheelUp(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled)
{
	::PostMessage(PreviewBox->Handle, WM_VSCROLL, SB_PAGEUP, 0);
	Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFontPreviewForm::NotifyFontChanged(bool antialiased)
{
	Antialiased = antialiased;
	PreviewBox->NotifyFontChanged();
	StatusBar->Panels->Items[1]->Text = "Total " + AnsiString(GetCharacterListCount()) + " glyphs";
}
//---------------------------------------------------------------------------
int __fastcall TFontPreviewForm::GetZoom() const
{
	return ZoomComboBox->ItemIndex + 1;
}
//---------------------------------------------------------------------------
const wchar_t * TFontPreviewForm::GetCharacterList() const
{
	return ((TFontMakerForm*)Owner)->GetCharacterList();
}
//---------------------------------------------------------------------------
int TFontPreviewForm::GetCharacterListCount() const
{
	return ((TFontMakerForm*)Owner)->GetCharacterListCount();
}
//---------------------------------------------------------------------------


