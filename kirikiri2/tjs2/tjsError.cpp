//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS2's C++ exception class and exception message
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop


#include "tjsScriptBlock.h"
#include "tjsError.h"
#include "tjs.h"

#define TJS_MAX_TRACE_TEXT_LEN 1500

namespace TJS
{

//---------------------------------------------------------------------------
// TJSGetExceptionObject : retrieves TJS 'Exception' object
//---------------------------------------------------------------------------
void TJSGetExceptionObject(tTJS *tjs, tTJSVariant *res, tTJSVariant &msg,
	tTJSVariant *trace/* trace is optional */)
{
	if(!res) return; // not prcess

	// retrieve class "Exception" from global
	iTJSDispatch2 *global = tjs->GetGlobalNoAddRef();
	tTJSVariant val;
	static tTJSString Exception_name(TJS_W("Exception"));
	tjs_error hr = global->PropGet(0, Exception_name.c_str(),
		Exception_name.GetHint(), &val, global);
	if(TJS_FAILED(hr)) TJS_eTJSError(TJSExceptionNotFound);
	// create an Exception object
	iTJSDispatch2 *excpobj;
	tTJSVariantClosure clo = val.AsObjectClosureNoAddRef();
	tTJSVariant *pmsg = &msg;
	hr = clo.CreateNew(0, NULL, NULL, &excpobj, 1, &pmsg, clo.ObjThis);
	if(TJS_FAILED(hr)) TJS_eTJSError(TJSExceptionNotFound);
	if(trace)
	{
		static tTJSString trace_name(TJS_W("trace"));
		excpobj->PropSet(TJS_MEMBERENSURE, trace_name.c_str(), trace_name.GetHint(),
			trace, excpobj);
	}
	*res = tTJSVariant(excpobj, excpobj);
	excpobj->Release();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// eTJSScriptException
//---------------------------------------------------------------------------
eTJSScriptError::tScriptBlockHolder::tScriptBlockHolder(tTJSScriptBlock *block)
{
	Block = block;
	Block->AddRef();
}
//---------------------------------------------------------------------------
eTJSScriptError::tScriptBlockHolder::~tScriptBlockHolder()
{
	Block->Release();
}
//---------------------------------------------------------------------------
eTJSScriptError::tScriptBlockHolder::tScriptBlockHolder(
		const tScriptBlockHolder &Holder)
{
	Block = Holder.Block;
	Block->AddRef();
}
//---------------------------------------------------------------------------
eTJSScriptError::eTJSScriptError(const ttstr &  Msg,
	tTJSScriptBlock *block, tjs_int pos) :
		eTJSError(Msg), Block(block), Position(pos)
{
}
//---------------------------------------------------------------------------
tjs_int eTJSScriptError::GetSourceLine() const
{
	return Block.Block->SrcPosToLine(Position) +1;
}
//---------------------------------------------------------------------------
const tjs_char * eTJSScriptError::GetBlockName() const
{
	const tjs_char * name = Block.Block->GetName() ;
	return name ? name : TJS_W("");
}
//---------------------------------------------------------------------------
bool eTJSScriptError::AddTrace(tTJSScriptBlock *block, tjs_int pos)
{
	tjs_int len = Trace.GetLen();
	if(len >= TJS_MAX_TRACE_TEXT_LEN) return false;

	tjs_int line = block->SrcPosToLine(pos)+1;
	const tjs_char *c_name = block->GetName();
	ttstr name;
	if(c_name)
	{
		name = c_name;
	}
	else
	{
		tjs_char ptr[128];
		TJS_sprintf(ptr, TJS_W("0x%p"), block);
		name = ttstr(TJS_W("anonymous@")) + ptr;
	}

	if(len != 0) Trace += TJS_W(" <-- ");
	Trace += name + TJS_W("(") + ttstr(line) + TJS_W(")");

	return true;
}
//---------------------------------------------------------------------------
bool eTJSScriptError::AddTrace(tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos)
{
	if(AddTrace(block, pos))
	{
		Trace += TJS_W("[") + context->GetShortDescription() + TJS_W("]");
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool eTJSScriptError::AddTrace(const ttstr & data)
{
	tjs_int len = Trace.GetLen();
	if(len >= TJS_MAX_TRACE_TEXT_LEN) return false;
	if(len != 0) Trace += TJS_W(" <-- ");
	Trace += data;
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// throw helper functions
//---------------------------------------------------------------------------
ttstr TJSGetExceptionSourceString(tTJSScriptBlock *block, tjs_int pos)
{
	tjs_int line = block->SrcPosToLine(pos)+1;
	const tjs_char *c_name = block->GetName();
	tjs_char ptr[128];
	TJS_sprintf(ptr, TJS_W("0x%p"), block);
	ttstr name;
	if(c_name)
	{
		name = c_name;
	}
	else
	{
		tjs_char ptr[128];
		TJS_sprintf(ptr, TJS_W("0x%p"), block);
		name = ttstr(TJS_W("anonymous@")) + ptr;
	}

	return name + TJS_W(" line ") + ttstr(line);
}
//---------------------------------------------------------------------------
ttstr TJSGetExceptionSourceString(tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos)
{
	return TJSGetExceptionSourceString(block, pos) + TJS_W(" [") +
		context->GetShortDescription() + TJS_W("]");
}
//---------------------------------------------------------------------------
static void TJSReportExceptionSource(const ttstr &msg, tTJSScriptBlock *block,
	tjs_int pos)
{
	tTJS *tjs = block->GetTJS();
	tjs->OutputExceptionToConsole((msg + TJS_W(" at ") + TJSGetExceptionSourceString(
		block, pos)).c_str());
}
//---------------------------------------------------------------------------
static void TJSReportExceptionSource(const ttstr &msg, tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos)
{
	tTJS *tjs = block->GetTJS();
	tjs->OutputExceptionToConsole((msg + TJS_W(" at ") + TJSGetExceptionSourceString(
		context, block, pos)).c_str());
}
//---------------------------------------------------------------------------
void TJS_eTJS() { throw eTJS(); }
//---------------------------------------------------------------------------
void TJS_eTJSError(const ttstr & msg) { throw eTJSError(msg); }
void TJS_eTJSError(const tjs_char* msg) { throw eTJSError(msg); }
//---------------------------------------------------------------------------
void TJS_eTJSVariantError(const ttstr & msg) { throw eTJSVariantError(msg); }
void TJS_eTJSVariantError(const tjs_char * msg) { throw eTJSVariantError(msg); }
//---------------------------------------------------------------------------
void TJS_eTJSScriptError(const ttstr &msg, tTJSScriptBlock *block, tjs_int pos)
{
	TJSReportExceptionSource(msg, block, pos);
	throw eTJSScriptError(msg, block, pos);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptError(const tjs_char *msg, tTJSScriptBlock *block, tjs_int pos)
{
	TJSReportExceptionSource(msg, block, pos);
	throw eTJSScriptError(msg, block, pos);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptError(const ttstr &msg, tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos)
{
	TJSReportExceptionSource(msg, context, block, pos);
	throw eTJSScriptError(msg, block, pos);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptError(const tjs_char *msg, tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos)
{
	TJSReportExceptionSource(msg, context, block, pos);
	throw eTJSScriptError(msg, block, pos);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptException(const ttstr &msg, tTJSScriptBlock *block,
	tjs_int pos, tTJSVariant &val)
{
	TJSReportExceptionSource(msg, block, pos);
	throw eTJSScriptException(msg, block, pos, val);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptException(const tjs_char *msg, tTJSScriptBlock *block,
	tjs_int pos, tTJSVariant &val)
{
	TJSReportExceptionSource(msg, block, pos);
	throw eTJSScriptException(msg, block, pos, val);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptException(const ttstr &msg, tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos, tTJSVariant &val)
{
	TJSReportExceptionSource(msg, context, block, pos);
	throw eTJSScriptException(msg, block, pos, val);
}
//---------------------------------------------------------------------------
void TJS_eTJSScriptException(const tjs_char *msg, tTJSInterCodeContext *context,
	tTJSScriptBlock *block, tjs_int pos, tTJSVariant &val)
{
	TJSReportExceptionSource(msg, context, block, pos);
	throw eTJSScriptException(msg, block, pos, val);
}
//---------------------------------------------------------------------------
void TJS_eTJSCompileError(const ttstr & msg, tTJSScriptBlock *block, tjs_int pos)
{
	TJSReportExceptionSource(msg, block, pos);
	throw eTJSCompileError(msg, block, pos);
}
//---------------------------------------------------------------------------
void TJS_eTJSCompileError(const tjs_char *msg, tTJSScriptBlock *block, tjs_int pos)
{
	TJSReportExceptionSource(msg, block, pos);
	throw eTJSCompileError(msg, block, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJSThrowFrom_tjs_error(tjs_error hr, const tjs_char *name)
{
	// raise an exception descripted as tjs_error
	// name = variable name ( otherwide it can be NULL )

	switch(hr)
	{
	case TJS_E_MEMBERNOTFOUND:
	  {
		if(name)
		{
			ttstr str(TJSMemberNotFound);
			str.Replace(TJS_W("%1"), name);
			TJS_eTJSError(str);
		}
		else
		{
			TJS_eTJSError(TJSMemberNotFoundNoNameGiven);
		}
	  }
	case TJS_E_NOTIMPL:
		TJS_eTJSError(TJSNotImplemented);
	case TJS_E_INVALIDPARAM:
		TJS_eTJSError(TJSInvalidParam);
	case TJS_E_BADPARAMCOUNT:
		TJS_eTJSError(TJSBadParamCount);
	case TJS_E_INVALIDTYPE:
		TJS_eTJSError(TJSInvalidType);
	case TJS_E_ACCESSDENYED:
		TJS_eTJSError(TJSAccessDenyed);
	case TJS_E_INVALIDOBJECT:
		TJS_eTJSError(TJSInvalidObject);
	case TJS_E_NATIVECLASSCRASH:
		TJS_eTJSError(TJSNativeClassCrash);
	default:
		if(TJS_FAILED(hr))
		{
			tjs_char buf[256];
			TJS_sprintf(buf, TJS_W("Unknown failure : %08X"), hr);
			TJS_eTJSError(buf);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// error messages  ( can be localized )
//---------------------------------------------------------------------------
ttstr TJSNonamedException = TJS_W("No-named exception");
//---------------------------------------------------------------------------
} // namespace TJS


#define TJS_DECL_MESSAGE_BODY
#undef tjsErrorH
#include "tjsError.h"
//---------------------------------------------------------------------------

