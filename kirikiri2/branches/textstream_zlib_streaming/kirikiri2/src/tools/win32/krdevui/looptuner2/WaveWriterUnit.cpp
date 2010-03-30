//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <mmsystem.h>
#include "WaveReaderUnit.h"

#include "WaveWriterUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWaveWriterForm *WaveWriterForm;
//---------------------------------------------------------------------------
__fastcall TWaveWriterForm::TWaveWriterForm(TComponent* Owner)
	: TForm(Owner)
{
	CloseLock = false;
}
//---------------------------------------------------------------------------
#define ONE_SIZE 65536
void __fastcall TWaveWriterForm::WriteWaveData(TWaveReader *reader, int start, int length,
		AnsiString filename)
{
	UserBreak = false;
	CloseLock = true;
	Label->Caption = ExtractFileName(filename) +" に出力中 ...";
	TStream *out;

	try
	{
		 out = new TFileStream(filename, fmCreate|fmShareDenyWrite);
	}
	catch(Exception &e)
	{
		MessageDlg("出力ファイルに書き込むことができません", mtError,
				TMsgDlgButtons() << mbOK, 0);
  		CloseLock = false;
		ModalResult = mrCancel;
		return;
	}


	// ヘッダを書く
	try
	{
		DWORD size;
		out->Write("RIFF", 4);
		size = length * reader->Channels * 2 + 44; // 44= ヘッダサイズ
		out->Write(&size, 4);
		out->Write("WAVEfmt \x10\x00\x00\x00", 12); // WAVEfmt
		WAVEFORMATEX wfe;
		wfe.wFormatTag = (WORD)WAVE_FORMAT_PCM;
		wfe.nChannels = (WORD)reader->Channels;
		wfe.nSamplesPerSec = reader->Frequency;
		wfe.nAvgBytesPerSec = reader->Frequency * reader->Channels * 2;
		wfe.nBlockAlign = (WORD)reader->Channels * 2;
		wfe.wBitsPerSample = (WORD)reader->BitsPerSample;
		wfe.cbSize = 0;  // ただし、このメンバは記録されない

		out->Write(&wfe, 16);

		out->Write("data", 4);
		size = length * reader->Channels * 2;
		out->Write(&size, 4);


		int total = length;
		int current = 0;

		while(length)
		{
			ProgressBar->Position = (__int64)current *50L / (__int64)total;

			Application->ProcessMessages();
			if(UserBreak)
			{
				delete out;
				DeleteFile(filename);
				CloseLock = false;
				ModalResult = mrCancel;
				return;
			}

			int onesize = ONE_SIZE > length  ? length : ONE_SIZE;
			short int *buf = new short int [onesize * reader->Channels ];

			reader->GetData(buf, start, onesize);

			if(onesize > out->Write(buf, onesize * reader->Channels * 2))
			{
				MessageDlg("出力ファイルに書き込むことができません", mtError,
					TMsgDlgButtons() << mbOK, 0);
				delete [] buf;
				delete out;
				DeleteFile(filename);
				CloseLock = false;
				ModalResult = mrCancel;
				return;
			}

			delete [] buf;
			start += onesize;
			current += onesize;
			length -= onesize;

		}

		delete out;
		CloseLock = false;
		ModalResult = mrOk;
		return;
	}
	catch(...)
	{
		MessageDlg("ファイル出力中にエラーが発生しました", mtError,
			TMsgDlgButtons() << mbOK, 0);
		CloseLock = false;
		ModalResult = mrCancel;
		return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TWaveWriterForm::CancelButtonClick(TObject *Sender)
{
	UserBreak = true;
}
//---------------------------------------------------------------------------

void __fastcall TWaveWriterForm::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
	if(CloseLock)
	{
		UserBreak = true;
		CanClose = false;
	}
	else
	{
		CanClose = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TWaveWriterForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree; // 自動的に解放されるので、呼び出し側での解放は不要	
}
//---------------------------------------------------------------------------

void __fastcall TWaveWriterForm::TimerTimer(TObject *Sender)
{
	Timer->Enabled = false;
	CancelButton->Enabled = true;
	WriteWaveData(WaveReader, Start, Length, FileName);
}
//---------------------------------------------------------------------------

void __fastcall TWaveWriterForm::FormDestroy(TObject *Sender)
{
	//
}
//---------------------------------------------------------------------------

