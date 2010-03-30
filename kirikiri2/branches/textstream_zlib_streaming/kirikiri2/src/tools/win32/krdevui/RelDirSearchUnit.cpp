//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <inifiles.hpp>

#include "RelDirSearchUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRelDirSearchForm *RelDirSearchForm;
//---------------------------------------------------------------------------
__fastcall TRelDirSearchForm::TRelDirSearchForm(TComponent* Owner)
	: TForm(Owner)
{
	Aborted = false;
	FileList = new TStringList();
	ExtList = new TStringList();
	FileList->Duplicates = dupAccept;
	FileList->Sorted = false;
	ExtList->Duplicates = dupIgnore;
	ExtList->Sorted = true;
}
//---------------------------------------------------------------------------
void __fastcall TRelDirSearchForm::FormDestroy(TObject *Sender)
{
	delete FileList;
	delete ExtList;
}
//---------------------------------------------------------------------------
void __fastcall TRelDirSearchForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	Aborted = true;
	CanClose = true;
}
//---------------------------------------------------------------------------
void __fastcall TRelDirSearchForm::AbortButtonClick(TObject *Sender)
{
	Aborted = true;
}
//---------------------------------------------------------------------------
void __fastcall TRelDirSearchForm::GetFileList(AnsiString path, int baselen)
{
	TSearchRec r;
	int done;

	done=FindFirst(path + "*.*" ,faAnyFile, r);
	try
	{
		while(!done)
		{
			if(r.FindData.cFileName[0]!='.')
			{
				if(! (r.Attr & faDirectory))
				{
					// a file
					AnsiString name = AnsiString(path.c_str()+ baselen) +
						r.FindData.cFileName;
					FileList->Add(name);
					CurrentLabel->Caption = name;
					AnsiString fileext = ExtractFileExt(r.FindData.cFileName);
					ExtList->Add(fileext);

					// message processing
					Application->ProcessMessages();
					if(Aborted)
					{
						throw EAbort("Aborted");  // raise an aborting exception
					}
				}
				else
				{
					// a directory
					if(r.Name != "." && r.Name != ".." &&
						!(r.Name == "CVS" &&
							FileExists(path + AnsiString("CVS\\Repository"))))
								// ignoring CVS meta-data directory
					{
						GetFileList(path  + r.FindData.cFileName+
							AnsiString("\\"), baselen);
					}
				}
			}
			done=FindNext(r);
		}
	}
	catch(Exception &e)
	{
		FindClose(r);
		throw Exception(e);
	}
	FindClose(r);
}
//---------------------------------------------------------------------------
bool __fastcall TRelDirSearchForm::GetFileList(AnsiString folder)
{
	try
	{
		GetFileList(folder, folder.Length());
	}
	catch(...)
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------



