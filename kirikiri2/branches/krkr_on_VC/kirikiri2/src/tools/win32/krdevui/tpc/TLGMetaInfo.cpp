//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TLGMetaInfo.h"

//---------------------------------------------------------------------------
void SaveTLGWithMetaInfo(TStrings *names, Graphics::TBitmap *in,
	AnsiString filename,
	void (*saveproc)(Graphics::TBitmap *in, TStream *out))
{
	// open output filename
	TFileStream *filestream =
		new TFileStream(filename, fmCreate|fmShareDenyWrite);
	try
	{
		// if no names given, simply write TLG stream
		if(!names || names->Count == 0)
		{
			saveproc(in, filestream);
		}
		else
		{
			// write TLG0.0 Structured Data Stream header
			filestream->WriteBuffer("TLG0.0\x00sds\x1a\x00", 11);
			unsigned int rawlenpos = filestream->Position;
			filestream->WriteBuffer("0000", 4);

			// write raw TLG stream
			saveproc(in, filestream);

			// write raw data size
			unsigned int pos_save = filestream->Position;
			filestream->Position = rawlenpos;
			unsigned int size = pos_save - rawlenpos - 4;
			unsigned char bin[4]; // buffer for writing 32bit integer as little endian format
			bin[0] = size & 0xff;
			bin[1] = (size >> 8) & 0xff;
			bin[2] = (size >> 16) & 0xff;
			bin[3] = (size >> 24) & 0xff;
			filestream->WriteBuffer(bin, 4);
			filestream->Position = pos_save;


			// write "tags" chunk name
			filestream->WriteBuffer("tags", 4);

			// build tag data
			AnsiString tag;
			for(int i = 0; i < names->Count; i++)
			{
				AnsiString name = names->Names[i];
				if(name == "") continue;
				AnsiString value = names->Values[name];
				tag += AnsiString(name.Length()) + ":" + name + "=" +
					AnsiString(value.Length()) + ":" + value + ",";
			}

			// write chunk size
			size = tag.Length();
			bin[0] = size & 0xff;
			bin[1] = (size >> 8) & 0xff;
			bin[2] = (size >> 16) & 0xff;
			bin[3] = (size >> 24) & 0xff;
			filestream->WriteBuffer(bin, 4);

			// write chunk data
			filestream->WriteBuffer(tag.c_str(), tag.Length());
		}
	}
	catch(...)
	{
		delete filestream;
		throw;
	}
	delete filestream;

}
//---------------------------------------------------------------------------

#pragma package(smart_init)
