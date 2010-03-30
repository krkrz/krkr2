//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <ctype.h>
#include <vector>

#include <zlib/zlib.h>

#include "RelSettingsUnit.h"
#include "RelMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ConfMainFrameUnit"
#pragma resource "*.dfm"
TRelSettingsForm *RelSettingsForm;
//---------------------------------------------------------------------------
#define SPACER  6
__fastcall TRelSettingsForm::TRelSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
	GoImmediate = false;
	WriteDefaultRPF = true;

	FileList = new TStringList();
	ExeOptionsSheet->TabVisible = 0;

	// adjust components
	SelectFolderButton->Left = SPACER;
	SelectFolderButton->Top = ClientHeight - SPACER - SelectFolderButton->Height;
	OKButton->Top = ClientHeight - SPACER - OKButton->Height;
	OKButton->Left = ClientWidth - SPACER - CancelButton->Width - SPACER -
		OKButton->Width;
	CancelButton->Top = ClientHeight - SPACER - CancelButton->Height;
	CancelButton->Left = ClientWidth - SPACER - CancelButton->Width;
	ProgressLabel->Top = CancelButton->Top;
	ProgressLabel->Left = SPACER;
	ProgressLabel->Width = OKButton->Left - SPACER - ProgressLabel->Left;

	PageControl->Left = SPACER;
	PageControl->Top = SPACER;
	PageControl->Width = ClientWidth - SPACER*2;
	PageControl->Height = OKButton->Top - PageControl->Top- SPACER;
	LoadProfileButton->Top = PageControl->Top - 2;
	LoadProfileButton->Left = ClientWidth - LoadProfileButton->Width - SPACER;
	SaveProfileButton->Top = PageControl->Top - 2;
	SaveProfileButton->Left = LoadProfileButton->Left - SaveProfileButton->Width
		- SPACER;
	StaticText1->Top = PageControl->Top;
	StaticText1->Left = SaveProfileButton->Left - StaticText1->Width - SPACER;

	ConfMainFrame->ReadOptions(GetKrKrFileName());

	if(!XP3EncDLLAvailable) UseXP3EncDLLCheck->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::FormDestroy(TObject *Sender)
{
	if(FileList) delete FileList;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TRelSettingsForm::GetKrKrFileName()
{
	AnsiString fn = ExtractFileDir(ParamStr(0)) + "\\krkr.eXe";
	if(!FileExists(fn))
	{
		fn = ExtractFileDir(ParamStr(0)) + "\\..\\krkr.eXe";
		if(!FileExists(fn)) return "";
	}
	return fn;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::SetProjFolder(AnsiString folder)
{
	ProjFolder = folder;
	InputDirectoryMemo->Text = ProjFolder;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::SetFileList(TStringList *list)
{
	FileList->Assign(list);
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::SetExtList(TStringList *list)
{
	// distributes the list among three default lists;
	// compress, just store, discard.

	CompressExtList->Items->BeginUpdate();
	StoreExtList->Items->BeginUpdate();
	DiscardExtList->Items->BeginUpdate();

	int i;
	for(i=0; i<list->Count; i++)
	{
		AnsiString ext = list->Strings[i];
		if(
			!ext.AnsiCompareIC(".xpk") ||
			!ext.AnsiCompareIC(".xp3") ||
			!ext.AnsiCompareIC(".exe") ||
			!ext.AnsiCompareIC(".bat") ||
			!ext.AnsiCompareIC(".tmp") ||
			!ext.AnsiCompareIC(".db") ||
			!ext.AnsiCompareIC(".sue") ||
			!ext.AnsiCompareIC(".vix") ||
			!ext.AnsiCompareIC(".ico") ||
			!ext.AnsiCompareIC(".aul") ||
			!ext.AnsiCompareIC(".aue") ||
			!ext.AnsiCompareIC(".rpf") ||
			!ext.AnsiCompareIC(".bak") ||
			!ext.AnsiCompareIC(".log") ||
			!ext.AnsiCompareIC(".kep") ||
			!ext.AnsiCompareIC(".cf")  ||
			!ext.AnsiCompareIC(".cfu") ||
			ext == "" || !strncmp(ext.c_str(), ".~", 2))
		{
			if(ext == "") ext = NoExtNameLabel->Caption;
			DiscardExtList->Items->Add(ext);
		}
		else if(
			!ext.AnsiCompareIC(".wav") ||
			!ext.AnsiCompareIC(".dll") ||
			!ext.AnsiCompareIC(".tpi") ||
			!ext.AnsiCompareIC(".spi") ||
			!ext.AnsiCompareIC(".txt") ||
			!ext.AnsiCompareIC(".mid") ||
			!ext.AnsiCompareIC(".smf") ||
			!ext.AnsiCompareIC(".swf") ||
			!ext.AnsiCompareIC(".ks")  ||
			!ext.AnsiCompareIC(".tjs") ||
			!ext.AnsiCompareIC(".ma") ||
			!ext.AnsiCompareIC(".asq") ||
			!ext.AnsiCompareIC(".asd") ||
			!ext.AnsiCompareIC(".ttf") ||
			!ext.AnsiCompareIC(".ttc") ||
			!ext.AnsiCompareIC(".bmp") ||
			!ext.AnsiCompareIC(".tft") ||
			!ext.AnsiCompareIC(".cks") )
		{
			CompressExtList->Items->Add(ext);
		}
		else
		{
			StoreExtList->Items->Add(ext);
		}

	}

	CompressExtList->Items->EndUpdate();
	StoreExtList->Items->EndUpdate();
	DiscardExtList->Items->EndUpdate();

}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::LoadDefaultProfile()
{
	AnsiString n = ProjFolder + "default.rpf";
	if(FileExists(n))
	{
		LoadProfile(n);
	}
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::MoveSelected(TListBox *dest, TListBox *src)
{
	// move selected items from src to dest
	int i;
	for(i=0; i<src->Items->Count; i++)
	{
		if(src->Selected[i])
		{
			dest->Items->Add(src->Items->Strings[i]);
		}
	}

	for(i=src->Items->Count-1; i>=0; i--)
	{
		if(src->Selected[i])
		{
			src->Items->Delete(i);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::CompressExtListStartDrag(TObject *Sender,
	  TDragObject *&DragObject)
{
	if(((TListBox*)Sender)->SelCount ==0 ) CancelDrag();
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::CompressExtListDragOver(TObject *Sender,
	  TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	Accept = false;
	if(Source!=Sender && AnsiString(Source->ClassName()) == "TListBox")  Accept=true;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::CompressExtListDragDrop(TObject *Sender,
	  TObject *Source, int X, int Y)
{
	MoveSelected((TListBox*)Sender, (TListBox*)Source);
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::Button1Click(TObject *Sender)
{
	MoveSelected(CompressExtList, StoreExtList);
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::Button2Click(TObject *Sender)
{
	MoveSelected(StoreExtList, CompressExtList);
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::Button3Click(TObject *Sender)
{
	MoveSelected(StoreExtList, DiscardExtList);
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::Button4Click(TObject *Sender)
{
	MoveSelected(DiscardExtList, StoreExtList);
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::SetFileNameButtonClick(TObject *Sender)
{
	SaveDialog->FileName = OutputFileNameEdit->Text;
	if(SaveDialog->Execute())
	{
		OutputFileNameEdit->Text = SaveDialog->FileName;
		if(SaveDialog->FilterIndex == 1)
		{
			XP3ArchiveRadio->Checked = true;
			XP3ArchiveRadioClick(this);
		}
		else if(SaveDialog->FilterIndex == 2)
		{
			ExecutableRadio->Checked = true;
			ExecutableRadioClick(this);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::XP3ArchiveRadioClick(TObject *Sender)
{
	if(OutputFileNameEdit->Text != "")
	{
		OutputFileNameEdit->Text =
			ChangeFileExt(OutputFileNameEdit->Text, ".xp3");
	}
	SaveDialog->FilterIndex = 1;
	ExeOptionsSheet->TabVisible = false;
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::ExecutableRadioClick(TObject *Sender)
{
	if(GetKrKrFileName() == "")
	{
		MessageDlg(
			MissingKrKrEXELabel->Caption,
			mtError, TMsgDlgButtons() << mbOK, 0);
		XP3ArchiveRadio->Checked = true;
		return;
	}
	if(OutputFileNameEdit->Text != "")
	{
		OutputFileNameEdit->Text =
			ChangeFileExt(OutputFileNameEdit->Text, ".exe");
	}
	SaveDialog->FilterIndex =2;
	ExeOptionsSheet->TabVisible = true;
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::CompressLimitCheckClick(TObject *Sender)
{
	bool b = CompressLimitCheck->Checked;
	CompressLimitSizeEdit->Enabled = b;
	CompressLimitSizeLabel->Font->Color = b?clWindowText:clGrayText;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::CompressLimitSizeEditKeyPress(
	  TObject *Sender, char &Key)
{
	if(isprint(Key) && ( Key < '0' || Key > '9')) Key = 0;
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::CompressLimitSizeEditExit(
	  TObject *Sender)
{
	CompressLimitSizeEdit->Text = atoi(CompressLimitSizeEdit->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::SaveProfile(AnsiString name)
{
	TMemIniFile *ini = new TMemIniFile(name);
	try
	{
		SaveProfileToIni(ini);
		ini->UpdateFile();
	}
	catch(...)
	{
		delete ini;
		throw;
	}
	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::LoadProfile(AnsiString name)
{
	TMemIniFile *ini = new TMemIniFile(name);
	try
	{
		LoadProfileFromIni(ini);
	}
	catch(...)
	{
		delete ini;
		throw;
	}
	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::SaveProfileToIni(TMemIniFile *ini)
{
	ini->WriteBool("Output", "Executable", ExecutableRadio->Checked);
	ini->WriteString("Output", "OutputFileName", OutputFileNameEdit->Text);

	ini->WriteString("Extensions", "Compress", CompressExtList->Items->CommaText);
	ini->WriteString("Extensions", "Store", StoreExtList->Items->CommaText);
	ini->WriteString("Extensions", "Discard", DiscardExtList->Items->CommaText);

	ini->WriteBool("Options", "DoCompressSizeLimit", CompressLimitCheck->Checked);
	ini->WriteInteger("Options", "CompressSizeLimit",
		CompressLimitSizeEdit->Text.ToInt());
	ini->WriteBool("Options", "Protect", ProtectCheck->Checked);
	ini->WriteBool("Options", "OVBookShare", OVBookShareCheck->Checked);
	ini->WriteBool("Options", "CompressIndex", CompressIndexCheck->Checked);
	ini->WriteBool("Options", "UseXP3EncDLL", UseXP3EncDLLCheck->Checked);

	ConfMainFrame->WriteToIni(ini);
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::LoadProfileFromIni(TMemIniFile *ini)
{
	ConfMainFrame->ReadFromIni(ini);
	ConfMainFrame->ApplyReadDataToUI();

	ExecutableRadio->Checked = ini->ReadBool("Output", "Executable",
		false);
	if(ExecutableRadio->Checked)
	{
		ExecutableRadioClick(this);
	}
	else
	{
		XP3ArchiveRadio->Checked = true;
		XP3ArchiveRadioClick(this);
	}

	OutputFileNameEdit->Text = ini->ReadString("Output", "OutputFileName", "");

	LoadExtList(ini->ReadString("Extensions", "Compress", ""),
		CompressExtList->Items, StoreExtList->Items, DiscardExtList->Items);
	LoadExtList(ini->ReadString("Extensions", "Store", ""),
		StoreExtList->Items, DiscardExtList->Items, CompressExtList->Items);
	LoadExtList(ini->ReadString("Extensions", "Discard", ""),
		DiscardExtList->Items, StoreExtList->Items, CompressExtList->Items);

	CompressLimitCheck->Checked = ini->ReadBool("Options", "DoCompressSizeLimit",
		true);
	CompressLimitSizeEdit->Text = AnsiString(ini->ReadInteger("Options",
		"CompressSizeLimit", 1024));
	CompressLimitCheckClick(this);
	ProtectCheck->Checked = ini->ReadBool("Options", "Protect", false);
	OVBookShareCheck->Checked = ini->ReadBool("Options", "OVBookShare", true);
	CompressIndexCheck->Checked = ini->ReadBool("Options", "CompressIndex", true);
	UseXP3EncDLLCheck->Checked = ini->ReadBool("Options", "UseXP3EncDLL", false);

	// adjust
	if(!XP3EncDLLAvailable)
		UseXP3EncDLLCheck->Checked  = false; // xp3encdll not found
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::LoadExtList(AnsiString ext, TStrings *to,
	TStrings *f1, TStrings *f2)
{
	// split the ext into ext list
	TStringList *list = new TStringList();
	list->CommaText = ext;
	for(int i= 0; i< list->Count; i++)
	{
		AnsiString e =list->Strings[i];
		if(-1 == to->IndexOf(e))
		{
			int n;
			if(-1 != (n = f1->IndexOf(e)))
			{
				f1->Delete(n);
				to->Add(e);
			}
			else if(-1 != (n = f2->IndexOf(e)))
			{
				f2->Delete(n);
				to->Add(e);
			}
		}
	}
	delete list;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::SaveProfileButtonClick(TObject *Sender)
{
	if(SaveProfileDialog->Execute())
	{
		SaveProfile(SaveProfileDialog->FileName);
	}
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::LoadProfileButtonClick(TObject *Sender)
{
	if(OpenProfileDialog->Execute())
	{
		LoadProfile(OpenProfileDialog->FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::OKButtonClick(TObject *Sender)
{
	if(ExecutableRadio->Checked && GetKrKrFileName() == "")
	{
		MessageDlg(
			MissingKrKrEXELabel->Caption,
			mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	if(OutputFileNameEdit->Text == "")
	{
		SetFileNameButton->Click();
		if(OutputFileNameEdit->Text == "") return;
	}

	if(ExecutableRadio->Checked)
	{
		if(!ConfMainFrame->CheckOK()) return;
	}

	if(!GoImmediate)
	{
		if(FileExists(OutputFileNameEdit->Text))
		{
			int mr =
				MessageDlg(OutputFileNameEdit->Text + ConfirmOverwriteLabel->Caption,
				mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0);
				if(mr != mrYes) return;
		}
	}

	PageControl->Enabled = false;
	SaveProfileButton->Enabled = false;
	LoadProfileButton->Enabled = false;
	SelectFolderButton->Visible = false;
	OKButton->Enabled = false;

	try
	{
		_CreateArchive();
	}
	__finally
	{
		PageControl->Enabled = true;
		SaveProfileButton->Enabled = true;
		LoadProfileButton->Enabled = true;
		SelectFolderButton->Visible = true;
		OKButton->Enabled = true;
		ProgressLabel->Caption = "";
	}


	if(GoImmediate) ::PostMessage(Handle, WM_USER + 0x11, 0, 0);

}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	if(CancelButton->Caption == InterruptNameLabel->Caption)
	{
		CanClose = false;
		Aborted = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TRelSettingsForm::CancelButtonClick(TObject *Sender)
{
	if(CancelButton->Caption == InterruptNameLabel->Caption)
	{
		Aborted = true;
	}
	else
	{
		ModalResult = mrCancel;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::_CreateArchive(void)
{
	Aborted = false;
	AnsiString orgcancelcap = CancelButton->Caption;
	CancelButton->Caption = InterruptNameLabel->Caption;
	try
	{
		CreateArchive();
	}
	catch(...)
	{
		CancelButton->Caption = orgcancelcap;
		throw;
	}
	if(!GoImmediate)
	{
		MessageDlg(ExecutableRadio->Checked?
			ExecutableCreatedNameLabel->Caption:ArchiveCreatedNameLabel->Caption,
			mtInformation, TMsgDlgButtons() << mbOK, 0);
	}
	CancelButton->Caption = ExitNameLabel->Caption;
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::FormShow(TObject *Sender)
{
	if(GoImmediate) ::PostMessage(Handle, WM_USER + 0x10, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::WMStart(TMessage &msg)
{
	OKButton->Click();
}
//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::WMClose(TMessage &msg)
{
	CancelButton->Click();
}
//---------------------------------------------------------------------------
struct TFileSegment
{
	unsigned __int32 Flags;
	unsigned __int64 Offset;
	unsigned __int64 StoreSize;
	unsigned __int64 OrgSize;
};

struct TFileInfo
{
	unsigned __int32 Flags;
	unsigned __int64 FileSize;
	unsigned __int64 StoreSize;
	unsigned __int32 Adler32; // adler32 running checksum
	std::vector<TFileSegment> Segments;
	AnsiString LocalFileName;
	AnsiString FileName;
};

#define OV_MATCH_BUF_LEN 32768
#define OV_MATCH_MIN 128
#define OV_MATCH_DIF_MAX 256
#define OV_MATCH_HOLD 16

static bool DetectMatch(unsigned char *ref, int reflen, unsigned char *buf, int buflen,
	std::vector<int> &refstart, std::vector<int> &bufstart, std::vector<int> &len)
{
	if(reflen <= OV_MATCH_MIN) return false;
	if(buflen <= OV_MATCH_MIN) return false;

	bool anymatched = false;
	int bufpos;
	int refpos;
	for(bufpos = 0; bufpos<buflen-OV_MATCH_MIN;)
	{
		bool matched = false;
		int matchlen;
		int refmax = bufpos + OV_MATCH_DIF_MAX;
		if(refmax >= reflen - OV_MATCH_MIN) refmax =  reflen - OV_MATCH_MIN;
		for(refpos = (bufpos<OV_MATCH_DIF_MAX?0:(bufpos-OV_MATCH_DIF_MAX));
			refpos<refmax; refpos++)
		{
			if(buf[bufpos] == ref[refpos])
			{
				// first character matched
				int remain =
					buflen-bufpos > reflen-refpos ? reflen-refpos :  buflen-bufpos;
				unsigned char *rp = ref + refpos;
				unsigned char *bp = buf + bufpos;
				while(*rp == *bp && remain) rp++, bp++, remain--;
				matchlen = rp - (ref + refpos);

				if(matchlen >= OV_MATCH_MIN)
				{
					// matched
					refstart.push_back(refpos);
					bufstart.push_back(bufpos);
					len.push_back(matchlen);
					matched = true;
					break;
				}
			}
		}

		if(matched)
			bufpos += matchlen, anymatched = true;
		else
			bufpos++;
	}

	return anymatched;
}

struct TOVCodeBookInfo
{
	unsigned __int64 Offset[OV_MATCH_HOLD]; // offset in archive file
	int HeadersLen[OV_MATCH_HOLD]; // byte length in Headers
	unsigned char Headers[OV_MATCH_HOLD][OV_MATCH_BUF_LEN];
		// first OV_MATCH_BUF_LEN of the ogg vorbis flie
};
//---------------------------------------------------------------------------
static void Copy(tTVPLocalFileStream &dest, tTVPLocalFileStream &src,
	unsigned __int64 length)
{
	// copy stream

	if(length == 0) return;

	unsigned __int64 remain = length;
	BYTE *buf = new BYTE[1024*1024];
	try
	{
		while(remain)
		{
			int onesize =
				remain > 1*1024*1024 ? 1*1024*1024 : (int)remain;
			src.ReadBuffer(buf, onesize);
			dest.WriteBuffer(buf, onesize);
			remain -= onesize;
		}
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}
	delete [] buf;
}
//---------------------------------------------------------------------------
static unsigned __int32 GetFileCheckSum(tTVPLocalFileStream &src)
{
	// compute file checksum via adler32.
	// src's file pointer is to be reset to zero.
	unsigned __int32 adler32sum = adler32(0L, Z_NULL, 0);

	BYTE *buf = new BYTE[1024*1024];
	try
	{
		while(true)
		{
			int onesize = src.Read(buf, 1024*1024);
			if(onesize == 0) break;
			adler32sum = adler32(adler32sum, buf, onesize);
		}
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}
	delete [] buf;

	src.SetPosition(0);

	return adler32sum;
}
//---------------------------------------------------------------------------
static bool CompareFile(AnsiString file1, AnsiString file2)
{
	// compare two files;
	// returns true if all matched.
	tTVPLocalFileStream f1(file1.c_str(), TVP_ST_READ);
	tTVPLocalFileStream f2(file2.c_str(), TVP_ST_READ);

	BYTE *buf1 = NULL;
	BYTE *buf2 = NULL;
	bool matched = true;
	try
	{
		buf1 = new BYTE[1024*1024];
		buf2 = new BYTE[1024*1024];
		while(true)
		{
			int onesize1 = f1.Read(buf1, 1024*1024);
			int onesize2 = f2.Read(buf2, 1024*1024);
			if(onesize1 != onesize2)
			{
				matched = false;
				break;
			}
			if(onesize1 == 0) break;
			if(memcmp(buf1, buf2, onesize1))
			{
				matched = false;
				break;
			}
		}
	}
	catch(...)
	{
		if(buf1) delete [] buf1;
		if(buf2) delete [] buf2;
		throw;
	}
	if(buf1) delete [] buf1;
	if(buf2) delete [] buf2;

	return matched;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall TRelSettingsForm::CreateArchive(void)
{
	// setup input filter
	if(XP3EncDLLAvailable && UseXP3EncDLLCheck->Checked)
		XP3ArchiveAttractFilter_v2 = XP3ArchiveAttractFilter_v2_org;
	else
		XP3ArchiveAttractFilter_v2 = NULL;

	// information vector
	std::vector<TFileInfo> fileinfo;

	// ogg vorbis code books
	int ovcb_index = 0;
	std::auto_ptr<TOVCodeBookInfo> ovcb(new TOVCodeBookInfo);
	for(int i = 0; i< OV_MATCH_HOLD; i++)
	{
		ovcb->HeadersLen[i] = 0;
	}

	// copy executable file or write a stub
	if(ExecutableRadio->Checked)
	{
		ConfMainFrame->SetSourceAndTargetFileName(GetKrKrFileName(), OutputFileNameEdit->Text);
		ConfMainFrame->CopyExe();
	}
	else
	{
		// just touch
		TFileStream * stream = new TFileStream(OutputFileNameEdit->Text, fmCreate);
		delete stream;
	}

	// open target file
	tTVPLocalFileStream stream(OutputFileNameEdit->Text.c_str(), TVP_ST_APPEND);


	unsigned __int64 offset = stream.GetPosition();  // real xp3 header start

	unsigned __int64 compsizelimit = (!CompressLimitCheck->Checked) ?
		(__int64)-1 : CompressLimitSizeEdit->Text.ToInt() * (unsigned __int64 )1024;

	// count total files
	int totalcount = 0;
	for(int i=0; i<FileList->Count; i++)
	{

		AnsiString ext = ExtractFileExt(FileList->Strings[i]).LowerCase();
		if(ext == "") ext = NoExtNameLabel->Caption;
		if(DiscardExtList->Items->IndexOf(ext) != -1)
			continue;
		totalcount ++;
	}

	// write a header
	stream.WriteBuffer("XP3\r\n \n\x1a ", 8);
	stream.WriteBuffer("\x8b\x67\x01", 3);
	stream.WriteInt64(11+4+8); // to cushion header
	stream.WriteBuffer("\x01\x00\x00\x00", 4); // header minor version

	// cushion header
	stream.WriteBuffer("\x80", 1); // continue
	stream.WriteBuffer("\0\0\0\0\0\0\0\0", 8); // index size = 0
	unsigned __int64 index_pointer_pos = stream.GetPosition();
	stream.WriteBuffer("        ", 8); // to real header

	// write protection warning
	if(ProtectCheck->Checked)
	{
        static char dummy_png [] =
		"\x89\x50\x4e\x47\x0a\x1a\x0a\x00\x00\x00\x0d\x49\x48\x44\x52\x00"
		"\x00\x00\x01\x00\x00\x00\x01\x08\x02\x00\x00\x00\x90\x77\x53\xde"
		"\x00\x00\x00\xa5\x74\x45\x58\x74\x57\x61\x72\x6e\x69\x6e\x67\x00"
		"\x57\x61\x72\x6e\x69\x6e\x67\x3a\x20\x45\x78\x74\x72\x61\x63\x74"
		"\x69\x6e\x67\x20\x74\x68\x69\x73\x20\x61\x72\x63\x68\x69\x76\x65"
		"\x20\x6d\x61\x79\x20\x69\x6e\x66\x72\x69\x6e\x67\x65\x20\x6f\x6e"
		"\x20\x61\x75\x74\x68\x6f\x72\x27\x73\x20\x72\x69\x67\x68\x74\x73"
		"\x2e\x20\x8c\x78\x8d\x90\x20\x3a\x20\x82\xb1\x82\xcc\x83\x41\x81"
		"\x5b\x83\x4a\x83\x43\x83\x75\x82\xf0\x93\x57\x8a\x4a\x82\xb7\x82"
		"\xe9\x82\xb1\x82\xc6\x82\xc9\x82\xe6\x82\xe8\x81\x41\x82\xa0\x82"
		"\xc8\x82\xbd\x82\xcd\x92\x98\x8d\xec\x8e\xd2\x82\xcc\x8c\xa0\x97"
		"\x98\x82\xf0\x90\x4e\x8a\x51\x82\xb7\x82\xe9\x82\xa8\x82\xbb\x82"
		"\xea\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb7\x81\x42\x4b\x49\x44"
		"\x27\x00\x00\x00\x0c\x49\x44\x41\x54\x78\x9c\x63\xf8\xff\xff\x3f"
		"\x00\x05\xfe\x02\xfe\x0d\xef\x46\xb8\x00\x00\x00\x00\x49\x45\x4e"
		"\x44\xae\x42\x60\x82\x89\x50\x4e\x47\x0a\x1a\x0a\x00\x00\x00\x0d"
		"\x49\x48\x44\x52\x00\x00\x01\xef\x00\x00\x00\x13\x01\x03\x00\x00"
		"\x00\x83\x60\x17\x58\x00\x00\x00\x06\x50\x4c\x54\x45\x00\x00\x00"
		"\xff\xff\xff\xa5\xd9\x9f\xdd\x00\x00\x02\x4f\x49\x44\x41\x54\x78"
		"\xda\xd5\xd3\x31\x6b\xdb\x40\x14\x07\x70\x1d\x0a\x55\xb3\x44\x6d"
		"\xb2\xc4\x20\xac\x14\x9b\x78\x15\xf1\x12\x83\xf1\x0d\x1d\x4a\x21"
		"\x44\x1f\xa2\xa1\x5a\xd3\x78\x49\xc0\x44\x86\x14\xb2\x04\xec\xc4"
		"\x53\x40\xf8\xbe\x8a\x4c\x42\x6a\x83\xf0\x7d\x05\xb9\x15\xba\x55"
		"\xe8\x2d\x3e\x10\x7a\x3d\x25\xf9\x06\x19\x4a\x6f\x38\x74\x9c\x7e"
		"\xf7\x7f\x8f\xe3\x34\xc4\x37\x8c\x52\x7b\x8b\xfe\xe7\x3c\xe3\x8b"
		"\xd7\xef\x02\x45\x06\x99\xae\x99\x02\x11\x10\x39\xa2\x2c\x7d\x2e"
		"\x68\x3b\xf7\x53\x1f\x27\x65\x17\xd6\xba\x44\x51\xed\x31\x79\xcd"
		"\xd4\xff\xbc\xd4\x62\xa2\x78\x3c\xb0\x48\xb5\xcc\x00\xc0\xe1\x82"
		"\xc4\x7d\x89\xbc\xf1\xc2\x0f\xb5\x33\x3f\xbd\x34\xc7\x4e\x02\x12"
		"\xd4\xd9\x04\x0a\xe3\x56\xf1\xdb\x67\x9e\x6d\x0e\x6d\xc4\xb5\x2b"
		"\x15\x5f\x92\xe1\xa9\xae\xbd\x27\x80\x00\x06\xdf\xc4\x70\xd7\x20"
		"\x73\xb3\x9d\xea\x5a\xb1\xbf\x51\x24\xc3\x33\xbd\x33\x27\x10\xd2"
		"\xe5\xc6\xfa\x88\xfc\x0c\x1d\x5d\xf1\x46\x47\x15\x9e\x7b\xf7\x18"
		"\x9b\x4c\x8a\xdc\x55\xe9\xaa\xc0\x1d\x9c\xd5\x54\x7a\x9f\x73\x1a"
		"\x25\x2c\x91\xed\xe1\x87\xa6\x00\x45\x85\x00\xc6\x6e\x56\x26\xbb"
		"\x79\x4e\x2f\xbf\xaa\x3a\x15\x9f\xb0\x82\xdb\x72\x55\xf5\x6e\xcc"
		"\x70\xdd\x47\xde\xc1\xac\x77\x11\xba\x5d\x32\x9b\x6a\xb5\xf6\x66"
		"\x37\x59\xc5\x44\xa2\x31\x03\x4e\x03\xd9\xd2\x83\xb0\x6e\x56\xbd"
		"\x6b\x26\x62\xea\x4d\xc2\x6e\x64\xcb\xc7\x03\x97\x2e\xbd\x00\x25"
		"\x54\x3c\xb6\x04\xe3\xf4\x41\x5c\x09\x68\x6f\x9f\x9f\x3c\x3a\x52"
		"\xdb\xf2\x82\xec\x50\xb7\xe4\x3e\x09\x8a\x82\xbf\x5e\x9c\x48\xbd"
		"\x6b\x2c\x16\x0c\xe6\x19\xd9\x3b\xf6\x7a\x1e\xbc\xf0\x23\xc5\x0f"
		"\x35\x8f\x0d\xfb\x07\xda\x6e\x73\x9e\xeb\x58\x7a\xbd\x6f\x8c\x5a"
		"\xb0\xbf\xf1\xc2\xd7\x46\x48\x91\xa7\x1e\x43\xc9\x19\x64\xf9\x8e"
		"\xc3\x8d\x27\x57\x40\xcf\xec\xe0\x3c\xba\x9e\x44\xbd\x8e\x98\x6e"
		"\xf5\x0f\xb6\x4f\x93\x0c\x5a\x21\x35\x9e\x3e\x4f\x2f\x2d\x68\xde"
		"\x04\x71\x69\xd6\x55\x3a\xa7\x9c\x27\x82\x56\x5c\x24\xf9\x97\x3d"
		"\x57\xdc\xb9\x22\x1f\x3c\x48\x16\x2d\x1a\xad\xc5\x2e\x11\x57\xe1"
		"\x59\x7f\x6c\x15\x09\x8c\x38\x15\x77\x15\x6f\x77\xa3\x22\xa2\xcb"
		"\x63\x95\xce\x55\xba\xa3\x26\xe0\x8c\xab\x2e\x1c\xce\xc7\xbc\x95"
		"\x0f\x16\xc0\x17\xf7\x9f\x5a\x2b\xb3\x23\xd8\xf2\xdc\xbb\x1b\x14"
		"\x02\x5a\x2a\x6a\xfc\x30\xf5\x83\x66\xf7\x5e\x46\x6c\x7a\xac\x49"
		"\xa1\x8a\x8f\x2f\xea\x3e\xa6\x36\xb3\xb3\xe6\xc7\xe6\xc8\x9e\xc5"
		"\xa7\xb5\x77\xf6\x2f\xf1\x9b\x8e\xb2\x13\x9f\x08\x16\x0e\x46\x63"
		"\x6b\x9d\x39\x3f\x42\x6a\xcf\x12\x6a\x4c\xbf\x5f\x36\xfe\xac\x4a"
		"\x5a\x57\xe9\xff\xf1\x8b\x7b\x1b\xff\x0b\x28\x8d\x8d\xf8\xb3\xe9"
		"\xa1\xdf\x00\x00\x00\x00\x49\x45\x4e\x44\xae\x42\x60\x82";
		const int dummy_png_text_start = 0x30;
		const int dummy_png_text_length = 157;

		unsigned __int64 pwarnstart = stream.GetPosition() - offset;
		int size = dummy_png_text_length;

		stream.WriteBuffer( // dummy PNG header
		    dummy_png, sizeof(dummy_png));
		pwarnstart += dummy_png_text_start;

		TFileInfo info;
		info.FileName =
			ProtectWarnFileNameLabel1->Caption +
			ProtectWarnFileNameLabel1->Caption +
			ProtectWarnFileNameLabel1->Caption +
			ProtectWarnFileNameLabel2->Caption;
		info.LocalFileName = "";
		info.Flags = 0;
		info.FileSize = size;
		info.StoreSize = size;

		unsigned __int32 adler32sum = adler32(0L, Z_NULL, 0);
		adler32sum = adler32(adler32sum, dummy_png + dummy_png_text_start, size);

		info.Adler32 = adler32sum;

		TFileSegment seg;
		seg.Offset = pwarnstart;
		seg.StoreSize = size;
		seg.OrgSize = size;
		seg.Flags = 0;
		info.Segments.push_back(seg);

		fileinfo.push_back(info);
	}

	// write each flie
	int currentno = 0;
	for(int i=0; i<FileList->Count; i++)
	{

		AnsiString ext = ExtractFileExt(FileList->Strings[i]).LowerCase();
		if(ext == "") ext = NoExtNameLabel->Caption;
		if(DiscardExtList->Items->IndexOf(ext) != -1) continue;

		AnsiString storefilename = FileList->Strings[i];
		AnsiString filename = ProjFolder + storefilename;

		ProgressLabel->Caption = FileList->Strings[i] + " ("+
			AnsiString(currentno + 1) + "/" + AnsiString(totalcount) + ")" ;
//		Sleep(500);

		Application->ProcessMessages();
		if(Aborted) throw Exception(InterruptedNameLabel->Caption);

		// open input flie
		unsigned __int64 filesize;
		unsigned __int32 adler32sum;
		bool avoidstore = false;

		{
			tTVPLocalFileStream infile(filename.c_str(), TVP_ST_READ);

			{
				DWORD h;
				DWORD l = GetFileSize(infile.GetHandle(), &h);
				filesize = ((unsigned __int64)l) + (((unsigned __int64)h)<<32);
			}


			// check checksum
			adler32sum = GetFileCheckSum(infile);
			for(std::vector<TFileInfo>::iterator i = fileinfo.begin();
				i != fileinfo.end(); i++)
			{
				if(i->Adler32 == adler32sum && i->FileSize == filesize &&
					i->LocalFileName != "")
				{
					// two files may be identical
					if(CompareFile(filename, i->LocalFileName))
					{
						// files are identical
						avoidstore = true;
						TFileInfo info;
						info = *i; // copy from reference file info
						info.LocalFileName = filename;
						info.FileName = storefilename;
						fileinfo.push_back(info);
						break;
					}
				}
			}
		}

		// reopen input file
		tTVPLocalFileStream infile(filename.c_str(), TVP_ST_READ, true, adler32sum);

		if(!avoidstore)
		{
			// compress
			BYTE *outbuf;
			unsigned __int64 storesize;

			bool isov = OVBookShareCheck->Checked && ext == ".ogg"; // ogg vorbis

			if(CompressExtList->Items->IndexOf(ext) == -1 ||
				(compsizelimit != (__int64)-1 && filesize >= compsizelimit) ||
					filesize >= ((unsigned __int64)2*1024*1024*1024-1) || isov)
			{
				// avoid compress
				outbuf = NULL;
				storesize = filesize;
			}
			else
			{
				BYTE *src = new BYTE[(size_t)filesize];
				try
				{
					unsigned long compsize = 0;
					outbuf = new BYTE[compsize =
						(unsigned long)(filesize + filesize /100 + 1024)];
					infile.Read(src, (unsigned int )filesize);
					infile.Seek(0, TVP_ST_SEEK_SET);

					int res;
					unsigned long srclen = (unsigned long) filesize;
					try
					{
						// compress with zlib deflate
						res = compress2(outbuf, &compsize, src, srclen,
							Z_BEST_COMPRESSION);
						if(res != Z_OK) compsize = -1;
					}
					catch(...)
					{
						compsize = -1;
					}

					if(compsize == (unsigned long)-1)
					{
						delete [] outbuf;
						outbuf = NULL;
						storesize = filesize;
					}
					else
					{
						storesize = compsize;
					}
				}
				catch(...)
				{
					delete [] src;
					throw;
				}
				delete [] src;
			}



			// write
			try
			{
				TFileInfo info;
				info.FileSize = filesize;
				info.StoreSize = storesize;
				unsigned __int32 flags = ProtectCheck->Checked ? (1<<31)/*protected*/:0;
				info.Flags = flags;
				info.Adler32 = adler32sum;
				info.LocalFileName = filename;

				// align to 8bytes
/*
				int padding = 8 - ((int)stream.GetPosition() & 0x07);
				if(padding != 8)
				{
					stream.WriteBuffer("padding-", padding);
				}
*/

				// write
				if(outbuf)
				{
					// write the compressed image
					TFileSegment seg;
					seg.Offset = stream.GetPosition() - offset;
					seg.StoreSize = storesize;
					seg.OrgSize = filesize;
					seg.Flags = 1; // compressed segment
					info.Segments.push_back(seg);
					stream.WriteBuffer(outbuf, storesize);
					delete [] outbuf;
					outbuf = NULL;
				}
				else
				{
					// copy whole file

					// share ogg vorbis codebooks
					bool segment_detected = false;
					std::vector<int> seg_refstart;
					std::vector<int> seg_bufstart;
					std::vector<int> seg_length;
					unsigned __int64 ref_offset;
					if(isov)
					{
						// is ogg vorbis
						ovcb->HeadersLen[ovcb_index] =
							infile.Read(ovcb->Headers[ovcb_index], 
								OV_MATCH_BUF_LEN);
						ovcb->Offset[ovcb_index] = stream.GetPosition();
						infile.SetPosition(0);

						// detect match
						for(int i = 0; i<OV_MATCH_HOLD; i++)
						{
							if(i == ovcb_index) continue;
							int reflen;
							if((reflen = ovcb->HeadersLen[i]) != 0)
							{
								unsigned char *ref = ovcb->Headers[i];
								int buflen = ovcb->HeadersLen[ovcb_index];
								unsigned char *buf = ovcb->Headers[ovcb_index];

								seg_refstart.clear();
								seg_bufstart.clear();
								seg_length.clear();

								bool b = DetectMatch(ref, reflen, buf, buflen,
									seg_refstart, seg_bufstart, seg_length);
								if(b)
								{
									segment_detected = true;
									ref_offset = ovcb->Offset[i];
									break;
								}
							}
						}

						if(segment_detected)
						{
							ovcb->HeadersLen[ovcb_index] = 0;
						}
						else
						{
							// to next ...
							ovcb_index ++;
							if(ovcb_index == OV_MATCH_HOLD) ovcb_index = 0;
						}
					}

					if(segment_detected)
					{
						std::vector<int>::iterator rs, bs, l;
						TFileSegment seg;
						int pos = 0;
						rs = seg_refstart.begin();
						bs = seg_bufstart.begin();
						l = seg_length.begin();
						while(rs != seg_refstart.end())
						{
							int blen = *bs - pos;
							if(blen != 0)
							{
								seg.Flags = 0;
								seg.Offset = stream.GetPosition() - offset;
								Copy(stream, infile, blen);
								seg.OrgSize = seg.StoreSize = blen;
								info.Segments.push_back(seg);
							}

							seg.Flags = 0;
							seg.Offset = *rs + ref_offset - offset;
							seg.OrgSize = seg.StoreSize = *l;
							info.Segments.push_back(seg);
							infile.SetPosition(infile.GetPosition() + *l);

							pos = *bs + *l;

							rs++;
							bs++;
							l++;
						}

						seg.Offset = stream.GetPosition() - offset;
						unsigned __int64 in_pos = infile.GetPosition();
						seg.OrgSize = seg.StoreSize = storesize - in_pos;
						if(seg.OrgSize)
						{
							info.Segments.push_back(seg);
							Copy(stream, infile, seg.OrgSize);
						}
					}
					else
					{
						TFileSegment seg;
						seg.Flags = 0;
						seg.Offset = stream.GetPosition() - offset;
						seg.OrgSize = seg.StoreSize = storesize;

						info.Segments.push_back(seg);
						Copy(stream, infile, storesize);
					}
				}
				info.FileName = storefilename;
				fileinfo.push_back(info);
			}
			catch(...)
			{
				if(outbuf) delete [] outbuf;
				throw;
			}
		}

		currentno ++;
	}


	// compute index size
	int index_size = 0;

	std::vector<TFileInfo>::iterator it;
	for(it = fileinfo.begin(); it != fileinfo.end(); it++)
	{
		WideString str = (it->FileName);

		// for each file
		index_size += 4 + 8 + 4 + 8 + 4 + 8 + 8 + 2 + str.Length()*2 + 4 + 8 +
			4 + 8 + 4 +
			it->Segments.size()*28;
	}

	// write index pos
	{
		unsigned __int64 pos_save = stream.GetPosition();
		stream.SetPosition(index_pointer_pos);
		stream.WriteInt64(pos_save - offset);
		stream.SetPosition(pos_save);
	}

	// write index
	ProgressLabel->Caption = "Writing index ...";
	Application->ProcessMessages();
	if(Aborted) throw Exception(InterruptedNameLabel->Caption);

	unsigned char *index_buf_alloc_p = new unsigned char [index_size];
	unsigned char *index_buf = index_buf_alloc_p;
	unsigned char *index_compressed = NULL;

	try
	{

		it = fileinfo.begin();
		for(it = fileinfo.begin(); it != fileinfo.end(); it++)
		{
			WideString str = (it->FileName);

			// for each file
			memcpy(index_buf, "File", 4);
			index_buf += 4;
			*(__int64*)(index_buf) = 4 + 8 + 4 + 8 + 8 + 2 + str.Length()*2 + 4 + 8 +
				4 + 8 + 4 +	it->Segments.size()*28;
			index_buf += 8;

			// write "info"
			memcpy(index_buf, "info", 4);
			index_buf += 4;
			*(__int64*)(index_buf) = 4 + 8 + 8  + 2 + str.Length()*2;
			index_buf += 8;
			*(__int32*)(index_buf) = it->Flags;
			index_buf += 4;
			*(__int64*)(index_buf) = it->FileSize;
			index_buf += 8;
			*(__int64*)(index_buf) = it->StoreSize;
			index_buf += 8;
			*(__int16*)(index_buf) = str.Length();
			index_buf += 2;

			wchar_t *tstr = new wchar_t[str.Length() +1];
			try
			{
				// change path delimiter to '/'
				wcscpy(tstr, str.c_bstr());
				wchar_t *p = tstr;
				while(*p)
				{
					if(*p == '\\') *p = '/';
					p++;
				}
				memcpy(index_buf, tstr, str.Length() * 2);
				index_buf += str.Length() * 2;
			}
			catch(...)
			{
				delete [] tstr;
				throw;
			}
			delete [] tstr;

			// write "segm"
			memcpy(index_buf, "segm", 4);
			index_buf += 4;
			*(__int64*)(index_buf) = it->Segments.size() * 28;
			index_buf += 8;
			std::vector<TFileSegment>::iterator sit;
			for(sit = it->Segments.begin(); sit != it->Segments.end(); sit++)
			{
				*(__int32*)(index_buf) = sit->Flags;
				index_buf += 4;
				*(__int64*)(index_buf) = sit->Offset;
				index_buf += 8;
				*(__int64*)(index_buf) = sit->OrgSize;
				index_buf += 8;
				*(__int64*)(index_buf) = sit->StoreSize;
				index_buf += 8;
			}

			// write "adlr" (adler32 check sum)
			memcpy(index_buf, "adlr", 4);
			index_buf += 4;
			*(__int64*)(index_buf) = 4;
			index_buf += 8;
			*(unsigned __int32*)(index_buf) = it->Adler32;
			index_buf += 4;

		}

		unsigned long comp_size;
		if(CompressIndexCheck->Checked)
		{
			index_compressed = new unsigned char [comp_size =
				index_size + index_size/100 + 1024];

			int res;
			unsigned long srclen = (unsigned long) index_size;
			try
			{
				// compress with zlib deflate
				res = compress2(index_compressed, &comp_size, index_buf_alloc_p, srclen,
					Z_BEST_COMPRESSION);
				if(res != Z_OK) comp_size = -1;
			}
			catch(...)
			{
				comp_size = -1;
			}

			if(comp_size == (unsigned long)-1)
			{
				delete [] index_compressed;
				index_compressed = NULL;
			}
		}

		if(index_compressed)
		{
			int flag = 1;
			stream.WriteBuffer(&flag, 1);
			stream.WriteInt64(comp_size);
			stream.WriteInt64(index_size);
			stream.WriteBuffer(index_compressed, comp_size);
		}
		else
		{
			int flag = 0;
			stream.WriteBuffer(&flag, 1);
			stream.WriteInt64(index_size);
			stream.WriteBuffer(index_buf_alloc_p, index_size);
		}

	}
	catch(...)
	{
		delete [] index_buf_alloc_p;
		if(index_compressed) delete [] index_compressed;
		throw;
	}

	delete [] index_buf_alloc_p;
	if(index_compressed) delete [] index_compressed;

	// save current profile to the project folder
	if(WriteDefaultRPF)
	{
		try
		{
			SaveProfile(ProjFolder + "default.rpf");
		}
		catch(...)
		{
			// ignode errors
		}
	}
}
//---------------------------------------------------------------------------



