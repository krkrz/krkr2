//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// configuration
//---------------------------------------------------------------------------


#include "tjsCommHead.h"
#pragma hdrstop

#ifdef TJS_DEBUG_PROFILE_TIME
#include <windows.h>
#endif

#ifdef __WIN32__
#include "float.h"
#endif

namespace TJS
{

//---------------------------------------------------------------------------
// debug support
//---------------------------------------------------------------------------
#ifdef TJS_DEBUG_PROFILE_TIME
tjs_uint TJSGetTickCount()
{
	return GetTickCount();
}
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// some wchar_t support functions
//---------------------------------------------------------------------------
tjs_int TJS_atoi(const tjs_char *s)
{
	int r = 0;
	bool sign = false;
	while(*s && *s <= 0x20) s++; // skip spaces
	if(!*s) return 0;
	if(*s == TJS_W('-'))
	{
		sign = true;
		s++;
		while(*s && *s <= 0x20) s++; // skip spaces
		if(!*s) return 0;
	}

	while(*s >= TJS_W('0') && *s <= TJS_W('9'))
	{
		r *= 10;
		r += *s - TJS_W('0');
		s++;
	}
	if(sign) r = -r;
	return r;
}
//---------------------------------------------------------------------------
tjs_char * TJS_int_to_str(tjs_int value, tjs_char *string)
{
	tjs_char *ostring = string;

	if(value<0) *(string++) = TJS_W('-'), value = -value;

	tjs_char buf[40];

	tjs_char *p = buf;

	do
	{
		*(p++) = (value % 10) + TJS_W('0');
		value /= 10;
	} while(value);

	p--;
	while(buf <= p) *(string++) = *(p--);
	*string = 0;

	return ostring;
}
//---------------------------------------------------------------------------
tjs_char * TJS_tTVInt_to_str(tjs_int64 value, tjs_char *string)
{
	if(value == TJS_UI64_VAL(0x8000000000000000))
	{
		// this is a special number which we must avoid normal conversion
		TJS_strcpy(string, TJS_W("-9223372036854775808"));
		return string;
	}

	tjs_char *ostring = string;

	if(value<0) *(string++) = TJS_W('-'), value = -value;

	tjs_char buf[40];

	tjs_char *p = buf;

	do
	{
		*(p++) = (value % 10) + TJS_W('0');
		value /= 10;
	} while(value);

	p--;
	while(buf <= p) *(string++) = *(p--);
	*string = 0;

	return ostring;
}
//---------------------------------------------------------------------------
tjs_int TJS_strnicmp(const tjs_char *s1, const tjs_char *s2,
	size_t maxlen)
{
	while(maxlen--)
	{
		if(*s1 == TJS_W('\0')) return (*s2 == TJS_W('\0')) ? 0 : -1;
		if(*s2 == TJS_W('\0')) return (*s1 == TJS_W('\0')) ? 0 : 1;
		if(*s1 < *s2) return -1;
		if(*s1 > *s2) return 1;
		s1++;
		s2++;
	}

	return 0;
}
//---------------------------------------------------------------------------
tjs_int TJS_stricmp(const tjs_char *s1, const tjs_char *s2)
{
	// we only support basic alphabets
	// fixme: complete alphabets support

	for(;;)
	{
		tjs_char c1 = *s1, c2 = *s2;
		if(c1 >= TJS_W('a') && c1 <= TJS_W('z')) c1 += TJS_W('Z')-TJS_W('z');
		if(c2 >= TJS_W('a') && c2 <= TJS_W('z')) c2 += TJS_W('Z')-TJS_W('z');
		if(c1 == TJS_W('\0')) return (c2 == TJS_W('\0')) ? 0 : -1;
		if(c2 == TJS_W('\0')) return (c1 == TJS_W('\0')) ? 0 : 1;
		if(c1 < c2) return -1;
		if(c1 > c2) return 1;
		s1++;
		s2++;
	}
}
//---------------------------------------------------------------------------
void TJS_strcpy_maxlen(tjs_char *d, const tjs_char *s, size_t len)
{
	tjs_char ch;
	len++;
	while((ch=*s)!=0 && --len) *(d++) = ch, s++;
	*d = 0;
}
//---------------------------------------------------------------------------
void TJS_strcpy(tjs_char *d, const tjs_char *s)
{
	tjs_char ch;
	while((ch=*s)!=0) *(d++) = ch, s++;
	*d = 0;
}
//---------------------------------------------------------------------------
size_t TJS_strlen(const tjs_char *d)
{
	const tjs_char *p = d;
	while(*d) d++;
	return d-p;
}
//---------------------------------------------------------------------------
#if  defined(__GNUC__)
tjs_int TJS_sprintf(tjs_char *s, const tjs_char *format, ...)
{
	tjs_int r;
	va_list param;
	va_start(param, format);
	r = TJS_vsnprintf(s, INT_MAX, format, param);
	va_end(param);
	return r;
}
#endif
//---------------------------------------------------------------------------

void TJS_cdecl TJS_debug_out(const tjs_char *format, ...)
{
	va_list param;
	va_start(param, format);
	TJS_vfprintf(stderr, format, param);
	va_end(param);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTJSNarrowStringHolder
//---------------------------------------------------------------------------
tTJSNarrowStringHolder::tTJSNarrowStringHolder(const wchar_t * wide)
{
	int n;
	if(!wide)
		n = -1;
	else
		n = wcstombs(NULL, wide, 0);
		// TODO: check : wcstombs's first argument can be NULL?
	if( n == -1 )
	{
		Buf = TJS_N("");
		Allocated = false;
		return;
	}
	Buf = new tjs_nchar[n+1];
	Allocated = true;
	Buf[wcstombs(Buf, wide, n)] = 0;
}
//---------------------------------------------------------------------------
tTJSNarrowStringHolder::~tTJSNarrowStringHolder()
{
	if(Allocated) delete [] Buf;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// FPU control
//---------------------------------------------------------------------------
#if defined(__WIN32__) && !defined(__GNUC__)
static unsigned int TJSDefaultFPUCW = 0;
static unsigned int TJSNewFPUCW = 0;
static bool TJSFPUInit = false;
#endif
void TJSSetFPUE()
{
#if defined(__WIN32__) && !defined(__GNUC__)
	if(!TJSFPUInit)
	{
		TJSFPUInit = true;
		TJSDefaultFPUCW = _control87(0, 0);

		_default87 = TJSNewFPUCW = _control87(MCW_EM, MCW_EM);
#ifdef TJS_SUPPORT_VCL
		Default8087CW = TJSNewFPUCW;
#endif
	}

//	_fpreset();
	_control87(TJSNewFPUCW, 0xffff);
#endif

}

void TJSRestoreFPUE()
{

#if defined(__WIN32__) && !defined(__GNUC__)
	if(!TJSFPUInit) return;
	_control87(TJSDefaultFPUCW, 0xffff);
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
} // namespace TJS



