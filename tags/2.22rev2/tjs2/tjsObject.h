//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS2 "Object" class implementation
//---------------------------------------------------------------------------
#ifndef tjsObjectH
#define tjsObjectH

#include <vector>
#include "tjsInterface.h"
#include "tjsVariant.h"
#include "tjsUtils.h"
#include "tjsError.h"

namespace TJS
{
//---------------------------------------------------------------------------
// utility functions
//---------------------------------------------------------------------------
extern tjs_error
	TJSDefaultFuncCall(tjs_uint32 flag, tTJSVariant &targ, tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

extern tjs_error TJSDefaultPropGet(tjs_uint32 flag, tTJSVariant &targ, tTJSVariant *result,
	iTJSDispatch2 *objthis);

extern tjs_error TJSDefaultPropSet(tjs_uint32 flag, tTJSVariant &targ, const tTJSVariant *param,
	iTJSDispatch2 *objthis);

extern tjs_error
	TJSDefaultInvalidate(tjs_uint32 flag, tTJSVariant &targ, iTJSDispatch2 *objthis);

extern tjs_error
	TJSDefaultIsValid(tjs_uint32 flag, tTJSVariant &targ, iTJSDispatch2 * objthis);

extern tjs_error
	TJSDefaultCreateNew(tjs_uint32 flag, tTJSVariant &targ,
		iTJSDispatch2 **result, tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *objthis);

extern tjs_error
	TJSDefaultIsInstanceOf(tjs_uint32 flag, tTJSVariant &targ, const tjs_char *name,
		iTJSDispatch2 *objthis);

extern tjs_error
	TJSDefaultOperation(tjs_uint32 flag, tTJSVariant &targ,
		tTJSVariant *result, const tTJSVariant *param, iTJSDispatch2 *objthis);

TJS_EXP_FUNC_DEF(void, TJSDoVariantOperation, (tjs_int op, tTJSVariant &target, const tTJSVariant *param));
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// hash rebuilding
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(void, TJSDoRehash, ());
//---------------------------------------------------------------------------



/*[*/
//---------------------------------------------------------------------------
// tTJSDispatch
//---------------------------------------------------------------------------
/*
	tTJSDispatch is a base class that implements iTJSDispatch2, and every methods.
	most methods are maked as simply returning "TJS_E_NOTIMPL";
	methods, those access the object by index, call another methods that access
	the object by string.
*/
/*
#define TJS_SELECT_OBJTHIS(__closure__, __override__) \
	((__closure__).ObjThis?((__override__)?(__override__):(__closure__).ObjThis):(__override__))
*/
#define TJS_SELECT_OBJTHIS(__closure__, __override__) \
	((__closure__).ObjThis?(__closure__).ObjThis:(__override__))

class tTJSDispatch : public iTJSDispatch2
{
	virtual void BeforeDestruction(void) {;}
	bool BeforeDestructionCalled;
		// BeforeDestruction will be certainly called before object destruction
private:
	tjs_uint RefCount;
public:
	tTJSDispatch();
	virtual ~tTJSDispatch();

//	bool DestructionTrace;

public:
	tjs_uint TJS_INTF_METHOD  AddRef(void);
	tjs_uint TJS_INTF_METHOD  Release(void);

protected:
	tjs_uint GetRefCount() { return RefCount; }

public:
	tjs_error TJS_INTF_METHOD
	FuncCall(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	FuncCallByNum(
		tjs_uint32 flag,
		tjs_int num,
		tTJSVariant *result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	PropGet(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	PropGetByNum(
		tjs_uint32 flag,
		tjs_int num,
		tTJSVariant *result,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	PropSet(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	PropSetByNum(
		tjs_uint32 flag,
		tjs_int num,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis
		);
	
	tjs_error TJS_INTF_METHOD
	GetCount(
		tjs_int *result,
		const tjs_char *membername,
		tjs_uint32 *hint,
		iTJSDispatch2 *objthis
		)
	{
		return TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	GetCountByNum(
		tjs_int *result,
		tjs_int num,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	Reserved1()
	{
		return TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	Reserved2()
	{
		return TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	DeleteMember(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	DeleteMemberByNum(
		tjs_uint32 flag,
		tjs_int num,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	Invalidate(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	InvalidateByNum(
		tjs_uint32 flag,
		tjs_int num,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	IsValid(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	IsValidByNum(
		tjs_uint32 flag,
		tjs_int num,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	CreateNew(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	CreateNewByNum(
		tjs_uint32 flag,
		tjs_int num,
		iTJSDispatch2 **result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	GetSuperClass(
		tjs_uint32 flag,
		iTJSDispatch2 **result,
		iTJSDispatch2 *objthis
		)
	{
		return TJS_E_NOTIMPL;
	}


	tjs_error TJS_INTF_METHOD
	IsInstanceOf(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tjs_char *classname,
		iTJSDispatch2 *objthis
		)
	{
		return membername?TJS_E_MEMBERNOTFOUND:TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	IsInstanceOfByNum(
		tjs_uint32 flag,
		tjs_int num,
		const tjs_char *classname,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	Operation(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis
		);

	tjs_error TJS_INTF_METHOD
	OperationByNum(
		tjs_uint32 flag,
		tjs_int num,
		tTJSVariant *result,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis
		);


	tjs_error TJS_INTF_METHOD
	NativeInstanceSupport(
		tjs_uint32 flag,
		tjs_int32 classid,
		iTJSNativeInstance **pointer
		)
	{
		return TJS_E_NOTIMPL;
	}

	tjs_error TJS_INTF_METHOD
	ClassInstanceInfo( 
		tjs_uint32 flag,
		tjs_uint num,
		tTJSVariant *value
		)
	{
		return TJS_E_NOTIMPL;
	}

};
//---------------------------------------------------------------------------
/*]*/




//---------------------------------------------------------------------------
// tTJSCustomObject
//---------------------------------------------------------------------------

#define TJS_NAMESPACE_SHORT_NAME 18

#define TJS_NAMESPACE_DEFAULT_HASH_BITS 6

extern tjs_int TJSObjectHashBitsLimit;
	// this limits hash table size



#define TJS_SYMBOL_USING	0x1
#define TJS_SYMBOL_INIT     0x2
#define TJS_SYMBOL_LONG     0x4
#define TJS_SYMBOL_HIDDEN   0x8

#define TJS_MAX_NATIVE_CLASS 4
/*
	Number of "Native Class Instance" that can be used per a TJS Object is
	limited as the number above.
*/


class tTJSEnumMemberCallbackIntf
{
public:
	virtual bool EnumMemberCallback(const tjs_char *name, tjs_uint32 hint,
		const tTJSVariant & value) = 0;
	// called per each members.
	// if the function returns false, enumeration is to be interrupted.
	// to continue the enumeration, the function must return true.
};


class tTJSCustomObject : public tTJSDispatch
{
	typedef tTJSDispatch inherited;

	// tTJSSymbolData -----------------------------------------------------
public:
	struct tTJSSymbolData
	{
		tjs_uint32 Hash; // hash code of the name
		tjs_uint32 SymFlags; // management flags

		tTJSVariant_S Value; // the value
			/*
				TTJSVariant_S must work with construction that fills
					all member to zero.
			*/
		tjs_uint32 Flags;  // flags

		union
		{
			tjs_char ShortName[TJS_NAMESPACE_SHORT_NAME];
			tjs_char *LongName;
		};

		tTJSSymbolData * Next; // next chain


		void SelfClear(void)
		{
			memset(this, 0, sizeof(*this));
			SymFlags = TJS_SYMBOL_INIT;
		}

		void _SetName(const tjs_char * name)
		{
			if(SymFlags & TJS_SYMBOL_LONG)
				delete [] LongName, SymFlags &= ~TJS_SYMBOL_LONG;
			if(!name) TJS_eTJSError(TJSIDExpected);       ///
			size_t len = TJS_strlen(name);
			if(len==0) TJS_eTJSError(TJSIDExpected);      ///
			if(len >= TJS_NAMESPACE_SHORT_NAME -1)
			{
				LongName = new tjs_char[len+1];
				TJS_strcpy(LongName, name);
				SymFlags |= TJS_SYMBOL_LONG;
			}
			else
			{
				wcscpy(ShortName, name);
			}
		}

		void SetName(const tjs_char * name, tjs_uint32 hash)
		{
			_SetName(name);
			Hash = hash;
		}

		const tjs_char * GetName() const
		{
			if(SymFlags & TJS_SYMBOL_LONG)
				return LongName;
			else
				return ShortName;
		}

		void PostClear()
		{
			if(SymFlags & TJS_SYMBOL_LONG)
				delete [] LongName, SymFlags &= ~TJS_SYMBOL_LONG;
			((tTJSVariant*)(&Value))->~tTJSVariant();
			memset(&Value, 0, sizeof(Value));
			SymFlags &= ~TJS_SYMBOL_USING;
		}

		void Destory()
		{
			if(SymFlags & TJS_SYMBOL_LONG)
				delete [] LongName, SymFlags &= ~TJS_SYMBOL_LONG;
			((tTJSVariant*)(&Value))->~tTJSVariant();
		}

		static std::vector<tTJSSymbolData*> * SymbolHeap;
		static tTJSSymbolData ** SymbolFreeBlocks;
		static tjs_uint SymbolAllocatableBlockCount;
		static tjs_uint SymbolFreeCount;
		static tjs_uint SymbolFreeReadPoint;
		static tjs_uint SymbolFreeWritePoint;

		static void AddSymbolHeap();
		static void UninitSymbolHeaps();

		static tTJSSymbolData * AllocateSymbol();
		static void DeallocateSymbol(tTJSSymbolData *);
	};



	//---------------------------------------------------------------------
	tjs_int Count;
	tjs_int HashMask;
	tjs_int HashSize;
	tTJSSymbolData * Symbols;
	tjs_uint RebuildHashMagic;
	bool IsInvalidated;
	bool IsInvalidating;
	iTJSNativeInstance* ClassInstances[TJS_MAX_NATIVE_CLASS];
	tjs_int32 ClassIDs[TJS_MAX_NATIVE_CLASS];


	void _Finalize(void);

	//---------------------------------------------------------------------
protected:
	bool GetValidity() const { return !IsInvalidated; }
	bool CallFinalize; // set false if this object does not need to call "finalize"
	virtual void Finalize(void);
	std::vector<ttstr > ClassNames;

	//---------------------------------------------------------------------
public:

	tTJSCustomObject(tjs_int hashbits = TJS_NAMESPACE_DEFAULT_HASH_BITS);
	~tTJSCustomObject();

private:
	void BeforeDestruction(void);

	void CheckObjectClosureAdd(const tTJSVariant &val)
	{
		// adjust the reference counter when the object closure's "objthis" is
		// referring to the object itself.
		if(val.Type() == tvtObject)
		{
			iTJSDispatch2 *dsp = val.AsObjectClosureNoAddRef().ObjThis;
			if(dsp == (iTJSDispatch2*)this) this->Release();
		}
	}

	void CheckObjectClosureRemove(const tTJSVariant &val)
	{
		if(val.Type() == tvtObject)
		{
			iTJSDispatch2 *dsp = val.AsObjectClosureNoAddRef().ObjThis;
			if(dsp == (iTJSDispatch2*)this) this->AddRef();
		}
	}

	tTJSSymbolData * Add(const tjs_char * name, tjs_uint32 *hint);
		// Adds the symbol, returns the newly created data;
		// if already exists, returns the data.

	tTJSSymbolData * AddTo(const tjs_char * name,
		tTJSSymbolData *newdata, tjs_int newhashmask);
		// Adds member to the new hash space, used in RebuildHash

	void RebuildHash(); // rebuild hash table

	bool DeleteByName(const tjs_char * name, tjs_uint32 *hint);
		// Deletes Name

	void DeleteAllMembers(void);
		// Deletes all members
	void _DeleteAllMembers(void);
		// Deletes all members ( not to use std::vector )

	tTJSSymbolData * Find(const tjs_char * name, tjs_uint32 *hint) ;
		// Finds Name, returns its data; if not found, returns NULL

	//---------------------------------------------------------------------
public:
	void EnumMembers(tTJSEnumMemberCallbackIntf * intf);
	void Clear() { DeleteAllMembers(); }

	//---------------------------------------------------------------------
public:
	tjs_int GetValueInteger(const tjs_char * name, tjs_uint32 *hint);
		// service function for lexical analyzer
	//---------------------------------------------------------------------
public:

	tjs_error TJS_INTF_METHOD
	FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	 tTJSVariant *result,
		iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	 const tTJSVariant *param,
		iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	GetCount(tjs_int *result, const tjs_char *membername, tjs_uint32 *hint,
	 iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	DeleteMember(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	 iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	Invalidate(tjs_uint32 flag, const tjs_char *membername,  tjs_uint32 *hint,
	iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	IsValid(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	 iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	 iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param,	iTJSDispatch2 *objthis);
/*
	tjs_error TJS_INTF_METHOD
	GetSuperClass(tjs_uint32 flag, iTJSDispatch2 **result, iTJSDispatch2 *objthis);
*/
	tjs_error TJS_INTF_METHOD
	IsInstanceOf(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	 const tjs_char *classname,
		iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	Operation(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
	 tTJSVariant *result,
		const tTJSVariant *param,	iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	NativeInstanceSupport(tjs_uint32 flag, tjs_int32 classid,
		iTJSNativeInstance **pointer);

	tjs_error TJS_INTF_METHOD
	ClassInstanceInfo(tjs_uint32 flag, tjs_uint num, tTJSVariant *value);
};
//---------------------------------------------------------------------------
} // namespace TJS


#endif
