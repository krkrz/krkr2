//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "slide.h"

#define BLOCK_HEIGHT 4
//---------------------------------------------------------------------------
static void WriteInt32(long num, TStream *out)
{
	char buf[4];
	buf[0] = num & 0xff;
	buf[1] = (num >> 8) & 0xff;
	buf[2] = (num >> 16) & 0xff;
	buf[3] = (num >> 24) & 0xff;
	out->WriteBuffer(buf, 4);
}
//---------------------------------------------------------------------------
static void Compress(Graphics::TBitmap *bmp, TStream * out)
{
	// compress 'bmp' to 'out'.
	// bmp will be modified (destroyed).

	int colors;

	// check pixel format
	switch(bmp->PixelFormat)
	{
	case pfDevice:		bmp->PixelFormat = pf24bit;		break;
	case pf1bit:		bmp->PixelFormat = pf8bit;		break;
	case pf4bit:		bmp->PixelFormat = pf8bit;		break;
	case pf8bit:										break;
	case pf15bit:		bmp->PixelFormat = pf24bit;		break;
	case pf16bit:		bmp->PixelFormat = pf24bit;		break;
	case pf24bit:										break;
	case pf32bit:										break;
	default: return;
	}

	switch(bmp->PixelFormat)
	{
	case pf8bit:		colors = 1;		break;
	case pf24bit:		colors = 3;		break;
	case pf32bit:		colors = 4;		break;
	}

	// header
	{
		out->WriteBuffer("TLG5.0\x00raw\x1a\x00", 11);
		out->WriteBuffer(&colors, 1);
		int width = bmp->Width;
		int height = bmp->Height;
		WriteInt32(width, out);
		WriteInt32(height, out);
		int blockheight = BLOCK_HEIGHT;
		WriteInt32(blockheight, out);
	}

	int blockcount = (int)((bmp->Height - 1) / BLOCK_HEIGHT) + 1;


	// buffers/compressors
	SlideCompressor * compressor = NULL;
	unsigned char *cmpinbuf[4];
	unsigned char *cmpoutbuf[4];
	for(int i = 0; i < colors; i++)
		cmpinbuf[i] = cmpoutbuf[i] = NULL;
	long written[4];
	int *blocksizes;

	// allocate buffers/compressors
	try
	{
		compressor = new SlideCompressor();
		for(int i = 0; i < colors; i++)
		{
			cmpinbuf[i] = new unsigned char [bmp->Width * BLOCK_HEIGHT];
			cmpoutbuf[i] = new unsigned char [bmp->Width * BLOCK_HEIGHT * 9 / 4];
			written[i] = 0;
		}
		blocksizes = new int[blockcount];

		long blocksizepos = out->Position;
		// write block size header
		// (later fill this)
		for(int i = 0; i < blockcount; i++)
		{
			out->WriteBuffer("    ", 4);
		}


		//
		int block = 0;
		for(int blk_y = 0; blk_y < bmp->Height; blk_y += BLOCK_HEIGHT, block++)
		{
			int ylim = blk_y + BLOCK_HEIGHT;
			if(ylim > bmp->Height) ylim = bmp->Height;

			int inp = 0;


			for(int y = blk_y; y < ylim; y++)
			{
				// retrieve scan lines
				const unsigned char * upper;
				if(y != 0)
					upper = (const unsigned char *)bmp->ScanLine[y-1];
				else
					upper = NULL;
				const unsigned char * current;
				current = (const unsigned char *)bmp->ScanLine[y];

				// prepare buffer
				int prevcl[4];
				int val[4];

				for(int c = 0; c < colors; c++) prevcl[c] = 0;

				for(int x = 0; x < bmp->Width; x++)
				{
					for(int c = 0; c < colors; c++)
					{
						int cl;
						if(upper)
							cl = 0[current++] - 0[upper++];
						else
							cl = 0[current++];
						val[c] = cl - prevcl[c];
						prevcl[c] = cl;
					}
					// composite colors
					switch(colors)
					{
					case 1:
						cmpinbuf[0][inp] = val[0];
						break;
					case 3:
						cmpinbuf[0][inp] = val[0] - val[1];
						cmpinbuf[1][inp] = val[1];
						cmpinbuf[2][inp] = val[2] - val[1];
						break;
					case 4:
						cmpinbuf[0][inp] = val[0] - val[1];
						cmpinbuf[1][inp] = val[1];
						cmpinbuf[2][inp] = val[2] - val[1];
						cmpinbuf[3][inp] = val[3];
						break;
					}

					inp++;
				}

			}


			// compress buffer and write to the file

			// LZSS
			int blocksize = 0;
			for(int c = 0; c < colors; c++)
			{
				long wrote = 0;
				compressor->Store();
				compressor->Encode(cmpinbuf[c], inp,
					cmpoutbuf[c], wrote);
				if(wrote < inp)
				{
					out->WriteBuffer("\x00", 1);
					WriteInt32(wrote, out);
					out->WriteBuffer(cmpoutbuf[c], wrote);
					blocksize += wrote + 4 + 1;
				}
				else
				{
					compressor->Restore();
					out->WriteBuffer("\x01", 1);
					WriteInt32(inp, out);
					out->WriteBuffer(cmpinbuf[c], inp);
					blocksize += inp + 4 + 1;
				}
				written[c] += wrote;
			}

			blocksizes[block] = blocksize;
		}


		// write block sizes
		long pos_save = out->Position;
		out->Position = blocksizepos;
		for(int i = 0; i < blockcount; i++)
		{
			WriteInt32(blocksizes[i], out);
		}
		out->Position = pos_save;



		// deallocate buffers/compressors
	}
	catch(...)
	{
		for(int i = 0; i < colors; i++)
		{
			if(cmpinbuf[i]) delete [] cmpinbuf[i];
			if(cmpoutbuf[i]) delete [] cmpoutbuf[i];
		}
		if(compressor) delete compressor;
		if(blocksizes) delete [] blocksizes;
		throw;
	}
	for(int i = 0; i < colors; i++)
	{
		if(cmpinbuf[i]) delete [] cmpinbuf[i];
		if(cmpoutbuf[i]) delete [] cmpoutbuf[i];
	}
	if(compressor) delete compressor;
	if(blocksizes) delete [] blocksizes;
}
//---------------------------------------------------------------------------
void SaveTLG5(Graphics::TBitmap *in, TStream *out)
{
	Compress(in, out);
}
//---------------------------------------------------------------------------
