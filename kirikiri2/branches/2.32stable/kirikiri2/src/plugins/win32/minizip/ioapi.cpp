/* ioapi.h -- IO base function header for compress/uncompress .zip
   part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications for Zip64 support
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

         For more info read MiniZip_info.txt

*/

#include <windows.h>
#include <stdio.h>
#include "tp_stub.h"

#include "ioapi.h"

voidpf call_zopen64 (const zlib_filefunc64_32_def* pfilefunc,const void*filename,int mode)
{
    if (pfilefunc->zfile_func64.zopen64_file != NULL)
        return (*(pfilefunc->zfile_func64.zopen64_file)) (pfilefunc->zfile_func64.opaque,filename,mode);
    else
    {
        return (*(pfilefunc->zopen32_file))(pfilefunc->zfile_func64.opaque,(const char*)filename,mode);
    }
}

long call_zseek64 (const zlib_filefunc64_32_def* pfilefunc,voidpf filestream, ZPOS64_T offset, int origin)
{
    if (pfilefunc->zfile_func64.zseek64_file != NULL)
        return (*(pfilefunc->zfile_func64.zseek64_file)) (pfilefunc->zfile_func64.opaque,filestream,offset,origin);
    else
    {
        uLong offsetTruncated = (uLong)offset;
        if (offsetTruncated != offset)
            return -1;
        else
            return (*(pfilefunc->zseek32_file))(pfilefunc->zfile_func64.opaque,filestream,offsetTruncated,origin);
    }
}

ZPOS64_T call_ztell64 (const zlib_filefunc64_32_def* pfilefunc,voidpf filestream)
{
    if (pfilefunc->zfile_func64.zseek64_file != NULL)
        return (*(pfilefunc->zfile_func64.ztell64_file)) (pfilefunc->zfile_func64.opaque,filestream);
    else
    {
        uLong tell_uLong = (*(pfilefunc->ztell32_file))(pfilefunc->zfile_func64.opaque,filestream);
        if ((tell_uLong) == ((uLong)-1))
            return (ZPOS64_T)-1;
        else
            return tell_uLong;
    }
}

void fill_zlib_filefunc64_32_def_from_filefunc32(zlib_filefunc64_32_def* p_filefunc64_32,const zlib_filefunc_def* p_filefunc32)
{
    p_filefunc64_32->zfile_func64.zopen64_file = NULL;
    p_filefunc64_32->zopen32_file = p_filefunc32->zopen_file;
    p_filefunc64_32->zfile_func64.zerror_file = p_filefunc32->zerror_file;
    p_filefunc64_32->zfile_func64.zread_file = p_filefunc32->zread_file;
    p_filefunc64_32->zfile_func64.zwrite_file = p_filefunc32->zwrite_file;
    p_filefunc64_32->zfile_func64.ztell64_file = NULL;
    p_filefunc64_32->zfile_func64.zseek64_file = NULL;
    p_filefunc64_32->zfile_func64.zclose_file = p_filefunc32->zclose_file;
    p_filefunc64_32->zfile_func64.zerror_file = p_filefunc32->zerror_file;
    p_filefunc64_32->zfile_func64.opaque = p_filefunc32->opaque;
    p_filefunc64_32->zseek32_file = p_filefunc32->zseek_file;
    p_filefunc64_32->ztell32_file = p_filefunc32->ztell_file;
}



static voidpf  ZCALLBACK fopen_file_func OF((voidpf opaque, const char* filename, int mode));
static uLong   ZCALLBACK fread_file_func OF((voidpf opaque, voidpf stream, void* buf, uLong size));
static uLong   ZCALLBACK fwrite_file_func OF((voidpf opaque, voidpf stream, const void* buf,uLong size));
static ZPOS64_T ZCALLBACK ftell64_file_func OF((voidpf opaque, voidpf stream));
static long    ZCALLBACK fseek64_file_func OF((voidpf opaque, voidpf stream, ZPOS64_T offset, int origin));
static int     ZCALLBACK fclose_file_func OF((voidpf opaque, voidpf stream));
static int     ZCALLBACK ferror_file_func OF((voidpf opaque, voidpf stream));

static voidpf ZCALLBACK fopen_file_func (voidpf opaque, const char* filename, int mode)
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

static voidpf ZCALLBACK fopen64_file_func (voidpf opaque, const void* filename, int mode)
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
		file = TVPCreateIStream(ttstr((const char*)filename), tjsmode);
	}
    return file;
}


static uLong ZCALLBACK fread_file_func (voidpf opaque, voidpf stream, void* buf, uLong size)
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

static uLong ZCALLBACK fwrite_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
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


static ZPOS64_T ZCALLBACK ftell64_file_func (voidpf opaque, voidpf stream)
{
	IStream *is = (IStream *)stream;
	if (is) {
		LARGE_INTEGER move = {0};
		ULARGE_INTEGER newposition;
		if (is->Seek(move, STREAM_SEEK_CUR, &newposition) == S_OK) {
			return newposition.QuadPart;
		}
	}
	return -1;
}

static long ZCALLBACK fseek_file_func (voidpf  opaque, voidpf stream, uLong offset, int origin)
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

static long ZCALLBACK fseek64_file_func (voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin)
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

void fill_fopen64_filefunc (zlib_filefunc64_def*  pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen64_file = fopen64_file_func;
    pzlib_filefunc_def->zread_file = fread_file_func;
    pzlib_filefunc_def->zwrite_file = fwrite_file_func;
    pzlib_filefunc_def->ztell64_file = ftell64_file_func;
    pzlib_filefunc_def->zseek64_file = fseek64_file_func;
    pzlib_filefunc_def->zclose_file = fclose_file_func;
    pzlib_filefunc_def->zerror_file = ferror_file_func;
    pzlib_filefunc_def->opaque = NULL;
}
