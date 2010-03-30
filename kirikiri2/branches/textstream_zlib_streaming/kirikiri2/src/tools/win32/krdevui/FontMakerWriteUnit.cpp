//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FontMakerWriteUnit.h"
#include "FontMakerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFontMakerWriteForm *FontMakerWriteForm;

//---------------------------------------------------------------------------
static void ExpandLine
	(unsigned char *dest, const unsigned char *buf, int len)
{
	unsigned char *d=dest, *dlim;
	unsigned char b;

	dlim = dest + len-7;
	while(d < dlim)
	{
		b = *(buf++);
		d[0] = (b&0x80) ? 64 : 0;
		d[1] = (b&0x40) ? 64 : 0;
		d[2] = (b&0x20) ? 64 : 0;
		d[3] = (b&0x10) ? 64 : 0;
		d[4] = (b&0x08) ? 64 : 0;
		d[5] = (b&0x04) ? 64 : 0;
		d[6] = (b&0x02) ? 64 : 0;
		d[7] = (b&0x01) ? 64 : 0;
		d += 8;
	}
	dlim = dest + len;
	b = *buf;
	while(d<dlim)
	{
		*(d++) = (b&0x80) ? 64 : 0;
		b<<=1;
	}
}
//---------------------------------------------------------------------------
static void WriteDataCompress(TStream * out, unsigned char *buf, int size)
{
	// write data to stream with compressing data
	if(size == 0) return;

	unsigned char *newbuf = new unsigned char [size];
	int newsize = 0;
	int count = 0;
	unsigned char last = 0xff;

	int i;
	for(i = 0; i<size; i++)
	{
		if(last == buf[i])
		{
			count++;
		}
		else
		{
			if(count >= 2)
			{
				while(count)
				{
					int len = count > 190 ? 190 : count;
					newbuf[newsize++] = 0x40 + len; // running
					count -= len;
				}
			}
			else
			{
				while(count--)
					newbuf[newsize++] = last;
			}
			count = 0;
			newbuf[newsize++] = buf[i];
		}
		last = buf[i];
	}

	if(count >= 2)
	{
		while(count)
		{
			int len = count > 190 ? 190 : count;
			newbuf[newsize++] = 0x40 + len; // running
			count -= len;
		}
	}
	else
	{
		while(count--)
			newbuf[newsize++] = last;
	}

	try
	{
		out->WriteBuffer(newbuf, newsize);
	}
	catch(...)
	{
		delete [] newbuf;
		throw;
	}
	delete [] newbuf;
}
//---------------------------------------------------------------------------
__fastcall TFontMakerWriteForm::TFontMakerWriteForm(TComponent* Owner,
	AnsiString filename,
	TCanvas * targetcanvas, TCanvas * privtargetcanvas,
	bool antialiased, bool vertical, const wchar_t *list,
	int listcount) : TForm(Owner)
{
	Canceled = false;
	TargetCanvas = targetcanvas;
	PrivTargetCanvas = privtargetcanvas;
	Antialiased = antialiased;
	Vertical = vertical;
	CharacterList = list;
	CharacterListCount = listcount;

	FileName = filename;
	OutStream = new TFileStream(filename, fmCreate|fmShareDenyWrite);
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerWriteForm::FormDestroy(TObject *Sender)
{
	delete OutStream;
	if(Canceled) DeleteFile(FileName);
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerWriteForm::CancelButtonClick(TObject *Sender)
{
	Canceled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerWriteForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	Canceled = true;
	CanClose = false;
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerWriteForm::TimerTimer(TObject *Sender)
{
	Timer->Enabled = false;
	CancelButton->Enabled = true;

	// write header
	OutStream->WriteBuffer("TVP pre-rendered font\x1a\x01\x02", 24);
		// header + version + sizeof(wchar_t)
	OutStream->WriteBuffer("            ", 12);
		// count/offset to chindex/offset to index

	// make table
	// make SJIS table

	// ASCII and kana
	for(int i = 0; i < CharacterListCount; i++)
	{
		TCharacterItem item;
		item.Code = CharacterList[i];
		Items.push_back(item);
	}

	// sort table
	std::sort(Items.begin(), Items.end());

	// create each font
	std::vector<TCharacterItem>::iterator i;
	int count = 0;
	for(i = Items.begin(); i != Items.end(); i++, count++)
	{
		// process messages
		ProgressBar->Position = count * 100 / Items.size();

		Application->ProcessMessages();
		if(Canceled)
		{
			OnCloseQuery = NULL;
			ModalResult = mrCancel;
			return;
		}

		// retrieve character metrics
		GLYPHMETRICS gm;
		ZeroMemory(&gm, sizeof(gm));
		static MAT2 no_transform_affin_matrix = { {0,1}, {0,0}, {0,0}, {0,1} };

		UINT format = Antialiased ? GGO_GRAY8_BITMAP : GGO_BITMAP;
		int size;
		wchar_t code = i->Code;
		int pitch;
		int ncode = code;

		if(!procGetGlyphOutlineW)
		{
			unsigned char temp[4];
			int nch = WideCharToMultiByte(CP_ACP, 0, &code, 1, (char*)temp, 4, 0, NULL);
			if(nch == 2) ncode = temp[1] + (temp[0] << 8); else ncode = temp[0];
		}

		HDC dc = (code >= 0xe000 && code <= 0xf8ff) ?
			PrivTargetCanvas->Handle : TargetCanvas->Handle;

		if(procGetGlyphOutlineW)
			size = procGetGlyphOutlineW(dc, ncode, format,
				&gm, 0, NULL, &no_transform_affin_matrix);
		else
			size = GetGlyphOutline(dc, ncode, format,
				&gm, 0, NULL, &no_transform_affin_matrix);


		SIZE incsz;
		procGetTextExtentPoint32W(dc, &code, 1, &incsz);
		int inc = incsz.cx;

		// setup structure members
		i->Offset = OutStream->Position;
		i->Width = size?gm.gmBlackBoxX:0;
		i->Height = size?gm.gmBlackBoxY:0;
		i->IncX = gm.gmCellIncX;
		i->IncY = gm.gmCellIncY;
		i->OriginX = gm.gmptGlyphOrigin.x;
		i->OriginY = gm.gmptGlyphOrigin.y;
		i->Inc = inc;

		if(size && gm.gmBlackBoxX && gm.gmBlackBoxY)
		{
			if(Antialiased)
				pitch = (size / gm.gmBlackBoxY) & ~0x03; // data is aligned to DWORD
			else
				pitch = (((gm.gmBlackBoxX -1)>>5)+1)<<2; // data is aligned to DWORD

			// retrieve font data
			unsigned char *buf = new unsigned char[size];

			if(procGetGlyphOutlineW)
				procGetGlyphOutlineW(dc, ncode, format,
					&gm, size, buf, &no_transform_affin_matrix);
			else
				GetGlyphOutline(dc, ncode, format,
					&gm, size, buf, &no_transform_affin_matrix);


			// expand if not antialiased
			if(!Antialiased)
			{
				int newsize = gm.gmBlackBoxX * gm.gmBlackBoxY;

				unsigned char *newbuf = new unsigned char[newsize];
				unsigned char *np = newbuf;
				unsigned char *p = buf;
				int h = gm.gmBlackBoxY;
				while(h--)
				{
					ExpandLine(np, p, gm.gmBlackBoxX);
					np += gm.gmBlackBoxX, p += pitch;
				}
				pitch = gm.gmBlackBoxX;
				delete [] buf;
				buf = newbuf;
				size = newsize;
			}
			else
			{
				// or stuff data
				int h = i->Height;
				unsigned char *np;
				unsigned char *p;
				np = p = buf;
				while(h--)
				{
					if(np != p) memmove(np, p, i->Width);
					np += i->Width;
					p += pitch;
				}
				pitch = i->Width;
				size = gm.gmBlackBoxX * gm.gmBlackBoxY;
			}

			// write data
			try
			{
				if(size) WriteDataCompress(OutStream, buf, size);
			}
			catch(...)
			{
				delete [] buf;
				throw;
			}
			delete [] buf;
		}
	}

	// write character index/ index data

	int chindexpos = OutStream->Position;
	OutStream->WriteBuffer("\0\0\0\0", 4 - (chindexpos % 4));  // align
	chindexpos = OutStream->Position;
	for(i = Items.begin(); i != Items.end(); i++)
	{
		OutStream->WriteBuffer(&(i->Code), sizeof(wchar_t));
	}

	int indexpos = OutStream->Position;
	OutStream->WriteBuffer("\0\0\0\0", 4 - (indexpos % 4));  // align
	indexpos = OutStream->Position;
	for(i = Items.begin(); i != Items.end(); i++)
	{
		OutStream->WriteBuffer(
			(unsigned char *)&(*i) + sizeof(wchar_t),
			sizeof(TCharacterItem) - sizeof(wchar_t));
	}

	OutStream->Position = 24;
	OutStream->WriteBuffer(&count, 4);
	OutStream->WriteBuffer(&chindexpos, 4);
	OutStream->WriteBuffer(&indexpos, 4);

	OnCloseQuery = NULL;
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------

