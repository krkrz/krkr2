#include <windows.h>

#include "util.h"

/* check the specified file is kirikiri or its derivateive */
#define MARK_SEARCH_MIN   (500*1024)
#define MARK_SEARCH_MAX   (4*1024*1024)
#define MARK_ONE_READ_SIZE (4*1024)
static int check_is_tvp(const char *buf)
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


int find_kirikiri_executable(char * buf /* at least MAX_PATH + 22 size */)
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

	if(findhandle == INVALID_HANDLE_VALUE) return 0;

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
		me_strcpy(buf, file);
		me_strcpy(buf + me_strlen(file), "\"");

		return 1;
	}
	else
	{
		return 0;
	}
}

