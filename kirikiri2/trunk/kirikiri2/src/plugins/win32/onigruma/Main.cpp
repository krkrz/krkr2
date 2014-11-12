#include <windows.h>
#include "tp_stub.h"
#include "simplebinder.hpp"

using SimpleBinder::BindUtil;

class tTJSNativeClassForRegExp : public tTJSDispatch
{
public:
	tTJSNativeClassForRegExp(tjs_char const *className) : tTJSDispatch(), classname(className) {
		self = this;
		classobj = TJSCreateNativeClassForPlugin(className, _CreateNativeInstance);
	}

	static bool Link(iTJSDispatch2 *global) {
		Initialize();
		return self && self->Setup(global);
	}
	static bool Unlink(iTJSDispatch2 *global) {
		if (self) {
			self->Terminate(global);
			self->Release();
		}
		self = 0;
		return true;
	}

	// Array.split オーバーライド
	static tjs_error _Split(iTJSDispatch2 *objthis, tTJSVariant *r, tjs_int n, tTJSVariant **p) {
		return self ? self->Split(r, n, p, objthis) : TJS_E_FAIL;
	}

private:
	static tTJSNativeClassForRegExp * self;
	static iTJSNativeInstance * TJS_INTF_METHOD _CreateNativeInstance() {
		return self ? self->CreateNativeInstance() : NULL;
	}
	static void Initialize();

private:
	tTJSVariant origRegEx, arraySplit;
	bool Setup(iTJSDispatch2 *global) {
		global->PropGet(0, classname, NULL, &origRegEx, global);

		tTJSVariant val(classobj, NULL);
		classobj->Release();
		bool r = BindUtil(global, true).Variant(classname, val).IsValid();

		const tjs_char *split = TJS_W("split");
		iTJSDispatch2 *arr = BindUtil::GetObject(TJS_W("Array"), global);
		if (arr) {
			arr->PropGet(0, split, NULL, &arraySplit, arr);

			if (!BindUtil(arr, true).Function(split, &tTJSNativeClassForRegExp::_Split).IsValid()) r = false;
		}
		return r;
	}
	void Terminate(iTJSDispatch2 *global) {
		// delete global.RegExp
		global->DeleteMember(0, classname, NULL, global);

		// restore original RegExp
		if (origRegEx.Type() == tvtObject)
			BindUtil(global, true).Variant(classname, origRegEx);

		// restore original Array.split
		if (arraySplit.Type() == tvtObject)
			BindUtil(TJS_W("Array"), global, true).Variant(TJS_W("split"), arraySplit);
	}

	tjs_error Split(tTJSVariant *r, tjs_int n, tTJSVariant **p, iTJSDispatch2 *objthis) {
		if(n >= 2 && p[0]->Type() == tvtObject) {
			tTJSVariantClosure clo = p[0]->AsObjectClosureNoAddRef();
			if (clo.Object) {
				// func call split(targetstring, reserved, purgeempty, this);
				tTJSVariant result, tvoid;
				tTJSVariant *params[] = { p[1], &tvoid, &tvoid };
				if (n >= 3)  params[1] = p[2];
				if (n >= 4)  params[2] = p[3];
				static ttstr split(TJS_W("split"));
				if(TJS_SUCCEEDED(clo.FuncCall(0, split.c_str(), split.GetHint(),
											  &result, 3, params, NULL))) {
					static ttstr assign(TJS_W("assign"));
					params[0] = &result;
					Try_iTJSDispatch2_FuncCall(objthis, 0, assign.c_str(), assign.GetHint(), NULL, 1, params, NULL);
					result.Clear(); // release array object created on RegExp.split
					if(r) *r = tTJSVariant(objthis, objthis);
					return TJS_S_OK;
				}
			}
		}
		return (arraySplit.AsObjectNoAddRef())->FuncCall(0, NULL, NULL, r, n, p, objthis);
	}

protected:
	tjs_char const *classname;
	tTJSNativeClassForPlugin * classobj;
	virtual iTJSNativeInstance *CreateNativeInstance() = 0; // ->tTJSNC_RegExp
};
tTJSNativeClassForRegExp* tTJSNativeClassForRegExp::self = 0;


// hacks

//#define TJS_cdecl
#define TJS_strchr			wcschr

#define TJS_NATIVE_CLASSID_NAME tTJSNC_RegExp::ClassID
#define tTJSNativeClass tTJSNativeClassForRegExp

#define TJS_eTJSError TVPThrowExceptionMessage
inline void TVPThrowExceptionMessage(const tjs_nchar * msg) {
	ttstr msg_(msg);
	TVPThrowExceptionMessage(msg_.c_str());
}

//#pragma warning (disable: 4996)
//#pragma warning (disable: 4800)

#include "tjsRegExp.cpp"

void tTJSNativeClassForRegExp::Initialize()
{
	TJS::TJSCreateRegExpClass();
}

//---------------------------------------------------------------------------
bool onV2Link() {
	bool r = false;
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		r = tTJSNativeClassForRegExp::Link(global);
		global->Release();
	}
	return r;
}
bool onV2Unlink() {
	bool r = true;
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if(global) {
		r = tTJSNativeClassForRegExp::Unlink(global);
		global->Release();
	}
	return r;
}
