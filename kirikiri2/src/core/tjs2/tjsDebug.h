//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS Debugging support
//---------------------------------------------------------------------------
#ifndef tjsDebugH
#define tjsDebugH

#include "tjsString.h"

namespace TJS
{
//---------------------------------------------------------------------------
// ObjectHashMap : hash map to track object construction/destruction
//---------------------------------------------------------------------------
class tTJSScriptBlock;
struct tTJSObjectHashMapRecord
{
	ttstr Label; // a label which indicates where the object was created
	ttstr Type; // object type ("class Array" etc)
};


class tTJSObjectHashMap;
extern tTJSObjectHashMap * TJSObjectHashMap;
extern void TJSAddRefObjectHashMap();
extern void TJSReleaseObjectHashMap();
extern void TJSAddObjectHashRecord(void * object,
	const tTJSObjectHashMapRecord & record);
extern void TJSRemoveObjectHashRecord(void * object);
extern const tTJSObjectHashMapRecord * TJSSearchObject(void * object);
static inline bool TJSObjectHashMapEnabled() { return TJSObjectHashMap; }
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// StackTracer : stack to trace function call trace
//---------------------------------------------------------------------------
class tTJSStackTracer;
class tTJSInterCodeContext;
extern tTJSStackTracer * TJSStackTracer;
extern void TJSAddRefStackTracer();
extern void TJSReleaseStackTracer();
extern void TJSStackTracerPush(tTJSInterCodeContext *context, bool in_try);
extern void TJSStackTracerSetCodePointer(const tjs_int32 * codebase, tjs_int32 * const * codeptr);
extern void TJSStackTracerPop();
extern ttstr TJSGetStackTraceString(tjs_int limit = 0);
static inline bool TJSStackTracerEnabled() { return TJSStackTracer; }
//---------------------------------------------------------------------------

} // namespace TJS

#endif
