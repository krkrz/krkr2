/* ioapi.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant
*/

#include <windows.h>
#include <stdio.h>
#include "tp_stub.h"

#include "zlib.h"
#include "ioapi.h"

static voidpf ZCALLBACK fopen_file_func (voidpf opaque, const char *filename, int mode)
{
	IStream* file = NULL;
	int tjsmode = 0;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
		tjsmode = TJS_BS_READ;
    else
    if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
		tjsmode= TJS_BS_APPEND;
	else
    if (mode & ZLIB_FILEFUNC_MODE_CREATE)
		tjsmode = TJS_BS_WRITE;
	
	if ((filename!=NULL)) {
		file = TVPCreateIStream(filename, tjsmode);
	}
    return file;
}

static uLong ZCALLBACK fread_file_func (voidpf opaque, voidpf stream, void *buf, uLong size)
{
	IStream *is = (IStream*)stream;
	if (is) {
		ULONG len;
		if (is->Read(buf,size,&len) == S_OK) {
			return len;
		}
	}
	return 0;
}

static uLong ZCALLBACK fwrite_file_func (voidpf opaque, voidpf stream, const void *buf, uLong size)
{
	IStream *is = (IStream*)stream;
	if (is) {
		DWORD len;
		if (is->Write(buf,size,&len) == S_OK) {
			return len;
		}
	}
	return 0;
}

static long ZCALLBACK ftell_file_func (voidpf opaque, voidpf stream)
{
	IStream *is = (IStream *)stream;
	if (is) {
		LARGE_INTEGER move = {0};
		ULARGE_INTEGER newposition;
		if (is->Seek(move, STREAM_SEEK_CUR, &newposition) == S_OK) {
			return (long)newposition.QuadPart;
		}
	}
	return -1;
}

static long ZCALLBACK fseek_file_func (voidpf opaque, voidpf stream, uLong offset, int origin)
{
	DWORD dwOrigin;
	switch(origin) {
	case ZLIB_FILEFUNC_SEEK_CUR: dwOrigin = STREAM_SEEK_CUR; break;
	case ZLIB_FILEFUNC_SEEK_END: dwOrigin = STREAM_SEEK_END; break;
	case ZLIB_FILEFUNC_SEEK_SET: dwOrigin = STREAM_SEEK_SET; break;
	default: return -1; //failed
	}
	IStream *is = (IStream *)stream;
	if (is) {
		LARGE_INTEGER move;
		move.QuadPart = offset;
		ULARGE_INTEGER newposition;
		if (is->Seek(move, origin, &newposition) == S_OK) {
			return 0;
		}
	}
	return -1;
}

static int ZCALLBACK fclose_file_func (voidpf opaque, voidpf stream)
{
	IStream *is = (IStream *)stream;
	if (is) {
		is->Release();
		is = NULL;
		return 0;
	}
	return EOF;
}

static int ZCALLBACK ferror_file_func (voidpf opaque, voidpf stream)
{
	IStream *is = (IStream *)stream;
	if (is) {
		return 0;
	}
	return EOF;
}

void fill_fopen_filefunc (zlib_filefunc_def* pzlib_filefunc_def)
{
	pzlib_filefunc_def->zopen_file = fopen_file_func;
	pzlib_filefunc_def->zread_file = fread_file_func;
    pzlib_filefunc_def->zwrite_file = fwrite_file_func;
	pzlib_filefunc_def->ztell_file = ftell_file_func;
	pzlib_filefunc_def->zseek_file = fseek_file_func;
	pzlib_filefunc_def->zclose_file = fclose_file_func;
	pzlib_filefunc_def->zerror_file = ferror_file_func;
	pzlib_filefunc_def->opaque = NULL;
}
