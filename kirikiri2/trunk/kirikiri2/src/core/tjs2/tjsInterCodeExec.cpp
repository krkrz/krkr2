//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Intermediate Code Execution
//---------------------------------------------------------------------------

#include "tjsCommHead.h"
#pragma hdrstop

#include "tjsInterCodeExec.h"
#include "tjsInterCodeGen.h"
#include "tjsScriptBlock.h"
#include "tjsError.h"
#include "tjs.h"
#include "tjsUtils.h"
#include "tjsNative.h"
#include "tjsArray.h"


namespace TJS
{
//---------------------------------------------------------------------------
// utility functions
//---------------------------------------------------------------------------
static void ThrowFrom_tjs_error_num(tjs_error hr, tjs_int num)
{
	tjs_char buf[34];
	TJS_int_to_str(num, buf);
	TJSThrowFrom_tjs_error(hr, buf);
}
//---------------------------------------------------------------------------
static void ThrowInvalidVMCode()
{
	TJS_eTJSError(TJSInvalidOpecode);
}
//---------------------------------------------------------------------------
static void GetStringProperty(tTJSVariant *result, const tTJSVariant *str,
	const tTJSVariant &member)
{
	// processes properties toward strings.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const tjs_char *name = member.GetString();
		if(!name) TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, TJS_W(""));

		if(!TJS_strcmp(name, TJS_W("length")))
		{
			// get string length
			const tTJSVariantString * s = str->AsStringNoAddRef();
#ifdef __CODEGUARD__
			if(!s)
				*result = tTVInteger(0); // tTJSVariantString::GetLength can return zero if 'this' is NULL
			else
#endif
			*result = tTVInteger(s->GetLength());
			return;
		}
		else if(name[0] >= TJS_W('0') && name[0] <= TJS_W('9'))
		{
			const tTJSVariantString * valstr = str->AsStringNoAddRef();
			const tjs_char *s = str->GetString();
			tjs_int n = TJS_atoi(name);
			tjs_int len = valstr->GetLength();
			if(n == len) { *result = tTJSVariant(TJS_W("")); return; }
			if(n<0 || n>len)
				TJS_eTJSError(TJSRangeError);
			tjs_char bf[2];
			bf[1] = 0;
			bf[0] = s[n];
			*result = tTJSVariant(bf);
			return;
		}

		TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		const tTJSVariantString * valstr = str->AsStringNoAddRef();
		const tjs_char *s = str->GetString();
		tjs_int n = (tjs_int)member.AsInteger();
		tjs_int len = valstr->GetLength();
		if(n == len) { *result = tTJSVariant(TJS_W("")); return; }
		if(n<0 || n>len)
			TJS_eTJSError(TJSRangeError);
		tjs_char bf[2];
		bf[1] = 0;
		bf[0] = s[n];
		*result = tTJSVariant(bf);
		return;
	}
}
//---------------------------------------------------------------------------
static void SetStringProperty(tTJSVariant *param, const tTJSVariant *str,
	const tTJSVariant &member)
{
	// processes properties toward strings.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const tjs_char *name = member.GetString();
		if(!name) TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, TJS_W(""));

		if(!TJS_strcmp(name, TJS_W("length")))
		{
			TJSThrowFrom_tjs_error(TJS_E_ACCESSDENYED, TJS_W(""));
		}
		else if(name[0] >= TJS_W('0') && name[0] <= TJS_W('9'))
		{
			TJSThrowFrom_tjs_error(TJS_E_ACCESSDENYED, TJS_W(""));
		}

		TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		TJSThrowFrom_tjs_error(TJS_E_ACCESSDENYED, TJS_W(""));
	}
}
//---------------------------------------------------------------------------
static void GetOctetProperty(tTJSVariant *result, const tTJSVariant *octet,
	const tTJSVariant &member)
{
	// processes properties toward octets.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const tjs_char *name = member.GetString();
		if(!name) TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, TJS_W(""));

		if(!TJS_strcmp(name, TJS_W("length")))
		{
			// get string length
			tTJSVariantOctet *o = octet->AsOctetNoAddRef();
			if(o)
				*result = tTVInteger(o->GetLength());
			else
				*result = tTVInteger(0);
			return;
		}
		else if(name[0] >= TJS_W('0') && name[0] <= TJS_W('9'))
		{
			tTJSVariantOctet *o = octet->AsOctetNoAddRef();
			tjs_int n = TJS_atoi(name);
			tjs_int len = o?o->GetLength():0;
			if(n<0 || n>=len)
				TJS_eTJSError(TJSRangeError);
			*result = tTVInteger(o->GetData()[n]);
			return;
		}

		TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		tTJSVariantOctet *o = octet->AsOctetNoAddRef();
		tjs_int n = (tjs_int)member.AsInteger();
		tjs_int len = o?o->GetLength():0;
		if(n<0 || n>=len)
			TJS_eTJSError(TJSRangeError);
		*result = tTVInteger(o->GetData()[n]);
		return;
	}
}
//---------------------------------------------------------------------------
static void SetOctetProperty(tTJSVariant *param, const tTJSVariant *octet,
	const tTJSVariant &member)
{
	// processes properties toward octets.
	if(member.Type() != tvtInteger && member.Type() != tvtReal)
	{
		const tjs_char *name = member.GetString();
		if(!name) TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, TJS_W(""));

		if(!TJS_strcmp(name, TJS_W("length")))
		{
			TJSThrowFrom_tjs_error(TJS_E_ACCESSDENYED, TJS_W(""));
		}
		else if(name[0] >= TJS_W('0') && name[0] <= TJS_W('9'))
		{
			TJSThrowFrom_tjs_error(TJS_E_ACCESSDENYED, TJS_W(""));
		}

		TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, name);
	}
	else // member.Type() == tvtInteger || member.Type() == tvtReal
	{
		TJSThrowFrom_tjs_error(TJS_E_ACCESSDENYED, TJS_W(""));
	}
}

//---------------------------------------------------------------------------
// tTJSObjectProxy
//---------------------------------------------------------------------------
class tTJSObjectProxy : public iTJSDispatch2
{
/*
	a class that do:
	1. first access to the Dispatch1
	2. if failed, then access to the Dispatch2
*/
//	tjs_uint RefCount;

public:
	tTJSObjectProxy()
	{
//		RefCount = 1;
//		Dispatch1 = NULL;
//		Dispatch2 = NULL;
		// Dispatch1 and Dispatch2 are to be set by subsequent call of SetObjects
	};

	virtual ~tTJSObjectProxy()
	{
		if(Dispatch1) Dispatch1->Release();
		if(Dispatch2) Dispatch2->Release();
	};

	void SetObjects(iTJSDispatch2 *dsp1, iTJSDispatch2 *dsp2)
	{
		Dispatch1 = dsp1;
		Dispatch2 = dsp2;
		if(dsp1) dsp1->AddRef();
		if(dsp2) dsp2->AddRef();
	}

private:
	iTJSDispatch2 *Dispatch1;
	iTJSDispatch2 *Dispatch2;

public:

	tjs_uint TJS_INTF_METHOD  AddRef(void)
	{
		return 1;
//		return ++RefCount;
	}

	tjs_uint TJS_INTF_METHOD  Release(void)
	{
		return 1;
/*
		if(RefCount == 1)
		{
			delete this;
			return 0;
		}
		else
		{
			RefCount--;
		}
		return RefCount;
*/
	}

//--
#define OBJ1 ((objthis)?(objthis):(Dispatch1))
#define OBJ2 ((objthis)?(objthis):(Dispatch2))

