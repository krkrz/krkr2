//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS Debugging support
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "tjsDebug.h"
#include "tjsHashSearch.h"
#include "tjsInterCodeGen.h"



namespace TJS
{

//---------------------------------------------------------------------------
// ObjectHashMap : hash map to track object construction/destruction
//---------------------------------------------------------------------------
class tTJSObjectHashMap;
tTJSObjectHashMap * TJSObjectHashMap;
class tTJSObjectHashMap
{
	tTJSHashTable<void *, tTJSObjectHashMapRecord, tTJSHashFunc<void *>, 1024> Hash;

	tjs_int RefCount;

public:
	tTJSObjectHashMap()
	{
		RefCount = 1;
		TJSObjectHashMap = this;
	}

protected:
	~tTJSObjectHashMap()
	{
		TJSObjectHashMap = NULL;
	}

protected:
	void _AddRef() { RefCount ++; }
	void _Release() { if(RefCount == 1) delete this; else RefCount --; }

public:
	static void AddRef()
	{
		if(TJSObjectHashMap)
			TJSObjectHashMap->_AddRef();
		else
			new tTJSObjectHashMap();
	}

	static void Release()
	{
		if(TJSObjectHashMap)
			TJSObjectHashMap->_Release();
	}

	void Add(void * object,
		const tTJSObjectHashMapRecord & record)
	{
		Hash.Add(object, record);
	}

	void Remove(void * object)
	{
		Hash.Delete(object);
	}

};
//---------------------------------------------------------------------------
void TJSAddRefObjectHashMap()
{
	tTJSObjectHashMap::AddRef();
}
//---------------------------------------------------------------------------
void TJSReleaseObjectHashMap()
{
	tTJSObjectHashMap::Release();
}
//---------------------------------------------------------------------------
void TJSAddObjectHashRecord(void * object,
	const tTJSObjectHashMapRecord & record)
{
	if(TJSObjectHashMap) TJSObjectHashMap->Add(object, record);
}
//---------------------------------------------------------------------------
void TJSRemoveObjectHashRecord(void * object)
{
	if(TJSObjectHashMap) TJSObjectHashMap->Remove(object);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// StackTracer : stack to trace function call trace
//---------------------------------------------------------------------------
class tTJSStackTracer;
tTJSStackTracer * TJSStackTracer;
//---------------------------------------------------------------------------
struct tTJSStackRecord
{
	tTJSInterCodeContext * Context;
	const tjs_int * CodeBase;
	tjs_int * const * CodePtr;
	bool InTry;

	tTJSStackRecord(tTJSInterCodeContext * context, bool in_try)
	{
		CodeBase = NULL;
		CodePtr = NULL;
		InTry = in_try;
		Context = context;
		if(Context) Context->AddRef();
	}

	~tTJSStackRecord()
	{
		if(Context) Context->Release();
	}

	tTJSStackRecord(const tTJSStackRecord & rhs)
	{
		Context = NULL;
		*this = rhs;
	}

	void operator = (const tTJSStackRecord & rhs)
	{
		if(Context != rhs.Context)
		{
			if(Context) Context->Release(), Context = NULL;
			Context = rhs.Context;
			if(Context) Context->AddRef();
		}
		CodeBase = rhs.CodeBase;
		CodePtr = rhs.CodePtr;
		InTry = rhs.InTry;
	}
};

//---------------------------------------------------------------------------
class tTJSStackTracer
{
	std::vector<tTJSStackRecord> Stack;

	tjs_int RefCount;

public:
	tTJSStackTracer()
	{
		RefCount = 1;
		TJSStackTracer = this;
	}

protected:
	~tTJSStackTracer()
	{
		TJSStackTracer = NULL;
	}

protected:
	void _AddRef() { RefCount ++; }
	void _Release() { if(RefCount == 1) delete this; else RefCount --; }

public:
	static void AddRef()
	{
		if(TJSStackTracer)
			TJSStackTracer->_AddRef();
		else
			new tTJSStackTracer();
	}

	static void Release()
	{
		if(TJSStackTracer)
			TJSStackTracer->_Release();
	}

	void Push(tTJSInterCodeContext *context, bool in_try)
	{
		Stack.push_back(tTJSStackRecord(context, in_try));
	}

	void Pop()
	{
		Stack.pop_back();
	}

	void SetCodePointer(const tjs_int32 * codebase, tjs_int32 * const * codeptr)
	{
		tjs_uint size = Stack.size();
		if(size < 1) return;
		tjs_uint top = size - 1;
		Stack[top].CodeBase = codebase;
		Stack[top].CodePtr = codeptr;
	}

	ttstr GetTraceString(tjs_int limit)
	{
		// get stack trace string
		ttstr ret;
		tjs_int top = (tjs_int)(Stack.size() - 1);
		while(top >= 0)
		{
			if(!ret.IsEmpty()) ret += TJS_W(" <-- ");

			const tTJSStackRecord & rec = Stack[top];
			ttstr str;
			if(rec.CodeBase && rec.CodePtr)
			{
				str = rec.Context->GetPositionDescriptionString(
					*rec.CodePtr - rec.CodeBase);
			}
			else
			{
				str = rec.Context->GetShortDescription();
			}

			ret += str;

			// skip try block stack.
			// 'try { } catch' blocks are implemented as sub-functions
			// in a parent function.
			while(top >= 0 && Stack[top].InTry) top--;

			// check limit
			if(limit)
			{
				limit --;
				if(limit <= 0) break;
			}

			top --;
		}

		return ret;
	}
};
//---------------------------------------------------------------------------
void TJSAddRefStackTracer()
{
	tTJSStackTracer::AddRef();
}
//---------------------------------------------------------------------------
void TJSReleaseStackTracer()
{
	tTJSStackTracer::Release();
}
//---------------------------------------------------------------------------
void TJSStackTracerPush(tTJSInterCodeContext *context, bool in_try)
{
	if(TJSStackTracer)
		TJSStackTracer->Push(context, in_try);
}
//---------------------------------------------------------------------------
void TJSStackTracerSetCodePointer(const tjs_int32 * codebase,
	tjs_int32 * const * codeptr)
{
	if(TJSStackTracer)
		TJSStackTracer->SetCodePointer(codebase, codeptr);
}
//---------------------------------------------------------------------------
void TJSStackTracerPop()
{
	if(TJSStackTracer)
		TJSStackTracer->Pop();
}
//---------------------------------------------------------------------------
ttstr TJSGetStackTraceString(tjs_int limit)
{
	if(TJSStackTracer)
		return TJSStackTracer->GetTraceString(limit);
	else
		return ttstr();
}
//---------------------------------------------------------------------------





} // namespace TJS

