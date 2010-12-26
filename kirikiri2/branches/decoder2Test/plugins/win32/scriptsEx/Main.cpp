#include "ncbind/ncbind.hpp"

/**
 * 辞書のキー一覧取得用
 */
class DictMemberGetCaller : public tTJSDispatch /** EnumMembers 用 */
{
public:
	DictMemberGetCaller(iTJSDispatch2 *array) : array(array) {};
	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
												tjs_uint32 flag,			// calling flag
												const tjs_char * membername,// member name ( NULL for a default member )
												tjs_uint32 *hint,			// hint for the member name (in/out)
												tTJSVariant *result,		// result
												tjs_int numparams,			// number of parameters
												tTJSVariant **param,		// parameters
												iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			tTVInteger flag = param[1]->AsInteger();
			static tjs_uint addHint = NULL;
			if (!(flag & TJS_HIDDENMEMBER)) {
				array->FuncCall(0, TJS_W("add"), &addHint, 0, 1, &param[0], array);
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
protected:
	iTJSDispatch2 *array;
};


/**
 * メソッド追加用
 */
class ScriptsAdd {

public:
	ScriptsAdd(){};

	/**
	 * メンバ名一覧の取得
	 */
	static tjs_error TJS_INTF_METHOD getKeys(tTJSVariant *result,
											 tjs_int numparams,
											 tTJSVariant **param,
											 iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (result) {
			iTJSDispatch2 *array = TJSCreateArrayObject();
			DictMemberGetCaller *caller = new DictMemberGetCaller(array);
			tTJSVariantClosure closure(caller);
			param[0]->AsObjectClosureNoAddRef().EnumMembers(TJS_IGNOREPROP, &closure, NULL);
			caller->Release();
			*result = tTJSVariant(array, array);
			array->Release();
		}
		return TJS_S_OK;
	}

	/**
	 * メンバの個数の取得
	 */
	static tjs_error TJS_INTF_METHOD getCount(tTJSVariant *result,
											  tjs_int numparams,
											  tTJSVariant **param,
											  iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		if (result) {
			tjs_int count;
			param[0]->AsObjectClosureNoAddRef().GetCount(&count, NULL, NULL, NULL);
			*result = count;
		}
		return TJS_S_OK;
	}
	

};

NCB_ATTACH_CLASS(ScriptsAdd, Scripts) {
	RawCallback("getObjectKeys", &ScriptsAdd::getKeys, TJS_STATICMEMBER);
	RawCallback("getObjectCount", &ScriptsAdd::getCount, TJS_STATICMEMBER);
};
