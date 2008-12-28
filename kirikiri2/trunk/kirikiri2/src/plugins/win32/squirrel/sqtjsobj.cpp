#include <windows.h>
#include "tp_stub.h"
#include "sqobject.h"

// ログ出力用
extern void printFunc(HSQUIRRELVM v, const SQChar* format, ...);

extern void sq_pushvariant(HSQUIRRELVM v, tTJSVariant &variant);
extern SQRESULT sq_getvariant(HSQUIRRELVM v, int idx, tTJSVariant *result);

// 型情報
static const SQUserPointer TJSOBJTYPETAG = (SQUserPointer)"TJSOBJTYPETAG";

/**
 * 辞書データからクラス登録用のスクリプトを生成するためのオブジェクト
 */
class ScriptCreater : public tTJSDispatch /** EnumMembers 用 */
{
protected:
	// スクリプト記録用
	ttstr script;

	// メソッドのコードを生成
	void registMethod(const tjs_char *methodName) {
		script += "function ";
		script += methodName;
		script += "(){local args=[];for(local i=0;i<vargc;i++){args.add(vargc[i]);};return tjsInvoker(\"";
		script += methodName;
		script += "\", args);}";
	}

	// セッターのコードを生成
	void registSetter(const tjs_char *propertyName) {
		tstring name;
		getSetterName(name, propertyName);
		script += "function ";
		script += name.c_str();
		script += "(arg){tjsSetter(\"";
		script += propertyName;
		script += "\", arg);}";
	}

	// ゲッターのコードを生成
	void registGetter(const tjs_char *propertyName) {
		tstring name;
		getSetterName(name, propertyName);
		script += "function ";
		script += name.c_str();
		script += "(){return tjsGetter(\"";
		script += propertyName;
		script += "\");}";
	}

public:

	// コンストラクタ
	ScriptCreater(const tjs_char *className) {
		script += "constructor(...){local args=[];for(local i=0;i<vargc;i++){args.add(vargv[i]);};return tjsConstructor(\"";
		script += className;
		script += "\", args);}";
	};

	// データの取得
	const tjs_char *getData() {
		return script.c_str();
	}

	// データの長さの取得
	int getLength() {
		return script.GetLen();
	}

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
			if ((int)param[1] != TJS_HIDDENMEMBER) {
				const tjs_char *name = param[0]->GetString();
				int type = (int)*param[2];
				if (type == 0) {
					registMethod(name);
				} else {
					if ((type & 0x01)) {
						registGetter(name);
					}
					if ((type & 0x02)) {
						registSetter(name);
					}
				}
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

/**
 * 吉里吉里オブジェクトを保持するクラス
 */
class TJSObject : public MyObject {

protected:
	// 処理対象オブジェクト
	iTJSDispatch2 *dispatch;

protected:
	// コンストラクタ
	TJSObject() : dispatch(NULL) {
	}

	// デストラクタ
	~TJSObject() {
		if (dispatch) {
			dispatch->Release();
		}
	}
	
	/**
	 * @return スレッド情報オブジェクト
	 */
	static TJSObject *getTJSObject(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
#ifdef USEUD
		SQUserPointer typetag;
		if (SQ_SUCCEEDED(sq_getuserdata(v, idx, &up, &typetag)) &&
			(typetag == TJSOBJTYPETAG)) {
			return (TJSObject*)up;
		}
#else
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, TJSOBJTYPETAG))) {
			return (TJSObject*)up;
		}
#endif
		return NULL;
	}

	/**
	 * オブジェクトのリリーサ
	 */
	static SQInteger release(SQUserPointer up, SQInteger size) {
		TJSObject *self = (TJSObject*)up;
#ifdef USEUD
		self->~TJSObject();
#else
		delete self;
#endif
		return SQ_OK;
	}

	/**
	 * TJSオブジェクトのコンストラクタ
	 * 引数1 オブジェクト
	 * 引数2 クラス名
	 * 引数3 引数
	 */
	static SQInteger tjsConstructor(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
#ifdef USEUD
		TJSObject *self = getTJSObject(v, 1);
		if (self) {
			new (self) TJSObject();
			result = sq_setreleasehook(v, 1, release);
		} else {
			result = ERROR_CREATE(v);
		}
#else
		TJSObject *self = new TJSObject();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
			sq_setreleasehook(v, 1, release);
		} else {
			delete self;
		}
