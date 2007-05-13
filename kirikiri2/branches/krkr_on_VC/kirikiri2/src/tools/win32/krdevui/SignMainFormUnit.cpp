//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <clipbrd.hpp>

#define _MSC_VER
#include <tomcrypt.h>
#undef _MSC_VER

#include "SignMainFormUnit.h"
#include "RandomizeFormUnit.h"
#include "SignUnit.h"
#include "KrkrExecutableUnit.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSignMainForm *SignMainForm;
//---------------------------------------------------------------------------
__fastcall TSignMainForm::TSignMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::FormDestroy(TObject *Sender)
{
	Clipboard()->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::CloseButtonClick(TObject *Sender)
{
	Close();	
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::CopyPublicKeyButtonClick(TObject *Sender)
{
	GeneratedPublicKeyEdit->SelectAll();
	GeneratedPublicKeyEdit->CopyToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TSignMainForm::CopyPrivateKeyButtonClick(TObject *Sender)
{
	GeneratedPrivateKeyEdit->SelectAll();
	GeneratedPrivateKeyEdit->CopyToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::GenerateButtonClick(TObject *Sender)
{
	prng_state prng;
	int errnum;

	CopyPublicKeyButton->Enabled = false;
	CopyPrivateKeyButton->Enabled = false;

	// initialize random generator
	if(!RandomizePRNG(&prng)) return;

	// once process message to display window
	KeyGeneratingLabel->Visible = true;
	Application->ProcessMessages();

	// generate key
	rsa_key key;
	char buf[10240];
	char buf_asc[sizeof(buf)*3/2+2];

	errnum = rsa_make_key(&prng, find_prng("fortuna"), 128, 65537, &key);
	if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));


	try
	{
		// export keys
		unsigned long outlen, asc_outlen;

		// public key
		outlen = sizeof(buf) - 1;
		errnum = rsa_export(buf, &outlen, PK_PUBLIC, &key);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
		asc_outlen = sizeof(buf_asc) - 1;
		base64_encode(buf, outlen, buf_asc, &asc_outlen);
		buf_asc[asc_outlen] = 0;
		GeneratedPublicKeyEdit->Text =
			AnsiString("-----BEGIN PUBLIC KEY-----\r\n") + Split64(buf_asc) +
			AnsiString("-----END PUBLIC KEY-----\r\n");

		// private key
		outlen = sizeof(buf) - 1;
		errnum = rsa_export(buf, &outlen, PK_PRIVATE, &key);
		if(errnum != CRYPT_OK) throw Exception(error_to_string(errnum));
		asc_outlen = sizeof(buf_asc) - 1;
		base64_encode(buf, outlen, buf_asc, &asc_outlen);
		buf_asc[asc_outlen] = 0;
		GeneratedPrivateKeyEdit->Text =
			AnsiString("-----BEGIN RSA PRIVATE KEY-----\r\n") + Split64(buf_asc) +
			AnsiString("-----END RSA PRIVATE KEY-----\r\n");
	}
	catch(...)
	{
		rsa_free(&key);
		KeyGeneratingLabel->Visible = false;
		throw;
	}

	rsa_free(&key);

	CopyPublicKeyButton->Enabled = true;
	CopyPrivateKeyButton->Enabled = true;
	KeyGeneratingLabel->Visible = false;
}
//---------------------------------------------------------------------------



void __fastcall TSignMainForm::SignFileNameRefButtonClick(TObject *Sender)
{
	OpenDialog->FileName = SignFileNameEdit->Text;
	if(OpenDialog->Execute())
	{
		SignFileNameEdit->Text = OpenDialog->FileName;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::CheckSignFileNameRefButtonClick(TObject *Sender)
{
	OpenDialog->FileName = CheckSignFileNameEdit->Text;
	if(OpenDialog->Execute())
	{
		CheckSignFileNameEdit->Text = OpenDialog->FileName;
	}

}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::FormShow(TObject *Sender)
{
	DragAcceptFiles(Handle, true);
}
//---------------------------------------------------------------------------

void __fastcall TSignMainForm::SignButtonClick(TObject *Sender)
{
/*
//---------------------------------------------------------------------------
extern char XOPT_EMBED_AREA_[];// = " OPT_EMBED_AREA_";
extern char XCORE_SIG_______[];// = " CORE_SIG_______";
extern char XRELEASE_SIG____[];// = " RELEASE_SIG____";
//---------------------------------------------------------------------------
extern int CheckKrkrExecutable(AnsiString fn, const char *mark);
//---------------------------------------------------------------------------

*/
	bool result;

	int ignorestart = CheckKrkrExecutable(SignFileNameEdit->Text, XOPT_EMBED_AREA_);
	int signofs = CheckKrkrExecutable(SignFileNameEdit->Text, XRELEASE_SIG____);
	int xp3ofs = CheckKrkrExecutable(SignFileNameEdit->Text, XP3_SIG);

	if(ignorestart != 0 && signofs != 0)
	{
		// krkr executable
		result = SignFile(SignPrivateKeyEdit->Text, SignFileNameEdit->Text,
			ignorestart, xp3ofs?xp3ofs:-1, signofs+ 16+4);
	}
	else
	{
		if(ignorestart && !signofs)
		{
			Application->MessageBox("Warning: Signature area is not found.\n"
				"Target kirikiri executable may be too old.", Caption.c_str(),
				MB_OK|MB_ICONWARNING);
		}
		// normal file
		result = SignFile(SignPrivateKeyEdit->Text, SignFileNameEdit->Text, -1, -1, -1);
	}


	if(result)
	{
		Application->MessageBox(SignDoneLabel->Caption.c_str(), Caption.c_str(),
			MB_OK|MB_ICONINFORMATION);
	}
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::CheckSignatureButtonClick(TObject *Sender)
{

	bool result;

	int ignorestart = CheckKrkrExecutable(CheckSignFileNameEdit->Text, XOPT_EMBED_AREA_);
	int signofs = CheckKrkrExecutable(CheckSignFileNameEdit->Text, XRELEASE_SIG____);
	int xp3ofs = CheckKrkrExecutable(CheckSignFileNameEdit->Text, XP3_SIG);

	if(ignorestart != 0 && signofs != 0)
	{
		// krkr executable
		result = CheckSignatureOfFile(CheckSignPublicKeyEdit->Text,
			CheckSignFileNameEdit->Text, ignorestart, xp3ofs?xp3ofs:-1,
			signofs+ 16+4);
	}
	else
	{
		// normal file
		result = CheckSignatureOfFile(CheckSignPublicKeyEdit->Text,
			CheckSignFileNameEdit->Text, -1, -1, -1);
	}

	if(result)
	{
		Application->MessageBox(SignatureIsValidLabel->Caption.c_str(),
			Caption.c_str(), MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		Application->MessageBox(SignatureIsInvalidLabel->Caption.c_str(),
			Caption.c_str(), MB_OK|MB_ICONINFORMATION);
	}
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::SignPrivateKeyEditChange(TObject *Sender)
{
	AnsiString mark1("-----BEGIN RSA PRIVATE KEY-----");
	AnsiString mark2("-----END RSA PRIVATE KEY-----");
	bool state =
		strstr(SignPrivateKeyEdit->Text.c_str(), mark1.c_str()) &&
		strstr(SignPrivateKeyEdit->Text.c_str(), mark2.c_str()) &&
		FileExists(SignFileNameEdit->Text);
	SignButton->Enabled = state;
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::CheckSignPublicKeyEditChange(TObject *Sender)
{
	AnsiString mark1("-----BEGIN PUBLIC KEY-----");
	AnsiString mark2("-----END PUBLIC KEY-----");
	bool state =
		strstr(CheckSignPublicKeyEdit->Text.c_str(), mark1.c_str()) &&
		strstr(CheckSignPublicKeyEdit->Text.c_str(), mark2.c_str()) &&
		FileExists(CheckSignFileNameEdit->Text) /*&& FileExists(CheckSignFileNameEdit->Text + ".sig")*/;
	CheckSignatureButton->Enabled = state;
}
//---------------------------------------------------------------------------
void __fastcall TSignMainForm::WMDropFiles(TMessage &Msg)
{
	HDROP hd = (HDROP)Msg.WParam;

	char filename[ MAX_PATH ];
	int filecount=
		DragQueryFile(hd, 0xFFFFFFFF, NULL, MAX_PATH);

	for(int i = filecount-1; i>=0; i--)
	{
		DragQueryFile(hd, i, filename, MAX_PATH);

		WIN32_FIND_DATA fd;
		HANDLE h;

		// existence checking
		if((h = FindFirstFile(filename, &fd)) != INVALID_HANDLE_VALUE)
		{
			FindClose(h);

			if(PageControl->ActivePage == SignTabSheet)
				SignFileNameEdit->Text = filename;
			else if(PageControl->ActivePage == CheckSignTabSheet)
				CheckSignFileNameEdit->Text = filename;

			break;
		}

	}
	DragFinish(hd);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
static AnsiString ReadTextFile(AnsiString fn)
{
	// fn ‚ð‘S‚Ä“Ç‚ÝAAnsiString ‚Æ‚µ‚Ä•Ô‚·
	TFileStream *s = new TFileStream(fn, fmOpenRead|fmShareDenyWrite);
	AnsiString ret;
	char *buf = NULL;
	try
	{
		int size = s->Size;
		buf = new char [ size + 1];
		s->ReadBuffer(buf, size);
		buf[size] = '\0';
		ret = buf;
	}
	catch(...)
	{
		if(buf) delete [] buf;
		delete s;
		throw;
	}
	if(buf) delete [] buf;
	delete s;
	return ret;
}
//---------------------------------------------------------------------------
extern "C" void _export PASCAL UIExecSign()
{
	// check argvs
	try
	{
		int argc = ParamCount();
		if(argc >= 3 && !strcmp(ParamStr(1).c_str(), "-sign"))
		{
			AnsiString target = ParamStr(2);
			AnsiString privkey = ReadTextFile(ParamStr(3));
			AnsiString mark = XRELEASE_SIG____;
			if(argc >= 5) mark = ParamStr(4);

			int ignorestart = CheckKrkrExecutable(target, XOPT_EMBED_AREA_);
			int signofs = CheckKrkrExecutable(target, mark.c_str());
			int xp3ofs = CheckKrkrExecutable(target, XP3_SIG);

			if(ignorestart != 0 && signofs != 0)
			{
				// krkr executable
				SignFile(privkey, target,
					ignorestart, xp3ofs?xp3ofs:-1, signofs+ 16+4);
			}
			else
			{
				// normal file
				SignFile(privkey, target, -1, -1, -1);
			}
			return;
		}


		// execute
		TSignMainForm * signmainform = new TSignMainForm(Application);
		signmainform->ShowModal();
		delete signmainform;
	}
	catch(Exception &e)
	{
		MessageBox(NULL, e.Message.c_str(), "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}
//---------------------------------------------------------------------------

