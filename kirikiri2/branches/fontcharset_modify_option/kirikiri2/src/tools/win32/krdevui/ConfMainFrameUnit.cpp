//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef TVP_ENVIRON
	#include <vcl.h>
	#pragma hdrstop
#else
	#include "tjsCommHead.h"
#endif

#include <Shlobj.h>
#include <vector>
#include <stdio.h>
#include <Registry.hpp>
#include "ConfMainFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"





//---------------------------------------------------------------------------
static int HexNum(char ch)
{
	if(ch>='a' && ch<='f')return ch-'a'+10;
	if(ch>='A' && ch<='F')return ch-'A'+10;
	if(ch>='0' && ch<='9')return ch-'0';
	return -1;
}
//---------------------------------------------------------------------------
static AnsiString EncodeString(AnsiString str)
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
static AnsiString DecodeString(AnsiString str)
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





//---------------------------------------------------------------------------
TLayeredOption::TLayeredOption()
{
	Parent = NULL;
	Strings = new TStringList();
}
//---------------------------------------------------------------------------
TLayeredOption::~TLayeredOption()
{
	delete Strings;
}
//---------------------------------------------------------------------------
void TLayeredOption::Clear()
{
	Strings->Text = "";
}
//---------------------------------------------------------------------------
void TLayeredOption::FromString(const AnsiString & content)
{
	Strings->Text = content;
}
//---------------------------------------------------------------------------
void TLayeredOption::LoadFromIni(TMemIniFile * ini)
{
	Strings->Clear();
	ini->ReadSectionValues("Executable-options", Strings);
}
//---------------------------------------------------------------------------
void TLayeredOption::LoadFromFile(const AnsiString & filename)
{
	Strings->LoadFromFile(filename);

	// leap comment lines
	for(int i = Strings->Count - 1; i >= 0; i--)
	{
		if(Strings->Strings[i].c_str()[0] == ';')
		{
			Strings->Delete(i);
		}
	}
}
//---------------------------------------------------------------------------
AnsiString TLayeredOption::ToString() const
{
	// for each options, exclude option which is identical to the parent value
	AnsiString ret;
	for(int i = 0; i < Strings->Count; i++)
	{
		AnsiString key = Strings->Names[i];
		AnsiString value = DecodeString(Strings->Values[Strings->Names[i]]);
		if(Parent)
		{
			AnsiString parent_value;
			if(Parent->GetOptionValue(key, parent_value))
			{
				if(parent_value == value) continue; // exclude
			}
		}
		ret += Strings->Strings[i] + "\r\n";
	}
	return ret;
}
//---------------------------------------------------------------------------
void TLayeredOption::SaveToFile(const AnsiString & filename) const
{
	AnsiString options = ToString();
	const char * warnings =
"; ============================================================================\n"
"; *DO NOT EDIT* this file unless you are understanding what you are doing.\n"
"; FYI:\n"
";  Each line consists of NAME=\"VALUE\" pair, VALUE is a series of\n"
";  \\xNN, where NN is hexadecimal representation of UNICODE codepoint.\n"
";  For example, opt=\"\\x61\\x62\\x63\\x3042\\x3044\\x3046\" means that the\n"
";  value of options \"opt\" is alphabets a, b, and c followed by Japanese\n"
";  Hiraganas A, I, and U.\n"
";  DO NOT PUT non-escaped value like opt=\"abc\". This doesn't work and should\n"
";  be like opt=\"\\x61\\x62\\x63\".\n"
"; ============================================================================\n"
"";

	TFileStream * fs = new TFileStream(filename, fmCreate|fmShareDenyWrite);
	try
	{
		fs->Write(warnings, strlen(warnings));
		fs->Write(options.c_str(), options.Length());
	}
	catch(...)
	{
		delete fs;
		throw;
	}
	delete fs;
}
//---------------------------------------------------------------------------
void TLayeredOption::SaveToIni(TMemIniFile * ini) const
{
	try { ini->EraseSection("Executable-options"); } catch(...) { }

	TStringList * list = new TStringList();
	try
	{
		list->Text = ToString();
		for(int i = 0; i < list->Count; i++)
		{
			ini->WriteString("Executable-options", list->Names[i], list->Values[list->Names[i]]);
		}
	}
	catch(...)
	{
		delete list;
		throw;
	}
	delete list;
}
//---------------------------------------------------------------------------
bool TLayeredOption::GetOptionValue(const AnsiString & key, AnsiString & value) const
{
	// search options in this object
	for(int i = 0; i < Strings->Count; i++)
	{
		if(key == Strings->Names[i])
		{
			value = DecodeString(Strings->Values[Strings->Names[i]]);
			return true;
		}
	}

	// search parent
	if(Parent) return Parent->GetOptionValue(key, value);
	return false;
}
//---------------------------------------------------------------------------
void TLayeredOption::SetOptionValue(const AnsiString & key, const AnsiString & value)
{
	Strings->Values[key] = EncodeString(value);
}
//---------------------------------------------------------------------------
void TLayeredOption::InternalListOptionsNames(TStringList * dest) const
{
	for(int i = 0; i < Strings->Count; i++)
	{
		dest->Add(Strings->Names[i]);
	}
	if(Parent) Parent->InternalListOptionsNames(dest);
}
//---------------------------------------------------------------------------
void TLayeredOption::ListOptionNames(TStringList * dest) const
{
	// list all options names, including parent namespace
	dest->Text = "";
	dest->Sorted = true;
	dest->Duplicates = dupIgnore; // ignore duplicates, just overwrite
	InternalListOptionsNames(dest);
}
//---------------------------------------------------------------------------






