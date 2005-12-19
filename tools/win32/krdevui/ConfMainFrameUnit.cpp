//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef TVP_ENVIRON
	#include <vcl.h>
	#pragma hdrstop
#else
	#include "tjsCommHead.h"
#endif

#include <vector>
#include <stdio.h>
#include "ConfMainFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConfMainFrame *ConfMainFrame;

#define KRKR_MAX_SIZE (4*1024*1024)
	// maximum possible size of krkr.eXe executable

//---------------------------------------------------------------------------
#ifndef TVP_ENVIRON
void __fastcall CopyChangingIcon(AnsiString outname, AnsiString inname,
	AnsiString iconname)
{
	// check input exe file
	TStream *st = new TFileStream(inname, fmOpenRead|fmShareDenyWrite);

	char buf[16];
	st->Position=0;
	st->ReadBuffer(buf,2);

	if(memcmp(buf,"MZ",2))
	{
		delete st;
		throw Exception("Invalid exe type!\nOperation was aborted.");
	}

	int v,pebase;
	st->Position= 0x3c;
	pebase=0;
	st->ReadBuffer(&pebase,2); // PE header position
	st->Position=pebase;
	v=0;
	st->ReadBuffer(&v,2); // 'PE'
	if(v!='PE')
	{
		delete st;
		throw Exception("PE header was not found!\nOperation was aborted.\n");
	}

	// search .rsrc section      pebase = 'PE' position
	st->Position=pebase+6;
	int objcount=0;
	st->ReadBuffer(&objcount,1); // Object Count
	int i;
	for(i=0;i<objcount;i++)
	{
		char buf[6];
		st->Position=pebase+ i*40 + 0xf8;
		st->ReadBuffer(buf,6);
		if(!strcmp(buf,".rsrc")) break;
	}
	if(i==objcount)
	{
		delete st;
		throw Exception(".rsrc section was not found!\nOperation was aborted.\n");
	}

	int rsrcofs=0;
	int ofsbias; // offset bias
	st->Position=pebase+ i*40 + 0xf8 +0x14;
	st->ReadBuffer(&rsrcofs,4);  // Phys. offset
	ofsbias=0;
	st->Position=pebase+ i*40 + 0xf8 +0x0c;
	st->ReadBuffer(&ofsbias,4); // RVA
	ofsbias-=rsrcofs;

	// search icon resource
	int ne=0,ie=0; // named entry, id entry
	st->Position=rsrcofs+0x0c;
	st->ReadBuffer(&ne,2);  // named entry
	st->Position=rsrcofs+0x0e;
	st->ReadBuffer(&ie,2);  // id entry
	for(i=0;i<ie;i++)
	{
		int type=0;
		st->Position=rsrcofs+16+ne*8+i*8;
		st->ReadBuffer(&type,2); // type
		if(type==3) break; // icon!
	}

	if(i==ie)
	{
		delete st;
		throw Exception("ICON resource section was not found!\nOperation was aborted.\n");
	}

	// search group icon resource
	st->Position= rsrcofs+16+ne*8+i*8+4;
	int icondir0ofs=0;
	st->ReadBuffer(&icondir0ofs,4);
	icondir0ofs=(icondir0ofs-0x80000000)+rsrcofs;

	st->Position=rsrcofs+0x0c;
	st->ReadBuffer(&ne,2);  // named entry
	st->Position=rsrcofs+0x0e;
	st->ReadBuffer(&ie,2);  // id entry
	for(i=0;i<ie;i++)
	{
		int type=0;
		st->Position=rsrcofs+16+ne*8+i*8;
		st->ReadBuffer(&type,2); // type
		if(type==14) break; // group icon!
	}

	if(i==ie)
	{
		delete st;
		throw Exception("GROUP ICON resource section was not found!\nOperation was aborted.\n");
	}

	st->Position= rsrcofs+16+ne*8+i*8+4;
	int grpicondir0ofs=0;
	st->ReadBuffer(&grpicondir0ofs,4);
	grpicondir0ofs=(grpicondir0ofs-0x80000000)+rsrcofs;


	// here comes icon directory;
	// icon is limited by
	// 1. count of icon images
	// 2. total size of the icon
	int maxicons;
	st->Position=icondir0ofs+0x0e;
	maxicons=0;
	st->ReadBuffer(&maxicons,2); // id entry
	int maxsize=0;
	for(i=0;i<maxicons;i++)
	{
		st->Position= icondir0ofs+16+i*8+4;
		int icondir1ofs=0;
		st->ReadBuffer(&icondir1ofs,4);
		icondir1ofs=(icondir1ofs-0x80000000)+rsrcofs;
		int psize=0;
		st->Position=icondir1ofs+ 4+16;
		st->ReadBuffer(&psize,4);
		int size=0;
		st->Position=psize+rsrcofs+4;
		st->ReadBuffer(&size,4);
		maxsize+=size;
	}

	// analyze icon file
	TFileStream *iconst;
	try
	{
		iconst=new TFileStream(iconname,fmOpenRead);
	}
	catch(Exception &e)
	{
		delete st;
		throw Exception(e);
	}

	int numicons=0;
	iconst->Position=4;
	iconst->ReadBuffer(&numicons,2);
	if(numicons>maxicons)
	{
		delete iconst;
		delete st;
		throw Exception("New icon image count exceeds original icon image count!\n"
			"Operation was aborted.\n");
	}

	int iconsize=iconst->Size-6- numicons*16;
	if(iconsize>maxsize)
	{
		delete iconst;
		delete st;
		throw Exception("New icon size exceeds original icon size!\n"
			"Operation was aborted.\n");
	}

	// copy the program
	TFileStream *outst;
	if(inname != outname)
	{
		try
		{
			outst=new TFileStream(outname, fmCreate );
		}
		catch(Exception &e)
		{
			delete st;
			delete iconst;
			throw Exception(e);
		}

		outst->CopyFrom(st,0);
		delete outst;
	}

	delete st;
	outst = new TFileStream(outname, fmOpenReadWrite|fmShareDenyWrite);

	// update icondir0
	outst->Position=icondir0ofs+0x0e;
	outst->WriteBuffer(&numicons,2);

	// update data index
	int maindatastart=0;
	int icondataoffset=0;
	for(i=0;i<numicons;i++)
	{
		outst->Position= icondir0ofs+16+i*8+4;
		int icondir1ofs=0;
		outst->ReadBuffer(&icondir1ofs,4);
		icondir1ofs=(icondir1ofs-0x80000000)+rsrcofs;
		int pdataindex=0;
		outst->Position=icondir1ofs+ 4+16;
		outst->ReadBuffer(&pdataindex,4);
		outst->Position=pdataindex+rsrcofs;
		if(i==0)
		{
			outst->ReadBuffer(&maindatastart,4); // data offset
		}
		outst->Position=pdataindex+rsrcofs;
		int v=maindatastart+icondataoffset;
		outst->WriteBuffer(&v,4); // data offset
		iconst->Position=i*16+6+0x08;
		iconst->ReadBuffer(&v,4); // image size
		outst->Position=pdataindex+rsrcofs+4;
		outst->WriteBuffer(&v,4); // image size
		icondataoffset+=v;
	}

	// copy icon image data
	maindatastart-=ofsbias;
	for(i=0;i<numicons;i++)
	{
		int ofs;
		iconst->Position=i*16+6+0x0c;
		iconst->ReadBuffer(&ofs,4);
		int size;
		iconst->Position=i*16+6+0x08;
		iconst->ReadBuffer(&size,4);
		BYTE *data=new BYTE[size];
		iconst->Position=ofs;
		iconst->ReadBuffer(data,size);
		outst->Position=maindatastart;
		outst->WriteBuffer(data,size);
		delete [] data;
		maindatastart+=size;
	}

	// update group icon
	outst->Position=grpicondir0ofs+16+4;    // for first group icon ...
	int grpicondir1ofs=0;
	outst->ReadBuffer(&grpicondir1ofs,4);
	grpicondir1ofs=(grpicondir1ofs-0x80000000)+rsrcofs;
	int pdataindex=0;
	outst->Position=grpicondir1ofs+4+16;
	outst->ReadBuffer(&pdataindex,4);
	outst->Position=pdataindex+rsrcofs;
	int grpdatastart;
	outst->ReadBuffer(&grpdatastart,4);
	grpdatastart-=ofsbias;
	outst->Position=pdataindex+rsrcofs+4;
	v= 6+ numicons*14;
	outst->WriteBuffer(&v,4); // size
	outst->Position=grpdatastart+4;
	v = numicons;
	outst->WriteBuffer(&v,2); // count
	for(i=0;i<numicons;i++)
	{
		BYTE buf[8];
		iconst->Position=i*16+6;
		iconst->ReadBuffer(buf,8);
		int ofs;
		iconst->Position=i*16+6+0x0c;
		iconst->ReadBuffer(&ofs,4);
		iconst->Position=ofs+0x0e;
		int cc=0;
		iconst->ReadBuffer(&cc,2);
		buf[2]=(BYTE) ( (cc>=8) ? 0 : (1<<cc) );
		buf[4]=1;
		buf[5]=0;
		buf[6]=(BYTE)(cc);
		buf[7]=0;

		outst->Position=grpdatastart+ 6+ i*14;
		outst->WriteBuffer(buf,8);
		int size;
		iconst->Position=i*16+6+0x08;
		iconst->ReadBuffer(&size,4);
		outst->Position=grpdatastart+ 6 + i*14 +8;
		outst->WriteBuffer(&size,4);
		outst->Position=grpdatastart+ 6 + i*14 +12;
		v=i+1;
		outst->WriteBuffer(&v,2);
	}

	//finish
	delete outst;
	delete iconst;
}
#endif
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void SplitAnsiString(AnsiString str, AnsiString splitter, std::vector<AnsiString> & array)
{
	// split given string by given splitter letter.
	array.clear();
	int splen = splitter.Length();
	int i;
	while((i = str.AnsiPos(splitter)) != 0)
	{
		array.push_back(AnsiString(str.c_str(), i-1));
		str = str.c_str() + i + splen -1;
	}
	array.push_back(str);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
struct TOptionData
{
	AnsiString Category;
	AnsiString Caption;
	AnsiString Description;
	AnsiString OptionName;
	AnsiString OptionValueCaption;
	AnsiString OptionValue;
	AnsiString DefaultOptionValueCaption;
	AnsiString DefaultOptionValue;
	AnsiString OptionType;
	bool Invisible;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
__fastcall TConfMainFrame::TConfMainFrame(TComponent* Owner)
	: TFrame(Owner)
{
	DescGot = false;
	ExcludeOptions = NULL;
	UnrecognizedOptions = NULL;
	ExcludeInvisibleOptions = false;
}
//---------------------------------------------------------------------------
__fastcall TConfMainFrame::~TConfMainFrame()
{
	FreeOptionsData();
	if(ExcludeOptions) delete ExcludeOptions;
	if(UnrecognizedOptions) delete UnrecognizedOptions;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::RefIconButtonClick(TObject *Sender)
{
	if(OpenPictureDialog->Execute())
	{
		try
		{
			IconImage->Picture->LoadFromFile(OpenPictureDialog->FileName);
		}
		catch(...)
		{
			OpenPictureDialog->FileName = "";
			throw;
		}
		ChangeIconCheck->Checked = true;
	}

}
//---------------------------------------------------------------------------
bool __fastcall TConfMainFrame::CheckOK()
{
	if(ChangeIconCheck->Checked && OpenPictureDialog->FileName == "")
	{
		RefIconButtonClick(this);
		if(OpenPictureDialog->FileName == "") return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::CopyExe(AnsiString to)
{
#ifndef TVP_ENVIRON
	// copy specified exe file to destination, applying options.
	if(ChangeIconCheck->Checked)
	{
		CopyChangingIcon(to, SourceExe, OpenPictureDialog->FileName);
	}
	else
	{
		TFileStream *src = NULL;
		TFileStream *dest = NULL;
		try
		{
			src = new TFileStream(SourceExe, fmOpenRead|fmShareDenyWrite);
			dest = new TFileStream(to, fmCreate|fmShareDenyWrite);
			dest->CopyFrom(src, 0);

		}
		catch(...)
		{
			if(src) delete src;
			if(dest) delete dest;
			throw;
		}

		if(src) delete src;
		if(dest) delete dest;
	}

	SaveOptionsToExe(to);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::ModifyExe()
{
#ifndef TVP_ENVIRON
	if(ChangeIconCheck->Checked)
	{
		CopyChangingIcon(SourceExe, SourceExe, OpenPictureDialog->FileName);
	}
#endif
	SaveOptionsToExe(SourceExe);
}
//---------------------------------------------------------------------------
static int HexNum(char ch)
{
	if(ch>='a' && ch<='f')return ch-'a'+10;
	if(ch>='A' && ch<='F')return ch-'A'+10;
	if(ch>='0' && ch<='9')return ch-'0';
	return -1;
}
//---------------------------------------------------------------------------
AnsiString TConfMainFrame::EncodeString(AnsiString str)
{
	if(str == "") return "\"\"";

#ifdef TVP_ENVIRON
	ttstr ws(str.c_str());
	const wchar_t *p = ws.c_str();
#else
	WideString ws(str);
	const wchar_t *p = ws.c_bstr();
#endif

	AnsiString ret = "\"";
	while(*p)
	{
		char tmp[10];
		sprintf(tmp, "\\x%X", *p);
		ret += tmp;
		p++;
	}
	return ret + "\"";
}
//---------------------------------------------------------------------------
AnsiString TConfMainFrame::DecodeString(AnsiString str)
{
	if(str == "") return "";

#ifdef TVP_ENVIRON
	ttstr ret;
#else
	WideString ret;
#endif

	const char *p = str.c_str();

	if(p[0] != '\"') return str;

	p++;
	while(*p)
	{
		if(*p != '\\') break;
		p++;
		if(*p != 'x') break;
		p++;
		wchar_t ch = 0;
		while(true)
		{
			int n = HexNum(*p);
			if(n == -1) break;
			ch <<= 4;
			ch += n;
			p++;
		}
		ret += ch;
	}

#ifdef TVP_ENVIRON
	return ret.AsAnsiString();
#else
	return AnsiString(ret);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetOptionsFromString(AnsiString ops)
{
	if(ops != "")
	{
		std::vector<AnsiString> array;
		SplitAnsiString(ops, ",", array);
		std::vector<AnsiString>::iterator i;
		for(i = array.begin(); i != array.end(); i++)
		{
			std::vector<AnsiString> pair;
			SplitAnsiString(*i, ":", pair);
			if(pair.size() != 2) break; // read error
			SetOptionValue(pair[0], pair[1]);
		}
	}
}
//---------------------------------------------------------------------------
AnsiString __fastcall TConfMainFrame::GetOptionString()
{
	AnsiString options = "";
	if(OptionsTreeView->Items->Count)
	{
		TTreeNode *node;
		node = OptionsTreeView->Items->Item[0];
		TOptionData * data;
		do
		{
			data = (TOptionData*)node->Data;
			if(data && data->OptionType != "")
			{
				if(data->OptionValue != data->DefaultOptionValue)
				{
					if(options != "") options += ",";
					options += data->OptionName + ":" + EncodeString(data->OptionValue);
				}
			}
		} while((node = node->GetNext())!=NULL);
	}

	if(UnrecognizedOptions)
	{
		for(int i = 0; i < UnrecognizedOptions->Count; i++)
		{
			AnsiString name = UnrecognizedOptions->Names[i];
			if(name != "")
			{
				AnsiString value = UnrecognizedOptions->Values[name];
				if(options != "") options += ",";
				options += name + ":" + value;
			}
		}
	}

	return options;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetExcludeOptionsFromString(AnsiString str)
{
	std::vector<AnsiString> array;
	SplitAnsiString(str, ",", array);
	if(!ExcludeOptions) ExcludeOptions = new TStringList;
	for(unsigned int i = 0; i <array.size(); i++)
	{
		ExcludeOptions->Values[array[i]] = "i";
		if(OptionsTreeView->Items->Count)
		{
			TTreeNode *node;
			node = OptionsTreeView->Items->Item[0];
			TOptionData * data;
			do
			{
				data = (TOptionData*)node->Data;

				if(data && data->OptionName == array[i])
				{
					SetNodeInvisibleState(node, true);
				}

			} while((node = node->GetNext())!=NULL);
		}
	}
}
//---------------------------------------------------------------------------
AnsiString __fastcall TConfMainFrame::GetExcludeOptionString()
{
	AnsiString options = "";
	if(OptionsTreeView->Items->Count)
	{
		TTreeNode *node;
		node = OptionsTreeView->Items->Item[0];
		TOptionData * data;
		do
		{
			data = (TOptionData*)node->Data;
			if(data && data->OptionType != "")
			{
				if(data->Invisible)
				{
					if(options != "") options += ",";
					options += data->OptionName;
				}
			}
		} while((node = node->GetNext())!=NULL);
	}

	return options;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::LoadProfileFromIni(TMemIniFile *ini)
{
	ChangeIconCheck->Checked = ini->ReadBool("Executable", "ChangeIcon", false);

	OpenPictureDialog->FileName = ini->ReadString("Executable", "Icon", "");
	if(OpenPictureDialog->FileName != "")
	{
		try
		{
			IconImage->Picture->LoadFromFile(OpenPictureDialog->FileName);
		}
		catch(...)
		{
			OpenPictureDialog->FileName = "";
		}
	}

	// read option overrides
	AnsiString ops = ini->ReadString("Executable", "Options", "");
	SetOptionsFromString(ops);
	AnsiString exops = ini->ReadString("Executable", "InvisibleOptions", "");
	SetExcludeOptionsFromString(exops);
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SaveProfileToIni(TMemIniFile *ini)
{
	ini->WriteBool("Executable", "ChangeIcon", ChangeIconCheck->Checked);
	ini->WriteString("Executable", "Icon",  OpenPictureDialog->FileName);

	// write option overrides
	ini->WriteString("Executable", "Options", GetOptionString());
	ini->WriteString("Executable", "InvisibleOptions", GetExcludeOptionString());
}
//---------------------------------------------------------------------------
AnsiString __fastcall TConfMainFrame::GetPluginOptions(AnsiString module)
{
	static char has_option_sig[] =
		" --has-option--)";
	has_option_sig[0] = '(';

	char buf[2048 + 20];
	ZeroMemory(buf, 2048 + 20);
	TFileStream *fs = new TFileStream(module, fmOpenRead|fmShareDenyWrite);
	try
	{
		fs->Read(buf, 2048);
	}
	catch(...)
	{
		delete fs;
		return "";
	}
	delete fs;

	int i;
	for(i = 0; i < 2048; i++)
	{
		if(buf[i] == '(' && !memcmp(buf + i, has_option_sig, 16))
		{
			// valid DLL
			HMODULE mod = LoadLibrary(module.c_str());
			if(!mod) return "";
			wchar_t *(__stdcall *ptr)();
			ptr = (wchar_t *(__stdcall *)())GetProcAddress(mod, "GetOptionDesc");
			if(!ptr)
			{
				FreeLibrary(mod);
				return "";
			}
			AnsiString ret = ptr();
			FreeLibrary(mod);
			return ret;
		}
	}

	return "";
}
//---------------------------------------------------------------------------
AnsiString __fastcall TConfMainFrame::SearchPluginOptions(AnsiString path, AnsiString ext)
{
	AnsiString exepath =
		IncludeTrailingBackslash(ExtractFileDir(SourceExe));
	path = exepath + path;
	AnsiString mask = path + "*." + ext;

	TSearchRec sr;
	AnsiString ret;
	if(FindFirst(mask, faAnyFile, sr) == 0)
	{
		do
		{
			try
			{
				ret += GetPluginOptions(path + sr.FindData.cFileName);
			}
			catch(...)
			{
			}
		} while(FindNext(sr) == 0);
		FindClose(sr);
	}

	return ret;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::ParseData()
{
	// cast into treeview
	AnsiString optionsdata;
	TFileStream *stream = NULL;
	wchar_t * buf = NULL;

	try
	{
		stream = new TFileStream(OptionsFileName,
			fmOpenRead|fmShareDenyWrite);
		unsigned int size = stream->Size;

		buf = new wchar_t[size/2 + 1];

		stream->ReadBuffer(buf, size);
		buf[size/2] = 0;
	}
	catch(...)
	{
		DeleteFile(OptionsFileName);
		if(stream) delete stream;
		if(buf) delete [] buf;
	}

	optionsdata = buf;

	optionsdata += SearchPluginOptions("\\", "dll");
	optionsdata += SearchPluginOptions("\\", "tpm");
	optionsdata += SearchPluginOptions("plugin\\", "dll");
	optionsdata += SearchPluginOptions("plugin\\", "tpm");

	delete [] buf;
	delete stream;
	stream = NULL;

	try
	{
		stream = new TFileStream(OptionsFileName,
			fmCreate|fmShareDenyWrite);
		stream->WriteBuffer(optionsdata.c_str(), optionsdata.Length());
	}
	catch(...)
	{
		DeleteFile(OptionsFileName);
		if(stream) delete stream;
	}

	delete stream;

	TStrings *strings = NULL;
	try
	{
		strings = new TStringList();
		strings->LoadFromFile(OptionsFileName);
		ParseOptionsData(strings);
	}
	catch(...)
	{
		if(strings) delete strings;
		DeleteFile(OptionsFileName);
		throw;
	}
	if(strings) delete strings;
	DeleteFile(OptionsFileName);


	OptionsGroupBox->Enabled = true;
	OptionsGroupBox->Caption = OriginalOptionsGroupBoxLabel;
}
//---------------------------------------------------------------------------
const static char * OptionsParseError = "Options parse error";
void __fastcall TConfMainFrame::ParseOptionsData(TStrings *options)
{
	// parse options data stored in tree string
	TTreeNode *node = NULL;
//	TTreeNode *lastparent = NULL;
	TOptionData * data = NULL;
	OptionsTreeView->Items->BeginUpdate();
	try
	{
		for(int i = 0; i<options->Count; i++)
		{
			// category:caption and description|option name|possible option values
			AnsiString cap = options->Strings[i];
			std::vector<AnsiString> array;
			SplitAnsiString(cap, "|", array);

			AnsiString text, optionname, optiontype;
			text = array[0];
			optionname = array.size() >= 2 ? array[1] : AnsiString();
			optiontype = array.size() >= 3 ? array[2] : AnsiString();

			TOptionData * data = new TOptionData();
			data->Invisible = false;

			if(ExcludeOptions)
			{
				if(ExcludeOptions->Values[optionname] == "i")
				{
					if(ExcludeInvisibleOptions)
					{
						delete data;
						continue;
					}
					data->Invisible = true;
				}
			}

			data->OptionName = optionname;
			data->OptionType = optiontype;

			// split caption and description
			SplitAnsiString(text, ";", array);
			AnsiString caption, category, desc;
			if(array.size() >= 2)
			{
				caption = array[0];
				desc = array[1];
			}
			else
			{
				caption = text;
			}

			SplitAnsiString(caption, ":", array);
			node = NULL;
			if(array.size() >= 2)
			{
				// category found
				caption = array[1];
				category = array[0];
				bool found = false;
				for(int i = 0; i <OptionsTreeView->Items->Count; i++)
				{
					if(OptionsTreeView->Items->Item[i]->Text == category)
					{
						// parent found
						for(int j = 0; j < OptionsTreeView->Items->Item[i]->Count; j++)
							if(OptionsTreeView->Items->Item[i]->Item[j]->Data)
							{
								TOptionData* olddata = (TOptionData*)
									OptionsTreeView->Items->Item[i]->Item[j]->Data;
								if(olddata->Caption == caption)
								{
									node = OptionsTreeView->Items->Item[i]->Item[j];
									delete olddata;
								}
							}
						if(!node)
							node = OptionsTreeView->Items->AddChild(OptionsTreeView->Items->Item[i], "");
						found = true;
						break;
					}
				}
				if(!found)
				{
					// create new category
					node = OptionsTreeView->Items->Add(OptionsTreeView->TopItem, category);
					node = OptionsTreeView->Items->AddChild(node, "");
				}
			}
			else
			{
				category = text;
			}

			if(node == NULL)
			{
				// add to root
				node = OptionsTreeView->Items->Add(OptionsTreeView->TopItem, "");
			}

			data->Caption = caption;
			data->Description = desc;
			data->Category = category;


			node->Data = data;

			// retrieve default value
			SplitAnsiString(data->OptionType, ",", array);

			unsigned int i;
			for(i = 1; i < array.size(); i++)
			{
				if(array[i].c_str()[0] == '*')
				{
					// default
					GetValueCaptionAndValue(array[i],
						data->DefaultOptionValueCaption,
						data->DefaultOptionValue);
				}
			}

			// set default value
			SetNodeDefaultOptionValue(node);

			data = NULL;
		}
	}
	catch(...)
	{
		OptionsTreeView->Items->EndUpdate();
		if(data) delete data;
		throw;
	}
	OptionsTreeView->FullExpand();
	OptionsTreeView->TopItem = OptionsTreeView->Items->Item[0];
	OptionsTreeView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::FreeOptionsData()
{
	// free options data associated with tree nodes
	if(!OptionsTreeView->Items->Count) return;

	TTreeNode *node;
	node = OptionsTreeView->Items->Item[0];
	TOptionData * data;
	do
	{
		data = (TOptionData*)node->Data;
		delete data;
	} while((node = node->GetNext())!=NULL);
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::GetValueCaptionAndValue(AnsiString str,
		AnsiString &valuecaption, AnsiString &value)
{
	if(str.Length() >= 1 && str.c_str()[0] == '*') str = str.c_str() + 1;
	std::vector<AnsiString> array;
	SplitAnsiString(str, ";", array);
	if(array.size() <= 1)
	{
		valuecaption = array[0].c_str();
		value = array[0].c_str();
	}
	else
	{
		valuecaption = array[1];
		value = array[0].c_str();
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetOptionValue(AnsiString name, AnsiString value)
{
	if(!OptionsTreeView->Items->Count) return;

	TTreeNode *node;
	node = OptionsTreeView->Items->Item[0];
	TOptionData * data;
	AnsiString orgvalue = value;
	value = DecodeString(value);
	do
	{
		data = (TOptionData*)node->Data;

		if(data && data->OptionName == name)
		{
			std::vector<AnsiString> array;
			SplitAnsiString(data->OptionType, ",", array);
			std::vector<AnsiString>::iterator i;
			if(array.size() >= 2)
			{
				if(array[0] == "select")
				{
					for(i = array.begin() + 1; i != array.end(); i++)
					{
						AnsiString valcap;
						AnsiString val;
						GetValueCaptionAndValue(*i, valcap, val);
						if(value == val)
						{
							SetNodeOptionValue(node, valcap, val);
							return;
						}
					}
				}
				else if(!strncmp(array[0].c_str(), "string(", 7))
				{
					SetNodeOptionValue(node, value, value);
					return;
				}
			}
		}

	} while((node = node->GetNext())!=NULL);

	if(!UnrecognizedOptions) UnrecognizedOptions = new TStringList;
	UnrecognizedOptions->Values[name] = orgvalue;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetNodeOptionValue(TTreeNode *node,
	AnsiString valuecaption, AnsiString value)
{
	TOptionData * data = (TOptionData*)node->Data;
	if(!data) return;
	if(data->OptionType == "") return;
	data->OptionValue = value;
	data->OptionValueCaption = valuecaption;

	AnsiString mark;
	if(data->DefaultOptionValue != data->OptionValue) mark = "*";
	if(data->Invisible) mark += "!";
	if(mark != "") mark += " ";
	node->Text = mark + data->Caption + " : " + data->OptionValueCaption;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetNodeInvisibleState(TTreeNode *node, bool invisible)
{
	TOptionData * data = (TOptionData*)node->Data;
	if(!data) return;
	data->Invisible = invisible;

	AnsiString mark;
	if(data->DefaultOptionValue != data->OptionValue) mark = "*";
	if(data->Invisible) mark += "!";
	if(mark != "") mark += " ";
	node->Text = mark + data->Caption + " : " + data->OptionValueCaption;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetNodeDefaultOptionValue(TTreeNode *node)
{
	// currently only "select" option type is supported

	std::vector<AnsiString> array;
	TOptionData * data = (TOptionData*)node->Data;
	if(data->OptionType == "") return;
	SetNodeOptionValue(node, data->DefaultOptionValueCaption, data->DefaultOptionValue);
}
//---------------------------------------------------------------------------

void __fastcall TConfMainFrame::OptionsTreeViewChange(TObject *Sender,
	  TTreeNode *Node)
{
	// currently only "select" option type is supported

	if(!Node)
	{
		OptionValueComboBox->Items->Clear();
		OptionValueComboBox->Enabled = false;
		InvisibleCheckBox->Enabled = false;
		InvisibleCheckBox->Checked = false;
		RestoreDefaultButton->Enabled = false;
		OptionValueEdit->Visible = false;
		OptionValueComboBox->Visible = true;
		return;
	}

	// find description
	OptionDescMemo->Text = "";
	TTreeNode *node = Node;
	while(true)
	{
		if(node->Data)
		{
			TOptionData * data = ((TOptionData*)node->Data);
			if(data && data->Description != "")
			{
				AnsiString desc = data->Description;
				if(((TOptionData*)Node->Data)->OptionName != "")
					desc = "-" + ((TOptionData*)Node->Data)->OptionName +
						"\r\n" + desc;
				OptionDescMemo->Text = desc;
				break;
			}
		}
		node = node->Parent;
		if(!node) break;
	}

	// set option combo box
	OptionValueComboBox->Items->Clear();

	TOptionData *data = ((TOptionData*)Node->Data);

	if(data && data->OptionType != "")
	{
		int selectidx = -1;
		std::vector<AnsiString> array;
		SplitAnsiString(data->OptionType, ",", array);
		if(array[0] == "select")
		{

			unsigned int i;
			OptionValueComboBox->Items->BeginUpdate();
			for(i = 1; i < array.size(); i++)
			{
				AnsiString str = array[i];
				if(str.c_str()[0] == '*') str = str.c_str() + 1;
				std::vector<AnsiString> optarray;
				SplitAnsiString(str, ";", optarray);
				AnsiString value;
				if(optarray.size() <= 1)
				{
					value = array[i];
					OptionValueComboBox->Items->Add(array[i]);
				}
				else
				{
					value = optarray[0];
					OptionValueComboBox->Items->Add(optarray[1] + " / " + optarray[0]);
				}
				if(value == data->OptionValue) selectidx = i -1;
			}
			OptionValueComboBox->Items->EndUpdate();
			OptionValueComboBox->ItemIndex = selectidx;
			OptionValueComboBox->Enabled = true;
			OptionValueEdit->Visible = false;
			OptionValueComboBox->Visible = true;
		}
		else if(!strncmp(array[0].c_str(), "string(", 7))
		{
			int limit = atoi(array[0].c_str() + 7);
			OptionValueEdit->Text = data->OptionValue;
			OptionValueEdit->Left = OptionValueComboBox->Left;
			OptionValueEdit->Top = OptionValueComboBox->Top;
			OptionValueEdit->Width = OptionValueComboBox->Width;
			OptionValueEdit->Height = OptionValueComboBox->Height;
			OptionValueEdit->MaxLength = limit;
			OptionValueEdit->Visible = true;
			OptionValueComboBox->Visible = false;
		}
		else  throw Exception(OptionsParseError);
		InvisibleCheckBox->Enabled = true;
		InvisibleCheckBox->Checked = data->Invisible;
		RestoreDefaultButton->Enabled = true;
	}
	else
	{
		OptionValueComboBox->Enabled = false;
		InvisibleCheckBox->Enabled = false;
		InvisibleCheckBox->Checked = false;
		RestoreDefaultButton->Enabled = false;
		OptionValueEdit->Visible = false;
		OptionValueComboBox->Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::OptionValueComboBoxChange(TObject *Sender)
{
	if(OptionsTreeView->Selected && OptionValueComboBox->ItemIndex != -1)
	{
		AnsiString opt =
			OptionValueComboBox->Items->Strings[OptionValueComboBox->ItemIndex];
		std::vector<AnsiString> array;
		SplitAnsiString(opt, " / ", array);
		if(array.size() <= 1)
			SetNodeOptionValue(OptionsTreeView->Selected, opt, opt);
		else
			SetNodeOptionValue(OptionsTreeView->Selected, array[0], array[1]);
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::OptionValueEditChange(TObject *Sender)
{
	if(OptionsTreeView->Selected)
	{
		AnsiString opt = OptionValueEdit->Text;
		SetNodeOptionValue(OptionsTreeView->Selected, opt, opt);
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::InvisibleCheckBoxClick(TObject *Sender)
{
	SetNodeInvisibleState(OptionsTreeView->Selected, InvisibleCheckBox->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::RestoreDefaultButtonClick(TObject *Sender)
{
	if(OptionsTreeView->Selected)
	{
		SetNodeDefaultOptionValue(OptionsTreeView->Selected);
		OptionsTreeViewChange(this, OptionsTreeView->Selected);
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::ReadOptionInfoFromExe()
{
	if(DescGot) return;

	if(!FileExists(SourceExe)) return;

	GetSecurityOptionFromExe(SourceExe);

	// get temporary file name
	char dir[MAX_PATH];
	dir[0] = 0;
	GetTempPath(MAX_PATH, dir);
	AnsiString eventname = AnsiString("krkrot") +
		AnsiString((int)GetCurrentProcessId());
	OptionsFileName = dir + eventname;
	DeleteFile(OptionsFileName);

	// create mutex object
	HANDLE event = CreateEvent(NULL, FALSE, FALSE, eventname.c_str());

	// execute krkr.exe to retrieve command line description
	OriginalOptionsGroupBoxLabel = OptionsGroupBox->Caption;
	OptionsGroupBox->Caption = ReadingOptionsLabel->Caption;

	if(WinExec(("\"" + SourceExe + "\" -@cmddesc " +
		AnsiString(reinterpret_cast<int>(Handle)) + " \"" +
		OptionsFileName + "\" \"" + eventname + "\"").c_str(), SW_HIDE) <= 31)
		OptionsGroupBox->Caption = OptionsReadFailedLabel->Caption;

	if(WAIT_TIMEOUT	== WaitForSingleObject(event, 10000))
	{
		CloseHandle(event);
		throw Exception("The program \"" + SourceExe + "\" did not reply!");
	}
	CloseHandle(event);

	ParseData();

	DescGot = true;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::LoadOptionsFromExe(int mode)
{
	// mode=0 : normal options
	// mode=1 : excluded options

	// ".conf" file exists?
	bool confmode = false;
	AnsiString conffilename = ChangeFileExt(SourceExe, ".tof");
	if(FileExists(conffilename))
	{
		TStream *stream = NULL;
		stream = new TFileStream(conffilename, fmOpenRead|fmShareDenyWrite);
		int size = stream->Size;
		delete stream;
		if(size != 0)
			confmode = true;
	}

	if(!confmode) conffilename = SourceExe;

	// find "XOPT_EMBED_AREA_"
	if(!confmode && !FileExists(SourceExe)) return;

	unsigned int offset = confmode?0:FindOptionAreaOffset(conffilename);

	TStream *stream = NULL;
	unsigned char *buf = NULL;
	stream = new TFileStream(conffilename, fmOpenRead|fmShareDenyWrite);

	try
	{
		if(!confmode) stream->Position = offset + 16;

		// read options area size
		unsigned int size = stream->Size;
		if(!confmode) stream->Read(&size, 4);

		// allocate buffer and read options
		if(!confmode || stream->Size > 0)
		{
			buf = new unsigned char[confmode?stream->Size:size];
			if(size) stream->Read(buf, size);

			AnsiString ops = (char*)buf;
			if(mode == 0)
				SetOptionsFromString(ops);

			ops = (char*)buf + ops.Length() + 1;
			OrgExcludeOptions = ops;
			if(mode == 1)
				SetExcludeOptionsFromString(ops);
		}
	}
	catch(...)
	{
		if(buf) delete [] buf;
		delete stream;
		throw;
	}

	delete [] buf;
	delete stream;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SaveOptionsToExe(AnsiString target)
{
	SetSecurityOptionToExe(target);

	// ".conf" file exists?
	bool confmode = false;
	AnsiString conffilename = ChangeFileExt(target, ".tof");
	if(FileExists(conffilename))
		confmode = true;
	else
		conffilename = target;

	// find "XOPT_EMBED_AREA_"
	unsigned int offset = confmode?0:FindOptionAreaOffset(conffilename);

	TStream *stream = NULL;
	stream = new TFileStream(conffilename,
		confmode ?
			(fmOpenWrite|fmShareDenyWrite):(fmOpenReadWrite|fmShareDenyWrite));

	try
	{
		if(!confmode) stream->Position = offset + 16;

		// read options area size
		unsigned int size = 0;
		if(!confmode) stream->Read(&size, 4);

		// allocate buffer and read options
		AnsiString options = GetOptionString();
		AnsiString exoptions;
		if(!ExcludeInvisibleOptions)
			exoptions = GetExcludeOptionString();
		else
			exoptions = OrgExcludeOptions;
		if(!confmode && options.Length() + 1 + exoptions.Length() + 1>= (int)size)
			throw Exception("Too large options string.");

		stream->Write(options.c_str(), options.Length()+1);
		stream->Write(exoptions.c_str(), exoptions.Length()+1);
	}
	catch(...)
	{
		delete stream;
		throw;
	}

	delete stream;
}
//---------------------------------------------------------------------------
unsigned int __fastcall TConfMainFrame::FindOptionAreaOffset(AnsiString target)
{
	static char xopt[] = " OPT_EMBED_AREA_";

	xopt[0] = 'X';

	TStream *stream = new TFileStream(target, fmOpenRead|fmShareDenyWrite);
	unsigned char *buf = NULL;

	unsigned int offset;
	bool found = false;
	try
	{
		unsigned int size = stream->Size;
		if(size > KRKR_MAX_SIZE) size = KRKR_MAX_SIZE;
		buf = new unsigned char [size];
		stream->Read(buf, size);

		// search "XOPT_EMBED_AREA_" ( aligned by 16 )
		for(offset = 0; offset < size; offset += 16)
		{
			if(buf[offset] == 'X')
			{
				if(!memcmp(buf+offset, xopt, 16))
				{
					found = true;
					break;
				}
			}
		}
	}
	catch(...)
	{
		if(buf) delete [] buf;
		delete stream;
		throw;
	}

	delete [] buf;
	delete stream;

	if(!found)
	{
		throw Exception(xopt + AnsiString(" not found in executable."));
	}

	return offset;
}
//---------------------------------------------------------------------------
int TConfMainFrame::GetSecurityOptionItem(const char *options, const char *name)
{
	size_t namelen = strlen(name);
	const char *p = strstr(options, name);
	if(!p) return 0;
	if(p[namelen] == '(' && p[namelen + 2] == ')')
		return p[namelen+1] - '0';
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::GetSecurityOptionFromExe(AnsiString exe)
{
	TStream *stream = new TFileStream(exe, fmOpenRead|fmShareDenyWrite);
	unsigned char *buf = NULL;
	size_t read_size;
	try
	{
		buf = new unsigned char [KRKR_MAX_SIZE];
		read_size = stream->Read(buf, KRKR_MAX_SIZE);

		for(int i = 0; i < read_size; i++)
		{
			if(buf[i] == '-' && buf[i+1] == '-')
			{
				if(!strncmp((const char *)(buf+i+2), " TVPSystemSecurityOptions", 25))
				{
					DisableMessageMapCheckBox->Checked =
							GetSecurityOptionItem(buf+i+25+2, "disablemsgmap") != 0;
					ForceDataXP3CheckBox->Checked =
							GetSecurityOptionItem(buf+i+25+2, "forcedataxp3") != 0;
				}
			}
		}
	}
	catch(...)
	{
		delete stream;
		delete [] buf;
		throw;
	}
	delete stream;
	delete [] buf;
}
//---------------------------------------------------------------------------
void TConfMainFrame::SetSecurityOptionItem(char *options, const char *name, int value)
{
	size_t namelen = strlen(name);
	char *p = strstr(options, name);
	if(!p) return;
	if(p[namelen] == '(' && p[namelen + 2] == ')')
		p[namelen+1] = (char)(value + '0');
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetSecurityOptionToExe(AnsiString exe)
{
	TStream *stream = new TFileStream(exe, fmOpenRead|fmShareDenyWrite);
	unsigned char *buf = NULL;
	size_t read_size;
	try
	{
		buf = new unsigned char [KRKR_MAX_SIZE];
		read_size = stream->Read(buf, KRKR_MAX_SIZE);

		for(int i = 0; i < read_size; i++)
		{
			if(buf[i] == '-' && buf[i+1] == '-')
			{
				if(!strncmp((const char *)(buf+i+2), " TVPSystemSecurityOptions", 25))
				{
					SetSecurityOptionItem(buf+i+25+2, "disablemsgmap",
						DisableMessageMapCheckBox->Checked ? 1:0);
					SetSecurityOptionItem(buf+i+25+2, "forcedataxp3",
						ForceDataXP3CheckBox->Checked ? 1:0);
				}
			}
		}
		delete stream; stream = NULL;
		stream = new TFileStream(exe, fmOpenWrite|fmShareDenyWrite);
		stream->WriteBuffer(buf, read_size);
	}
	catch(...)
	{
		delete stream;
		delete [] buf;
		throw;
	}
	delete stream;
	delete [] buf;
}
//---------------------------------------------------------------------------


