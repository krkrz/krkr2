#include "ncbind/ncbind.hpp"

#include "../json/Writer.hpp"

static void
quoteString(const tjs_char *str, IWriter *writer)
{
	if (str) {
		writer->write((tjs_char)'"');
		const tjs_char *p = str;
		int ch;
		while ((ch = *p++)) {
			if (ch == '"') {
				writer->write(L"\\\"");
			} else if (ch == '\\') {
				writer->write(L"\\\\");
			} else {
				writer->write((tjs_char)ch);
			}
		}
		writer->write((tjs_char)'"');
	} else {
		writer->write(L"null");
	}
}

static void getVariantString(tTJSVariant &var, IWriter *writer);

/**
 * 辞書の内容表示用の呼び出しロジック
 */
class DictMemberDispCaller : public tTJSDispatch /** EnumMembers 用 */
{
protected:
	IWriter *writer;
	bool first;
public:
	DictMemberDispCaller(IWriter *writer) : writer(writer) { first = true; };
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
				if (first) {
					first = false;
				} else {
					writer->write((tjs_char)',');
					writer->newline();
				}
				const tjs_char *name = param[0]->GetString();
				quoteString(name, writer);
				writer->write(L"=>");
				getVariantString(*param[2], writer);
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

static void getDictString(iTJSDispatch2 *dict, IWriter *writer)
{
	writer->write(L"%[");
	//writer->addIndent();
	DictMemberDispCaller caller(writer);
	tTJSVariantClosure closure(&caller);
	dict->EnumMembers(TJS_IGNOREPROP, &closure, dict);
	//writer->delIndent();
	writer->write((tjs_char)']');
}

static void getArrayString(iTJSDispatch2 *array, IWriter *writer)
{
	writer->write((tjs_char)'[');
	//writer->addIndent();
	tjs_int count = 0;
	array->GetCount(&count,NULL,NULL,array);
	for (tjs_int i=0; i<count; i++) {
		if (i != 0) {
			writer->write((tjs_char)',');
			writer->newline();
		}
		tTJSVariant result;
		if (array->PropGetByNum(TJS_IGNOREPROP, i, &result, array) == TJS_S_OK) {
			getVariantString(result, writer);
		}
	}
	//writer->delIndent();
	writer->write((tjs_char)']');
}

static void
getVariantString(tTJSVariant &var, IWriter *writer)
{
	switch(var.Type()) {

	case tvtVoid:
		writer->write(L"void");
		break;
		
	case tvtObject:
		{
			iTJSDispatch2 *obj = var.AsObjectNoAddRef();
			if (obj == NULL) {
				writer->write(L"null");
			} else if (obj->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Array",obj) == TJS_S_TRUE) {
				getArrayString(obj, writer);
			} else {
				getDictString(obj, writer);
			}
		}
		break;
		
	case tvtString:
		quoteString(var.GetString(), writer);
		break;

	case tvtInteger:
		writer->write(L"int ");
		writer->write((tTVInteger)var);
		break;

	case tvtReal:
		writer->write(L"real ");
		writer->write((tTVReal)var);
		break;

	default:
		writer->write(L"void");
		break;
	};
}

//---------------------------------------------------------------------------

/**
 * メソッド追加用
 */
class ArrayAdd {

public:
	ArrayAdd(){};

	/**
	 * save 形式での辞書または配列の保存
	 * @param filename ファイル名
	 * @param utf true なら UTF-8 で出力
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD save2(tTJSVariant *result,
										   tjs_int numparams,
										   tTJSVariant **param,
										   iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		IFileWriter writer(param[0]->GetString(), numparams > 1 ? (int)*param[1] != 0: false);
		tjs_int count = 0;
		objthis->GetCount(&count,NULL,NULL,objthis);
		for (int i=0; i<count; i++) {
			tTJSVariant result;
			if (objthis->PropGetByNum(TJS_IGNOREPROP, i, &result, objthis) == TJS_S_OK) {
				writer.write(result.GetString());
				writer.newline();
			}
		}
		return TJS_S_OK;
	}

	/**
	 * saveStruct 形式でのオブジェクトの保存
	 * @param filename ファイル名
	 * @param utf true なら UTF-8 で出力
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD saveStruct2(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		IFileWriter writer(param[0]->GetString(), numparams > 1 ? (int)*param[1] != 0: false);
		getArrayString(objthis, &writer);
		return TJS_S_OK;
	}
	
	/**
	 * saveStruct 形式で文字列化
	 * @param obj オブジェクト
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD toStructString(tTJSVariant *result,
													tjs_int numparams,
													tTJSVariant **param,
													iTJSDispatch2 *objthis) {
		if (result) {
			IStringWriter writer;
			getArrayString(objthis, &writer);
			*result = writer.buf;
		}
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(ArrayAdd, Array) {
	RawCallback("save2", &ArrayAdd::save2,TJS_STATICMEMBER);
	RawCallback("saveStruct2", &ArrayAdd::saveStruct2, TJS_STATICMEMBER);
	RawCallback("toStructString", &ArrayAdd::toStructString, TJS_STATICMEMBER);
};

/**
 * メソッド追加用
 */
class DictAdd {

public:
	DictAdd(){};

	/**
	 * saveStruct 形式でのオブジェクトの保存
	 * @param filename ファイル名
	 * @param utf true なら UTF-8 で出力
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD saveStruct2(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		IFileWriter writer(param[0]->GetString(), numparams > 1 ? (int)*param[1] != 0: false);
		getDictString(objthis, &writer);
		return TJS_S_OK;
	}
	
	/**
	 * saveStruct 形式で文字列化
	 * @param obj オブジェクト
	 * @return 実行結果
	 */
	static tjs_error TJS_INTF_METHOD toStructString(tTJSVariant *result,
													tjs_int numparams,
													tTJSVariant **param,
													iTJSDispatch2 *objthis) {
		if (result) {
			IStringWriter writer;
			getDictString(objthis, &writer);
			*result = writer.buf;
		}
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(DictAdd, Dictionary) {
	RawCallback("saveStruct2", &DictAdd::saveStruct2, TJS_STATICMEMBER);
	RawCallback("toStructString", &DictAdd::toStructString, TJS_STATICMEMBER);
};
