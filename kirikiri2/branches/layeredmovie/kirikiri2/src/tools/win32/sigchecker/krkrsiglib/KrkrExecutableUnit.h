//---------------------------------------------------------------------------

#ifndef KrkrExecutableUnitH
#define KrkrExecutableUnitH
//---------------------------------------------------------------------------
extern char XOPT_EMBED_AREA_[];// = " OPT_EMBED_AREA_";
extern char XCORE_SIG_______[];// = " CORE_SIG_______";
extern char XRELEASE_SIG____[];// = " RELEASE_SIG____";
extern char XP3_SIG[]; //  = " P3\x0d\x0a\x20\x0a\x1a\x8b\x67\x01";   // mark_size = 11
//---------------------------------------------------------------------------
extern int CheckKrkrExecutable(AnsiString fn, const char *mark);
//---------------------------------------------------------------------------

#endif
