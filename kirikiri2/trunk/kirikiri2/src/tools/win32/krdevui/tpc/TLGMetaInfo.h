//---------------------------------------------------------------------------

#ifndef TLGMetaInfoH
#define TLGMetaInfoH
//---------------------------------------------------------------------------
extern void SaveTLGWithMetaInfo(TStrings *names, Graphics::TBitmap *in,
	AnsiString filename,
	void (*saveproc)(Graphics::TBitmap *in, TStream *out));
//---------------------------------------------------------------------------

#endif
