#include <windows.h>

/* some compatibility functions
   (standerd runtime library cannot be used) */
void zeromemory(void *ptr, unsigned long count)
{
	char *p = (char*)ptr;
	unsigned long i;
	for(i = 0; i < count; i++) p[i] = 0;
}
int comparememory(const void *ptr1, const void *ptr2, unsigned long count)
{
	const char *p1 = (const char*)ptr1;
	const char *p2 = (const char*)ptr2;
	while(count--) if(*(p1++) != *(p2++)) return 0;
	return 1;
}

long me_strlen(const char *s)
{
	const char *o = s;
	while(*s) s++;
	return s - o;
}
void me_strcpy(char *d, const char *s)
{
	while((*(d++) = *(s++)) != '\0');
}


/* check the specified file is kirikiri or its derivateive */
#define MARK_SEARCH_MIN   (500*1024)
#define MARK_SEARCH_MAX   (4*1024*1024)
#define MARK_ONE_READ_SIZE (4*1024)
int check_is_tvp(const char *buf)
{
	/* kirikiri always have "XOPT_EMBED_AREA_" as
	  a mark of embed option, at near the end of the executable. */
	/* return non-zero if "buf" is an executable of kirikiri,
	   otherwise return 0 */

	HANDLE filehandle;
	DWORD result;
	unsigned long fileptr = MARK_SEARCH_MIN;
	char tmp[MARK_ONE_READ_SIZE + 17];

	/* open the file */
	filehandle = CreateFile(
		buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(filehandle == INVALID_HANDLE_VALUE ) return 0;

	/* seek to the start minimum point */
	result = SetFilePointer(filehandle, fileptr, NULL, FILE_BEGIN);
	if(result == 0xffffffff || result != fileptr)
	{
		CloseHandle(filehandle);
		return 0;
	}

	/* search the mark */
	while(fileptr < MARK_SEARCH_MAX)
	{
		DWORD read = 0;
		int i;

		ReadFile(filehandle, tmp, MARK_ONE_READ_SIZE, &read, NULL);
		zeromemory((void*)(tmp + read), sizeof(tmp) - read);

		for(i = 0; i < MARK_ONE_READ_SIZE; i+=16) /*  aligned to 16 bytes */
		{
			if(tmp[i] == 'X' && comparememory(tmp + i + 1, "OPT_EMBED_AREA_", 15))
			{
				/* found */
				CloseHandle(filehandle);
				return 1;
			}
		}

		if(read < MARK_ONE_READ_SIZE) break;

		fileptr += read;
	}

	/* not found */
	CloseHandle(filehandle);
	return 0;
}


void process(void)
{
	/* search kirikiri executable through the executable's path */
	int pathlast, found;
	char *cp;
	HANDLE findhandle;
	WIN32_FIND_DATA fd;
	char filemask[MAX_PATH+20];
	char file[MAX_PATH+22];

	/* retrieve file path and make file mask */
	filemask[0] = '\0';
	GetModuleFileName(GetModuleHandle(NULL), filemask, MAX_PATH);
	cp = filemask;
	pathlast = -1;
	while(1)
	{
		if(*cp == 0) break;
		if(*cp == ':' || *cp == '\\' || *cp == '/')
			pathlast = cp - filemask;
		cp = CharNext(cp);
	}
	pathlast ++;
	me_strcpy(filemask + pathlast, "*.exe");

	/* find the file */
	found = 0;
	file[0] = '\"';
	zeromemory(&fd, sizeof(fd));
	findhandle = FindFirstFile(filemask, &fd);

	if(findhandle == INVALID_HANDLE_VALUE) return;

	do
	{
		me_strcpy(file + 1, filemask); /* file[0] is '\"' */
		me_strcpy(file + pathlast + 1, fd.cFileName);
		if(check_is_tvp(file + 1))
		{
			found = 1;
			break;
		}
	} while(FindNextFile(findhandle, &fd));

	FindClose(findhandle);

	if(found)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		BOOL ret;

		zeromemory(&si, sizeof(si));

		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWNORMAL;

		me_strcpy(file + me_strlen(file), "\" -userconf");
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



/* UIExecUserConfig is called immediately after entry point */
void UIExecUserConfig(void)
{
	process();
}


