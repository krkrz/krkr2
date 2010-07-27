#include "ncbind/ncbind.hpp"

#ifdef _DEBUG
#define dm(msg) TVPAddLog(msg)
#else
#define dm(msg)
#endif

class WindowRegistory
{
public:
	WindowRegistory(){}

	static tjs_error TJS_INTF_METHOD writeRegistory(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		if(numparams < 3)
			return TJS_E_BADPARAMCOUNT;

		HKEY	hKey;
		DWORD	dwDisposition;
		LONG	res;
		ttstr	key	= param[0]->AsStringNoAddRef();
		res	= RegCreateKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
		if(res != ERROR_SUCCESS)
			return TJS_E_FAIL;

		ttstr	name= param[1]->AsStringNoAddRef();
		switch(param[2]->Type())
		{
		case tvtString:
			{
				ttstr	value	= param[2]->AsStringNoAddRef();
				res	= RegSetValueEx(hKey, name.c_str(), 0, REG_SZ,
					(LPBYTE)value.c_str(), (DWORD)value.length()*2+1);
			}
			break;
		case tvtInteger:
			{
				tjs_uint32	value	= (tjs_uint32)param[2]->AsInteger();
				res	= RegSetValueEx(hKey, name.c_str(), 0, REG_DWORD, (LPBYTE)&value, sizeof(tjs_uint32));
			}
			break;
		}
		RegCloseKey(hKey);

		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD readRegistory(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
//	ttstr readRegistory(ttstr key, ttstr name)
		if(numparams < 2)
			return TJS_E_BADPARAMCOUNT;
		HKEY	hKey;
		DWORD	dwDisposition;
		LONG	res;

		ttstr	key	= param[0]->AsStringNoAddRef();
		res	= RegCreateKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisposition);
		if(res != ERROR_SUCCESS)
			return TJS_E_FAIL;

		ttstr	name= param[1]->AsStringNoAddRef();
		DWORD	dwType;
		DWORD	bufsize	= 1;
		res	= RegQueryValueEx(hKey, name.c_str(), 0, &dwType, NULL, &bufsize);
		if(res == ERROR_SUCCESS)
		{
			switch(dwType)
			{
			case REG_SZ:
				{
					tjs_char	*buf	= new tjs_char[bufsize];
					res	= RegQueryValueEx(hKey, name.c_str(), 0, &dwType, (LPBYTE)buf, &bufsize);
					if(result)
						*result	= ttstr(buf);
					delete [] buf;
				}
				break;
			case REG_DWORD:
				{
					tjs_uint32	buf;
					bufsize	= sizeof(tjs_uint32);
					res	= RegQueryValueEx(hKey, name.c_str(), 0, &dwType, (LPBYTE)&buf, &bufsize);
					if(result)
						*result	= (tjs_int)buf;
				}
				break;
			}
		}
		RegCloseKey(hKey);

		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(WindowRegistory, Window)
{
	RawCallback("writeRegistory", &Class::writeRegistory, 0);
	RawCallback("readRegistory", &Class::readRegistory, 0);
};