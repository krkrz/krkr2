//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DeePNGUnit.h"
#pragma option push -pr
#include <libpng\png.h> // libpng
#pragma option pop


//---------------------------------------------------------------------------
// Check for pure virtual functions
//---------------------------------------------------------------------------
static void TestDeePNG(void)
{
	new TDeePNG();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Check for grayscale bitmap
//---------------------------------------------------------------------------
static bool __fastcall IsGrayscaleBitmap(Graphics::TBitmap *dib)
{
	int psize;
	int icol;

	switch(dib->PixelFormat)
	{
	case pfDevice:
		return false;
	case pf1bit:
		psize = 2; break;
	case pf4bit:
		psize = 16; break;
	case pf8bit:
		psize = 256; break;
	default:
		return false;
	}

	PALETTEENTRY palentry[256];
	if(GetPaletteEntries(dib->Palette, 0, psize, palentry) != (UINT)psize)
		return false;


	int c = 0;
	icol = 255/(psize-1);
	for(int i = 0; i < psize; i++)
	{
		if(c >= 256) c = 255;
		if(
			palentry[i].peRed != c ||
			palentry[i].peGreen != c ||
			palentry[i].peBlue != c) return false;
		c += icol;
	}

	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Get bitmap color depth
//---------------------------------------------------------------------------
static int __fastcall GetBitmapColorDepth(Graphics::TBitmap *bmp)
{
	switch(bmp->PixelFormat)
	{
	case pfDevice:
		return 0;
	case pf1bit:
		return 1;
	case pf4bit:
		return 4;
	case pf8bit:
		return 8;
	case pf15bit:
		return 15;
	case pf16bit:
		return 16;
	case pf24bit:
		return 24;
	case pf32bit:
		return 32;
	default:
		return 0;
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Set color depth to bitmap
//---------------------------------------------------------------------------
static int __fastcall SetColorDepth(Graphics::TBitmap *dib, int bit)
{
	int psize;

	if(bit == 1)
	{
		dib->PixelFormat = pf1bit;
		psize = 2;
	}
	else if(bit == 2)
	{
		dib->PixelFormat = pf4bit; // 2bpp is not supported on Windows
		psize = 4;
	}
	else if(bit == 4)
	{
		dib->PixelFormat = pf4bit;
		psize = 16;
	}
	else if(bit == 8)
	{
		dib->PixelFormat = pf8bit;
		psize = 256;
	}
	else if(bit == 16)
	{
		dib->PixelFormat = pf8bit; // 16Bit grayscale is not supported
		psize = 256;
	}
	else
	{
		return 0;
	}

	return psize;
}
//---------------------------------------------------------------------------
// Set grayscale palette to bitmap
//---------------------------------------------------------------------------
void __fastcall SetGrayscalePalette(Graphics::TBitmap *dib, int bit)
{
	int psize;
	int icol;

	psize = SetColorDepth(dib, bit);

	icol = 255/(psize-1);

	int i;
	int c = 0;

#pragma pack(push, 1)
	struct
	{
		WORD	palVersion;
		WORD	palNumEntries;
		PALETTEENTRY entry[256];
	} pal;
#pragma pack(pop)

	pal.palVersion = 0x300;
	pal.palNumEntries = psize;

	for(i = 0; i < psize; i++)
	{
		if(c >= 256) c = 255;
		pal.entry[i].peRed = pal.entry[i].peGreen = pal.entry[i].peBlue = (BYTE)c;
		pal.entry[i].peFlags = 0;
		c += icol;
	}

	dib->Palette = CreatePalette((const LOGPALETTE*)&pal);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// function - user_malloc_fn
static png_voidp __fastcall DeePNG_malloc(png_structp ps, png_size_t size)
{
	return System::SysGetMem(size);
}
//---------------------------------------------------------------------------
// function - user_free_fn
static void __fastcall DeePNG_free (png_structp ps,void*/* png_structp*/ mem)
{
	System::SysFreeMem(mem);
}
//---------------------------------------------------------------------------
// function - user_error_fn
static void __fastcall DeePNG_error (png_structp ps, png_const_charp msg)
{
	throw EDeePNG(msg);
}
//---------------------------------------------------------------------------
// function - user_warning_fn
static void __fastcall DeePNG_warning (png_structp ps, png_const_charp msg)
{
	// nothing to do
}
//---------------------------------------------------------------------------
// function - user_read_data
static void __fastcall DeePNG_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	((Classes::TStream *)png_get_io_ptr(png_ptr))->ReadBuffer((void*)data, length);
}
//---------------------------------------------------------------------------
// function - user_write_data
static void __fastcall DeePNG_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	((Classes::TStream *)png_get_io_ptr(png_ptr))->WriteBuffer((void*)data, length);
}
//---------------------------------------------------------------------------
// function - user_flush
static void __fastcall DeePNG_flush(png_structp png_ptr)
{
	// nothing to do
}
//---------------------------------------------------------------------------
// user supplied function - read_row_callback
static void __fastcall DeePNG_read_row_callback(png_structp png_ptr, png_uint_32 row, int pass)
{

}
//---------------------------------------------------------------------------
// read_chunk_callback
static int __fastcall PNG_read_chunk_callback(png_structp png_ptr,png_unknown_chunkp chunk)
{
	// handle vpAg chunk (this will contain the virtual page size of the image)
	// vpAg chunk can be embeded by ImageMagick -trim option etc.
	// we don't care about how the chunk bit properties are being provided.
	if(	(chunk->name[0] == 0x76/*'v'*/ || chunk->name[0] == 0x56/*'V'*/) &&
		(chunk->name[1] == 0x70/*'p'*/ || chunk->name[1] == 0x50/*'P'*/) &&
		(chunk->name[2] == 0x61/*'a'*/ || chunk->name[2] == 0x41/*'A'*/) &&
		(chunk->name[3] == 0x67/*'g'*/ || chunk->name[3] == 0x47/*'G'*/) && chunk->size >= 9)
	{
		TDeePNG * deepng =
			reinterpret_cast<TDeePNG *>(png_get_user_chunk_ptr(png_ptr));
		// vpAg found
		/*
			uint32 width
			uint32 height
			uchar unit
		*/
		// be careful because the integers are stored in network byte order
		#define PNG_read_be32(a) (((unsigned long)(a)[0]<<24)+\
			((unsigned long)(a)[1]<<16)+((unsigned long)(a)[2]<<8)+\
			((unsigned long)(a)[3]))
		unsigned long width  = PNG_read_be32(chunk->data+0);
		unsigned long height = PNG_read_be32(chunk->data+4);
		unsigned char unit   = chunk->data[8];

		// set vpag information
		deepng->SetVirtualPage(width, height, unit);

		return 1; // chunk read success
	}
	return 0; // did not recognize
}


//---------------------------------------------------------------------------
// TDeePNG
//---------------------------------------------------------------------------
__fastcall TDeePNG::TDeePNG(void)
{
	// constructor
	ofs_set = false;
	vpag_set = false;
}
//---------------------------------------------------------------------------
__fastcall TDeePNG::~TDeePNG(void)
{
	// desturctor
}
//---------------------------------------------------------------------------
void __fastcall TDeePNG::LoadFromStream(Classes::TStream * Stream)
{
	// LoadFromStream

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_infop end_info = NULL;

	png_bytep *row_pointers = NULL;
	BYTE *image = NULL;

	png_uint_32 i;

	try
	{
		// create png_struct
		png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
			(png_voidp)this, DeePNG_error, DeePNG_warning,
			(png_voidp)this, DeePNG_malloc, DeePNG_free);

		// set read_chunk_callback
		png_set_read_user_chunk_fn(png_ptr,
			reinterpret_cast<void*>(this),
			PNG_read_chunk_callback);
		png_set_keep_unknown_chunks(png_ptr, 2, NULL, 0);
			// keep only if safe-to-copy chunks, for all unknown chunks

		// create png_info
		info_ptr = png_create_info_struct(png_ptr);

		// create end_info
		end_info = png_create_info_struct(png_ptr);

		// set stream input functions
		png_set_read_fn(png_ptr, (voidp)Stream, DeePNG_read_data);

		// set read_row_callback
		png_set_read_status_fn(png_ptr, DeePNG_read_row_callback);

		// call png_read_info
		png_read_info(png_ptr, info_ptr);

		// retrieve IHDR
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type, compression_type, filter_type;
		png_get_IHDR(png_ptr,info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, &compression_type, &filter_type);

		// expand palletted image which has transparent color, to 32bpp
		if (png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS))
		{
			png_set_expand(png_ptr);
			color_type=PNG_COLOR_TYPE_RGB_ALPHA;
		}

		// analyse IHDR ( color_type )
		switch(color_type)
		{
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			PixelFormat=pf32bit;
			break;
		case PNG_COLOR_TYPE_GRAY:
			// w/b
			SetGrayscalePalette(this,bit_depth);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			SetColorDepth(this,bit_depth);
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			PixelFormat=pf32bit;
			break;
		case PNG_COLOR_TYPE_RGB:
			PixelFormat=pf24bit;
			break;
		default:
			throw EDeePNG("EDeePNG : Non-supported color type.");
		}

		// retrieve offset information
		png_int_32 offset_x, offset_y;
		int offset_unit_type;
		if(png_get_oFFs(png_ptr, info_ptr, &offset_x, &offset_y, &offset_unit_type))
		{
			ofs_x = offset_x;
			ofs_y = offset_y;
			ofs_unit = offset_unit_type;
			ofs_set = true;
		}
		else
		{
			ofs_set = false;
		}

		// check size
		if(width>=65536 || height>=65536)
		{
			throw EDeePNG("EDeePNG : Too large image size.");
		}


		// retrieve palette
		if(color_type == PNG_COLOR_TYPE_PALETTE)
		{
			int num_palette;
			png_color *palette = NULL;
			png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

			int i;
		#pragma pack(push, 1)
			struct
			{
				WORD	palVersion;
				WORD	palNumEntries;
				PALETTEENTRY entry[256];
			} pal;
		#pragma pack(pop)
			pal.palVersion = 0x300;
			pal.palNumEntries = num_palette;
			for(i = 0; i < num_palette; i++)
			{
				pal.entry[i].peRed = palette[i].red;
				pal.entry[i].peGreen = palette[i].green;
				pal.entry[i].peBlue = palette[i].blue;
				pal.entry[i].peFlags = 0;
			}
			Palette = CreatePalette((const LOGPALETTE*)&pal);
		}

		// collapse 16bit precision data to 8bit
		if(bit_depth == 16) png_set_strip_16(png_ptr);

		// change color component order
		if (color_type == PNG_COLOR_TYPE_RGB ||
			color_type == PNG_COLOR_TYPE_RGB_ALPHA)
				png_set_bgr(png_ptr);

		// call png_read_update_info ...
		png_read_update_info(png_ptr, info_ptr);

		// set size
		Width=width, Height=height;

		// allocate memory for row_pointers
		row_pointers = new png_bytep[height];
		png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		image = new BYTE[rowbytes*height];
		for(i = 0;i < height; i++)
		{
			row_pointers[i] = image + i*rowbytes;
		}

		// load image
		png_read_image(png_ptr, row_pointers);

		// finish loading image
		png_read_end(png_ptr, info_ptr);

		// set image to ScanLines

		BYTE *imageptr = image;
		if(color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			// IA IA IA ....

			for(i = 0; i < height; i++)
			{
				BYTE *scanptr = (BYTE*)ScanLine[i];
				png_uint_32 j;
				for(j = 0; j < width; j++)
				{
					BYTE i = *(imageptr++);
					scanptr[0] = i;
					scanptr[1] = i;
					scanptr[2] = i;
					scanptr[3] = *(imageptr++);
					scanptr += 4;
				}
			}
		}
		else
		{
			// intact copy

			for(i = 0; i < height; i++)
			{
				BYTE *scanptr = (BYTE*)ScanLine[i];
				memcpy(scanptr, imageptr, rowbytes);
				imageptr += rowbytes;
			}


		}
	}
	catch(...)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		if(row_pointers) delete [] row_pointers;
		if(image) delete [] image;
		throw;
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	if(row_pointers) delete [] row_pointers;
	if(image) delete [] image;
}
//---------------------------------------------------------------------------
void __fastcall TDeePNG::SaveToStream(Classes::TStream * Stream)
{
	// SaveToStream method
	// warning: this method would change the pixelformat to pf24bit
	// if the pixelformat is pfDevice, pf15bit, pf16bit, or pfCustom.


	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row_buffer = NULL;

	try
	{
		// create png_struct
		png_ptr = png_create_write_struct_2
			(PNG_LIBPNG_VER_STRING,
			(png_voidp)this, DeePNG_error, DeePNG_warning,
			(png_voidp)this, DeePNG_malloc, DeePNG_free);


		// create png_info
		info_ptr = png_create_info_struct(png_ptr);

		// set write function
		png_set_write_fn(png_ptr, (png_voidp)Stream,
			DeePNG_write_data, DeePNG_flush);


		// set IHDR
		if(PixelFormat == pfDevice || PixelFormat == pf15bit ||
			PixelFormat == pf16bit || PixelFormat == pfCustom)
			PixelFormat = pf24bit;

		bool grayscale = IsGrayscaleBitmap(this);
		int colordepth = GetBitmapColorDepth(this);

		int w = Width;
		int h = Height;

		int color_type;
		if(grayscale)
			color_type = PNG_COLOR_TYPE_GRAY;
		else if(colordepth <= 8)
			color_type = PNG_COLOR_TYPE_PALETTE;
		else if(colordepth == 32)
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		else
			color_type = PNG_COLOR_TYPE_RGB;

		png_set_IHDR(png_ptr, info_ptr, w, h,
			colordepth < 8 ? colordepth : 8,
			color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

		// set oFFs
		if(ofs_set)
			png_set_oFFs(png_ptr, info_ptr, ofs_x, ofs_y, ofs_unit);

		// set palette
		if(color_type == PNG_COLOR_TYPE_PALETTE)
		{
			png_color *palette = (png_colorp)png_malloc(png_ptr, 256 * sizeof(png_color));
			PALETTEENTRY palentry[256];
			int num_palette = GetPaletteEntries(Palette, 0, (1<<colordepth), palentry);
			for(int i = 0; i < num_palette; i++)
			{
				palette[i].red = palentry[i].peRed;
				palette[i].green = palentry[i].peGreen;
				palette[i].blue = palentry[i].peBlue;
			}
			png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
		}

		// write info
		png_write_info(png_ptr, info_ptr);

		// write vpAg private chunk
		if(vpag_set)
		{
			png_byte png_vpAg[5] = {118, 112,  65, 103, '\0'};
			unsigned char vpag_chunk_data[9];
		#define PNG_write_be32(p, a) (\
			((unsigned char *)(p))[0] = (unsigned char)(((a) >>24) & 0xff), \
			((unsigned char *)(p))[1] = (unsigned char)(((a) >>16) & 0xff), \
			((unsigned char *)(p))[2] = (unsigned char)(((a) >> 8) & 0xff), \
			((unsigned char *)(p))[3] = (unsigned char)(((a)     ) & 0xff)  )
			PNG_write_be32(vpag_chunk_data,     vpag_w);
			PNG_write_be32(vpag_chunk_data + 4, vpag_h);
			vpag_chunk_data[8] = (unsigned char)vpag_unit;
			png_write_chunk(png_ptr, png_vpAg, vpag_chunk_data, 9);
		}

/*
		// change RGB order
		if(color_type = PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		   png_set_bgr(png_ptr);
		// ???? why this does not work ?
*/
		// write image
		if(color_type == PNG_COLOR_TYPE_RGB)
		{
			row_buffer = (png_bytep)png_malloc(png_ptr, 3 * w + 6);
			try
			{
				png_bytep row_pointer = row_buffer;

				for(int i = 0; i < h; i++)
				{
					png_bytep in = (png_bytep)ScanLine[i];
					png_bytep out = row_buffer;
					for(int x = 0; x < w; x++)
					{
						out[2] = in[0];
						out[1] = in[1];
						out[0] = in[2];
						out += 3;
						in += 3;
					}
					png_write_row(png_ptr, row_pointer);
				}
			}
			catch(...)
			{
				png_free(png_ptr, row_buffer);
				throw;
			}
			png_free(png_ptr, row_buffer);
		}
		else if(color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			row_buffer = (png_bytep)png_malloc(png_ptr, 4 * w + 6);
			try
			{
				png_bytep row_pointer = row_buffer;

				for(int i = 0; i < h; i++)
				{
					png_bytep in = (png_bytep)ScanLine[i];
					png_bytep out = row_buffer;
					for(int x = 0; x < w; x++)
					{
						out[2] = in[0];
						out[1] = in[1];
						out[0] = in[2];
						out[3] = in[3];
						out += 4;
						in += 4;
					}
					png_write_row(png_ptr, row_pointer);
				}
			}
			catch(...)
			{
				png_free(png_ptr, row_buffer);
				throw;
			}
			png_free(png_ptr, row_buffer);
		}
		else
		{
			for(int i = 0; i < h; i++)
			{
				png_bytep row_pointer = (png_bytep)ScanLine[i];
				png_write_row(png_ptr, row_pointer);
			}
		}


		// finish writing
		png_write_end(png_ptr, info_ptr);

	}
	catch(...)
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		throw;
	}

	png_destroy_write_struct(&png_ptr, &info_ptr);
}
//---------------------------------------------------------------------------
void TDeePNG::SetTags(TStringList *tags)
{
	if(tags)
	{
		AnsiString offs_x = tags->Values["offs_x"];
		AnsiString offs_y = tags->Values["offs_y"];
		AnsiString offs_unit = tags->Values["offs_unit"];
		if(offs_x != "" && offs_y != "" &&
			(offs_unit == "pixel" || offs_unit == "micrometer"))
		{
			int unit ;
			if(offs_unit == "pixel")
				unit = PNG_OFFSET_PIXEL;
			else if(offs_unit == "micrometer")
				unit = PNG_OFFSET_MICROMETER;
			SetOffset(offs_x.ToInt(), offs_y.ToInt(), unit);
		}
	}

	if(tags)
	{
		AnsiString vpag_w = tags->Values["vpag_w"];
		AnsiString vpag_h = tags->Values["vpag_h"];
		AnsiString vpag_unit = tags->Values["vpag_unit"];
		if(vpag_w != "" && vpag_h != "" &&
			(vpag_unit == "pixel" || vpag_unit == "micrometer"))
		{
			int unit ;
			if(vpag_unit == "pixel")
				unit = PNG_OFFSET_PIXEL;
			else if(vpag_unit == "micrometer")
				unit = PNG_OFFSET_MICROMETER;
			SetVirtualPage(vpag_w.ToInt(), vpag_h.ToInt(), unit);
		}
	}
}
//---------------------------------------------------------------------------
void TDeePNG::AppendTags(TStringList *tags)
{
	if(tags && ofs_set)
	{
		tags->Append("offs_x=" + AnsiString(ofs_x));
		tags->Append("offs_y=" + AnsiString(ofs_y));
		switch(ofs_unit)
		{
		case PNG_OFFSET_PIXEL:
			tags->Append("offs_unit=pixel");
			break;
		case PNG_OFFSET_MICROMETER:
			tags->Append("offs_unit=micrometer");
			break;
		default:
			tags->Append("offs_unit=unknown");
			break;
		}
	}

	if(tags && vpag_set)
	{
		tags->Append("vpag_w=" + AnsiString(vpag_w));
		tags->Append("vpag_h=" + AnsiString(vpag_h));
		switch(vpag_unit)
		{
		case PNG_OFFSET_PIXEL:
			tags->Append("vpag_unit=pixel");
			break;
		case PNG_OFFSET_MICROMETER:
			tags->Append("vpag_unit=micrometer");
			break;
		default:
			tags->Append("vpag_unit=unknown");
			break;
		}
	}
}
//---------------------------------------------------------------------------
static void InitTDeePNG()
{
	TPicture::RegisterFileFormat(
		__classid(TPicture), "png", "Portable Network Graphic",
		__classid(TDeePNG));
}
static void UninitTDeePNG()
{
	TPicture::UnregisterGraphicClass(__classid(TPicture),__classid(TDeePNG));
}
#pragma startup InitTDeePNG
#pragma exit UninitTDeePNG
//---------------------------------------------------------------------------

//#pragma package(smart_init)
