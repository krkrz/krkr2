#include <stdio.h>
#include <iostream>
#include <string>
#include "ncbind/ncbind.hpp"

struct Stdio
{
	// 標準入力からテキストを読み込む
	static tjs_error TJS_INTF_METHOD in(tTJSVariant *result,
										tjs_int numparams,
										tTJSVariant **param,
										iTJSDispatch2 *objthis) {
		if (result) {
			bool utf8 = numparams> 0 && (int)*param[0] != 0;
			std::string str;
			std::getline(std::cin, str);
			if (utf8) {
				const char *s = str.c_str();
				tjs_int len = TVPUtf8ToWideCharString(s, NULL);
				if (len > 0) {
					tjs_char *dat = new tjs_char[len+1];
					try {
						TVPUtf8ToWideCharString(s, dat);
						dat[len] = TJS_W('\0');
					}
					catch(...) {
						delete [] dat;
						throw;
					}
					*result = ttstr(dat);
					delete [] dat;
				}				
			} else {
				*result = ttstr(str.c_str());
			}
		}
		return TJS_S_OK;
	}

	// テキスト出力下請け関数
	static tjs_error TJS_INTF_METHOD _out(tjs_int numparams,
										  tTJSVariant **param,
										  std::ostream &os) {
		if (numparams > 0) {
			bool utf8 = numparams> 1 && (int)*param[1] != 0;
			ttstr str = *param[0];
			if (utf8) {
				const tjs_char *s = str.c_str();
				tjs_int len = TVPWideCharToUtf8String(s, NULL);
				char *dat = new char [len+1];
				try {
					TVPWideCharToUtf8String(s, dat);
					dat[len] = '\0';
				}
				catch(...)	{
					delete [] dat;
					throw;
				}
				os << dat;
				delete [] dat;
			} else {
				tjs_int len = str.GetNarrowStrLen();
				tjs_nchar *dat = new tjs_nchar[len+1];
				try {
					str.ToNarrowStr(dat, len+1);
				}
				catch(...)	{
					delete [] dat;
					throw;
				} 
				os << dat;
				delete [] dat;
			}
		}
		return TJS_S_OK;
	}

	// 標準出力にテキストを出力
	static tjs_error TJS_INTF_METHOD out(tTJSVariant *result,
										 tjs_int numparams,
										 tTJSVariant **param,
										 iTJSDispatch2 *objthis) {
		return _out(numparams, param, std::cout);
	}
	
	// 標準エラー出力にテキストを出力
	static tjs_error TJS_INTF_METHOD err(tTJSVariant *result,
										 tjs_int numparams,
										 tTJSVariant **param,
										 iTJSDispatch2 *objthis) {
		return _out(numparams, param, std::cerr);
	}
};

NCB_ATTACH_CLASS(Stdio, System) {
	RawCallback("stdin",  &Stdio::in, TJS_STATICMEMBER);
	RawCallback("stdout", &Stdio::out, TJS_STATICMEMBER);
	RawCallback("stderr", &Stdio::err, TJS_STATICMEMBER);
}

void PreRegisterCallback()
{
	int in  = _fileno(stdin);
	int out = _fileno(stdout);
	int err = _fileno(stderr);
	// 接続先が無い場合はコンソールを開いてそこに接続する
	if (in == -2 || out == -2 || err == -2) {
		typedef BOOL (WINAPI* AttachConsoleFunc)(DWORD dwProcessId);
		HINSTANCE hDLL = LoadLibrary(L"kernel32.dll");
		AttachConsoleFunc AttachConsole = (AttachConsoleFunc)GetProcAddress(hDLL, "AttachConsole");
		if (AttachConsole && (*AttachConsole)(-1) || ::AllocConsole()) {
			if (in == -2) freopen("CON", "r", stdin); 
			if (out == -2) freopen("CON", "w", stdout);
			if (err == -2) freopen("CON", "w", stderr);
		} else {
			TVPAddImportantLog(L"faild to attach console");
		}
		FreeLibrary(hDLL);
	}
}

NCB_PRE_REGIST_CALLBACK(PreRegisterCallback);