	tjs_error TJS_INTF_METHOD
	FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->FuncCall(flag, membername, hint, result, numparams, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->FuncCall(flag, membername, hint, result, numparams, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	FuncCallByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->FuncCallByNum(flag, num, result, numparams, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->FuncCallByNum(flag, num, result, numparams, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->PropGet(flag, membername, hint, result, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropGet(flag, membername, hint, result, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->PropGetByNum(flag, num, result, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropGetByNum(flag, num, result, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	const tTJSVariant *param,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->PropSet(flag, membername, hint, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropSet(flag, membername, hint, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant *param,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->PropSetByNum(flag, num, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropSetByNum(flag, num, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	GetCount(tjs_int *result, const tjs_char *membername, tjs_uint32 *hint,
	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->GetCount(result, membername, hint, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->GetCount(result, membername, hint, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	GetCountByNum(tjs_int *result, tjs_int num, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->GetCountByNum(result, num, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->GetCountByNum(result, num, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	PropSetByVS(tjs_uint32 flag, tTJSVariantString *membername,
		const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->PropSetByVS(flag, membername, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->PropSetByVS(flag, membername, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	EnumMembers(tjs_uint32 flag, tTJSVariantClosure *callback, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	DeleteMember(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->DeleteMember(flag, membername, hint, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->DeleteMember(flag, membername, hint, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->DeleteMemberByNum(flag, num, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->DeleteMemberByNum(flag, num, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	Invalidate(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->Invalidate(flag, membername, hint, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->Invalidate(flag, membername, hint, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	InvalidateByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->InvalidateByNum(flag, num, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->InvalidateByNum(flag, num, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	IsValid(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->IsValid(flag, membername, hint, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsValid(flag, membername, hint, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	IsValidByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->IsValidByNum(flag, num, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsValidByNum(flag, num, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->CreateNew(flag, membername, hint, result, numparams, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->CreateNew(flag, membername, hint, result, numparams, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	CreateNewByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->CreateNewByNum(flag, num, result, numparams, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->CreateNewByNum(flag, num, result, numparams, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	GetSuperClass(tjs_uint32 flag, iTJSDispatch2 **result,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->GetSuperClass(flag, result, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->GetSuperClass(flag, result, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	const tjs_char *classname,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->IsInstanceOf(flag, membername, hint, classname, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsInstanceOf(flag, membername, hint, classname, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	IsInstanceOfByNum(tjs_uint32 flag, tjs_int num, const tjs_char *classname,
		iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->IsInstanceOfByNum(flag, num, classname, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->IsInstanceOfByNum(flag, num, classname, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	Operation(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	tTJSVariant *result,
		const tTJSVariant *param,	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->Operation(flag, membername, hint, result, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->Operation(flag, membername, hint, result, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	OperationByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result,
		const tTJSVariant *param,	iTJSDispatch2 *objthis)
	{
		tjs_error hr =
			Dispatch1->OperationByNum(flag, num, result, param, OBJ1);
		if(hr == TJS_E_MEMBERNOTFOUND && Dispatch1 != Dispatch2)
			return Dispatch2->OperationByNum(flag, num, result, param, OBJ2);
		return hr;
	}

	tjs_error TJS_INTF_METHOD
	NativeInstanceSupport(tjs_uint32 flag, tjs_int32 classid,
		iTJSNativeInstance **pointer)  { return TJS_E_NOTIMPL; }

	tjs_error TJS_INTF_METHOD
	ClassInstanceInfo(tjs_uint32 flag, tjs_uint num, tTJSVariant *value)
		{ return TJS_E_NOTIMPL;	}
};
#undef OBJ1
#undef OBJ2

//---------------------------------------------------------------------------
// tTJSVariantArrayStack
//---------------------------------------------------------------------------
// TODO: adjust TJS_VA_ONE_ALLOC_MIN
#define TJS_VA_ONE_ALLOC_MAX 1024
#define TJS_COMPACT_FREQ 10000
static tjs_int TJSCompactVariantArrayMagic = 0;

class tTJSVariantArrayStack
{
//	tTJSCriticalSection CS;

	struct tVariantArray
	{
		tTJSVariant *Array;
		tjs_int Using;
		tjs_int Allocated;
	};

	tVariantArray * Arrays; // array of array
	tjs_int NumArraysAllocated;
	tjs_int NumArraysUsing;
	tVariantArray * Current;
	tjs_int CompactVariantArrayMagic;
	tjs_int OperationDisabledCount;

	void IncreaseVariantArray(tjs_int num);

	void DecreaseVariantArray(void);

	void InternalCompact(void);


public:
	tTJSVariantArrayStack();
	~tTJSVariantArrayStack();

	tTJSVariant * Allocate(tjs_int num);

	void Deallocate(tjs_int num, tTJSVariant *ptr);

	void Compact() { InternalCompact(); }

} *TJSVariantArrayStack = NULL;
//---------------------------------------------------------------------------
tTJSVariantArrayStack::tTJSVariantArrayStack()
{
	NumArraysAllocated = NumArraysUsing = 0;
	Arrays = NULL;
	Current = NULL;
	OperationDisabledCount = 0;
	CompactVariantArrayMagic = TJSCompactVariantArrayMagic;
}
//---------------------------------------------------------------------------
tTJSVariantArrayStack::~tTJSVariantArrayStack()
{
	OperationDisabledCount++;
	tjs_int i;
	for(i = 0; i<NumArraysAllocated; i++)
	{
		delete [] Arrays[i].Array;
	}
	TJS_free(Arrays), Arrays = NULL;
}
//---------------------------------------------------------------------------
void tTJSVariantArrayStack::IncreaseVariantArray(tjs_int num)
{
	// increase array block
	NumArraysUsing++;
	if(NumArraysUsing > NumArraysAllocated)
	{
		Arrays = (tVariantArray*)
			TJS_realloc(Arrays, sizeof(tVariantArray)*(NumArraysUsing));
		NumArraysAllocated = NumArraysUsing;
		Current = Arrays + NumArraysUsing -1;
		Current->Array = new tTJSVariant[num];
	}
	else
	{
		Current = Arrays + NumArraysUsing -1;
	}

	Current->Allocated = num;
	Current->Using = 0;
}
//---------------------------------------------------------------------------
void tTJSVariantArrayStack::DecreaseVariantArray(void)
{
	// decrease array block
	NumArraysUsing--;
	if(NumArraysUsing == 0)
		Current = NULL;
	else
		Current = Arrays + NumArraysUsing-1;
}
//---------------------------------------------------------------------------
void tTJSVariantArrayStack::InternalCompact(void)
{
	// minimize variant array block
	OperationDisabledCount++;
	try
	{
		while(NumArraysAllocated > NumArraysUsing)
		{
			NumArraysAllocated --;
			delete [] Arrays[NumArraysAllocated].Array;
		}

		if(Current)
		{
			for(tjs_int i = Current->Using; i < Current->Allocated; i++)
				Current->Array[i].Clear();
		}

		if(NumArraysUsing == 0)
		{
			if(Arrays) TJS_free(Arrays), Arrays = NULL;
		}
		else
		{
			Arrays = (tVariantArray*)
				TJS_realloc(Arrays, sizeof(tVariantArray)*(NumArraysUsing));
		}
	}
	catch(...)
	{
		OperationDisabledCount--;
		throw;
	}
	OperationDisabledCount--;
}
//---------------------------------------------------------------------------
inline tTJSVariant * tTJSVariantArrayStack::Allocate(tjs_int num)
{
//		tTJSCSH csh(CS);

	if(!OperationDisabledCount && num < TJS_VA_ONE_ALLOC_MAX)
	{
		if(!Current || Current->Using + num > Current->Allocated)
		{
			IncreaseVariantArray( TJS_VA_ONE_ALLOC_MAX );
		}
		tTJSVariant *ret = Current->Array + Current->Using;
		Current->Using += num;
		return ret;
	}
	else
	{
		return new tTJSVariant[num];
	}
}
//---------------------------------------------------------------------------
inline void tTJSVariantArrayStack::Deallocate(tjs_int num, tTJSVariant *ptr)
{
//		tTJSCSH csh(CS);

	if(!OperationDisabledCount && num < TJS_VA_ONE_ALLOC_MAX)
	{
		Current->Using -= num;
		if(Current->Using == 0)
		{
			DecreaseVariantArray();
		}
	}
	else
	{
		delete [] ptr;
	}

	if(!OperationDisabledCount)
	{
		if(CompactVariantArrayMagic != TJSCompactVariantArrayMagic)
		{
			Compact();
			CompactVariantArrayMagic = TJSCompactVariantArrayMagic;
		}
	}
}
//---------------------------------------------------------------------------
static tjs_int TJSVariantArrayStackRefCount = 0;
//---------------------------------------------------------------------------
void TJSVariantArrayStackAddRef()
{
	if(TJSVariantArrayStackRefCount == 0)
	{
		TJSVariantArrayStack = new tTJSVariantArrayStack;
	}
	TJSVariantArrayStackRefCount++;
}
//---------------------------------------------------------------------------
void TJSVariantArrayStackRelease()
{
	if(TJSVariantArrayStackRefCount == 1)
	{
		delete TJSVariantArrayStack;
		TJSVariantArrayStack = NULL;
		TJSVariantArrayStackRefCount = 0;
	}
	else
	{
		TJSVariantArrayStackRefCount--;
	}
}
//---------------------------------------------------------------------------
void TJSVariantArrayStackCompact()
{
	TJSCompactVariantArrayMagic++;
}
//---------------------------------------------------------------------------
void TJSVariantArrayStackCompactNow()
{
	if(TJSVariantArrayStack) TJSVariantArrayStack->Compact();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSInterCodeContext ( class definitions are in tjsInterCodeGen.h )
//---------------------------------------------------------------------------
/*
void tTJSInterCodeContext::ExecuteAsGlobal(tTJSVariant *result)
{
	tjs_int num_alloc = MaxVariableCount + VariableReserveCount + 1 + MaxFrameCount;
	TJSVariantArrayStackAddRef();
//	AddRef();
	try
	{
		tTJSVariant *regs =
			TJSVariantArrayStack->Allocate(num_alloc);
		tTJSVariant *ra = regs + MaxVariableCount + VariableReserveCount;

		iTJSDispatch2 *global = Block->GetTJS()->GetGlobalNoAddRef();
		ra[-1] = tTJSVariant(global, global);
		ra[0].Clear();

		try
		{
			ExecuteCode(ra, 0, NULL, 0, result);
		}
		catch(...)
		{
#if 0
			for(tjs_int i=0; i<num_alloc; i++)
				regs[i].Clear();
#endif
			TJSVariantArrayStack->Deallocate(num_alloc, regs);
			throw;
		}

#if 0
		for(tjs_int i=0; i<MaxVariableCount + VariableReserveCount; i++)
			regs[i].Clear();
#endif
		TJSVariantArrayStack->Deallocate(num_alloc, regs);
	}
	catch(...)
	{
//		Release();
		TJSVariantArrayStackRelease();
		throw;
	}
//	Release();
	TJSVariantArrayStackRelease();
}
*/
//---------------------------------------------------------------------------
void tTJSInterCodeContext::ExecuteAsFunction(iTJSDispatch2 *objthis,
	tTJSVariant **args, tjs_int numargs, tTJSVariant *result, tjs_int start_ip)
{
	tjs_int num_alloc = MaxVariableCount + VariableReserveCount + 1 + MaxFrameCount;
	TJSVariantArrayStackAddRef();
//	AddRef();
//	if(objthis) objthis->AddRef();
	try
	{
		tTJSVariant *regs =
			TJSVariantArrayStack->Allocate(num_alloc);
		tTJSVariant *ra = regs + MaxVariableCount + VariableReserveCount; // register area

		// objthis-proxy

		tTJSObjectProxy proxy;
		if(objthis)
		{
			proxy.SetObjects(objthis, Block->GetTJS()->GetGlobalNoAddRef());
			// TODO: caching of objthis-proxy

			ra[-2] = &proxy;
		}
		else
		{
			proxy.SetObjects(NULL, NULL);

			iTJSDispatch2 *global = Block->GetTJS()->GetGlobalNoAddRef();

			ra[-2].SetObject(global, global);
		}

/*
		if(objthis)
		{
			// TODO: caching of objthis-proxy
			tTJSObjectProxy *proxy = new tTJSObjectProxy();
			proxy->SetObjects(objthis, Block->GetTJS()->GetGlobalNoAddRef());

			ra[-2] = proxy;

			proxy->Release();
		}
		else
		{
			iTJSDispatch2 *global = Block->GetTJS()->GetGlobalNoAddRef();

			ra[-2].SetObject(global, global);
		}
*/
		try
		{
			ra[-1].SetObject(objthis, objthis);
			ra[0].Clear();

			// transfer arguments
			if(numargs >= FuncDeclArgCount)
			{
				// given arguments are greater than or equal to desired arguments
				if(FuncDeclArgCount)
				{
					tTJSVariant *r = ra - 3;
					tTJSVariant **a = args;
					tjs_int n = FuncDeclArgCount;
					while(true)
					{
						*r = **(a++);
						n--;
						if(!n) break;
						r--;
					}
				}
			}
			else
			{
				// given arguments are less than desired arguments
				tTJSVariant *r = ra - 3;
				tTJSVariant **a = args;
				tjs_int i;
				for(i = 0; i<numargs; i++) *(r--) = **(a++);
				for(; i<FuncDeclArgCount; i++) (r--)->Clear();
			}

			// execute
			ExecuteCode(ra, start_ip, args, numargs, result);
		}
		catch(...)
		{
#if 0
			for(tjs_int i=0; i<num_alloc; i++) regs[i].Clear();
#endif
			ra[-2].Clear(); // at least we must clear the object placed at local stack
			TJSVariantArrayStack->Deallocate(num_alloc, regs);
			throw;
		}

#if 0
		for(tjs_int i=0; i<MaxVariableCount + VariableReserveCount; i++)
			regs[i].Clear();
#endif
		ra[-2].Clear(); // at least we must clear the object placed at local stack

		TJSVariantArrayStack->Deallocate(num_alloc, regs);
	}
	catch(...)
	{
//		if(objthis) objthis->Release();
//		Release();
		TJSVariantArrayStackRelease();
		throw;
	}
//	if(objthis) objthis->Release();
//	Release();
	TJSVariantArrayStackRelease();
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::DisplayExceptionGeneratedCode(tjs_int codepos,
	const tTJSVariant *ra)
{
	tTJS *tjs = Block->GetTJS();
	ttstr info(
		TJS_W("==== An exception occured at ") +
		GetPositionDescriptionString(codepos) +
		TJS_W(", VM ip = ") + ttstr(codepos) + TJS_W(" ===="));
	tjs_int info_len = info.GetLen();

	tjs->OutputToConsole(info.c_str());
	tjs->OutputToConsole(TJS_W("-- Disassembled VM code --"));
	DisassenbleSrcLine(codepos);

	tjs->OutputToConsole(TJS_W("-- Register dump --"));

	const tTJSVariant *ra_start = ra - (MaxVariableCount + VariableReserveCount);
	tjs_int ra_count = MaxVariableCount + VariableReserveCount + 1 + MaxFrameCount;
	ttstr line;
	for(tjs_int i = 0; i < ra_count; i ++)
	{
		ttstr reg_info = TJS_W("%") + ttstr(i - (MaxVariableCount + VariableReserveCount))
			+ TJS_W("=") + TJSVariantToReadableString(ra_start[i]);
		if(line.GetLen() + reg_info.GetLen() + 2 > info_len)
		{
			tjs->OutputToConsole(line.c_str());
			line = reg_info;
		}
		else
		{
			if(!line.IsEmpty()) line += TJS_W("  ");
			line += reg_info;
		}
	}

	if(!line.IsEmpty())
	{
		tjs->OutputToConsole(line.c_str());
	}

	tjs->OutputToConsoleSeparator(TJS_W("-"), info_len);
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::ThrowScriptException(tTJSVariant &val,
	tTJSScriptBlock *block, tjs_int srcpos)
{
	tTJSString msg;
	if(val.Type() == tvtObject)
	{
		try
		{
			tTJSVariantClosure clo = val.AsObjectClosureNoAddRef();
			if(clo.Object != NULL)
			{
				tTJSVariant v2;
				static tTJSString message_name(TJS_W("message"));
				tjs_error hr = clo.PropGet(0, message_name.c_str(),
					message_name.GetHint(), &v2, NULL);
				if(TJS_SUCCEEDED(hr))
				{
					msg = ttstr(TJS_W("script exception : ")) + ttstr(v2);
				}
			}
		}
		catch(...)
		{
		}
	}

	if(msg.IsEmpty())
	{
		msg = TJS_W("script exception");
	}

	TJS_eTJSScriptException(msg, this, srcpos, val);
}
//---------------------------------------------------------------------------
tjs_int tTJSInterCodeContext::ExecuteCode(tTJSVariant *ra_org, tjs_int startip,
	tTJSVariant **args, tjs_int numargs, tTJSVariant *result)
{
	// execute VM codes
	tjs_int32 *codesave;
	try
	{
		tjs_int32 *code = codesave = CodeArea + startip;
		tTJSVariant *ra = ra_org;
		tTJSVariant *da = DataArea;

		bool flag = false;

		while(true)
		{
			codesave = code;
			switch(*code)
			{
			case VM_NOP:
				code ++;
				break;

			case VM_CONST:
				TJS_GET_VM_REG(ra, code[1]).CopyRef(TJS_GET_VM_REG(da, code[2]));
				code += 3;
				break;

			case VM_CP:
				TJS_GET_VM_REG(ra, code[1]).CopyRef(TJS_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CL:
				TJS_GET_VM_REG(ra, code[1]).Clear();
				code += 2;
				break;

			case VM_CCL:
				ContinuousClear(ra, code);
				code += 3;
				break;

			case VM_TT:
				flag = TJS_GET_VM_REG(ra, code[1]).operator bool();
				code += 2;
				break;

			case VM_TF:
				flag = !(TJS_GET_VM_REG(ra, code[1]).operator bool());
				code += 2;
				break;

			case VM_CEQ:
				flag = TJS_GET_VM_REG(ra, code[1]).NormalCompare(
					TJS_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CDEQ:
				flag = TJS_GET_VM_REG(ra, code[1]).DiscernCompare(
					TJS_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CLT:
				flag = TJS_GET_VM_REG(ra, code[1]).GreaterThan(
					TJS_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_CGT:
				flag = TJS_GET_VM_REG(ra, code[1]).LittlerThan(
					TJS_GET_VM_REG(ra, code[2]));
				code += 3;
				break;

			case VM_SETF:
				TJS_GET_VM_REG(ra, code[1]) = flag;
				code += 2;
				break;

			case VM_SETNF:
				TJS_GET_VM_REG(ra, code[1]) = !flag;
				code += 2;
				break;

			case VM_LNOT:
				TJS_GET_VM_REG(ra, code[1]).logicalnot();
				code += 2;
				break;

			case VM_NF:
				flag = !flag;
				code ++;
				break;

			case VM_JF:
				if(flag)
					TJS_ADD_VM_CODE_ADDR(code, code[1]);
				else
					code += 2;
				break;

			case VM_JNF:
				if(!flag)
					TJS_ADD_VM_CODE_ADDR(code, code[1]);
				else
					code += 2;
				break;

			case VM_JMP:
				TJS_ADD_VM_CODE_ADDR(code, code[1]);
				break;

			case VM_INC:
				TJS_GET_VM_REG(ra, code[1]).increment();
				code += 2;
				break;

			case VM_INCPD:
				OperatePropertyDirect0(ra, code, TJS_OP_INC);
				code += 4;
				break;

			case VM_INCPI:
				OperatePropertyIndirect0(ra, code, TJS_OP_INC);
				code += 4;
				break;

			case VM_DEC:
				TJS_GET_VM_REG(ra, code[1]).decrement();
				code += 2;
				break;

			case VM_DECPD:
				OperatePropertyDirect0(ra, code, TJS_OP_DEC);
				code += 4;
				break;

			case VM_DECPI:
				OperatePropertyIndirect0(ra, code, TJS_OP_DEC);
				code += 4;
				break;

#define TJS_DEF_VM_P(vmcode, rope) \
		case VM_##vmcode: \
			TJS_GET_VM_REG(ra, code[1]).rope(TJS_GET_VM_REG(ra, code[2])); \
			code += 3; \
			break; \
		case VM_##vmcode##PD: \
			OperatePropertyDirect(ra, code, TJS_OP_##vmcode); \
			code += 5; \
			break; \
		case VM_##vmcode##PI: \
			OperatePropertyIndirect(ra, code, TJS_OP_##vmcode); \
			code += 5; \
			break

			TJS_DEF_VM_P(LOR, logicalorequal);
			TJS_DEF_VM_P(LAND, logicalandequal);
			TJS_DEF_VM_P(BOR, operator |=);
			TJS_DEF_VM_P(BXOR, operator ^=);
			TJS_DEF_VM_P(BAND, operator &=);
			TJS_DEF_VM_P(SAR, operator >>=);
			TJS_DEF_VM_P(SAL, operator <<=);
			TJS_DEF_VM_P(SR, rbitshiftequal);
			TJS_DEF_VM_P(ADD, operator +=);
			TJS_DEF_VM_P(SUB, operator -=);
			TJS_DEF_VM_P(MOD, operator %=);
			TJS_DEF_VM_P(DIV, operator /=);
			TJS_DEF_VM_P(IDIV, idivequal);
			TJS_DEF_VM_P(MUL, operator *=);

#undef TJS_DEF_VM_P

			case VM_BNOT:
				TJS_GET_VM_REG(ra, code[1]).bitnot();
				code += 2;
				break;

			case VM_ASC:
				CharacterCodeOf(TJS_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_CHR:
				CharacterCodeFrom(TJS_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_NUM:
				TJS_GET_VM_REG(ra, code[1]).tonumber();
				code += 2;
				break;

			case VM_CHS:
				TJS_GET_VM_REG(ra, code[1]).changesign();
				code += 2;
				break;

			case VM_INV:
				TJS_GET_VM_REG(ra, code[1]) =
					(TJS_GET_VM_REG(ra, code[1]).AsObjectClosureNoAddRef().Invalidate(0,
					NULL, NULL, ra[-1].AsObjectNoAddRef()) == TJS_S_TRUE);
				code += 2;
				break;

			case VM_CHKINV:
				TJS_GET_VM_REG(ra, code[1]) =
					TJSIsObjectValid(TJS_GET_VM_REG(ra, code[1]).AsObjectClosureNoAddRef().IsValid(0,
					NULL, NULL, ra[-1].AsObjectNoAddRef()));
				code += 2;
				break;

			case VM_INT:
				TJS_GET_VM_REG(ra, code[1]).ToInteger();
				code += 2;
				break;

			case VM_REAL:
				TJS_GET_VM_REG(ra, code[1]).ToReal();
				code += 2;
				break;

			case VM_STR:
				TJS_GET_VM_REG(ra, code[1]).ToString();
				code += 2;
				break;

			case VM_OCTET:
				TJS_GET_VM_REG(ra, code[1]).ToOctet();
				code += 2;
				break;

			case VM_TYPEOF:
				TypeOf(TJS_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_TYPEOFD:
				TypeOfMemberDirect(ra, code, TJS_MEMBERMUSTEXIST);
				code += 4;
				break;

			case VM_TYPEOFI:
				TypeOfMemberIndirect(ra, code, TJS_MEMBERMUSTEXIST);
				code += 4;
				break;

			case VM_EVAL:
				Eval(TJS_GET_VM_REG(ra, code[1]),
					TJSEvalOperatorIsOnGlobal ? NULL : ra[-1].AsObjectNoAddRef(),
					true);
				code += 2;
				break;

			case VM_EEXP:
				Eval(TJS_GET_VM_REG(ra, code[1]),
					TJSEvalOperatorIsOnGlobal ? NULL : ra[-1].AsObjectNoAddRef(),
					false);
				code += 2;
				break;

			case VM_CHKINS:
				InstanceOf(TJS_GET_VM_REG(ra, code[2]),
					TJS_GET_VM_REG(ra, code[1]));
				code += 3;
				break;

			case VM_CALL:
			case VM_NEW:
				CallFunction(ra, code, args, numargs);
				code += (code[3]<0?0:code[3]) + 4;
				break;

			case VM_CALLD:
				CallFunctionDirect(ra, code, args, numargs);
				code += (code[4]<0?0:code[4]) + 5;
				break;

			case VM_CALLI:
				CallFunctionIndirect(ra, code, args, numargs);
				code += (code[4]<0?0:code[4]) + 5;
				break;

			case VM_GPD:
				GetPropertyDirect(ra, code, 0);
				code += 4;
				break;

			case VM_GPDS:
				GetPropertyDirect(ra, code, TJS_IGNOREPROP);
				code += 4;
				break;

			case VM_SPD:
				SetPropertyDirect(ra, code, 0);
				code += 4;
				break;

			case VM_SPDE:
				SetPropertyDirect(ra, code, TJS_MEMBERENSURE);
				code += 4;
				break;

			case VM_SPDEH:
				SetPropertyDirect(ra, code, TJS_MEMBERENSURE|TJS_HIDDENMEMBER);
				code += 4;
				break;

			case VM_SPDS:
				SetPropertyDirect(ra, code, TJS_MEMBERENSURE|TJS_IGNOREPROP);
				code += 4;
				break;

			case VM_GPI:
				GetPropertyIndirect(ra, code, 0);
				code += 4;
				break;

			case VM_GPIS:
				GetPropertyIndirect(ra, code, TJS_IGNOREPROP);
				code += 4;
				break;

			case VM_SPI:
				SetPropertyIndirect(ra, code, 0);
				code += 4;
				break;

			case VM_SPIE:
				SetPropertyIndirect(ra, code, TJS_MEMBERENSURE);
				code += 4;
				break;

			case VM_SPIS:
				SetPropertyIndirect(ra, code, TJS_MEMBERENSURE|TJS_IGNOREPROP);
				code += 4;
				break;

			case VM_DELD:
				DeleteMemberDirect(ra, code);
				code += 4;
				break;

			case VM_DELI:
				DeleteMemberIndirect(ra, code);
				code += 4;
				break;

			case VM_SRV:
				if(result) result->CopyRef(TJS_GET_VM_REG(ra, code[1]));
				code += 2;
				break;

			case VM_RET:
				return code+1-CodeArea;

			case VM_ENTRY:
				code = CodeArea + ExecuteCodeInTryBlock(ra, code-CodeArea + 3, args,
					numargs, result, TJS_FROM_VM_CODE_ADDR(code[1])+code-CodeArea,
					TJS_FROM_VM_REG_ADDR(code[2]));
				break;

			case VM_EXTRY:
				return code+1-CodeArea;  // same as ret

			case VM_THROW:
				ThrowScriptException(TJS_GET_VM_REG(ra, code[1]),
					Block, CodePosToSrcPos(code-CodeArea));
				code += 2; // actually here not proceed...
				break;

			case VM_CHGTHIS:
				TJS_GET_VM_REG(ra, code[1]).ChangeClosureObjThis(
					TJS_GET_VM_REG(ra, code[2]).AsObjectNoAddRef());
				code += 3;
				break;

			case VM_GLOBAL:
				TJS_GET_VM_REG(ra, code[1]) = Block->GetTJS()->GetGlobalNoAddRef();
				code += 2;
				break;

			case VM_ADDCI:
				AddClassInstanceInfo(ra, code);
				code+=3;
				break;

			case VM_DEBUGGER:
				TJSNativeDebuggerBreak();
				code ++;
				break;

			default:
				ThrowInvalidVMCode();
			}
		}
	}
	catch(eTJSSilent &e)
	{
		throw e;
	}
	catch(eTJSScriptException &e)
	{
		e.AddTrace(this, codesave-CodeArea);
		throw e;
	}
	catch(eTJSScriptError &e)
	{
		e.AddTrace(this, codesave-CodeArea);
		throw e;
	}
	catch(eTJS &e)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		TJS_eTJSScriptError(e.GetMessage(), this, codesave-CodeArea);
	}
	catch(exception &e)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		TJS_eTJSScriptError(e.what(), this, codesave-CodeArea);
	}
	catch(const wchar_t *text)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		TJS_eTJSScriptError(text, this, codesave-CodeArea);
	}
	catch(const char *text)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		TJS_eTJSScriptError(text, this, codesave-CodeArea);
	}
#ifdef TJS_SUPPORT_VCL
	catch(const EAccessViolation &e)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		TJS_eTJSScriptError(e.Message.c_str(), this, codesave-CodeArea);
	}
	catch(const Exception &e)
	{
		DisplayExceptionGeneratedCode(codesave - CodeArea, ra_org);
		TJS_eTJSScriptError(e.Message.c_str(), this, codesave-CodeArea);
	}
#endif

	return codesave-CodeArea;
}
//---------------------------------------------------------------------------
tjs_int tTJSInterCodeContext::ExecuteCodeInTryBlock(tTJSVariant *ra, tjs_int startip,
	tTJSVariant **args, tjs_int numargs, tTJSVariant *result, tjs_int catchip,
	tjs_int exobjreg)
{
	// execute codes in a try-protected block

	try
	{
		return ExecuteCode(ra, startip, args, numargs, result);
	}
	TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT(Block->GetTJS(), exobjreg, ra + exobjreg, {;}, { return catchip; })
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::ContinuousClear(
	tTJSVariant *ra, const tjs_int32 *code)
{
	tTJSVariant *r = TJS_GET_VM_REG_ADDR(ra, code[1]);
	tTJSVariant *rl = r + code[2]; // code[2] is count ( not reg offset )
	while(r<rl) (r++)->Clear();
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::GetPropertyDirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 flags)
{
	// ra[code[1]] = ra[code[2]][DataArea[ra[code[3]]]];

	tTJSVariant * ra_code2 = TJS_GET_VM_REG_ADDR(ra, code[2]);
	tTJSVariantType type = ra_code2->Type();
	if(type == tvtString)
	{
		GetStringProperty(TJS_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			TJS_GET_VM_REG(DataArea, code[3]));
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(TJS_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			TJS_GET_VM_REG(DataArea, code[3]));
		return;
	}


	tjs_error hr;
	tTJSVariantClosure clo = ra_code2->AsObjectClosureNoAddRef();
	tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[3]);
	hr = clo.PropGet(flags,
		name->GetString(), name->GetHint(), TJS_GET_VM_REG_ADDR(ra, code[1]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(TJS_FAILED(hr))
		TJSThrowFrom_tjs_error(hr, TJS_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::SetPropertyDirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 flags)
{
	// ra[code[1]][DataArea[ra[code[2]]]] = ra[code[3]]]

	tTJSVariant * ra_code1 = TJS_GET_VM_REG_ADDR(ra, code[1]);
	tTJSVariantType type = ra_code1->Type();
	if(type == tvtString)
	{
		SetStringProperty(TJS_GET_VM_REG_ADDR(ra, code[3]), ra_code1,
			TJS_GET_VM_REG(DataArea, code[2]));
		return;
	}
	if(type == tvtOctet)
	{
		SetOctetProperty(TJS_GET_VM_REG_ADDR(ra, code[3]), ra_code1,
			TJS_GET_VM_REG(DataArea, code[2]));
		return;
	}

	tjs_error hr;
	tTJSVariantClosure clo = ra_code1->AsObjectClosureNoAddRef();
	tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[2]);
	hr = clo.PropSetByVS(flags,
		name->AsStringNoAddRef(), TJS_GET_VM_REG_ADDR(ra, code[3]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(hr == TJS_E_NOTIMPL)
		hr = clo.PropSet(flags,
			name->GetString(), name->GetHint(), TJS_GET_VM_REG_ADDR(ra, code[3]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	if(TJS_FAILED(hr))
		TJSThrowFrom_tjs_error(hr, TJS_GET_VM_REG(DataArea, code[2]).GetString());
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::GetPropertyIndirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 flags)
{
	// ra[code[1]] = ra[code[2]][ra[code[3]]];

	tTJSVariant * ra_code2 = TJS_GET_VM_REG_ADDR(ra, code[2]);
	tTJSVariantType type = ra_code2->Type();
	if(type == tvtString)
	{
		GetStringProperty(TJS_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			TJS_GET_VM_REG(ra, code[3]));
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(TJS_GET_VM_REG_ADDR(ra, code[1]), ra_code2,
			TJS_GET_VM_REG(ra, code[3]));
		return;
	}

	tjs_error hr;
	tTJSVariantClosure clo = ra_code2->AsObjectClosureNoAddRef();
	tTJSVariant * ra_code3 = TJS_GET_VM_REG_ADDR(ra, code[3]);
	if(ra_code3->Type() != tvtInteger)
	{
		tTJSVariantString *str;
		str = ra_code3->AsString();

		try
		{
			// TODO: verify here needs hint holding
			hr = clo.PropGet(flags, *str, NULL, TJS_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			throw;
		}
		if(str) str->Release();
	}
	else
	{
		hr = clo.PropGetByNum(flags, (tjs_int)ra_code3->AsInteger(),
			TJS_GET_VM_REG_ADDR(ra, code[1]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
		if(TJS_FAILED(hr)) ThrowFrom_tjs_error_num(hr, (tjs_int)ra_code3->AsInteger());
	}
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::SetPropertyIndirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 flags)
{
	// ra[code[1]][ra[code[2]]] = ra[code[3]]]

	tTJSVariant *ra_code1 = TJS_GET_VM_REG_ADDR(ra, code[1]);
	tTJSVariantType type = ra_code1->Type();
	if(type == tvtString)
	{
		SetStringProperty(TJS_GET_VM_REG_ADDR(ra, code[3]),
			TJS_GET_VM_REG_ADDR(ra, code[1]), TJS_GET_VM_REG(ra, code[2]));
		return;
	}
	if(type == tvtOctet)
	{
		SetOctetProperty(TJS_GET_VM_REG_ADDR(ra, code[3]),
			TJS_GET_VM_REG_ADDR(ra, code[1]), TJS_GET_VM_REG(ra, code[2]));
		return;
	}

	tTJSVariantClosure clo = ra_code1->AsObjectClosure();
	tTJSVariant *ra_code2 = TJS_GET_VM_REG_ADDR(ra, code[2]);
	if(ra_code2->Type() != tvtInteger)
	{
		tTJSVariantString *str;
		try
		{
			str = ra_code2->AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		tjs_error hr;

		try
		{
			hr = clo.PropSetByVS(flags,
				str, TJS_GET_VM_REG_ADDR(ra, code[3]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(hr == TJS_E_NOTIMPL)
				hr = clo.PropSet(flags,
					*str, NULL, TJS_GET_VM_REG_ADDR(ra, code[3]),
						clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		tjs_error hr;

		try
		{
			hr = clo.PropSetByNum(flags,
				(tjs_int)ra_code2->AsInteger(),
					TJS_GET_VM_REG_ADDR(ra, code[3]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr))
				ThrowFrom_tjs_error_num(hr,
					(tjs_int)TJS_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::OperatePropertyDirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][DataArea[ra[code[3]]]], /*param=*/ra[code[4]]);

	tTJSVariantClosure clo =  TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tjs_error hr;
	try
	{
		tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.Operation(ope,
			name->GetString(), name->GetHint(),
			code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL,
			TJS_GET_VM_REG_ADDR(ra, code[4]),
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(TJS_FAILED(hr))
		TJSThrowFrom_tjs_error(hr, TJS_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::OperatePropertyIndirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][ra[code[3]]], /*param=*/ra[code[4]]);

	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tTJSVariant *ra_code3 = TJS_GET_VM_REG_ADDR(ra, code[3]);
	if(ra_code3->Type() != tvtInteger)
	{
		tTJSVariantString *str;
		try
		{
			str = ra_code3->AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		tjs_error hr;
		try
		{
			hr = clo.Operation(ope, *str, NULL,
				code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL,
				TJS_GET_VM_REG_ADDR(ra, code[4]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		tjs_error hr;
		try
		{
			hr = clo.OperationByNum(ope, (tjs_int)ra_code3->AsInteger(),
				code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL,
				TJS_GET_VM_REG_ADDR(ra, code[4]),
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr))
				ThrowFrom_tjs_error_num(hr,
					(tjs_int)TJS_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::OperatePropertyDirect0(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][DataArea[ra[code[3]]]]);

	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tjs_error hr;
	try
	{
		tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.Operation(ope,
			name->GetString(), name->GetHint(),
			code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
			ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(TJS_FAILED(hr))
		TJSThrowFrom_tjs_error(hr, TJS_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::OperatePropertyIndirect0(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 ope)
{
	// ra[code[1]] = ope(ra[code[2]][ra[code[3]]]);

	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tTJSVariant *ra_code3 = TJS_GET_VM_REG_ADDR(ra, code[3]);
	if(ra_code3->Type() != tvtInteger)
	{
		tTJSVariantString *str;
		try
		{
			str = ra_code3->AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		tjs_error hr;
		try
		{
			hr = clo.Operation(ope, *str, NULL,
				code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		tjs_error hr;
		try
		{
			hr = clo.OperationByNum(ope, (tjs_int)TJS_GET_VM_REG(ra, code[3]).AsInteger(),
				code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL, NULL,
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(TJS_FAILED(hr))
				ThrowFrom_tjs_error_num(hr,
					(tjs_int)TJS_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::DeleteMemberDirect(tTJSVariant *ra,
	const tjs_int32 *code)
{
	// ra[code[1]] = delete ra[code[2]][DataArea[ra[code[3]]]];

	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tjs_error hr;
	try
	{
		tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.DeleteMember(0,
			name->GetString(), name->GetHint(), ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(code[1])
	{
		if(TJS_FAILED(hr))
			TJS_GET_VM_REG(ra, code[1]) = false;
		else
			TJS_GET_VM_REG(ra, code[1]) = true;
	}
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::DeleteMemberIndirect(tTJSVariant *ra,
	const tjs_int32 *code)
{
	// ra[code[1]] = delete ra[code[2]][ra[code[3]]];

	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tTJSVariantString *str;
	try
	{
		str = TJS_GET_VM_REG(ra, code[3]).AsString();
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	tjs_error hr;

	try
	{
		hr = clo.DeleteMember(0, *str, NULL,
			clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
		if(code[1])
		{
			if(TJS_FAILED(hr))
				TJS_GET_VM_REG(ra, code[1]) = false;
			else
				TJS_GET_VM_REG(ra, code[1]) = true;
		}
	}
	catch(...)
	{
		if(str) str->Release();
		clo.Release();
		throw;
	}
	if(str) str->Release();
	clo.Release();
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::TypeOfMemberDirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 flags)
{
	// ra[code[1]] = typeof ra[code[2]][DataArea[ra[code[3]]]];
	tTJSVariantType type = TJS_GET_VM_REG(ra, code[2]).Type();
	if(type == tvtString)
	{
		GetStringProperty(TJS_GET_VM_REG_ADDR(ra, code[1]),
			TJS_GET_VM_REG_ADDR(ra, code[2]),
			TJS_GET_VM_REG(DataArea,code[3]));
		TypeOf(TJS_GET_VM_REG(ra, code[1]));
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(TJS_GET_VM_REG_ADDR(ra, code[1]),
			TJS_GET_VM_REG_ADDR(ra, code[2]),
			TJS_GET_VM_REG(DataArea, code[3]));
		TypeOf(TJS_GET_VM_REG(ra, code[1]));
		return;
	}

	tjs_error hr;
	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	try
	{
		tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[3]);
		hr = clo.PropGet(flags,
			name->GetString(), name->GetHint(), TJS_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(hr == TJS_S_OK)
	{
		TypeOf(TJS_GET_VM_REG(ra, code[1]));
	}
	else if(hr == TJS_E_MEMBERNOTFOUND)
	{
		TJS_GET_VM_REG(ra, code[1]) = TJS_W("undefined");
	}
	else if(TJS_FAILED(hr))
		TJSThrowFrom_tjs_error(hr, TJS_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::TypeOfMemberIndirect(tTJSVariant *ra,
	const tjs_int32 *code, tjs_uint32 flags)
{
	// ra[code[1]] = typeof ra[code[2]][ra[code[3]]];

	tTJSVariantType type = TJS_GET_VM_REG(ra, code[2]).Type();
	if(type == tvtString)
	{
		GetStringProperty(TJS_GET_VM_REG_ADDR(ra, code[1]),
			TJS_GET_VM_REG_ADDR(ra, code[2]),
			TJS_GET_VM_REG(ra, code[3]));
		TypeOf(ra[code[1]]);
		return;
	}
	if(type == tvtOctet)
	{
		GetOctetProperty(TJS_GET_VM_REG_ADDR(ra, code[1]),
			TJS_GET_VM_REG_ADDR(ra, code[2]),
			TJS_GET_VM_REG(ra, code[3]));
		TypeOf(TJS_GET_VM_REG(ra, code[1]));
		return;
	}

	tjs_error hr;
	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	if(TJS_GET_VM_REG(ra, code[3]).Type() != tvtInteger)
	{
		tTJSVariantString *str;
		try
		{
			str = TJS_GET_VM_REG(ra, code[3]).AsString();
		}
		catch(...)
		{
			clo.Release();
			throw;
		}

		try
		{
			// TODO: verify here needs hint holding
			hr = clo.PropGet(flags, *str, NULL, TJS_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(hr == TJS_S_OK)
			{
				TypeOf(TJS_GET_VM_REG(ra, code[1]));
			}
			else if(hr == TJS_E_MEMBERNOTFOUND)
			{
				TJS_GET_VM_REG(ra, code[1]) = TJS_W("undefined");
			}
			else if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, *str);
		}
		catch(...)
		{
			if(str) str->Release();
			clo.Release();
			throw;
		}
		if(str) str->Release();
		clo.Release();
	}
	else
	{
		try
		{
			hr = clo.PropGetByNum(flags,
				(tjs_int)TJS_GET_VM_REG(ra, code[3]).AsInteger(),
				TJS_GET_VM_REG_ADDR(ra, code[1]),
				clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());
			if(hr == TJS_S_OK)
			{
				TypeOf(TJS_GET_VM_REG(ra, code[1]));
			}
			else if(hr == TJS_E_MEMBERNOTFOUND)
			{
				TJS_GET_VM_REG(ra, code[1]) = TJS_W("undefined");
			}
			else if(TJS_FAILED(hr))
				ThrowFrom_tjs_error_num(hr,
					(tjs_int)TJS_GET_VM_REG(ra, code[3]).AsInteger());
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	}
}
//---------------------------------------------------------------------------
#define CALL_E(numargs, args) \
	tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();  \
	try \
	{ \
		if(code[0] == VM_CALL) \
		{ \
			hr = clo.FuncCall(0, NULL, NULL, \
				code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL, (numargs), (args), \
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef()); \
		} \
		else \
		{ \
			iTJSDispatch2 *dsp; \
			hr = clo.CreateNew(0, NULL, NULL, \
				&dsp, (numargs), (args), \
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef()); \
			if(TJS_SUCCEEDED(hr)) \
			{ \
				if(dsp) \
				{ \
					if(code[1]) TJS_GET_VM_REG(ra, code[1]) = tTJSVariant(dsp, dsp); \
					dsp->Release(); \
				} \
			} \
		}  \
	} \
	catch(...) \
	{  \
		clo.Release(); \
		throw; \
	} \
	clo.Release();
	// TODO: Null Check


void tTJSInterCodeContext::CallFunction(tTJSVariant *ra,
	const tjs_int32 *code, tTJSVariant **args, tjs_int numargs)
{
	// function calling
	tjs_error hr;
	if(code[3] < 0) // code[3] = argument count
	{
		CALL_E(numargs, args);
	}
	else if(code[3] <= 32)
	{
		tTJSVariant *ptr[32];

		for(tjs_int i = 0; i<code[3]; i++)
			ptr[i] = TJS_GET_VM_REG_ADDR(ra, code[4+i]);

		CALL_E(code[3], ptr);
	}
	else
	{
		tTJSVariant **ptr = new tTJSVariant *[code[3]];

		for(tjs_int i = 0; i<code[3]; i++)
			ptr[i] = TJS_GET_VM_REG_ADDR(ra, code[4+i]);

		try
		{
			CALL_E(code[3], ptr);
		}
		catch(...)
		{
			delete [] ptr;
			throw;
		}
		delete [] ptr;
	}

	if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, TJS_W(""));
}
//---------------------------------------------------------------------------
#undef CALL_E
#define CALL_E(name, hint, numargs, args) \
		hr = clo.FuncCall(0, \
			(name), (hint), \
			code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL, (numargs), (args), \
					clo.ObjThis?clo.ObjThis:ra[-1].AsObjectNoAddRef());

void tTJSInterCodeContext::CallFunctionDirect(tTJSVariant *ra,
	const tjs_int32 *code, tTJSVariant **args, tjs_int numargs)
{
	tTJSVariantType type = TJS_GET_VM_REG(ra, code[2]).Type();
	if(type == tvtString)
	{
		ProcessStringFunction(ra, code, code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL);
		return;
	}
	if(type == tvtOctet)
	{
		ProcessOctetFunction(ra, code, code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL);
		return;
	}

	tTJSVariantClosure clo =  TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tjs_error hr;
	try
	{
		tTJSVariant *name = TJS_GET_VM_REG_ADDR(DataArea, code[3]);
		if(code[4] < 0) // code[4] is argument count
		{
			CALL_E(name->GetString(), name->GetHint(), numargs, args);
		}
		else if(code[4] <= 32)
		{
			tTJSVariant *ptr[32];

			for(tjs_int i = 0; i<code[4]; i++)
				ptr[i] = TJS_GET_VM_REG_ADDR(ra, code[5+i]);

			CALL_E(name->GetString(), name->GetHint(), code[4], ptr);
		}
		else
		{
			tTJSVariant **ptr = new tTJSVariant *[code[4]];

			for(tjs_int i = 0; i<code[4]; i++)
				ptr[i] = TJS_GET_VM_REG_ADDR(ra, code[5+i]);

			try
			{
				CALL_E(name->GetString(), name->GetHint(), code[4], ptr);
			}
			catch(...)
			{
				delete [] ptr;
				throw;
			}
			delete [] ptr;
		}
	}
	catch(...)
	{
		clo.Release();
		throw;
	}
	clo.Release();
	if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, TJS_GET_VM_REG(DataArea, code[3]).GetString());
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::CallFunctionIndirect(tTJSVariant *ra,
	const tjs_int32 *code, tTJSVariant **args, tjs_int numargs)
{
	tTJSVariantType type = TJS_GET_VM_REG(ra, code[2]).Type();
	if(type == tvtString)
	{
		ProcessStringFunction(ra, code, code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL);
		return;
	}
	if(type == tvtOctet)
	{
		ProcessOctetFunction(ra, code, code[1]?TJS_GET_VM_REG_ADDR(ra, code[1]):NULL);
		return;
	}

	tTJSVariantClosure clo =  TJS_GET_VM_REG(ra, code[2]).AsObjectClosure();
	tTJSVariantString *str;
	try
	{
		str = TJS_GET_VM_REG(ra, code[3]).AsString();
	}
	catch(...)
	{
		clo.Release();
		throw;
	}

	tjs_error hr;
	try
	{
		if(code[4] < 0) // code[4] is argument count
		{
			CALL_E((const tjs_char*)*str, NULL, numargs, args);
		}
		else if(code[4] <= 32)
		{
			tTJSVariant *ptr[32];

			for(tjs_int i = 0; i<code[4]; i++)
				ptr[i] = TJS_GET_VM_REG_ADDR(ra, code[5+i]);

			CALL_E((const tjs_char*)*str, NULL, code[4], ptr);
		}
		else
		{
			tTJSVariant **ptr = new tTJSVariant *[code[4]];

			for(tjs_int i = 0; i<code[4]; i++)
				ptr[i] = TJS_GET_VM_REG_ADDR(ra, code[5+i]);

			try
			{
				CALL_E((const tjs_char*)*str, NULL, code[4], ptr);
			}
			catch(...)
			{
				clo.Release();
				delete [] ptr;
				if(str) str->Release();
				throw;
			}
			delete [] ptr;
		}

	}
	catch(...)
	{
		clo.Release();
		if(str) str->Release();
		throw;
	}
	clo.Release();
	if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr, (const tjs_char*)*str);
	str->Release();
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::AddClassInstanceInfo(tTJSVariant *ra,
	const tjs_int32 *code)
{
	iTJSDispatch2 *dsp;
	dsp = TJS_GET_VM_REG(ra, code[1]).AsObjectNoAddRef();
	if(dsp)
	{
		dsp->ClassInstanceInfo(TJS_CII_ADD, 0, TJS_GET_VM_REG_ADDR(ra, code[2]));
	}
	else
	{
		// ?? must be an error
	}
}
//---------------------------------------------------------------------------
#define TJS_STRFUNC_MAX 10
static tjs_char *StrFuncs[] = { TJS_W("charAt"), TJS_W("indexOf"), TJS_W("toUpperCase"),
	TJS_W("toLowerCase"), TJS_W("substring"), TJS_W("substr"), TJS_W("sprintf"),
		TJS_W("replace"), TJS_W("escape"), TJS_W("split") };
static tjs_int32 StrFuncHash[TJS_STRFUNC_MAX];
static bool TJSStrFuncInit = false;
static void InitTJSStrFunc()
{
	TJSStrFuncInit = true;
	for(tjs_int i=0; i<TJS_STRFUNC_MAX; i++)
	{
		tjs_char *p = StrFuncs[i];
		tjs_int32 hash = 0;
		while(*p) hash += *p, p++;
		StrFuncHash[i] = hash;
	}
}

void tTJSInterCodeContext::ProcessStringFunction(tTJSVariant *ra,
	const tjs_int32 *code, tTJSVariant *result)
{
	if(!TJSStrFuncInit) InitTJSStrFunc();

	tjs_int32 hash;

	tTJSVariantString *mem;
	if(code[0] == VM_CALLI)
		mem = TJS_GET_VM_REG(ra, code[3]).AsString();
	else
		mem = TJS_GET_VM_REG(DataArea, code[3]).AsString();
	if(!mem) TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, TJS_W(""));
	const tjs_char *member = (const tjs_char*)*mem;
	const tjs_char *m = member;
	hash = 0;
	while(*m) hash += *m, m++;

	tjs_int argnum = code[4];
	const tjs_char *s = TJS_GET_VM_REG(ra, code[2]).GetString(); // target string
	const tjs_int s_len = TJS_GET_VM_REG(ra, code[2]).AsStringNoAddRef()->GetLength();


	// code[2] is target string, copde[5]+ are arguments

	if(hash == StrFuncHash[0] && !TJS_strcmp(StrFuncs[0], member)) // charAt
	{
		mem->Release();
		if(argnum != 1) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);
		if(!s)
		{
			if(result) *result = TJS_W("");
			return;
		}
		tjs_int i = (tjs_int)TJS_GET_VM_REG(ra, code[5]).AsInteger();
		if(i<0 || i>=s_len)
		{
			if(result) *result = TJS_W("");
			return;
		}
		tjs_char bt[2];
		bt[1] = 0;
		bt[0] = s[i];
		if(result) *result = bt;
		return;
	}
	else if(hash == StrFuncHash[1] && !TJS_strcmp(StrFuncs[1], member)) // indexOf
	{
		mem->Release();
		if(argnum != 1 && argnum != 2) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);
		tTJSVariantString *pstr = TJS_GET_VM_REG(ra, code[5]).AsString(); // sub string

		if(!s || !pstr)
		{
			if(result) *result = (tjs_int)-1;
			if(pstr) pstr->Release();
			return;
		}
		tjs_int start;
		if(argnum == 1)
		{
			start = 0;
		}
		else
		{
			try // integer convertion may raise an exception
			{
				start = (tjs_int)TJS_GET_VM_REG(ra, code[6]).AsInteger();
			}
			catch(...)
			{
				pstr->Release();
				throw;
			}
		}
		if(start >= s_len)
		{
			if(result) *result = (tjs_int)-1;
			if(pstr) pstr->Release();
			return;
		}
		tjs_char *p;
		p = wcsstr(s + start, (const tjs_char*)*pstr);
		if(!p)
		{
			if(result) *result = (tjs_int)-1;
		}
		else
		{
			if(result) *result = (tjs_int)(p-s);
		}
		if(pstr) pstr->Release();
		return;
	}
	else if(hash == StrFuncHash[2] && !TJS_strcmp(StrFuncs[2], member)) // toUpperCase
	{
		mem->Release();
		if(argnum != 0) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);
		if(result)
		{
			*result = s; // here s is copyed to *result ( not reference )
			const tjs_char *pstr = result->GetString();  // buffer in *result
			if(pstr)
			{
				tjs_char *p = (tjs_char*)pstr;    // WARNING!! modification of const
				while(*p)
				{
					if(*p>=TJS_W('a') && *p<=TJS_W('z')) *p += TJS_W('Z')-TJS_W('z');
					p++;
				}
			}
		}
		return;
	}
	else if(hash == StrFuncHash[3] && !TJS_strcmp(StrFuncs[3], member)) // toLowerCase
	{
		mem->Release();
		if(argnum != 0) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);
		if(result)
		{
			*result = s;
			const tjs_char *pstr = result->GetString();
			if(pstr)
			{
				tjs_char *p = (tjs_char*)pstr;    // WARNING!! modification of const
				while(*p)
				{
					if(*p>=TJS_W('A') && *p<=TJS_W('Z')) *p += TJS_W('z')-TJS_W('Z');
					p++;
				}
			}
		}
		return;
	}
	else if((hash == StrFuncHash[4]  && !TJS_strcmp(StrFuncs[4], member)) ||
		(hash == StrFuncHash[5] && !TJS_strcmp(StrFuncs[5], member))) // substring , substr
	{
		mem->Release();
		if(argnum != 1 && argnum != 2) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);
		tjs_int start = (tjs_int)TJS_GET_VM_REG(ra, code[5]).AsInteger();
		if(start < 0 || start >= s_len)
		{
			if(result) *result=TJS_W("");
			return;
		}
		tjs_int count;
		if(argnum == 2)
		{
			count = (tjs_int)TJS_GET_VM_REG(ra, code[6]).AsInteger();
			if(count<0)
			{
				if(result) *result = TJS_W("");
				return;
			}
			if(start + count > s_len) count = s_len - start;
			if(result)
			{
				tjs_char *buf = new tjs_char[count+1];
				TJS_strcpy_maxlen(buf, s+start, count);
				*result = buf;
				delete [] buf;
			}
			return;
		}
		else
		{
			if(result) *result = s + start;
		}
		return;
	}
	else if(hash == StrFuncHash[6] && !TJS_strcmp(StrFuncs[6], member))
	{
		mem->Release();

		if(result)
		{
			tTJSVariant **args = new tTJSVariant *[argnum];
			for(tjs_int i = 0; i < argnum; i++)
				args[i] = TJS_GET_VM_REG_ADDR(ra, code[5 + i]);
			tTJSVariantString *res;
			try
			{
				res = TJSFormatString(s, argnum, args);
			}
			catch(...)
			{
				delete [] args;
				throw;
			}
			delete [] args;
			*result = res;
			if(res) res->Release();
		}
		return;
	}
	else if(hash == StrFuncHash[7] && !TJS_strcmp(StrFuncs[7], member))  // replace
	{
		mem->Release();

		// string.replace(pattern, replacement-string)  -->
		// pattern.replace(string, replacement-string)
		if(argnum < 2) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);

		tTJSVariantClosure clo = TJS_GET_VM_REG(ra, code[5]).AsObjectClosureNoAddRef();
		tTJSVariant *params[3] = { TJS_GET_VM_REG_ADDR(ra, code[2]),
								TJS_GET_VM_REG_ADDR(ra, code[6]) };
		static tTJSString replace_name(TJS_W("replace"));
		clo.FuncCall(0, replace_name.c_str(), replace_name.GetHint(),
			result, 2, params, NULL);

		return;
	}
	else if(hash == StrFuncHash[8] && !TJS_strcmp(StrFuncs[8], member))  // escape
	{
		mem->Release();

		if(result)
		{
			*result = ttstr(s).EscapeC();
		}

		return;
	}
	else if(hash == StrFuncHash[9] && !TJS_strcmp(StrFuncs[9], member))  // split
	{
		mem->Release();

		// string.split(pattern, reserved, purgeempty) -->
		// Array.split(pattern, string, reserved, purgeempty)
		if(argnum < 1) TJSThrowFrom_tjs_error(TJS_E_BADPARAMCOUNT);

		iTJSDispatch2 * array = TJSCreateArrayObject();
		try
		{
			tTJSVariant *params[4] = {
				TJS_GET_VM_REG_ADDR(ra, code[5]),
				TJS_GET_VM_REG_ADDR(ra, code[2]),
				argnum >= 2 ? TJS_GET_VM_REG_ADDR(ra, code[6]): TJS_GET_VM_REG_ADDR(ra, 0),
				argnum >= 3 ? TJS_GET_VM_REG_ADDR(ra, code[7]): TJS_GET_VM_REG_ADDR(ra, 0)};
			static tTJSString split_name(TJS_W("split"));
			array->FuncCall(0, split_name.c_str(), split_name.GetHint(),
				NULL, 4, params, array);

			if(result) *result = tTJSVariant(array, array);
		}
		catch(...)
		{
			array->Release();
			throw;
		}
		array->Release();

		return;
	}

	try
	{
		TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, *mem);
	}
	catch(...)
	{
		mem->Release();
		throw;
	}
	mem->Release(); // this will be not executed.
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::ProcessOctetFunction(tTJSVariant *ra,
	const tjs_int32 *code, tTJSVariant *result)
{
	tTJSVariantString *mem;
	if(code[0] == VM_CALLI)
		mem = TJS_GET_VM_REG(ra, code[3]).AsString();
	else
		mem = TJS_GET_VM_REG(DataArea, code[3]).AsString();
	if(!mem) TJSThrowFrom_tjs_error(TJS_E_MEMBERNOTFOUND, TJS_W(""));
	const tjs_char *member = (const tjs_char*)*mem;

	tjs_int argnum = code[4];
	const tjs_char *s = TJS_GET_VM_REG(ra, code[2]).GetString(); // target string


	if(TJS_strcmp(TJS_W("unpack"), member))
	{
		mem->Release();

		// unpack ( from perl function )


		// TODO: unpack/pack implementation
	}


	mem->Release();
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::TypeOf(tTJSVariant &val)
{
	// processes TJS2's typeof operator.
	switch(val.Type())
	{
	case tvtVoid:
		val = TJS_W("void");   // differs from TJS1
		break;

	case tvtObject:
		val = TJS_W("Object");
		break;

	case tvtString:
		val = TJS_W("String");
		break;

	case tvtInteger:
		val = TJS_W("Integer"); // differs from TJS1
		break;

	case tvtReal:
		val = TJS_W("Real"); // differs from TJS1
		break;

	case tvtOctet:
		val = TJS_W("Octet");
		break;
	}
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::Eval(tTJSVariant &val,
	iTJSDispatch2 * objthis, bool resneed)
{
	if(objthis) objthis->AddRef();
	try
	{
		tTJSVariant res;
		ttstr str(val);
		if(!str.IsEmpty())
		{
			if(resneed)
				Block->GetTJS()->EvalExpression(str, &res, objthis);
			else
				Block->GetTJS()->EvalExpression(str, NULL, objthis);
		}
		if(resneed) val = res;
	}
	catch(...)
	{
		if(objthis) objthis->Release();
		throw;
	}
	if(objthis) objthis->Release();
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::CharacterCodeOf(tTJSVariant &val)
{
	// puts val's character code on val
	tTJSVariantString * str = val.AsString();
	if(str)
	{
		const tjs_char *ch = (const tjs_char*)*str;
		val = tTVInteger(ch[0]);
		str->Release();
		return;
	}
	val = tTVInteger(0);
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::CharacterCodeFrom(tTJSVariant &val)
{
	tjs_char ch[2];
	ch[0] = val.AsInteger();
	ch[1] = 0;
	val = ch;
}
//---------------------------------------------------------------------------
void tTJSInterCodeContext::InstanceOf(const tTJSVariant &name, tTJSVariant &targ)
{
	// checks instance inheritance.
	tTJSVariantString *str = name.AsString();
	if(str)
	{
		tjs_error hr;
		try
		{
			hr = TJSDefaultIsInstanceOf(0, targ, (const tjs_char*)*str, NULL);
		}
		catch(...)
		{
			str->Release();
			throw;
		}
		str->Release();
		if(TJS_FAILED(hr)) TJSThrowFrom_tjs_error(hr);

		targ = (hr == TJS_S_TRUE);
		return;
	}
	targ = false;
}
//---------------------------------------------------------------------------
#define TJS_DO_SUPERCLASS_PROXY_BEGIN \
		std::vector<tjs_int> &pointer = SuperClassGetter->SuperClassGetterPointer; \
		if(pointer.size() != 0) \
		{ \
			std::vector<tjs_int>::iterator i; \
			for(i = pointer.end()-1; \
				i >=pointer.begin(); i--) \
			{ \
				tTJSVariant res; \
				SuperClassGetter->ExecuteAsFunction(NULL, NULL, 0, &res, *i); \
				tTJSVariantClosure clo = res.AsObjectClosureNoAddRef();


#define TJS_DO_SUPERCLASS_PROXY_END \
				if(hr != TJS_E_MEMBERNOTFOUND) break; \
			} \
		}

tjs_error TJS_INTF_METHOD  tTJSInterCodeContext::FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
			tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if(!GetValidity())
		return TJS_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		switch(ContextType)
		{
		case ctTopLevel:
			ExecuteAsFunction(
				objthis?objthis:Block->GetTJS()->GetGlobalNoAddRef(),
				NULL, 0, result, 0);
			break;

		case ctFunction:
		case ctExprFunction:
		case ctClass: // on super class' initialization
		case ctPropertyGetter:
		case ctPropertySetter:
			ExecuteAsFunction(objthis, param, numparams, result, 0);
			break;

		case ctProperty:
			return TJS_E_INVALIDTYPE;
		}

		return TJS_S_OK;
	}

	tjs_error hr = inherited::FuncCall(flag, membername, hint, result,
		numparams, param, objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.FuncCall(flag, membername, hint, result,
				numparams, param, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD  tTJSInterCodeContext::PropGet(tjs_uint32 flag,
	const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,
		iTJSDispatch2 *objthis)
{
	if(!GetValidity())
		return TJS_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(ContextType == ctProperty)
		{
			// executed as a property getter
			if(PropGetter)
				return PropGetter->FuncCall(0, NULL, NULL, result, 0, NULL,
					objthis);
			else
				return TJS_E_ACCESSDENYED;
		}
	}

	tjs_error hr = inherited::PropGet(flag, membername, hint, result, objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.PropGet(flag, membername, hint, result, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;

}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD  tTJSInterCodeContext::PropSet(tjs_uint32 flag,
	const tjs_char *membername, tjs_uint32 *hint,
		const tTJSVariant *param, iTJSDispatch2 *objthis)
{
	if(!GetValidity())
		return TJS_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(ContextType == ctProperty)
		{
			// executed as a property setter
			if(PropSetter)
				return PropSetter->FuncCall(0, NULL, NULL, NULL, 1,
					const_cast<tTJSVariant **>(&param), objthis);
			else
				return TJS_E_ACCESSDENYED;

				// WARNING!! const tTJSVariant ** -> tTJSVariant** force casting
		}
	}

	tjs_error hr;
	if(membername != NULL && ContextType == ctClass && SuperClassGetter)
	{
		tjs_uint32 pseudo_flag =
			(flag & TJS_IGNOREPROP) ? flag : (flag &~ TJS_MEMBERENSURE);
			// member ensuring is temporarily disabled unless TJS_IGNOREPROP

		hr = inherited::PropSet(pseudo_flag, membername, hint, param,
			objthis);
		if(hr == TJS_E_MEMBERNOTFOUND)
		{
			TJS_DO_SUPERCLASS_PROXY_BEGIN
				hr = clo.PropSet(pseudo_flag, membername, hint, param,
					objthis);
			TJS_DO_SUPERCLASS_PROXY_END
		}
		
		if(hr == TJS_E_MEMBERNOTFOUND && (flag & TJS_MEMBERENSURE))
		{
			// re-ensure the member for "this" object
			hr = inherited::PropSet(flag, membername, hint, param,
				objthis);
		}
	}
	else
	{
		hr = inherited::PropSet(flag, membername, hint, param,
			objthis);
	}

	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD  tTJSInterCodeContext::CreateNew(tjs_uint32 flag,
	const tjs_char * membername, tjs_uint32 *hint,
	iTJSDispatch2 **result, tjs_int numparams,
	tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if(!GetValidity())
		return TJS_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		if(ContextType != ctClass) return TJS_E_INVALIDTYPE;

		iTJSDispatch2 *dsp = new inherited();

		try
		{
			ExecuteAsFunction(dsp, NULL, 0, NULL, 0);
			FuncCall(0, Name, NULL, NULL, numparams, param, dsp);
		}
		catch(...)
		{
			dsp->Release();
			throw;
		}

		*result = dsp;
		return TJS_S_OK;
	}

	tjs_error hr = inherited::CreateNew(flag, membername, hint,
		result, numparams, param,
		objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.CreateNew(flag, membername, hint, result, numparams, param,
				objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD  tTJSInterCodeContext::IsInstanceOf(tjs_uint32 flag,
	const tjs_char *membername, tjs_uint32 *hint, const tjs_char *classname,
		iTJSDispatch2 *objthis)
{
	if(!GetValidity())
		return TJS_E_INVALIDOBJECT;

	if(membername == NULL)
	{
		switch(ContextType)
		{
		case ctTopLevel:
		case ctPropertySetter:
		case ctPropertyGetter:
		case ctSuperClassGetter:
			break;

		case ctFunction:
		case ctExprFunction:
			if(!TJS_strcmp(classname, TJS_W("Function"))) return TJS_S_TRUE;
			break;

		case ctProperty:
			if(!TJS_strcmp(classname, TJS_W("Property"))) return TJS_S_TRUE;
			break;
			
		case ctClass:
			if(!TJS_strcmp(classname, TJS_W("Class"))) return TJS_S_TRUE;
			break;
		}
	}

	tjs_error hr = inherited::IsInstanceOf(flag, membername, hint,
		classname, objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.IsInstanceOf(flag, membername, hint, classname, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSInterCodeContext::GetCount(tjs_int *result, const tjs_char *membername,
		tjs_uint32 *hint, iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::GetCount(result, membername, hint,
		objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.GetCount(result, membername, hint, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;

}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSInterCodeContext::DeleteMember(tjs_uint32 flag, const tjs_char *membername,
		tjs_uint32 *hint,  iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::DeleteMember(flag, membername, hint,
		objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.DeleteMember(flag, membername, hint, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSInterCodeContext::Invalidate(tjs_uint32 flag, const tjs_char *membername,
		tjs_uint32 *hint, iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::Invalidate(flag, membername, hint,
		objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.Invalidate(flag, membername, hint, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSInterCodeContext::IsValid(tjs_uint32 flag, const tjs_char *membername,
		tjs_uint32 *hint, iTJSDispatch2 *objthis)
{
	tjs_error hr = inherited::IsValid(flag, membername, hint,
		objthis);

	if(membername != NULL && hr == TJS_E_MEMBERNOTFOUND &&
		ContextType == ctClass && SuperClassGetter)
	{
		// look up super class
		TJS_DO_SUPERCLASS_PROXY_BEGIN
			hr = clo.IsValid(flag, membername, hint, objthis);
		TJS_DO_SUPERCLASS_PROXY_END
	}
	return hr;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSInterCodeContext::Operation(tjs_uint32 flag, const tjs_char *membername,
		tjs_uint32 *hint, tTJSVariant *result,
			const tTJSVariant *param,	iTJSDispatch2 *objthis)
{
	if(membername == NULL)
	{
		return inherited::Operation(flag, membername, hint, result, param,
			objthis);
	}

	tjs_error hr;

	if(membername != NULL && ContextType == ctClass && SuperClassGetter)
	{
		tjs_uint32 pseudo_flag =
			(flag & TJS_IGNOREPROP) ? flag : (flag &~ TJS_MEMBERENSURE);

		tjs_error hr = inherited::Operation(pseudo_flag, membername, hint,
			result, param, objthis);

		if(hr == TJS_E_MEMBERNOTFOUND)
		{
			// look up super class
			TJS_DO_SUPERCLASS_PROXY_BEGIN
				hr = clo.Operation(pseudo_flag, membername, hint, result, param,
					objthis);
			TJS_DO_SUPERCLASS_PROXY_END
		}

		if(hr == TJS_E_MEMBERNOTFOUND)
			hr = inherited::Operation(flag, membername, hint, result,
				param, objthis);

		return hr;
	}
	else
	{
		return inherited::Operation(flag, membername, hint, result, param,
			objthis);
	}
}
//---------------------------------------------------------------------------

}

