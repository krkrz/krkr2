#pragma once

#include <windows.h>
#include "tp_stub.h"

#ifdef UTF8HACK_REPLACEMACRO

#define TVPStringFromBMPUnicode(SRC) (ttstr((const tjs_char*)SRC))

#define TJS_strchr			wcschr
#define TJS_strncpy			wcsncpy

#define uncompress ZLIB_uncompress
#define Z_OK 0

#define TVPUnsupportedCipherMode       TJS_W("%1 is unsupported cipger mode")
#define TVPUnsupportedEncoding         TJS_W("%1 is unsupported encoding")
#define TJSNarrowToWideConversionError TJS_W("Cannot convert given narrow string to wide string")

#define TVPCreateStream  CompatTJSBinaryStream::CreateStream
#define tTJSBinaryStream CompatTJSBinaryStream

#define TVPCreateTextStreamForRead __fastcall ReplaceCreateTextStreamForRead

#endif // UTF8HACK_REPLACEMACRO

extern iTJSTextReadStream *        __fastcall ReplaceCreateTextStreamForRead(const ttstr & name, const ttstr & modestr);
extern size_t CodepageTextToWideCharString(UINT codepage, const tjs_nchar *text, size_t textlen, tjs_char *Buffer, size_t BufferLen);
extern UINT DefaultReadCodePage;
extern bool TextEncodePropEntry(bool);
extern void SetDefaultReadCodePage(const tjs_char *encoding);

//---------------------------------------------------------------------------
class CompatTJSBinaryStream
{
	IStream *Stream;
public:
	CompatTJSBinaryStream(IStream *stream) : Stream(stream) {
		Stream->AddRef();
	}
	virtual ~CompatTJSBinaryStream() {
		Stream->Release();
	}

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence) {
		DWORD origin;

		switch(whence)
		{
		case TJS_BS_SEEK_SET:			origin = STREAM_SEEK_SET;		break;
		case TJS_BS_SEEK_CUR:			origin = STREAM_SEEK_CUR;		break;
		case TJS_BS_SEEK_END:			origin = STREAM_SEEK_END;		break;
		default:						origin = STREAM_SEEK_SET;		break;
		}

		LARGE_INTEGER ofs;
		ULARGE_INTEGER newpos;

		ofs.QuadPart = 0;
		HRESULT hr = Stream->Seek(ofs, STREAM_SEEK_CUR, &newpos);
		bool orgpossaved;
		LARGE_INTEGER orgpos;
		if(FAILED(hr))
		{
			orgpossaved = false;
		}
		else
		{
			orgpossaved = true;
			*(LARGE_INTEGER*)&orgpos = *(LARGE_INTEGER*)&newpos;
		}

		ofs.QuadPart = offset;

		hr = Stream->Seek(ofs, origin, &newpos);
		if(FAILED(hr))
		{
			if(orgpossaved)
			{
				Stream->Seek(orgpos, STREAM_SEEK_SET, &newpos);
			}
			else
			{
				TVPThrowExceptionMessage(SeekError);
			}
		}

		return newpos.QuadPart;
	}

	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size) {
		ULONG cb = read_size;
		ULONG read;
		HRESULT hr = Stream->Read(buffer, cb, &read);
		if(FAILED(hr)) read = 0;
		return read;
	}

	tjs_uint64 TJS_INTF_METHOD GetSize() {
		HRESULT hr;
		STATSTG stg;

		hr = Stream->Stat(&stg, STATFLAG_NONAME);
		if (SUCCEEDED(hr)) stg.cbSize.QuadPart;

		tjs_uint64 orgpos = GetPosition();
		tjs_uint64 size = Seek(0, TJS_BS_SEEK_END);
		Seek(orgpos, SEEK_SET);
		return size;
	}

	tjs_uint64 GetPosition() {
		return Seek(0, SEEK_CUR);
	}

	void SetPosition(tjs_uint64 pos) {
		if(pos != Seek(pos, TJS_BS_SEEK_SET))
			TVPThrowExceptionMessage(SeekError);
	}

	void ReadBuffer(void *buffer, tjs_uint read_size) {
		if(Read(buffer, read_size) != read_size)
			TVPThrowExceptionMessage(ReadError);
	}

	// reads little-endian integers
	inline tjs_uint64 ReadI64LE() {
		tjs_uint64 temp;
		ReadBuffer(&temp, 8);
		return temp;
	}
	inline tjs_uint32 ReadI32LE() {
		tjs_uint32 temp;
		ReadBuffer(&temp, 4);
		return temp;
	}
	inline tjs_uint16 ReadI16LE() {
		tjs_uint16 temp;
		ReadBuffer(&temp, 2);
		return temp;
	}

public:
	static const tjs_char *ReadError;
	static const tjs_char *SeekError;
	static CompatTJSBinaryStream* CreateStream(const ttstr & name , tjs_uint32 flags);
};
