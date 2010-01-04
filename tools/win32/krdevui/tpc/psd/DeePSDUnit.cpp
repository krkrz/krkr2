//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
/*
	This is very limited loader for PSD (Photoshop native data format).
	This can only load PSD data of RGB, and the layer is 'normal' blend mode
	('linear dodge' blend mode can also be loaded in additive alpha mode),
	8bit for each color/alpha component.
	Otherwise the loading will fail.
*/
//---------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "DeePSDUnit.h"


//---------------------------------------------------------------------------
// Check for pure virtual functions
//---------------------------------------------------------------------------
static void TestDeePSD(void)
{
	new TDeePSD();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// integer types
//---------------------------------------------------------------------------
typedef unsigned long tui32;
typedef long ti32;
typedef unsigned short tui16;
typedef short ti16;
typedef unsigned char tui8;
typedef char ti8;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Endian converters
//---------------------------------------------------------------------------
static void SwapBytes32(tui8 * data)
{
	tui8 tmp;
	tmp = data[3];
	data[3] = data[0];
	data[0] = tmp;
	tmp = data[2];
	data[2] = data[1];
	data[1] = tmp;
}
//---------------------------------------------------------------------------
static void inline SwapUI32(tui32 & data) { SwapBytes32((tui8*)&data); }
//---------------------------------------------------------------------------
static void SwapBytes16(tui8 * data)
{
	tui8 tmp;
	tmp = data[1];
	data[1] = data[0];
	data[0] = tmp;
}
//---------------------------------------------------------------------------
static void inline SwapUI16(tui16 & data) { SwapBytes16((tui8*)&data); }
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Integer reader
//---------------------------------------------------------------------------
static tui32 Read32(TStream *in)
{
	tui32 tmp;
	in->ReadBuffer(&tmp, 4);
	SwapUI32(tmp);
	return tmp;
}
//---------------------------------------------------------------------------
static tui16 Read16(TStream *in)
{
	tui16 tmp;
	in->ReadBuffer(&tmp, 2);
	SwapUI16(tmp);
	return tmp;
}
//---------------------------------------------------------------------------
static tui8 Read8(TStream *in)
{
	tui8 tmp;
	in->ReadBuffer(&tmp, 1);
	return tmp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Structure definitions
//---------------------------------------------------------------------------
#pragma pack(push, 1)
struct TPSDFileHeader
{
	tui8 Signature[4];  // always "8BPS"
	tui16 Version; // BE; always 1
	tui8 Reserved[6]; // always zero
	tui16 Channels; // BE
	tui32 Rows; // BE
	tui32 Columns; // BE
	tui16 Depth; // BE
	tui16 Mode; // BE

	void ToLittleEndian()
	{
		SwapBytes16((tui8*)&Version);
		SwapBytes16((tui8*)&Channels);
		SwapBytes16((tui8*)&Depth);
		SwapBytes16((tui8*)&Mode);
		SwapBytes32((tui8*)&Rows);
		SwapBytes32((tui8*)&Columns);
	}
};

struct TPSDChannelInfo
{
	ti16 ID;
	tui32 Length;
};

struct TPSDLayerRecord
{
	ti32 Top;
	ti32 Left;
	ti32 Bottom;
	ti32 Right;
	tui16 Channels;

	TPSDChannelInfo ChannelInfo[5];

	tui8 BlendSig[4];
	tui8 BlendMode[4];

	tui8 Opacity;
	tui8 Clipping;
	tui8 Flags;

	tui8 Reserved;
};

#pragma pack(pop)
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 'normal' blend function
//---------------------------------------------------------------------------
static unsigned char OpacityOnOpacityTable[256*256];
static unsigned char NegativeMulTable[256*256];
static bool TableInit = false;
#define ALPHA_BLEND(s, d, opa) (((d)*(255-(opa)) + (s)*(opa)) * ((1<<20) / 255) >> 20)
//---------------------------------------------------------------------------
static void InitTable()
{
	if(TableInit) return;

	for(int a = 0; a<256; a++)
	{
		for(int b = 0; b<256; b++)
		{
			float c;
			int ci;
			int addr = b*256+ a;

			if(a)
			{
				float at = a/255.0, bt = b/255.0;
				c = bt / at;
				c /= (1.0 - bt + c);
				ci = (int)(c*255);
				if(ci>=256) ci = 255; /* will not overflow... */
			}
			else
			{
				ci=255;
			}

			OpacityOnOpacityTable[addr]=(unsigned char)ci;
				/* higher byte of the index is source opacity */
				/* lower byte of the index is destination opacity */

			NegativeMulTable[addr] = (unsigned char)
				( 255 - (255-a)*(255-b)/ 255 ); 
		}
	}

	TableInit = true;
}
//---------------------------------------------------------------------------
static void NormalBlendFunc(tui32 *dest, tui32 *src, int len)
{
	InitTable();

	while(len--)
	{
		tui32 s = *src;
		tui32 d = *dest;
		tui32 r;
		int addr = ((s >> 16) & 0xff00) + (d >> 24); // table addr
		r = NegativeMulTable[addr] << 24;
		int sopa = OpacityOnOpacityTable[addr];

		int sv, dv;
		sv = s & 0xff;
		dv = d & 0xff;
		r |= ALPHA_BLEND(sv, dv, sopa);
		sv = (s & 0xff00)>>8;
		dv = (d & 0xff00)>>8;
		r |= (ALPHA_BLEND(sv, dv, sopa) << 8);
		sv = (s & 0xff0000)>>16;
		dv = (d & 0xff0000)>>16;
		r |= (ALPHA_BLEND(sv, dv, sopa) << 16);
		*dest = r;

		src++;
		dest++;
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// 'additive' blend function
//---------------------------------------------------------------------------
static inline int satadd(int a, int b)
{
	int r = a + b;
	if(r > 255) r = 255;
	r |= -(r > 255);
	return r & 0xff;
}
static inline int ratio(int a, int b, int ratio)
{
	// return (b*ratio + a*(255-ratio))  / 255
	return  ((b*ratio + a*(255-ratio))  * ((1<<20) / 255)) >> 20;
}
static void AdditiveBlendFunc(tui32 * dest , tui32 * src, int len)
{
	while(len--)
	{
		tui32 s = *src;
		tui32 d = *dest;

		int opa = s >> 24;

		*dest =
			(ratio((d    )&0xff, satadd((d    )&0xff, (s    )&0xff), opa)    )+
			(ratio((d>> 8)&0xff, satadd((d>> 8)&0xff, (s>> 8)&0xff), opa)<< 8)+
			(ratio((d>>16)&0xff, satadd((d>>16)&0xff, (s>>16)&0xff), opa)<<16)+
			(d & 0xff000000) ;
		src++;
		dest++;
	}
}
//---------------------------------------------------------------------------
static void AdditiveCopyFunc(tui32 * dest , tui32 * src, int len)
{
	while(len--)
	{
		tui32 s = *src;
		tui32 d = *dest;

		int opa = s >> 24;

		*dest =	((((s    )&0xff) * opa / 255)    ) +
				((((s>> 8)&0xff) * opa / 255)<<8 ) +
				((((s>>16)&0xff) * opa / 255)<<16) ;
		src++;
		dest++;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// copy rectangle
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void CopyBitmapRect(Graphics::TBitmap *dest, int x, int y,
	Graphics::TBitmap *ref, TRect refrect, int mode)
{
	// Copy bitmap rectangle

	// bound check
	int bmpw, bmph;

	bmpw = ref->Width;
	bmph = ref->Height;

	if(refrect.left < 0)
		x -= refrect.left, refrect.left = 0;
	if(refrect.right > bmpw)
		refrect.right = bmpw;

	if(refrect.left >= refrect.right) return;

	if(refrect.top < 0)
		y -= refrect.top, refrect.top = 0;
	if(refrect.bottom > bmph)
		refrect.bottom = bmph;

	if(refrect.top >= refrect.bottom) return;

	bmpw = dest->Width;
	bmph = dest->Height;

	TRect rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + (refrect.right - refrect.left);
	rect.bottom = rect.top + (refrect.bottom - refrect.top);

	if(rect.left < 0)
	{
		refrect.left += -rect.left;
		rect.left = 0;
	}

	if(rect.right > bmpw)
	{
		refrect.right -= (rect.right - bmpw);
		rect.right = bmpw;
	}

	if(refrect.left >= refrect.right) return; // not drawable

	if(rect.top < 0)
	{
		refrect.top += -rect.top;
		rect.top = 0;
	}

	if(rect.bottom > bmph)
	{
		refrect.bottom -= (rect.bottom - bmph);
		rect.bottom = bmph;
	}

	if(refrect.top >= refrect.bottom) return; // not drawable


	// transfer
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	for(int y = 0; y < h; y ++)
	{
		tui32 *d = (tui32*)dest->ScanLine[y + rect.top];
		tui32 *s = (tui32*)ref->ScanLine[y + refrect.top];
		if(mode == 0)
			memmove(d+ rect.left, s + refrect.left, w * sizeof(tui32));
		else if(mode == 1)
			NormalBlendFunc(d+ rect.left, s + refrect.left, w);
		else if(mode == 2)
			AdditiveCopyFunc(d + rect.left, s + refrect.left, w);
		else if(mode == 3)
			AdditiveBlendFunc(d + rect.left, s + refrect.left, w);
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Set opacity to image
//---------------------------------------------------------------------------
static void SetBitmapOpacity(Graphics::TBitmap *bmp, int opa)
{
	// bmp must be a 32bpp bitmap

	int w = bmp->Width;
	int h = bmp->Height;

	opa <<= 20;
	opa /= 255;

	for(int y = 0; y < h; y++)
	{
		tui32 *scl = (tui32*)bmp->ScanLine[y];
		for(int x = 0; x< w; x++)
		{
			tui32 d = scl[x];
			int o = d >> 24;
			o *= opa;
			o >>= 20;
			d &= 0xffffff;
			d |= o << 24;
			scl[x] = d;
		}
	}

}
//---------------------------------------------------------------------------
/*
static void SetBitmapAdditiveOpacity(Graphics::TBitmap *bmp, int opa)
{
	// bmp must be a 32bpp bitmap

	int w = bmp->Width;
	int h = bmp->Height;

	opa <<= 20;
	opa /= 255;

	for(int y = 0; y < h; y++)
	{
		tui32 *scl = (tui32*)bmp->ScanLine[y];
		for(int x = 0; x< w; x++)
		{
			tui32 d = scl[x];

			scl[x] =
				((((d    ) & 0xff) * opa >> 20)     )+
				((((d>> 8) & 0xff) * opa >> 20) << 8)+
				((((d>>16) & 0xff) * opa >> 20) <<16);

		}
	}
}
*/
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// ConvertAlphaToAddAlpha
//---------------------------------------------------------------------------
void ConvertAlphaToAddAlpha(Graphics::TBitmap *in)
{
	// convert alpha to additive alpha

	int w = in->Width;
	int h = in->Height;

	for(int y = 0; y < h; y++)
	{
		DWORD * p = (DWORD *)in->ScanLine[y];
		for(int x = 0; x < w; x++)
		{
			DWORD s = p[x];
			int alpha = (s >> 24) & 0xff;

			p[x] = (s & 0xff000000) +
				((((s    ) & 0xff)  * alpha / 255)    )+
				((((s>>8 ) & 0xff)  * alpha / 255)<<8 )+
				((((s>>16) & 0xff)  * alpha / 255)<<16) ;
		}
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Read pixel data
//---------------------------------------------------------------------------
static Graphics::TBitmap *ReadPixelData30(TStream *Stream,
	int w, int h)
{
	// read Photoshop 3.0 data
	// channel count must be 3

	Graphics::TBitmap *bmp = new Graphics::TBitmap();

	try
	{
		// set BMP size
		bmp->PixelFormat = pf24bit;
		bmp->Width = w;
		bmp->Height = h;

		// for each channel
		int mode = Read16(Stream);

		if(mode == 0)
		{
			// uncompressed image
			throw EDeePSD("TDeePSD: Uncompressed Photoshop 3.0 image is not supported." + AnsiString(mode));
		}
		else if(mode == 1)
		{
			// RLE compressed image
			tui16 * linelen = NULL;
			tui8 * linebuf = NULL;
			try
			{
				int h3 = h * 3;
				linelen = new tui16[h3];
				tui16 * ll = linelen;
				Stream->ReadBuffer(linelen, h3 * sizeof(tui16));
				for(int i = 0; i < h3; i++)
					SwapBytes16((tui8*)(linelen + i));

				for(int c = 0; c < 3; c++)
				{
					for(int y = 0; y < h; y++)
					{
						tui8 *scl = (tui8*)bmp->ScanLine[y] + "\x02\x01\x00"[c];

						linebuf = new tui8[*ll];

						Stream->ReadBuffer(linebuf, *ll);

						ll++;

						int x = 0;
						tui8 *lp = linebuf;
						while(x < w)
						{
							int n;
							n = (int)*lp;
							lp++;
							if(n >= 128) n -= 256;
							if(n == -128)
							{
								;// ?? no operation
							}
							else if(n < 0) // run length
							{
								n = -n + 1;
								tui8 v = *lp;
								lp++;
								while(n--)
									*scl = v, scl += 3, x++;
							}
							else
							{
								// literal copy
								n++;
								while(n--)
									*scl = *(lp++), scl += 3, x++;
							}

						}

						delete [] linebuf;
						linebuf = NULL;
					}
				}
			}
			catch(...)
			{
				if(linelen) delete [] linelen;
				if(linebuf) delete [] linebuf;
				throw;
			}
			if(linelen) delete [] linelen;
			if(linebuf) delete [] linebuf;
		}
		else
		{
			throw EDeePSD("TDeePSD: The image has "
				"unsupported compression mode : " + AnsiString(mode));
		}
	}
	catch(...)
	{
		delete bmp;
		throw;
	}

	return bmp;
}
//---------------------------------------------------------------------------
static Graphics::TBitmap *ReadPixelData(TStream *Stream, TPSDLayerRecord *lr,
	int laynum)
{
	int w = lr->Right - lr->Left;
	int h = lr->Bottom - lr->Top;
	if(w < 0) w = -w; // is this needed?
	if(h < 0) h = -h;

	Graphics::TBitmap *bmp = new Graphics::TBitmap();

	try
	{
		// set BMP size
		bmp->PixelFormat = pf32bit;
		bmp->Width = w;
		bmp->Height = h;

		// clear BMP
		for(int y = 0; y < h; y++)
		{
			DWORD *scl = (DWORD*)bmp->ScanLine[y];
			for(int x = 0; x < w; x++)
			{
				scl[x] = 0xff000000;
			}
		}

		// for each channel
		for(int ch = 0; ch < lr->Channels; ch++)
		{
			int readstart = Stream->Position;
			int mode = Read16(Stream);
			int bitpos;
			bool readskip = false;
			tui32 bitmask;
			switch(lr->ChannelInfo[ch].ID)
			{
			case -2:
				readskip = true; break; // user supplied layer mask
			case -1:
				bitpos = 24; bitmask = 0x00ffffff; break; // alpha
			case 0:
				bitpos = 16; bitmask = 0xff00ffff; break; // R
			case 1:
				bitpos = 8;  bitmask = 0xffff00ff; break; // G
			case 2:
				bitpos = 0;  bitmask = 0xffffff00; break; // B
			default:
				readskip = true; break; // unknown channel ID
//				throw EDeePSD("TDeePSD: Layer #" + AnsiString(laynum) + " has "
//					"unsupported channel ID : " +
//					AnsiString((int)lr->ChannelInfo[ch].ID));
			}

			if(!readskip)
			{

				if(mode == 0)
				{
					// uncompressed image
					tui8 * data = new tui8[w * h];
					try
					{
						Stream->ReadBuffer(data, w * h);

						tui8 *p = data;
						for(int y = 0; y < h; y++)
						{
							tui32 * scl = (tui32*)bmp->ScanLine[y];
							for(int x = 0; x < w; x++)
							{
								tui32 d = scl[x];
								d &= bitmask;
								d |= (*p<<bitpos);
								scl[x] = d;
								p++;
							}
						}
					}
					catch(...)
					{
						delete [] data;
						throw;
					}
					delete [] data;
				}
				else if(mode == 1)
				{
					// RLE compressed image
					tui16 * linelen = NULL;
					tui8 * linebuf = NULL;
					try
					{
						linelen = new tui16[h];
						Stream->ReadBuffer(linelen, h * sizeof(tui16));
						for(int i = 0; i < h; i++)
							SwapBytes16((tui8*)(linelen + i));
						for(int y = 0; y < h; y++)
						{
							tui32 *scl = (tui32*)bmp->ScanLine[y];
							linebuf = new tui8[linelen[y]];

							Stream->ReadBuffer(linebuf, linelen[y]);

							int x = 0;
							tui8 *lp = linebuf;
							while(x < w)
							{
								int n;
								n = (int)*lp;
								lp++;
								if(n >= 128) n -= 256;
								if(n == -128)
								{
									;// ?? no operation
								}
								else if(n < 0) // run length
								{
									n = -n + 1;
									tui32 v = *lp << bitpos;
									lp++;
									while(n--)
									{
										tui32 d = scl[x];
										d &= bitmask;
										d |= v;
										scl[x] = d;
										x++;
									}
								}
								else
								{
									// literal copy
									n++;
									while(n--)
									{
										tui32 d = scl[x];
										d &= bitmask;
										d |= (*lp << bitpos);
										lp++;
										scl[x] = d;
										x++;
									}
								}

							}

							delete [] linebuf;
							linebuf = NULL;
						}
					}
					catch(...)
					{
						if(linelen) delete [] linelen;
						if(linebuf) delete [] linebuf;
						throw;
					}
					if(linelen) delete [] linelen;
					if(linebuf) delete [] linebuf;
				}
				else
				{
					throw EDeePSD("TDeePSD: Layer #" + AnsiString(laynum) + " has "
						"unsupported compression mode : " + AnsiString(mode));
				}
			}

			Stream->Position = readstart + lr->ChannelInfo[ch].Length;
		}
	}
	catch(...)
	{
		delete bmp;
		throw;
	}

	return bmp;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TDeePSD
//---------------------------------------------------------------------------
__fastcall TDeePSD::TDeePSD(void)
{
	// constructor
	FLayerMode = "";
}
//---------------------------------------------------------------------------
__fastcall TDeePSD::~TDeePSD(void)
{
	// desturctor

}
//---------------------------------------------------------------------------
void __fastcall TDeePSD::LoadFromStream(Classes::TStream * Stream)
{
	// LoadFromStream

	// check file header
	TPSDFileHeader header;
	Stream->ReadBuffer(&header, sizeof(header));
	header.ToLittleEndian();

	if(memcmp(&header.Signature, "8BPS", 4))
		throw EDeePSD("TDeePSD: This is not a PSD file.");

	if(header.Version != 1)
		throw EDeePSD("TDeePSD: Invalid PSD version.");

//	if(header.Channels != 4)
//		throw EDeePSD("TDeePSD: Unsupported number of channels " +
//			AnsiString((int)header.Channels)+ " (must be 4)");

	if(header.Mode != 3)
		throw EDeePSD("TDeePSD: Unsupported color mode (must be RGB)");

	if(header.Depth != 8)
		throw EDeePSD("TDeePSD: Unsupported bits per channel " +
			AnsiString((int)header.Depth) + " (must be 8)");

	// skip Color mode data section
	{
		tui32 size = Read32(Stream);
		Stream->Position += size;
	}

	// skip Image resources section
	{
		tui32 size = Read32(Stream);
		Stream->Position += size;
	}

	// Set size
	PixelFormat = pf32bit;
	Width = header.Columns;
	Height = header.Rows;

	// clear image
	for(int y = 0; y < Height; y++)
		memset(ScanLine[y], 0, Width * sizeof(tui32));

	// read layer info section
	int layinfosize = Read32(Stream); // size of layer info section; discard

	if(layinfosize)
	{

		Read32(Stream); // discard

		int nlayers;
		nlayers = (ti16)Read16(Stream);

		if(nlayers < 0) nlayers = -nlayers;

		TPSDLayerRecord * lr = new TPSDLayerRecord[nlayers];

		try
		{
			for(int lay = 0; lay < nlayers; lay++)
			{
				// for each layers
				TPSDLayerRecord *clr = lr + lay;

				clr->Top = Read32(Stream);
				clr->Left = Read32(Stream);
				clr->Bottom = Read32(Stream);
				clr->Right = Read32(Stream);
				clr->Channels = Read16(Stream);
				if(clr->Channels > 5)
					throw EDeePSD("TDeePSD: Layer #" + AnsiString(lay) + " has "
						"unsupported layer channel count " + AnsiString((int)clr->Channels) +
						" (must be smaller than 6)");
				for(int i = 0; i < clr->Channels; i++)
				{
					clr->ChannelInfo[i].ID = Read16(Stream);
					clr->ChannelInfo[i].Length = Read32(Stream);
				}

				Stream->ReadBuffer(clr->BlendSig, 4);
				if(memcmp(clr->BlendSig, "8BIM", 4))
					throw EDeePSD("TDeePSD: This file is corrupted.");

				Stream->ReadBuffer(clr->BlendMode, 4);

				clr->Opacity = Read8(Stream);
				clr->Clipping = Read8(Stream);
				clr->Flags = Read8(Stream);
				Read8(Stream); // filler

				// skip extra bytes
				tui32 extra_bytes = Read32(Stream);
				Stream->Position += extra_bytes;

			}


			// check layer mode structure
			AnsiString org_layer_mode = FLayerMode;
			bool visible_found = false;
			if(FLayerMode == "addalpha")
			{
				// Additive Alpha output mode
				bool additive_found = false;
				for(int lay = 0; lay < nlayers; lay++)
				{
					TPSDLayerRecord *clr = lr + lay;

					if(clr->Opacity != 0 && !(clr->Flags & 2))
					{

						if(!memcmp(clr->BlendMode, "norm", 4))
						{
							// alpha blend mode
							if(additive_found)
								throw EDeePSD("TDeePSD: Layer #" + AnsiString(lay) + " is normal blend mode but "
									"cannot load normal blend mode layer over linear dodge blend mode layer");
						}
						else if(!memcmp(clr->BlendMode, "lddg", 4))
						{
							// linear dodge (additive)
							additive_found = true;
						}
						else
							throw EDeePSD("TDeePSD: Layer #" + AnsiString(lay) + " has "
								"unsupported blend mode '" + AnsiString((char*)clr->BlendMode, 4) +
								"' (must be 'norm' [normal] blend or 'lddg' [linear dodge] blend)");

						visible_found = true;
					}
				}
			}
			else
			{
				// normal mode
				// we only can pile 'normal' blend mode.
				// other blend modes cannot be piled yet.

				FLayerMode = "";

				for(int lay = 0; lay < nlayers; lay++)
				{
					TPSDLayerRecord *clr = lr + lay;
					if(clr->Opacity != 0 && !(clr->Flags & 2))
					{


						if(FLayerMode != "")
						{
							if(FLayerMode != "alpha" || memcmp(clr->BlendMode, "norm", 4))
							{
								throw EDeePSD("TDeePSD: Layer #" + AnsiString(lay) + " has "
									"blend mode '" + AnsiString((char*)clr->BlendMode, 4) +
									"' but currently this PSD loader cannot pile layers more than one in this mode.");
							}
						}
						if(!memcmp(clr->BlendMode, "norm", 4))
							FLayerMode = "alpha";
						else if(!memcmp(clr->BlendMode, "lddg", 4))
							FLayerMode = "psadd";
						else if(!memcmp(clr->BlendMode, "lbrn", 4))
							FLayerMode = "pssub";
						else if(!memcmp(clr->BlendMode, "mul ", 4))
							FLayerMode = "psmul";
						else if(!memcmp(clr->BlendMode, "scrn", 4))
							FLayerMode = "psscreen";
						else if(!memcmp(clr->BlendMode, "over", 4))
							FLayerMode = "psoverlay";
						else if(!memcmp(clr->BlendMode, "hLit", 4))
							FLayerMode = "pshlight";
						else if(!memcmp(clr->BlendMode, "sLit", 4))
							FLayerMode = "psslight";
						else if(!memcmp(clr->BlendMode, "div ", 4))
							FLayerMode = "psdodge";
						else if(!memcmp(clr->BlendMode, "idiv", 4))
							FLayerMode = "psburn";
						else if(!memcmp(clr->BlendMode, "lite", 4))
							FLayerMode = "pslighten";
						else if(!memcmp(clr->BlendMode, "dark", 4))
							FLayerMode = "psdarken";
						else if(!memcmp(clr->BlendMode, "diff", 4))
							FLayerMode = "psdiff";
						else if(!memcmp(clr->BlendMode, "smud", 4))
							FLayerMode = "psexcl";
						else
							throw EDeePSD("TDeePSD: Layer #" + AnsiString(lay) + " has "
								"unsupported blend mode '" + AnsiString((char*)clr->BlendMode, 4) +
								"'");
						visible_found = true;
					}
				}

				if(org_layer_mode != "" && FLayerMode != org_layer_mode)
					throw EDeePSD("TDeePSD: Unexpected layer mode '" + FLayerMode + "'");
			}
			if(!visible_found)
				throw EDeePSD("TDeePSD: This image does not have any visible layer");

			// read each layer image
			bool firstlayer = true;
			bool additive_found = false;
			for(int lay = 0; lay < nlayers; lay++)
			{
				// for each layers again
				// read pixel data
				TPSDLayerRecord *clr = lr + lay;
				Graphics::TBitmap *layerbmp = ReadPixelData(Stream, lr + lay, lay);
				try
				{
					if(clr->Opacity != 0 && !(clr->Flags & 2))
					{
						TRect refrect;
						refrect.left = refrect.top = 0;
						refrect.right = layerbmp->Width;
						refrect.bottom = layerbmp->Height;
						// blend
						if(!memcmp(clr->BlendMode, "norm", 4))
						{
							// normal alpha blend
							if(clr->Opacity != 255) SetBitmapOpacity(layerbmp, clr->Opacity);
							if(firstlayer)
								CopyBitmapRect(this, clr->Left, clr->Top, layerbmp, refrect, 0);
							else
								CopyBitmapRect(this, clr->Left, clr->Top, layerbmp, refrect, 1);
						}
						else if(org_layer_mode == "addalpha" && !memcmp(clr->BlendMode, "lddg", 4))
						{
							// linear dodge (additive) blend
							if(clr->Opacity != 255) SetBitmapOpacity(layerbmp, clr->Opacity);
							if(!additive_found)
							{
								additive_found = true;
								ConvertAlphaToAddAlpha(this);
							}
							if(firstlayer)
								CopyBitmapRect(this, clr->Left, clr->Top, layerbmp, refrect, 2);
							else
								CopyBitmapRect(this, clr->Left, clr->Top, layerbmp, refrect, 3);
						}
						else
						{
							if(clr->Opacity != 255) SetBitmapOpacity(layerbmp, clr->Opacity);
							if(firstlayer)
								CopyBitmapRect(this, clr->Left, clr->Top, layerbmp, refrect, 0);
						}
						firstlayer = false;
					}
				}
				catch(...)
				{
					delete layerbmp;
					throw;
				}
				delete layerbmp;
			}
			if(org_layer_mode == "addalpha" && !additive_found)
				ConvertAlphaToAddAlpha(this);
		}
		catch(...)
		{
			delete [] lr;
			throw;
		}

		delete [] lr;
	}
	else
	{
		// layer info section is zero

		if(header.Channels != 3)
			throw EDeePSD("TDeePSD: Unsupported number of channels " +
				AnsiString((int)header.Channels)+ " (must be 3)");

		Graphics::TBitmap *layerbmp =
			ReadPixelData30(Stream, header.Columns, header.Rows);

		try
		{
			Assign(layerbmp);
		}
		catch(...)
		{
			delete layerbmp;
			throw;
		}

		delete layerbmp;
		FLayerMode = "alpha"; // always assumes alpha
	}
}
//---------------------------------------------------------------------------
void __fastcall TDeePSD::SaveToStream(Classes::TStream * Stream)
{
}
//---------------------------------------------------------------------------
static void InitTDeePSD()
{
	TPicture::RegisterFileFormat(
		__classid(TPicture), "psd", "Photoshop image",
		__classid(TDeePSD));
}
static void UninitTDeePSD()
{
	TPicture::UnregisterGraphicClass(__classid(TPicture),__classid(TDeePSD));
}
#pragma startup InitTDeePSD
#pragma exit UninitTDeePSD
//---------------------------------------------------------------------------

//#pragma package(smart_init)
