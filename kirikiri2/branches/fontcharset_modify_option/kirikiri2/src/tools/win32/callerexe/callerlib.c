#include <windows.h>

#define _MAKE_STRING(X) #X
#define MAKE_STRING(X) _MAKE_STRING(X)

void callermain()
{
	HMODULE module;
	void PASCAL (*proc)(void);

	module = LoadLibrary("krdevui.dll");
	if(!module)
		module = LoadLibrary("tools\\krdevui.dll");
	if(!module)
	{
		MessageBox(NULL,
			"Could not find krdevui.dll.\r\n"
			"Check that the program is properly placed.",
			"Error", MB_OK|MB_ICONSTOP);
		return;
	}

	proc = (void PASCAL (*)(void))
		GetProcAddress(module, MAKE_STRING(PROC_NAME));

	if(!proc)
	{
		MessageBox(NULL,
			"Could not find " MAKE_STRING(PROC_NAME) " in krdevui.dll.\r\n"
			"This may be version mismatch.\r\n"
			"Check that the program is placed "
			"with proper version of krdevui.dll.",
			"Error", MB_OK|MB_ICONSTOP);
		return;
	}
	proc();
}
