//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Script Block Management
//---------------------------------------------------------------------------
// 2004/ 5/13 W.Dee
//   Added code for context-dependent post-positioned ! operator behavior.
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "tjsScriptBlock.h"
#include "tjsInterCodeGen.h"



namespace TJS
{
//---------------------------------------------------------------------------
int yyparse(void*);
//---------------------------------------------------------------------------
// tTJSScriptBlock
//---------------------------------------------------------------------------
tTJSScriptBlock::tTJSScriptBlock(tTJS * owner)
{
	RefCount = 1;
	Owner = owner;
	Owner->AddRef();

	Script = NULL;
	Name = NULL;

	InterCodeContext = NULL;
	TopLevelContext = NULL;
	LexicalAnalyzer = NULL;

	UsingPreProcessor = false;

	LineOffset = 0;

	Owner->AddScriptBlock(this);
}
//---------------------------------------------------------------------------
tTJSScriptBlock::~tTJSScriptBlock()
{
	if(TopLevelContext) TopLevelContext->Release(), TopLevelContext = NULL;
	while(ContextStack.size())
	{
		ContextStack.top()->Release();
		ContextStack.pop();
	}

	Owner->RemoveScriptBlock(this);

	if(LexicalAnalyzer) delete LexicalAnalyzer;

	if(Script) delete [] Script;
	if(Name) delete [] Name;

	Owner->Release();
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::AddRef(void)
{
	RefCount ++;
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::Release(void)
{
	if(RefCount <= 1)
		delete this;
	else
		RefCount--;
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::SetName(const tjs_char *name, tjs_int lineofs)
{
	if(Name) delete [] Name, Name = NULL;
	if(name)
	{
		LineOffset = lineofs;
		Name = new tjs_char[ TJS_strlen(name) + 1];
		TJS_strcpy(Name, name);
	}
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::Add(tTJSInterCodeContext * cntx)
{
	InterCodeContextList.push_back(cntx);
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::Remove(tTJSInterCodeContext * cntx)
{
	InterCodeContextList.remove(cntx);
}
//---------------------------------------------------------------------------
tjs_uint tTJSScriptBlock::GetTotalVMCodeSize() const
{
	tjs_uint size = 0;

	std::list<tTJSInterCodeContext *>::const_iterator i;
	for(i = InterCodeContextList.begin(); i != InterCodeContextList.end(); i++)
	{
		size += (*i)->GetCodeSize();
	}
	return size;
}
//---------------------------------------------------------------------------
tjs_uint tTJSScriptBlock::GetTotalVMDataSize() const
{
	tjs_uint size = 0;

	std::list<tTJSInterCodeContext *>::const_iterator i;
	for(i = InterCodeContextList.begin(); i != InterCodeContextList.end(); i++)
	{
		size += (*i)->GetDataSize();
	}
	return size;
}
//---------------------------------------------------------------------------
tjs_char * tTJSScriptBlock::GetLine(tjs_int line, tjs_int *linelength) const
{
	// note that this function DOES matter LineOffset
	line -= LineOffset;
	if(linelength) *linelength = LineLengthVector[line];
	return Script + LineVector[line];
}
//---------------------------------------------------------------------------
tjs_int tTJSScriptBlock::SrcPosToLine(tjs_int pos) const
{
	tjs_uint s = 0;
	tjs_uint e = LineVector.size();
	while(true)
	{
		if(e-s <= 1) return s + LineOffset; // LineOffset is added
		tjs_uint m = s + (e-s)/2;
		if(LineVector[m] > pos)
			e = m;
		else
			s = m;
	}
}
//---------------------------------------------------------------------------
tjs_int tTJSScriptBlock::LineToSrcPos(tjs_int line) const
{
	// assumes line is added by LineOffset
	line -= LineOffset;
	return LineVector[line];	
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::ConsoleOutput(const tjs_char *msg, void *data)
{
	tTJSScriptBlock *blk = (tTJSScriptBlock*)data;
	blk->Owner->OutputToConsole(msg);
}
//---------------------------------------------------------------------------
#ifdef TJS_DEBUG_PROFILE_TIME
tjs_uint parsetime = 0;
extern tjs_uint time_make_np;
extern tjs_uint time_PutData;
extern tjs_uint time_PutCode;
extern tjs_uint time_this_proxy;
extern tjs_uint time_Commit;
extern tjs_uint time_yylex;
extern tjs_uint time_GenNodeCode;

extern tjs_uint time_ns_Push;
extern tjs_uint time_ns_Pop;
extern tjs_uint time_ns_Find;
extern tjs_uint time_ns_Add;
extern tjs_uint time_ns_Remove;
extern tjs_uint time_ns_Commit;

#endif

void tTJSScriptBlock::SetText(tTJSVariant *result, const tjs_char *text,
	iTJSDispatch2 * context, bool isexpression)
{
	TJS_F_TRACE("tTJSScriptBlock::SetText");


	// compiles text and executes its global level scripts.
	// the script will be compiled as an expression if isexpressn is true.
	if(!text) return;
	if(!text[0]) return;

	TJS_D((TJS_W("Counting lines ...\n")))

	Script = new tjs_char[TJS_strlen(text)+1];
	TJS_strcpy(Script, text);

	// calculation of line-count
	tjs_char *ls = Script;
	tjs_char *p = Script;
	while(*p)
	{
		if(*p == TJS_W('\r') || *p == TJS_W('\n'))
		{
			LineVector.push_back(int(ls - Script));
			LineLengthVector.push_back(int(p - ls));
			if(*p == TJS_W('\r') && p[1] == TJS_W('\n')) p++;
			p++;
			ls = p;
		}
		else
		{
			p++;
		}
	}

	if(p!=ls)
	{
		LineVector.push_back(int(ls - Script));
		LineLengthVector.push_back(int(p - ls));
	}


	// parse and execute
#ifdef TJS_DEBUG_PROFILE_TIME
	{
	tTJSTimeProfiler p(parsetime);
#endif

	Parse(text, isexpression, result != NULL);

#ifdef TJS_DEBUG_PROFILE_TIME
	}

	{
		char buf[256];
		sprintf(buf, "parsing : %d", parsetime);
		OutputDebugString(buf);
		if(parsetime)
		{
		sprintf(buf, "Commit : %d (%d%%)", time_Commit, time_Commit*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "yylex : %d (%d%%)", time_yylex, time_yylex*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "MakeNP : %d (%d%%)", time_make_np, time_make_np*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "GenNodeCode : %d (%d%%)", time_GenNodeCode, time_GenNodeCode*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "  PutCode : %d (%d%%)", time_PutCode, time_PutCode*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "  PutData : %d (%d%%)", time_PutData, time_PutData*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "  this_proxy : %d (%d%%)", time_this_proxy, time_this_proxy*100/parsetime);
		OutputDebugString(buf);

		sprintf(buf, "ns::Push : %d (%d%%)", time_ns_Push, time_ns_Push*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "ns::Pop : %d (%d%%)", time_ns_Pop, time_ns_Pop*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "ns::Find : %d (%d%%)", time_ns_Find, time_ns_Find*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "ns::Remove : %d (%d%%)", time_ns_Remove, time_ns_Remove*100/parsetime);
		OutputDebugString(buf);
		sprintf(buf, "ns::Commit : %d (%d%%)", time_ns_Commit, time_ns_Commit*100/parsetime);
		OutputDebugString(buf);

		}
	}
#endif

#ifdef TJS_DEBUG_DISASM
	std::list<tTJSInterCodeContext *>::iterator i =
		InterCodeContextList.begin();
	while(i != InterCodeContextList.end())
	{
		ConsoleOutput(TJS_W(""), (void*)this);
		ConsoleOutput((*i)->GetName(), (void*)this);
		(*i)->Disassemble(ConsoleOutput, (void*)this);
		i++;
	}
#endif

	// execute global level script
	ExecuteTopLevelScript(result, context);

	if(InterCodeContextList.size() != 1)
	{
		// this is not a single-context script block
		// (may hook itself)
		// release all contexts and global at this time
		if(TopLevelContext) TopLevelContext->Release(), TopLevelContext = NULL;
		while(ContextStack.size())
		{
			ContextStack.top()->Release();
			ContextStack.pop();
		}
	}
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::ExecuteTopLevelScript(tTJSVariant *result,
	iTJSDispatch2 * context)
{
	if(TopLevelContext)
	{
#ifdef TJS_DEBUG_PROFILE_TIME
		clock_t start = clock();
#endif
		TopLevelContext->FuncCall(0, NULL, NULL, result, 0, NULL, context);
#ifdef TJS_DEBUG_PROFILE_TIME
		tjs_char str[100];
		TJS_sprintf(str, TJS_W("%d"), clock() - start);
		ConsoleOutput(str, (void*)this);
#endif
	}
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::PushContextStack(const tjs_char *name, tTJSContextType type)
{
	tTJSInterCodeContext *cntx;
	cntx = new tTJSInterCodeContext(InterCodeContext, name, this, type);
	if(InterCodeContext==NULL)
	{
		if(TopLevelContext) TJS_eTJSError(TJSInternalError);
		TopLevelContext = cntx;
		TopLevelContext->AddRef();
	}
	ContextStack.push(cntx);
	InterCodeContext = cntx;
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::PopContextStack(void)
{
	InterCodeContext->Commit();
	InterCodeContext->Release();
	ContextStack.pop();
	if(ContextStack.size() >= 1)
		InterCodeContext = ContextStack.top();
	else
		InterCodeContext = NULL;
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::Parse(const tjs_char *script, bool isexpr, bool resultneeded)
{
	TJS_F_TRACE("tTJSScriptBlock::Parse");

	if(!script) return;

	CompileErrorCount = 0;


	LexicalAnalyzer = new tTJSLexicalAnalyzer(this, script, isexpr, resultneeded);

	try
	{
		yyparse(this);
	}
	catch(...)
	{
		delete LexicalAnalyzer; LexicalAnalyzer=NULL;
		throw;
	}

	delete LexicalAnalyzer; LexicalAnalyzer=NULL;

	if(CompileErrorCount)
	{
		TJS_eTJSScriptError(FirstError, this, FirstErrorPos);
	}
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::SetFirstError(const tjs_char *error, tjs_int pos)
{
	if(CompileErrorCount == 0)
	{
		FirstError = error;
		FirstErrorPos = pos;
	}
}
//---------------------------------------------------------------------------
ttstr tTJSScriptBlock::GetNameInfo() const
{
	if(LineOffset == 0)
	{
		return ttstr(Name);
	}
	else
	{
		return ttstr(Name) + TJS_W("(line +") + ttstr(LineOffset) + TJS_W(")");
	}
}
//---------------------------------------------------------------------------
void tTJSScriptBlock::Dump() const
{
	std::list<tTJSInterCodeContext *>::const_iterator i =
		InterCodeContextList.begin();
	while(i != InterCodeContextList.end())
	{
		ConsoleOutput(TJS_W(""), (void*)this);
		tjs_char ptr[256];
		TJS_sprintf(ptr, TJS_W(" 0x%p"), (*i));
		ConsoleOutput((ttstr(TJS_W("(")) + ttstr((*i)->GetContextTypeName()) +
			TJS_W(") ") + ttstr((*i)->GetName()) + ptr).c_str(), (void*)this);
		(*i)->Disassemble(ConsoleOutput, (void*)this);
		i++;
	}
}
//---------------------------------------------------------------------------



} // namespace



