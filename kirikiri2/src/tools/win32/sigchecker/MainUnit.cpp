//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <inifiles.hpp>
#include <clipbrd.hpp>
#include <stdio.h>


#include "MainUnit.h"

#if 0
// テスト用に使っているキーペア
-- PUBLIC KEY - ECC(160) --hgACAAEUFAAAABkiVqMkINmoCYhe41wzleMIqElUAA==
-- PRIVATE KEY - ECC(160) --hgACAAAUFAAAABkiVqMkINmoCYhe41wzleMIqElUABQAAADZsyrLo1RKsgt17saXG+pDfVSDfw==
#endif

#include "KrkrSignatureUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	// 変数初期化
	Checking = false;
	Aborting = false;

	// ini ファイルを読み込む
	TMemIniFile * memini = new TMemIniFile(ChangeFileExt(ParamStr(0), ".ini"));
	try
	{
		// キャプションを読み込む
		Caption = memini->ReadString("message", "caption",
				"File signature checker");
		Application->Title = Caption;

		// 公開鍵を読み込む
		PublicKey = memini->ReadString("key", "publickey", "");

		// notice の読み込みとコンポーネントの位置調整
		AnsiString notice =
			memini->ReadString("message", "notice",
				"Please press the check button below.");
		while(true)
		{
			int pos = notice.AnsiPos("[cr]");
			if(!pos) break;
			AnsiString left = notice.SubString(1, pos - 1);
			AnsiString right = notice.c_str() + pos + 4 - 1;
			notice = left + "\r\n" + right;
		}
		AnsiString pnotice = notice;

		Canvas->Font->Assign(NoticeMemo->Font);
		RECT r = {0, 0, NoticeMemo->Width - 4, NoticeMemo->Height - 4};
		DrawText(Canvas->Handle, pnotice.c_str(), -1, &r,
			DT_CALCRECT|DT_EDITCONTROL|DT_WORDBREAK);

		int ystep = r.bottom + 10 - NoticeMemo->Height;
		NoticeMemo->Height += ystep;
		NoticeMemo->Text = notice;
		ListView->Top += ystep;
		ListView->Height -= ystep;

	}
	catch(...)
	{
		delete memini;
		throw;
	}
	delete memini;

	// ファイルのリストを作成
	BaseDir = IncludeTrailingBackslash(ExtractFilePath(ParamStr(0)));
	ListView->Items->BeginUpdate();
	try
	{
		MakeFileList(BaseDir, BaseDir.Length());
	}
	catch(...)
	{
		ListView->Items->EndUpdate();
		throw;
	}
	ListView->Items->EndUpdate();

	// ファイル名にあわせて列幅を決定
	int maxwidth;
	Canvas->Font->Assign(ListView->Font);
	maxwidth = Canvas->TextWidth(ListView->Column[0]->Caption);
	for(int i = 0; i < ListView->Items->Count; i++)
	{
		int width = Canvas->TextWidth(ListView->Items->Item[i]->Caption);
		if(width > maxwidth) maxwidth = width;

		// 「未チェック」を状態欄に追加
		ListView->Items->Item[i]->SubItems->Add(NotYetCheckedLabel->Caption);
	}
	ListView->Column[0]->Width = maxwidth + 12;

	// 状態欄に表示される項目に合わせて列幅を決定
	maxwidth = Canvas->TextWidth(ListView->Column[0]->Caption);
	int width;
	width = Canvas->TextWidth(NotYetCheckedLabel->Caption);
	if(width > maxwidth) maxwidth = width;
	width = Canvas->TextWidth(CheckingLabel->Caption);
	if(width > maxwidth) maxwidth = width;
	width = Canvas->TextWidth(BrokenLabel->Caption);
	if(width > maxwidth) maxwidth = width;
	width = Canvas->TextWidth(NotBrokenLabel->Caption);
	if(width > maxwidth) maxwidth = width;
	ListView->Column[1]->Width = maxwidth + 20;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CloseButtonClick(TObject *Sender)
{
	if(Checking)
		Aborting = true;
	else
		Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CheckButtonClick(TObject *Sender)
{
	// 署名をチェックする
	Checking = true;
	CheckButton->Enabled = false;
	AnsiString orgcheckbuttoncaption = CheckButton->Caption;
	CheckButton->Caption = CheckingButtonLabel->Caption;

	bool all_ok = true;
	try
	{
		for(int i = 0; i < ListView->Items->Count; i++)
		{

			AnsiString filename = BaseDir + ListView->Items->Item[i]->Caption;
			ListView->Items->Item[i]->MakeVisible(false);
			CurrentCheckingItem = ListView->Items->Item[i];
			bool res;
			try
			{
				res = CheckSignature(filename, PublicKey, SigProgressNotify);
			}
			catch(...)
			{
				res = false;
			}
			all_ok = all_ok && res;
			if(res)
				ListView->Items->Item[i]->SubItems->Strings[0] = NotBrokenLabel->Caption;
			else
				ListView->Items->Item[i]->SubItems->Strings[0] = BrokenLabel->Caption;

			if(Aborting) break;
		}
	}
	catch(...)
	{
		Checking = false;
		CheckButton->Caption = orgcheckbuttoncaption;
		CheckButton->Enabled = true;
		throw;
	}
	Checking = false;
	CheckButton->Caption = orgcheckbuttoncaption;
	CheckButton->Enabled = true;

	MoreThanOneFileIsBrokenLabel->Visible = !all_ok;

	CopyResultButton->Enabled = true;

	if(Aborting) Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if(!Checking)
	{
		CanClose = true;
		return;
	}
	CanClose = false;
	Aborting = true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SigProgressNotify(int percent)
{
	// 署名チェック中の進行状態に合わせて呼ばれる
	DWORD curtick = GetTickCount();
	if(curtick - LastDisplayedTick < 100) return !Aborting;
	LastDisplayedTick = curtick;

	AnsiString str;
	str.sprintf(CheckingLabel->Caption.c_str(), percent);
	CurrentCheckingItem->SubItems->Strings[0] = str;

	Application->ProcessMessages();

	return !Aborting;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CopyResultButtonClick(TObject *Sender)
{
	// 結果をクリップボードにコピーする
	AnsiString str = TDateTime(Now()).DateTimeString() + "\r\n\r\n";

	for(int i = 0; i < ListView->Items->Count; i++)
	{
		AnsiString attr = "\t\t\t";
		AnsiString filename = BaseDir + ListView->Items->Item[i]->Caption;
		HANDLE hf = CreateFile(filename.c_str(), GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hf != INVALID_HANDLE_VALUE)
		{
			FILETIME time;
			ZeroMemory(&time, sizeof(time));
			GetFileTime(hf, NULL, NULL, &time);
			SYSTEMTIME st;
			ZeroMemory(&st, sizeof(st));
			FileTimeToSystemTime(&time, &st);
			char tmp[256];
			sprintf(tmp, "\t%d/%02d/%02d %02d:%02d:%02d",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);


			attr = tmp;
			DWORD sizelow;
			DWORD sizehigh;
			sizelow = GetFileSize(hf, &sizehigh);
			sprintf(tmp, "%Ld", (__int64)((__int64)(sizelow) +
				((__int64)sizehigh << 32)));
			attr += "\t" + AnsiString(tmp) + "\t";

			CloseHandle(hf);
		}


		str += filename + attr +
			ListView->Items->Item[i]->SubItems->Strings[0] + "\r\n";
	}

	Clipboard()->AsText = str;

	Application->MessageBox(TheResultWasCopiedIntoClipboardLabel->Caption.c_str(),
		Caption.c_str(), MB_OK|MB_ICONINFORMATION);

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MakeFileList(AnsiString path, int baselen)
{
	// ファイルのリストを作成する
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

					AnsiString fileext = ExtractFileExt(r.FindData.cFileName);
					bool add;
					if(!fileext.AnsiCompareIC(".sig"))
					{
						// 拡張子が sig は署名ファイルなので追加しない
						add = false;
					}
					else if(!fileext.AnsiCompareIC(".exe") &&
						CheckExeHasSignature(path + r.FindData.cFileName))
					{
						// EXEファイルかつ署名つきのファイルの場合は追加する
						add = true;
					}
					else
					{
						// それ以外の場合は、.sig ファイルが存在すれば追加する
						add = FileExists(path + r.FindData.cFileName + ".sig");
					}

					if(add)
					{
						TListItem * item = ListView->Items->Add();
						item->Caption = (path + r.FindData.cFileName).c_str() +
							baselen;
					}
				}
				else
				{
					// a directory
					if(r.Name != "." && r.Name != "..")
					{
						MakeFileList(path  + r.FindData.cFileName+
							AnsiString("\\"), baselen);
					}
				}
			}
			done=FindNext(r);
		}
	}
	catch(...)
	{
		FindClose(r);
		throw;
	}
	FindClose(r);
}
//---------------------------------------------------------------------------

