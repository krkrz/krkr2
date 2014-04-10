#include <stdio.h>
#include <iostream>
#include <string>
#include "ncbind/ncbind.hpp"

struct Stdio
{
	static int getState() {
		int state = 0;
		if (_fileno(stdin)  >= 0) state |= 0x01;
		if (_fileno(stdout) >= 0) state |= 0x02;
		if (_fileno(stderr) >= 0) state |= 0x04;
		return state;
	}

	// コンソールと接続
	static tjs_error TJS_INTF_METHOD attach(tTJSVariant *result,
											tjs_int numparams,
											tTJSVariant **param,
											iTJSDispatch2 *objthis) {
		int state = numparams > 0 ? *param[0] : 0;
		bool ret = true;
		if (state == 0) {
			//VS2012以降でこの記述で正しく判定できない。ランタイムのバグと思われる
			//if (_fileno(stdin)  == -2) state |= 0x01;
			//if (_fileno(stdout) == -2) state |= 0x02;
			//if (_fileno(stderr) == -2) state |= 0x04;
			if (GetStdHandle(STD_INPUT_HANDLE) == 0) state |= 0x01;
			if (GetStdHandle(STD_OUTPUT_HANDLE) == 0) state |= 0x02;
			if (GetStdHandle(STD_ERROR_HANDLE) == 0) state |= 0x04;
		}
		// 接続先が無い場合はコンソールを開いてそこに接続する
		if (state != 0) {
			typedef BOOL (WINAPI* AttachConsoleFunc)(DWORD dwProcessId);
			HINSTANCE hDLL = LoadLibrary(L"kernel32.dll");
			AttachConsoleFunc AttachConsole = (AttachConsoleFunc)GetProcAddress(hDLL, "AttachConsole");
			if (AttachConsole && (*AttachConsole)(-1)) {
				if ((state & 0x01)) freopen("CON", "r", stdin); 
				if ((state & 0x02)) freopen("CON", "w", stdout);
				if ((state & 0x04)) freopen("CON", "w", stderr);
			} else {
				ret = false;
			}
			FreeLibrary(hDLL);
		}
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}

	// コンソールの割り当て
	static tjs_error TJS_INTF_METHOD alloc(tTJSVariant *result,
										   tjs_int numparams,
										   tTJSVariant **param,
										   iTJSDispatch2 *objthis) {
		int state = numparams > 0 ? *param[0] : 0;
		bool ret = true;
		if (state == 0) {
			if (_fileno(stdin)  == -2) state |= 0x01;
			if (_fileno(stdout) == -2) state |= 0x02;
			if (_fileno(stderr) == -2) state |= 0x04;
		}
		// 接続先が無い場合はコンソールを開いてそこに接続する
		if (state != 0) {
			if (::AllocConsole()) {
				if ((state & 0x01)) freopen("CON", "r", stdin); 
				if ((state & 0x02)) freopen("CON", "w", stdout);
				if ((state & 0x04)) freopen("CON", "w", stderr);
			} else {
				ret = false;
			}
		}
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD free(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		bool ret = ::FreeConsole() != 0;
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}
	
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

	// 標準出力をフラッシュ
	static tjs_error TJS_INTF_METHOD flush(tTJSVariant *result,
										   tjs_int numparams,
										   tTJSVariant **param,
										   iTJSDispatch2 *objthis) {
		std::cout << std::flush;
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(Stdio, System) {
	Property("stdioState", &Stdio::getState, 0);
	RawCallback("attachConsole",  &Stdio::attach, TJS_STATICMEMBER);
	RawCallback("allocConsole",  &Stdio::alloc, TJS_STATICMEMBER);
	RawCallback("freeConsole",  &Stdio::free, TJS_STATICMEMBER);
	RawCallback("stdin",  &Stdio::in, TJS_STATICMEMBER);
	RawCallback("stdout", &Stdio::out, TJS_STATICMEMBER);
	RawCallback("stderr", &Stdio::err, TJS_STATICMEMBER);
	RawCallback("flush", &Stdio::flush, TJS_STATICMEMBER);
}

void PreRegisterCallback()
{
}

NCB_PRE_REGIST_CALLBACK(PreRegisterCallback);
