//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "KrkrExecutableUnit.h"

//---------------------------------------------------------------------------
#define MIN_KRKR_MARK_SEARCH 512*1024
#define MAX_KRKR_MARK_SEARCH 4*1024*1024
//---------------------------------------------------------------------------
char XOPT_EMBED_AREA_[] = " OPT_EMBED_AREA_";
char XCORE_SIG_______[] = " CORE_SIG_______";
char XRELEASE_SIG____[] = " RELEASE_SIG____";
char XP3_SIG[] = " P3\x0d\x0a\x20\x0a\x1a\x8b\x67\x01";   // mark_size = 11
//---------------------------------------------------------------------------
int CheckKrkrExecutable(AnsiString fn, const char *mark)
{
	// Find the mark in the krkr executable.
	// All krkr executable have
	// "XOPT_EMBED_AREA_" mark at the end of the executable image.
	// Additionally
	// "XCORE_SIG_______" and "XRELEASE_SIG____" are reserved area for
	// signatures.
	// "XP3\x0d\x0a\x20\x0a\x1a\x8b\x67\x01" are optional XP3 archive attached
	// to the executable.

	// This function returns the mark offset
	// (area size bofore specified mark)
	int mark_size = strlen(mark);

	TFileStream *st = new TFileStream(fn, fmOpenRead | fmShareDenyWrite);
	int imagesize = 0;
	try
	{
		int ofs = MIN_KRKR_MARK_SEARCH;
		char buf[4096];
		int read;
		bool found = false;
		st->Position = ofs;
		while( (read = st->Read(buf, sizeof(buf)) ) != 0)
		{
			for(int i = 0; i < read; i += 16) // the mark is aligned to paragraph (16bytes)
			{
				if(buf[i] == 'X')
				{
					if(!memcmp(buf + i + 1, mark + 1, mark_size - 1))
					{
						// mark found
						imagesize = i + ofs;
						found = true;
						break;
					}
				}
			}
			if(found) break;
			ofs += read;
			if(ofs >= MAX_KRKR_MARK_SEARCH) break;
		}
	}
	catch(...)
	{
		delete st;
		throw;
	}
	delete st;

	return imagesize;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
