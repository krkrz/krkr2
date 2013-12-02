//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <FileCtrl.hpp>

#include "RelMain.h"
#include "RelDirSearchUnit.h"
#include "RelFolderSelectorUnit.h"
#include "RelSettingsUnit.h"

//---------------------------------------------------------------------------
static void _UIExecReleaser()
{
	// load xp3enc.dll
	HMODULE xp3enc = LoadLibrary("xp3enc.dll");
	if(xp3enc)
	{
		void * funcptr;
		funcptr = GetProcAddress(xp3enc, "XP3ArchiveAttractFilter_v2");
		if(funcptr)
		{
			*(void**)&XP3ArchiveAttractFilter_v2_org = funcptr;
			XP3EncDLLAvailable = true;
		}
	}

	// start releaser
	AnsiString projfolder;
	AnsiString outputfile;
	AnsiString rpffile;
	TStringList *filelist;
	TStringList *extlist;

	bool go_immediate = false;
	bool write_rpf = true;

	int paramcount = ParamCount();
	for(int i = 1; i <= paramcount; i++)
	{
		AnsiString param = ParamStr(i);
		if(param.c_str()[0] != '-')
			projfolder = param;
		else if(param == "-go")
			go_immediate = true;
		else if(param == "-nowriterpf")
			write_rpf = false;
		else if(param == "-out")
		{
			i++;
			outputfile = ParamStr(i);
		}
		else if(param == "-rpf")
		{
			i++;
			rpffile = ParamStr(i);
		}
	}

	if(projfolder != "")
	{
		if(projfolder.c_str()[0] == '\\' &&
			projfolder.c_str()[1] == '\\')
		{
			projfolder = ExpandUNCFileName(projfolder);
		}
		else
		{
			projfolder = ExpandFileName(projfolder);
		}
		projfolder = ExcludeTrailingBackslash(projfolder);

		if(!DirectoryExists(projfolder))
		{
			MessageBox(NULL, ("Folder \"" + projfolder + "\" does not exist").c_str(),
				"Error", MB_OK);
			ExitProcess(0);
		}

		projfolder = IncludeTrailingBackslash(projfolder);

		filelist = new TStringList();
		extlist = new TStringList();
		goto search;
	}

start:

	filelist = new TStringList();
	extlist = new TStringList();
	{
		int res;
		TRelFolderSelectorForm *form = new TRelFolderSelectorForm(Application);
		res = form->ShowModal();
		projfolder = IncludeTrailingBackslash(form->ShellTreeView->Path);
		delete form;

		if(res == mrCancel)
		{
			delete filelist;
			delete extlist;
			return;
		}

	}

search:
	{
		bool state;
		TRelDirSearchForm * form = new TRelDirSearchForm(Application);
		form->Visible = true;
		state = form->GetFileList(projfolder);
		if(state == false)
		{
			delete form;
			delete filelist;
			delete extlist;
			return;
		}
		filelist->Assign(form->FileList);
		extlist->Assign(form->ExtList);
		delete form;
	}


	{
		int res;
		TRelSettingsForm * form = new TRelSettingsForm(Application);
		form->SetGoImmediate(go_immediate);
		form->SetWriteDefaultRPF(write_rpf);
		form->SetProjFolder(projfolder);
		form->SetFileList(filelist);
		delete filelist;
		form->SetExtList(extlist);
		delete extlist;
		if(rpffile == "")
			form->LoadDefaultProfile();
		else
			form->LoadProfile(rpffile);
		if(outputfile != "") form->SetOutputFilename(outputfile);
		res = form->ShowModal();
		delete form;
		if(res == mrRetry) goto start;
	}
}
//---------------------------------------------------------------------------
extern "C" void _export PASCAL UIExecReleaser()
{
	_UIExecReleaser();
	ExitProcess(0);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPStream base stream class
//---------------------------------------------------------------------------
void tTVPStream::SetEndOfStorage()
{
	throw Exception("cannot write");
}
//---------------------------------------------------------------------------
void tTVPStream::ReadBuffer(void *buffer, unsigned int read_size)
{
	if(Read(buffer, read_size)!=read_size)
		throw Exception("cannot read");
}
//---------------------------------------------------------------------------
void tTVPStream::WriteBuffer(const void *buffer, unsigned int write_size)
{
	if(Write(buffer, write_size)!=write_size)
		throw Exception("cannot write");
}
//---------------------------------------------------------------------------
void tTVPStream::WriteInt64C(__int64 n)
{
	// compressed int64
	int bytes = 0;
	__int64 mask = (__int64)-1;

	while(bytes < 8)
	{
		if((n & mask) == 0) break;
		mask <<= 8;
		bytes++;
	}

	WriteBuffer(&bytes, 1);
	if(bytes) WriteBuffer(&n, bytes);
}
//---------------------------------------------------------------------------
void tTVPStream::WriteInt64(__int64 n)
{
	// this function is for intel byte order systems
	WriteBuffer(&n, 8);
}
//---------------------------------------------------------------------------
void tTVPStream::WriteInt32(__int32 n)
{
	WriteBuffer(&n, 4);
}
//---------------------------------------------------------------------------
void tTVPStream::WriteInt16(__int16 n)
{
	WriteBuffer(&n, 2);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// XP3 filter related
//---------------------------------------------------------------------------
bool XP3EncDLLAvailable = false;
void (__stdcall * XP3ArchiveAttractFilter_v2_org)(
	unsigned __int32 hash,
	unsigned __int64 offset, void * buffer, long bufferlen) = NULL;
void (__stdcall * XP3ArchiveAttractFilter_v2)(
	unsigned __int32 hash,
	unsigned __int64 offset, void * buffer, long bufferlen) = NULL;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
tTVPLocalFileStream::tTVPLocalFileStream(const char *name,
	unsigned int flag, bool useencryption, unsigned __int32 salt)
{
	FileName = NULL;
	Handle = INVALID_HANDLE_VALUE;
	UseEncryption = useencryption;
	Salt = salt;

	FileName = new char[strlen(name) + 1];
	try
	{
		strcpy(FileName, name);

		unsigned int access = flag & TVP_ST_ACCESS_MASK;

		DWORD dwcd;
		DWORD rw;
		switch(access)
		{
		case TVP_ST_READ:
			rw = GENERIC_READ;					dwcd = OPEN_EXISTING;		break;
		case TVP_ST_WRITE:
			rw = GENERIC_WRITE;					dwcd = CREATE_ALWAYS;		break;
		case TVP_ST_APPEND:
			rw = GENERIC_WRITE;					dwcd = OPEN_ALWAYS;			break;
		case TVP_ST_UPDATE:
			rw = GENERIC_WRITE|GENERIC_READ;	dwcd = OPEN_EXISTING;		break;
		}

		Handle = CreateFile(
			name,
			rw,
			FILE_SHARE_READ, // read shared accesss is strongly needed
			NULL,
			dwcd,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if(Handle == INVALID_HANDLE_VALUE)
		{
			throw Exception(AnsiString("Cannot open  ")+ name);
		}

		if(access == TVP_ST_APPEND)
		{
			// move the file pointer to last
			SetFilePointer(Handle, 0, NULL, FILE_END);
		}
	}
	catch(...)
	{
		delete [] FileName;
		throw;
	}

}
//---------------------------------------------------------------------------
tTVPLocalFileStream::~tTVPLocalFileStream()
{
	if(Handle!=INVALID_HANDLE_VALUE) CloseHandle(Handle);
	if(FileName) delete [] FileName;
}
//---------------------------------------------------------------------------
unsigned __int64 tTVPLocalFileStream::Seek(__int64 offset, int whence)
{
	DWORD dwmm;
	switch(whence)
	{
	case TVP_ST_SEEK_SET:	dwmm = FILE_BEGIN;		break;
	case TVP_ST_SEEK_CUR:	dwmm = FILE_CURRENT;	break;
	case TVP_ST_SEEK_END:	dwmm = FILE_END;		break;
	default:				dwmm = FILE_BEGIN;		break; // may be enough
	}

	DWORD low;

	low = SetFilePointer(Handle, (LONG)offset, ((LONG*)&offset)+1, dwmm);

	if(low == 0xffffffff && GetLastError() != NO_ERROR)
	{
		return 0xffffffffffffffff;
	}
	unsigned __int64 ret;
	*(DWORD*)&ret = low;
	*((DWORD*)&ret+1) = *((DWORD*)&offset+1);

	return ret;
}
//---------------------------------------------------------------------------
unsigned int tTVPLocalFileStream::Read(void *buffer, unsigned int read_size)
{
	DWORD ret = 0;

	unsigned __int64 current;

	if(XP3ArchiveAttractFilter_v2)
		current = Seek(0, TVP_ST_SEEK_CUR);

	ReadFile(Handle, buffer, read_size, &ret, NULL);

	if(ret && XP3ArchiveAttractFilter_v2 && UseEncryption)
	{
		// v2 read filter
		XP3ArchiveAttractFilter_v2(Salt, current, buffer, ret);
	}

	return ret;
}
//---------------------------------------------------------------------------
unsigned int tTVPLocalFileStream::Write(const void *buffer, unsigned int write_size)
{
	DWORD ret = 0;
	WriteFile(Handle, buffer, write_size, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------
void tTVPLocalFileStream::SetEndOfStorage()
{
	if(!SetEndOfFile(Handle)) throw Exception("cannot write");
}
//---------------------------------------------------------------------------
unsigned __int64 tTVPLocalFileStream::GetSize()
{
	unsigned __int64 ret;
	*(DWORD*)&ret = GetFileSize(Handle, ((DWORD*)&ret+1));
	return ret;
}
//---------------------------------------------------------------------------



#pragma package(smart_init)