#endif
		if (SQ_SUCCEEDED(result)) {
			// クラスを生成する
			tTJSVariant classObj;
			TVPExecuteExpression(getString(v,2), &classObj);
			iTJSDispatch2 *dispatch = classObj.AsObjectNoAddRef();
			if (dispatch) {
				// 引数変換
				int argc = sq_getsize(v,3);
				tTJSVariant **args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
					sq_pushinteger(v, i);
					if (SQ_SUCCEEDED(sq_get(v, 3))) {
						sq_getvariant(v, -1, args[i]);
						sq_pop(v, 1);
					}
				}
				if (SUCCEEDED(dispatch->CreateNew(0, NULL, NULL, &self->dispatch, argc, args, dispatch))) {
					result = SQ_OK;
				} else {
					result = ERROR_CREATE(v);
				}
				// 引数破棄
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			} else {
				result = ERROR_CREATE(v);
			}
		}
		return result;
	}

	/**
	 * TJSオブジェクト用のメソッド
	 * 引数1 オブジェクト
	 * 引数2 メンバ名
	 * 引数3 引数の配列
	 */
	static SQInteger tjsInvoker(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			
			// 引数変換
			int argc = sq_getsize(v,3);
			tTJSVariant **args = new tTJSVariant*[argc];
			for (int i=0;i<argc;i++) {
				args[i] = new tTJSVariant();
				sq_pushinteger(v, i);
				if (SQ_SUCCEEDED(sq_get(v, 3))) {
					sq_getvariant(v, -1, args[i]);
					sq_pop(v, 1);
				}
			}
			
			// メソッド呼び出し
			int ret = 0;
			tTJSVariant result;
			if (SUCCEEDED(dispatch->FuncCall(0, getString(v,2), NULL, &result, argc, args, dispatch))) {
				if (result.Type() != tvtVoid) {
					sq_pushvariant(v, result);
					result = 1;
				} else {
					result = 0;
				}
			} else {
				result = ERROR_CALL(v);
			}
			
			// 引数破棄
			for (int i=0;i<argc;i++) {
				delete args[i];
			}
			delete[] args;
			return result;
		}
		return ERROR_BADINSTANCE(v);
	}

	/**
	 * TJSオブジェクト用のプロパティゲッター
	 * 引数1 オブジェクト
	 * 引数2 プロパティ名
	 */
	static SQInteger tjsGetter(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			tTJSVariant result;
			if (SUCCEEDED(dispatch->PropGet(0, getString(v,2), NULL, &result, dispatch))) {
				sq_pushvariant(v, result);
				return 1;
			}
			return ERROR_NOMEMBER(v);
		}
		return ERROR_BADINSTANCE(v);
	}

	/**
	 * TJSオブジェクト用のプロパティセッター
	 * 引数1 オブジェクト
	 * 引数2 プロパティ名
	 * 引数3 設定値
	 */
	static SQInteger tjsSetter(HSQUIRRELVM v) {
		TJSObject *self = getTJSObject(v, 1);
		if (self && self->dispatch) {
			iTJSDispatch2 *dispatch = self->dispatch;
			tTJSVariant result;
			sq_getvariant(v, 3, &result);
			if (SUCCEEDED(dispatch->PropSet(TJS_MEMBERENSURE, getString(v, 2), NULL, &result, dispatch))) {
				return SQ_OK;
			}
			return ERROR_NOMEMBER(v);
		}
		return ERROR_BADINSTANCE(v);
	}




	
public:
	static void registClassInit() {
		MyObject::pushTag(TJSOBJTYPETAG);
	}

	/**
	 * クラスの登録
	 */
	static void registClass(HSQUIRRELVM v, const tjs_char *className, const tjs_char *tjsClassName, iTJSDispatch2 *methods) {

		sq_pushroottable(v); // root
		sq_pushstring(v, className, -1);
		sq_pushstring(v, OBJECTNAME, -1);
		sq_get(v,-3);
		sq_newclass(v, true); // 継承する
		sq_settypetag(v, -1, TJSOBJTYPETAG);
#ifdef USEUD
		sq_setclassudsize(v, -1, sizeof TJSObject);
#endif
		// TJS用処理メソッドを登録
		REGISTMETHOD(tjsConstructor);
		REGISTMETHOD(tjsInvoker);
		REGISTMETHOD(tjsSetter);
		REGISTMETHOD(tjsGetter);
		
		// クラス生成用スクリプトを生成する
		ScriptCreater script(tjsClassName);
		if (methods) {
			tTJSVariantClosure closure(&script);
			methods->EnumMembers(TJS_IGNOREPROP, &closure, methods);
		}

		// クラスのコンテキストで実行
		if (SQ_SUCCEEDED(sq_compilebuffer(v, script.getData(), script.getLength(), className, SQTrue))) {
			sq_push(v, -2); // class
			if (SQ_FAILED(sq_call(v, 1, SQFalse, SQTrue))) {
				sq_getlasterror(v);
				const SQChar *str;
				sq_getstring(v, -1, &str);
				printFunc(v, str);
				sq_pop(v, 1);
			}
			sq_pop(v, 1);
		} else {
			sq_getlasterror(v);
			const SQChar *str;
			sq_getstring(v, -1, &str);
			printFunc(v, str);
			sq_pop(v, 1);
		}
	
		sq_createslot(v, -3);
		sq_pop(v, 1); // root
	}
};

void sqtjsobj_init()
{
	TJSObject::registClassInit();
}

void sqtjsobj_regist(HSQUIRRELVM v, const tjs_char *className, const tjs_char *tjsClassName, iTJSDispatch2 *methods)
{
	TJSObject::registClass(v, className, tjsClassName, methods);
};
