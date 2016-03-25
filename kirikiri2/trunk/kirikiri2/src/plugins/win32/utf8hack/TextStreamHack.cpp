#include "TextStreamHack.hpp"
#include "simplebinder.hpp"

#define CP_AUTO ((UINT)-1)

UINT DefaultReadCodePage = CP_AUTO;

static struct CodePageNames {
	UINT codepage;
	const tjs_char *encoding;
} const CodePageList[] = {
	{ 932,     TJS_W("Shift_JIS") },
	{ CP_UTF8, TJS_W("UTF-8") },
	{ CP_ACP,  TJS_W("ANSI") },
	{ CP_AUTO, TJS_W("auto") },
	{ 0, 0 } };

static ttstr CodePageToEncoding(UINT codepage) {
	for (const CodePageNames *list = CodePageList; list->encoding; list++) {
		if (codepage == list->codepage) return list->encoding;
	}
	ttstr r(TJS_W("cp"));
	r += (tjs_int)codepage;
	return r;
}
static UINT EncodingToCodePage(const ttstr &encode) {
	for (const CodePageNames *list = CodePageList; list->encoding; list++) {
		if (encode == list->encoding) return list->codepage;
	}
	if (encode.StartsWith("cp")) {
		return TJS_atoi(encode.c_str()+2);
	}
	//TVPThrowExceptionMessage(TVPUnsupportedEncoding, encode);
	return CP_ACP;
}
void SetDefaultReadCodePage(const tjs_char *encoding) {
	DefaultReadCodePage = EncodingToCodePage(encoding);
}

#include "nmh.hpp"
static UINT GuessCodepage(const tjs_nchar * &text, size_t &size)
{
	const unsigned char *str = (const unsigned char*)text;
	if (nmh_is_ascii(str, size)) return CP_ACP;

	int utf8 = nmh_is_utf8(str, size);
	if (utf8) {
		if (utf8 < 0) {
			// [XXX] Dead code: UTF8-BOM checkd before CodepageTextToWideCharString called.
			text += 3;
			size -= 3;
		}
		return CP_UTF8;
	}

	float sjis = nmh_is_sjis(str, size);
	float euc  = nmh_is_euc (str, size);
	if (sjis > 0.0 || euc > 0.0) return sjis >= euc ? 932 : 20932; // 51932 are not supported on MultiByteToWideChar?

	// unknown
	return CP_ACP;
}
//---------------------------------------------------------------------------

const tjs_char *CompatTJSBinaryStream::ReadError = TJS_W("Read error");
const tjs_char *CompatTJSBinaryStream::SeekError = TJS_W("Seek error");

CompatTJSBinaryStream* CompatTJSBinaryStream::CreateStream(const ttstr & name , tjs_uint32 flags)
{
	IStream *Stream = TVPCreateIStream(name, flags);
	if(!Stream) return NULL;
	
	CompatTJSBinaryStream *CompatStream = new CompatTJSBinaryStream(Stream);
	Stream->Release();
	return CompatStream;
}

size_t CodepageTextToWideCharString(UINT codepage, const tjs_nchar *s, size_t len, tjs_char *pwcs, size_t n)
{
	if (codepage == CP_AUTO)
		codepage = GuessCodepage(s, len);

	// utf8 specialization
	if (codepage == CP_UTF8) return TVPUtf8ToWideCharString(s, pwcs);

	// [MEMO] refer to tjsConfig.cpp:TJS_mbstowcs() implements

	if(pwcs && n == 0) return 0;

	if(pwcs)
	{
		// Try converting to wide characters. Here assumes pwcs is large enough
		// to store the result.
		int count = MultiByteToWideChar(codepage, MB_PRECOMPOSED|MB_ERR_INVALID_CHARS, s, -1, pwcs, n);
		if(count != 0) return count - 1;

		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) return (size_t) -1;

		// pwcs is not enough to store the result ...

		// count number of source characters to fit the destination buffer
		int charcnt = n;
		const unsigned char *p;
		for(p = (const unsigned char*)s; charcnt-- && *p; p++)
		{
			if(IsDBCSLeadByte(*p)) p++;
		}
		int bytecnt = (int)(p - (const unsigned char *)s);

		count = MultiByteToWideChar(codepage, MB_PRECOMPOSED, s, bytecnt, pwcs, n);
		if(count == 0) return (size_t) -1;

		return count;
	}
	else
	{
		int count = MultiByteToWideChar(codepage, MB_PRECOMPOSED|MB_ERR_INVALID_CHARS, s, -1, NULL, 0);
		if(count == 0) return (size_t)-1;
		return count - 1;
	}
}

//---------------------------------------------------------------------------

static tjs_error TextEncodingGetter(tTJSVariant *result) {
	ttstr encode(CodePageToEncoding(DefaultReadCodePage));
	if (result) *result = encode;
	return TJS_S_OK;
}
static tjs_error TextEncodingSetter(tTJSVariant const *setval) {
	SetDefaultReadCodePage(setval->GetString());
	return TJS_S_OK;
}
bool TextEncodePropEntry(bool link) {
	return (SimpleBinder::BindUtil(TJS_W("Scripts"), link)
			.Property(TJS_W("textEncoding"), &TextEncodingGetter, &TextEncodingSetter)
			.IsValid());
}

