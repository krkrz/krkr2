//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Dictionary class implementation
//---------------------------------------------------------------------------

#ifndef tjsDictionaryH
#define tjsDictionaryH

#include "tjsObject.h"
#include "tjsNative.h"
#include "tjsArray.h"

namespace TJS
{
//---------------------------------------------------------------------------
// tTJSDictionaryClass : Dictoinary Class
//---------------------------------------------------------------------------
class tTJSDictionaryClass : public tTJSNativeClass
{
	typedef tTJSNativeClass inherited;

public:
	tTJSDictionaryClass();
	~tTJSDictionaryClass();

protected:
	tTJSNativeInstance *CreateNativeInstance();
	iTJSDispatch2 *CreateBaseTJSObject();

	static tjs_uint32 ClassID;
private:

protected:
};
//---------------------------------------------------------------------------
// tTJSDictionaryNI : TJS Dictionary Native C++ instance
//---------------------------------------------------------------------------
class tTJSDictionaryNI : public tTJSNativeInstance,
							public tTJSEnumMemberCallbackIntf,
							public tTJSSaveStructuredDataCallback
{
	typedef tTJSNativeInstance inherited;

	tTJSCustomObject * Owner;
public:

	tTJSDictionaryNI();
	~tTJSDictionaryNI();

	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *obj);

private:
	void TJS_INTF_METHOD Invalidate(); // Invalidate override

public:
	void Assign(iTJSDispatch2 *dsp, bool clear = true);

	void Clear();

private:
	bool EnumMemberCallback(const tjs_char *name, tjs_uint32 hint,
		const tTJSVariant & value);
		// method from tTJSEnumMemberCallbackIntf

public:
	void SaveStructuredData(std::vector<iTJSDispatch2 *> &stack,
		tTJSStringAppender & string, const ttstr&indentstr);
		// method from tTJSSaveStructuredDataCallback
private:
	struct tSaveStructCallback : public tTJSEnumMemberCallbackIntf
	{
		std::vector<iTJSDispatch2 *> * Stack;
		tTJSStringAppender * String;
		const ttstr * IndentStr;
		bool First;

		bool EnumMemberCallback(const tjs_char *name, tjs_uint32 hint,
			const tTJSVariant & value);
	};
	friend class tSaveStructCallback;

public:
	void AssignStructure(iTJSDispatch2 * dsp, std::vector<iTJSDispatch2 *> &stack);

	struct tAssignStructCallback : public tTJSEnumMemberCallbackIntf
	{
		std::vector<iTJSDispatch2 *> * Stack;
		iTJSDispatch2 * Dest;

		bool EnumMemberCallback(const tjs_char *name, tjs_uint32 hint,
			const tTJSVariant & value);
	};
	friend class tAssignStructCallback;
};
//---------------------------------------------------------------------------
class tTJSDictionaryObject : public tTJSCustomObject
{
	typedef tTJSCustomObject inherited;

public:
	tTJSDictionaryObject();
	~tTJSDictionaryObject();


	tjs_error TJS_INTF_METHOD
	FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);


	tjs_error TJS_INTF_METHOD
	PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis);

	tjs_error TJS_INTF_METHOD
	CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);


	tjs_error TJS_INTF_METHOD
	Operation(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		tTJSVariant *result,
		const tTJSVariant *param, iTJSDispatch2 *objthis);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TJSGetDictionaryClassID
//---------------------------------------------------------------------------
extern tjs_int32 TJSGetDictionaryClassID();
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSCreateDictionaryObject
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(iTJSDispatch2 *, TJSCreateDictionaryObject, (
	iTJSDispatch2 **classout = NULL));
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace TJS
#endif
