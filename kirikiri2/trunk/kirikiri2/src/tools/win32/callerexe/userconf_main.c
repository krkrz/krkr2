#include <windows.h>
#include "util.h"
#include "find_kirikiri.h"


void process(void)
{
	/* search kirikiri executable through the executable's path */
	char file[MAX_PATH+22];

	if(find_kirikiri_executable(file))
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		BOOL ret;

		zeromemory(&si, sizeof(si));

		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWNORMAL;

		me_strcpy(file + me_strlen(file), " -userconf");
			/* append argument */

		ret =  /* execute the executable with "-userconf" option */
			CreateProcess(
				NULL,
				file,
				NULL,
				NULL,
				FALSE,
				0,
				NULL,
				NULL,
				&si,
				&pi);

		if(ret)
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}
	else
	{
		MessageBox(NULL,
			"Could not find kirikiri executable. "
			"Check that the program is properly placed.",
			"Error", MB_OK|MB_ICONSTOP);
	}
}



/* callermain is called immediately after entry point */
void callermain(void)
{
	process();
}