TConfMainFrame *ConfMainFrame;

#define KRKR_MAX_SIZE (4*1024*1024)
	// maximum possible size of krkr.eXe executable

//---------------------------------------------------------------------------
#ifndef TVP_ENVIRON
void __fastcall ChangeIcon(AnsiString outname, AnsiString inname,
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
AnsiString ReplaceAnsiStringAll(AnsiString str, AnsiString from, AnsiString to)
{
	while(true)
	{
		int pos = str.AnsiPos(from);
		if(!pos) break;
		AnsiString left = str.SubString(1, pos - 1);
		AnsiString right = str.c_str() + pos + from.Length() - 1;
		str = left + to + right;
	}
	return str;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static AnsiString GetRegistryValue(AnsiString key, AnsiString name)
{
	AnsiString s;
	TRegistry *reg = new TRegistry();
	try
	{
		reg->RootKey = HKEY_CURRENT_USER;
		reg->OpenKey(key, false); 
		s = reg->ReadString(name);
	}
	__finally
	{
		delete reg;
	}
	return s;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Static function for retrieving special folder path
//---------------------------------------------------------------------------
static AnsiString AnsiGetSpecialFolderPath(int csidl)
{
	char path[MAX_PATH+1];
	if(!SHGetSpecialFolderPathA(NULL, path, csidl, false))
		return AnsiString();
	return AnsiString(path);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// AnsiGetPersonalPath
//---------------------------------------------------------------------------
static AnsiString AnsiGetPersonalPath()
{
	AnsiString path;
	path = AnsiGetSpecialFolderPath(CSIDL_PERSONAL);
	if(path.IsEmpty())
		path = AnsiGetSpecialFolderPath(CSIDL_APPDATA);
	
	if(path != "")
	{
		return path;
	}

	return "";
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// AnsiGetAppDataPath
//---------------------------------------------------------------------------
static AnsiString AnsiGetAppDataPath()
{
	AnsiString path = AnsiGetSpecialFolderPath(CSIDL_APPDATA);

	if(path != "")
	{
		return path;
	}

	return "";
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
	UserConfMode = false;
	InitConfigData();
}
//---------------------------------------------------------------------------
__fastcall TConfMainFrame::~TConfMainFrame()
{
	FreeOptionsData();
	FreeConfigData();
	if(ExcludeOptions) delete ExcludeOptions;
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::SetUserConfMode()
{
	IconGroupBox->Visible = false;
	ReleaseOptionGroupBox->Visible = false;
	InvisibleCheckBox->Visible = false;
	UserConfMode = true;
	InitConfigData();
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
const static char * OptionsParseError = "Options parse error";
void __fastcall TConfMainFrame::OptionsTreeViewChange(TObject *Sender,
	  TTreeNode *Node)
{
	// currently only "select" and "string(<maxlength>)" option type is supported

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
				desc = ReplaceAnsiStringAll(desc, "[cr]", "\r\n");
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
			OptionValueEdit->Anchors = OptionValueComboBox->Anchors;
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

			data->OptionName = optionname;
			data->OptionType = optiontype;

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
			DefaultConfigData.SetOptionValue(data->OptionName, data->DefaultOptionValue);

            // exclude invisible options
			if(ExcludeOptions)
			{
				if(ExcludeOptions->Values[optionname] == "i")
				{
					if(UserConfMode)
					{
						delete data;
						continue;
					}
					data->Invisible = true;
				}
			}

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
	// currently only "select" and "string(<maxlength>)" option type is supported

	std::vector<AnsiString> array;
	TOptionData * data = (TOptionData*)node->Data;
	if(data->OptionType == "") return;
	SetNodeOptionValue(node, data->DefaultOptionValueCaption, data->DefaultOptionValue);
}
//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::ReadOptionInfoFromExe(AnsiString exename)
{
	if(DescGot) return;

	if(!FileExists(exename)) return;

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

	if(WinExec(("\"" + exename + "\" -@cmddesc " +
		AnsiString(reinterpret_cast<int>(Handle)) + " \"" +
		OptionsFileName + "\" \"" + eventname + "\"").c_str(), SW_HIDE) <= 31)
		OptionsGroupBox->Caption = OptionsReadFailedLabel->Caption;

	if(WAIT_TIMEOUT	== WaitForSingleObject(event, 10000))
	{
		CloseHandle(event);
		throw Exception("The program \"" + exename + "\" did not reply!");
	}
	CloseHandle(event);

	ParseData();

	DescGot = true;
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
void __fastcall TConfMainFrame::CopyExe()
{
#ifndef TVP_ENVIRON
	// copy specified exe file to destination, applying options.
	if(ChangeIconCheck->Checked)
	{
		ChangeIcon(TargetExe, SourceExe, OpenPictureDialog->FileName);
	}
	else
	{
		TFileStream *src = NULL;
		TFileStream *dest = NULL;
		try
		{
			src = new TFileStream(SourceExe, fmOpenRead|fmShareDenyWrite);
			dest = new TFileStream(TargetExe, fmCreate|fmShareDenyWrite);
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

	WriteOptions(TargetExe);
#endif
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void TConfMainFrame::InitConfigData()
{
	if(UserConfMode)
	{
		UserConfigData.SetParent(&ConfigData);
		ConfigData.SetParent(&EmbeddedConfigData);
		EmbeddedConfigData.SetParent(&DefaultConfigData);
		TargetConfigData = &UserConfigData;
	}
	else
	{
		ConfigData.SetParent(&EmbeddedConfigData);
		EmbeddedConfigData.SetParent(&DefaultConfigData);
		TargetConfigData = &ConfigData;
	}

	if(!ExcludeOptions) ExcludeOptions = new TStringList();
}
//---------------------------------------------------------------------------
void TConfMainFrame::FreeConfigData()
{
	delete ExcludeOptions, ExcludeOptions = NULL;
}
//---------------------------------------------------------------------------
void TConfMainFrame::ApplyReadDataToUI()
{
	// apply exclude options
	if(OptionsTreeView->Items->Count)
	{
		for(int i = 0; i < ExcludeOptions->Count; i++)
		{
			AnsiString optionname = ExcludeOptions->Names[i];
			TTreeNode *node;
			node = OptionsTreeView->Items->Item[0];
			TOptionData * data;
			do
			{
				data = (TOptionData*)node->Data;

				if(data && data->OptionName == optionname)
				{
					SetNodeInvisibleState(node, true);
				}

			} while((node = node->GetNext())!=NULL);
		}
	}

	// apply option values
	TStringList * option_names = new TStringList();
	try
	{
		TargetConfigData->ListOptionNames(option_names);
		for(int i = 0; i < option_names->Count; i++)
		{
			AnsiString value;
			if(TargetConfigData->GetOptionValue(option_names->Strings[i], value))
			{
				SetOptionValue(option_names->Strings[i], value);
			}
		}
	}
	catch(...)
	{
		delete option_names;
		throw;
	}
	delete option_names;

}
//---------------------------------------------------------------------------
void TConfMainFrame::ApplyUIDataToConfigData()
{
	// apply exclude options
	ExcludeOptions->Clear();
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
					ExcludeOptions->Values[data->OptionName] = "i";
				}
			}
		} while((node = node->GetNext())!=NULL);
	}

	// apply option values
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
				TargetConfigData->SetOptionValue(data->OptionName, data->OptionValue);
			}

		} while((node = node->GetNext())!=NULL);
	}
}
//---------------------------------------------------------------------------
void TConfMainFrame::SetSourceAndTargetFileName(AnsiString source, AnsiString target)
{
	SourceExe = source;
	TargetExe = target;
}
//---------------------------------------------------------------------------
AnsiString TConfMainFrame::GetConfigFileName(AnsiString exename)
{
	return ChangeFileExt(exename, ".cf");
}
//---------------------------------------------------------------------------
AnsiString TConfMainFrame::GetDataPathDirectory(AnsiString datapath, AnsiString exename)
{
	if(datapath == "") datapath = "$(exepath)\\savedata";

	AnsiString exepath = ExcludeTrailingBackslash(ExtractFileDir(exename));
	AnsiString personalpath = ExcludeTrailingBackslash(AnsiGetPersonalPath());
	AnsiString appdatapath = ExcludeTrailingBackslash(AnsiGetAppDataPath());
	if(personalpath == "") personalpath = exepath;
	if(appdatapath == "") appdatapath = exepath;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osinfo);

	bool vista_or_later = osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
						osinfo.dwMajorVersion >= 6;

	AnsiString vistapath = vista_or_later ? appdatapath : exepath;

	datapath = ReplaceAnsiStringAll(datapath, "$(exepath)", exepath);
	datapath = ReplaceAnsiStringAll(datapath, "$(personalpath)", personalpath);
	datapath = ReplaceAnsiStringAll(datapath, "$(appdatapath)", appdatapath);
	datapath = ReplaceAnsiStringAll(datapath, "$(vistapath)", vistapath);
	return IncludeTrailingBackslash(ExpandUNCFileName(datapath));
}
//---------------------------------------------------------------------------
AnsiString TConfMainFrame::GetUserConfigFileName(AnsiString datapath, AnsiString exename)
{
	// exepath, personalpath, appdatapath
	return GetDataPathDirectory(datapath, exename) + ExtractFileName(ChangeFileExt(exename, ".cfu"));
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
AnsiString TConfMainFrame::ReadOptionSectionFromExe(AnsiString filename, int section)
{
	unsigned int offset = FindOptionAreaOffset(filename);

	TStream *stream = NULL;
	unsigned char *buf = NULL;
	stream = new TFileStream(filename, fmOpenRead|fmShareDenyWrite);

	AnsiString opts;

	try
	{
		stream->Position = offset + 16;

		// read options area size
		unsigned int size;
		stream->Read(&size, sizeof(size));

		// allocate buffer and read options
		buf = new unsigned char[size];
		if(size) stream->Read(buf, size);
		unsigned char * p = buf;

		while(true)
		{
			opts = (char*)p;
			if(section == 0) break;
			section --;
			p += opts.Length() + 1;
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

	return opts;
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteOptionSectionToExe(AnsiString filename, int section, AnsiString content)
{
	unsigned int offset = FindOptionAreaOffset(filename);

	TStream *stream = NULL;
	unsigned char *buf = NULL;
	stream = new TFileStream(filename, fmOpenRead|fmShareDenyWrite);

	AnsiString opts;

	try
	{
		stream->Position = offset + 16;

		// read options area size
		unsigned int size;
		stream->Read(&size, sizeof(size));

		// allocate buffer and read all options
		std::vector<AnsiString> array;

		buf = new unsigned char[size];
		if(size) stream->Read(buf, size);
		unsigned char * p = buf;

		for(int i = 0; i < 2; i++)  // 2 = max sections
		{
			AnsiString str = (char*)p;
			array.push_back(str);
			p += str.Length() + 1;
		}

		// replace specified section
		array[section] = content;

		// check total length
		unsigned int new_size = 0;
		for(std::vector<AnsiString>::iterator i = array.begin();
			i != array.end(); i++)
		{
			new_size += i->Length() + 1;
		}

		if(new_size > size)
			throw Exception("Too large options string.");

		// write back to the buffer
		p = buf;
		for(std::vector<AnsiString>::iterator i = array.begin();
			i != array.end(); i++)
		{
			strcpy((char*)p, i->c_str());
			p += i->Length() + 1;
		}

		// write back to the file
		delete stream, stream = NULL;
		stream = new TFileStream(filename, fmOpenWrite|fmShareDenyWrite);
		stream->Position = offset + 16 + sizeof(size);
		stream->Write(buf, new_size);

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
void TConfMainFrame::ReadOptionsFromExe(TLayeredOption * dest, AnsiString filename)
{
	AnsiString opts = ReadOptionSectionFromExe(filename, 0);
	dest->FromString(opts);
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadOptionsFromConfFile(TLayeredOption * dest, AnsiString filename)
{
	if(FileExists(filename))
		dest->LoadFromFile(filename);
	else
		dest->Clear();
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadOptionsFromIni(TLayeredOption * dest, TMemIniFile * ini)
{
	dest->LoadFromIni(ini);
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadExcludeOptionsFromString(AnsiString str)
{
	std::vector<AnsiString> array;
	SplitAnsiString(str, ",", array);
	for(unsigned int i = 0; i <array.size(); i++)
	{
		ExcludeOptions->Values[array[i]] = "i";
	}
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadExcludeOptionsFromExe(AnsiString filename)
{
	ReadExcludeOptionsFromString(ReadOptionSectionFromExe(filename, 1));
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadExcludeOptionsFromIni(TMemIniFile * ini)
{
	ReadExcludeOptionsFromString(ini->ReadString("Executable-exclude-options", "ExcludeOptions", ""));
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
void TConfMainFrame::ReadSecurityOptionsFromExe(AnsiString filename)
{
	TStream *stream = new TFileStream(filename, fmOpenRead|fmShareDenyWrite);
	unsigned char *buf = NULL;
	size_t read_size;
	try
	{
		buf = new unsigned char [KRKR_MAX_SIZE];
		read_size = stream->Read(buf, KRKR_MAX_SIZE);

		for(unsigned int i = 0; i < read_size; i++)
		{
			if(buf[i] == '-' && buf[i+1] == '-')
			{
				if(!strncmp((const char *)(buf+i+2), " TVPSystemSecurityOptions", 25))
				{
					DisableMessageMapCheckBox->Checked =
							GetSecurityOptionItem((char*)(buf+i+25+2), "disablemsgmap") != 0;
					ForceDataXP3CheckBox->Checked =
							GetSecurityOptionItem((char*)(buf+i+25+2), "forcedataxp3") != 0;
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
void TConfMainFrame::ReadSecurityOptionsFromIni(TMemIniFile * ini)
{
	DisableMessageMapCheckBox->Checked =
		ini->ReadBool("Executable-security-options", "DisableMessageMap", false);
	ForceDataXP3CheckBox->Checked =
		ini->ReadBool("Executable-security-options", "ForceDataXP3", false);
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadOptions(AnsiString exename)
{
	ReadExcludeOptionsFromExe(exename);
	ReadOptionsFromExe(&EmbeddedConfigData, exename);
	ReadOptionInfoFromExe(exename);

	if(UserConfMode)
	{
		ReadOptionsFromConfFile(&ConfigData, GetConfigFileName(exename));
		AnsiString datapath;
		if(!ConfigData.GetOptionValue("datapath", datapath))
			datapath = "";
		ReadOptionsFromConfFile(&UserConfigData, GetUserConfigFileName(datapath, exename));
	}
	else
	{
#ifndef TVP_ENVIRON
		ReadOptionsFromConfFile(&ConfigData, GetConfigFileName(exename));
		ReadSecurityOptionsFromExe(exename);
		UserConfigData.Clear();
#endif
	}
}
//---------------------------------------------------------------------------
void TConfMainFrame::ReadFromIni(TMemIniFile *ini)
{
	ReadOptionsFromIni(TargetConfigData, ini);
	ReadExcludeOptionsFromIni(ini);
	ReadSecurityOptionsFromIni(ini);

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
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteOptionsToExe(const TLayeredOption * src, AnsiString filename)
{
	WriteOptionSectionToExe(filename, 0, src->ToString());
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteOptionsToConfFile(const TLayeredOption * src, AnsiString filename)
{
	src->SaveToFile(filename);
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteOptionsToIni(const TLayeredOption * src, TMemIniFile * ini)
{
	src->SaveToIni(ini);
}
//---------------------------------------------------------------------------
AnsiString TConfMainFrame::ConvertExcludeOptionsToString()
{
	AnsiString str;
	for(int i = 0; i < ExcludeOptions->Count; i++)
	{
		if(ExcludeOptions->Values[ExcludeOptions->Names[i]] == "i")
		{
			if(str != "") str += ",";
			str += ExcludeOptions->Names[i];
		}
	}
	return str;
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteExcludeOptionsToExe(AnsiString filename)
{
	// join with ','
	WriteOptionSectionToExe(filename, 1, ConvertExcludeOptionsToString());
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteExcludeOptionsToIni(TMemIniFile * ini)
{
	ini->WriteString("Executable-exclude-options", "ExcludeOptions", ConvertExcludeOptionsToString());
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
void TConfMainFrame::WriteSecurityOptionsToExe(AnsiString filename)
{
	TStream *stream = new TFileStream(filename, fmOpenRead|fmShareDenyWrite);
	unsigned char *buf = NULL;
	size_t read_size;
	try
	{
		buf = new unsigned char [KRKR_MAX_SIZE];
		read_size = stream->Read(buf, KRKR_MAX_SIZE);

		for(unsigned int i = 0; i < read_size; i++)
		{
			if(buf[i] == '-' && buf[i+1] == '-')
			{
				if(!strncmp((const char *)(buf+i+2), " TVPSystemSecurityOptions", 25))
				{
					SetSecurityOptionItem((char*)(buf+i+25+2), "disablemsgmap",
						DisableMessageMapCheckBox->Checked ? 1:0);
					SetSecurityOptionItem((char*)(buf+i+25+2), "forcedataxp3",
						ForceDataXP3CheckBox->Checked ? 1:0);
				}
			}
		}
		delete stream; stream = NULL;
		try
		{
			stream = new TFileStream(filename, fmOpenWrite|fmShareDenyWrite);
			stream->WriteBuffer(buf, read_size);
		}
		catch(...)
		{
			// ignore open errors;
			// this may occur if the config is to be stored in tof file
			// (we should open the file if exe file embedding are enebled, because
			//  the exe file write permittion checking is already done)
			// anyway we can not embed the security options into
			// tof file
			delete [] buf;
			delete stream; stream = NULL;
			return;
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
void TConfMainFrame::WriteSecurityOptionsToIni(TMemIniFile * ini)
{
	ini->WriteBool("Executable-security-options", "DisableMessageMap", DisableMessageMapCheckBox->Checked);
	ini->WriteBool("Executable-security-options", "ForceDataXP3", ForceDataXP3CheckBox->Checked);
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteOptions(AnsiString exename)
{
	ApplyUIDataToConfigData();
	if(UserConfMode)
	{
		AnsiString datapath;
		if(!ConfigData.GetOptionValue("datapath", datapath))
			datapath = "";
		WriteOptionsToConfFile(&UserConfigData, GetUserConfigFileName(datapath, exename));
	}
	else
	{
#ifndef TVP_ENVIRON
		WriteOptionsToConfFile(&ConfigData, GetConfigFileName(exename));
		WriteExcludeOptionsToExe(exename);
		WriteSecurityOptionsToExe(exename);
		if(ChangeIconCheck->Checked)
			ChangeIcon(exename, exename, OpenPictureDialog->FileName);
#endif
	}
}
//---------------------------------------------------------------------------
void TConfMainFrame::WriteToIni(TMemIniFile *ini)
{
	WriteOptionsToIni(TargetConfigData, ini);
	WriteExcludeOptionsToIni(ini);
	WriteSecurityOptionsToIni(ini);

	ini->WriteBool("Executable", "ChangeIcon", ChangeIconCheck->Checked);
	ini->WriteString("Executable", "Icon",  OpenPictureDialog->FileName);
}
//---------------------------------------------------------------------------

