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
#include "WaveReader.h"
#include "TSSWaveContext.h"
#include "RIFFWaveContext.h"

#include "tvpsnd.h"

//---------------------------------------------------------------------------
class TWaveReaderThread : public TThread
{
	TWaveReader * FOwner;
public:
	__fastcall TWaveReaderThread(TWaveReader *owner) : TThread(true)
	{
		FOwner = owner;
		Suspended = false;
	}

	__fastcall ~TWaveReaderThread(void)
	{
		Suspended = false;
		Terminate();
		WaitFor();
	}

	void __fastcall SyncMethod(void)
	{
		if(FOwner->FOnReadProgress)
		{
			FOwner->FOnReadProgress(NULL);
		}
	}

	void __fastcall Execute(void)
	{
		Sleep(100); // sleep a while; this will arrow other threads to do unprocessed events.
		DWORD tickstart = GetTickCount();
		while(!Terminated)
		{
			if(!FOwner->ReadBlock())
			{
				Synchronize(SyncMethod);
				Terminate();
			}
			else if(GetTickCount() - tickstart >= 100)
			{
				Synchronize(SyncMethod);
				tickstart = GetTickCount();
			}
		}
		Synchronize(SyncMethod);
	}
} ;
//---------------------------------------------------------------------------
static void FindPlugins(AnsiString path, AnsiString ext, TStringList *dest)
{
	TSearchRec sr;
	int done;
	done = FindFirst(path + "*." + ext,
		faReadOnly | faHidden | faSysFile | faArchive, sr);
	if(!done)
	{
		do
		{
			// 試しに DLL をロードしてみる
			AnsiString filename = path + sr.FindData.cFileName;
			HMODULE dll = LoadLibrary(filename.c_str());
			if(dll)
			{
				// 必要な関数を持っているか
				if(GetProcAddress(dll, "GetModuleInstance"))
				{
					dest->Add(filename);
				}
				FreeLibrary(dll);
			}

			done = FindNext(sr);
		} while(!done);
		FindClose(sr);
	}
}
//---------------------------------------------------------------------------
TWaveReader::TWaveReader()
{
	FReadDone = false;
	FPrevRange = 0;
	FNumSamples = 0;
	FSamplesRead = 0;

	ZeroMemory(&Format, sizeof(Format));

	FPeaks = NULL;
	FTmpStream = NULL;
	FInputContext = NULL;
	FReaderThread = NULL;
	FOnReadProgress = NULL;
	FMappingFile = NULL;
	FData = NULL;
	FMapping = NULL;

	FPlugins = NULL;
	FFilterString = NULL;

	FDecodePoint = NULL;

	FTmpFileName = ExtractFileDir(ParamStr(0)) + "\\temp.dat";
	FPlugins = new TStringList();

	// 使用可能なプラグインを検索する
	AnsiString exepath = ExtractFilePath(ParamStr(0));
	FindPlugins(exepath + "plugin\\", "dll", FPlugins);
	FindPlugins(exepath+  "plugin\\", "tpm", FPlugins);
	FindPlugins(exepath + "..\\plugin\\", "dll", FPlugins);
	FindPlugins(exepath + "..\\plugin\\", "tpm", FPlugins);


	// 使用可能な拡張子とそのファイルタイプを取得する

	TStringList *exts = new TStringList;
	TStringList *descs = new TStringList;
	typedef HRESULT _stdcall (*GetModuleInstanceProc)(ITSSModule **out,
			ITSSStorageProvider *provider, IStream * config, HWND mainwin);

	try
	{
		exts->Add(".wav");
		descs->Add("Wave ファイル");

		for(int i = 0; i < FPlugins->Count; i++)
		{
			HMODULE lib = LoadLibrary(FPlugins->Strings[i].c_str());
			if(lib)
			{
				ITSSModule * mod;
				GetModuleInstanceProc proc = (GetModuleInstanceProc)
					GetProcAddress(lib, "GetModuleInstance");
				if(proc)
				{
					HRESULT hr =
						proc(&mod, &StorageProvider, NULL,
							Application->Handle);
					if(SUCCEEDED(hr))
					{
						int idx = 0;
						for(;;)
						{
							wchar_t shortname[33];
							wchar_t ext[256];
							hr = mod->GetSupportExts(idx, shortname,
														ext, 256);
							if(hr != S_OK) break;
							if(wcslen(ext) >= 1)
							{
								exts->Add(ext);
								descs->Add(shortname);
							}
							idx++;
						}
						mod->Release();
					}
				}
				else
				{
					MessageDlg(AnsiString("このプラグインは扱えません : ") +
						FPlugins->Strings[i].c_str(), mtWarning, TMsgDlgButtons() << mbOK , 0);
				}
				FreeLibrary(lib);
			}
		}
	}
	catch(Exception &e)
	{
		delete exts;
		delete descs;
		throw Exception(e);
	}

	AnsiString ret;
	ret = "すべての形式 (";
	int i;
	for(i=0; i<exts->Count; i++)
	{
		if(i) ret+=";";
		ret+= "*" + exts->Strings[i];
	}
	ret += ")|";
	for(i=0; i<exts->Count; i++)
	{
		if(i) ret+=";";
		ret+= "*" + exts->Strings[i];
	}

	for(i=0; i<exts->Count; i++)
	{
		ret+="|";
		ret+= descs->Strings[i];
		ret+= " (*"+ exts->Strings[i] +")|";
		ret+= "*" + exts->Strings[i];
	}

	ret+="|すべてのファイル (*.*)|*.*";

	FFilterString = ret;
}
//---------------------------------------------------------------------------
TWaveReader::~TWaveReader()
{
	Clear();
	DeleteFile(FTmpFileName);
	delete FPlugins;
}
//---------------------------------------------------------------------------
void TWaveReader::Clear()
{
	if(FReaderThread) delete FReaderThread, FReaderThread = NULL;
	if(FPeaks) SysFreeMem(FPeaks), FPeaks = NULL;
	if(FData) UnmapViewOfFile(FData), FData = NULL;
	if(FMapping) CloseHandle(FMapping), FMapping = NULL;
	if(FMappingFile) CloseHandle(FMappingFile), FMappingFile = NULL;
	if(FTmpStream) delete FTmpStream, FTmpStream = NULL;
	if(FInputContext) delete FInputContext, FInputContext = NULL;
	FReadDone = false;
	FNumSamples = 0;
	FSamplesRead = 0;
	FPrevRange = 0;
}
//---------------------------------------------------------------------------
bool TWaveReader::ReadBlock()
{
	if(FReadDone) return false;
	__int16 *buf = new __int16[32768 * Format.Format.nChannels];
	int written = FInputContext->Read(buf, 32768);
	FSamplesRead += written;
	FTmpStream->Write(buf, written * Format.Format.nChannels * sizeof(__int16));
	delete [] buf;
	if(written==0)
	{
		// 読み込み終わり
		Map();
		FReadDone = true;
	}

	return written!=0;
}
//---------------------------------------------------------------------------
void TWaveReader::Map()
{
	if(!FData)
	{
		if(FTmpStream) delete FTmpStream , FTmpStream = NULL;

		FNumSamples = FSamplesRead;

		FMappingFile =
			CreateFile(FTmpFileName.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

		if(!FMappingFile)
		{
			return;
		}

		FNumSamples = GetFileSize(FMappingFile, NULL) / (Format.Format.nChannels*2);

		FMapping =
			CreateFileMapping(FMappingFile,
					NULL,
					PAGE_READONLY,
					0,
					0,
					("WaveReadMapping"+AnsiString(GetTickCount())).c_str());

		if(!FMapping)
		{
			CloseHandle(FMappingFile), FMappingFile = NULL;
			return;
		}

		FData = (__int16 *)MapViewOfFile(FMapping, FILE_MAP_READ, 0, 0, 0);

		// ピークを探す
		#define PEAK_RANGE 128

		if(FPeaks) SysFreeMem(FPeaks); // ピークキャッシュ

		int alloc;
		alloc = (FNumSamples / PEAK_RANGE + 2) * sizeof(__int16)*2 * Format.Format.nChannels;
		FPeaks = (__int16*)
			SysGetMem(alloc);

		alloc /=2;

		int pos;
		int idx = 0;
		for(pos=0; pos<FNumSamples + PEAK_RANGE; pos+=PEAK_RANGE)
		{
			int channel;
			for(channel = 0; channel< Format.Format.nChannels; channel++)
			{
				int high, low;
				int i;
				i = pos;
				int lim = i + PEAK_RANGE;
				if(i >= FNumSamples) i = FNumSamples-1;
				if(lim >= FNumSamples) lim = FNumSamples;
				high = low = FData[i*Format.Format.nChannels + channel];
				i++;
				while(i<lim)
				{
					int n = FData[i*Format.Format.nChannels + channel];
					if(low > n) low = n;
					if(high < n) high = n;
					i++;
				}
				FPeaks[idx++] = low;
				FPeaks[idx++] = high;
				if(idx > alloc)
				{
					return;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void TWaveReader::LoadWave(AnsiString filename)
{
	Clear();

	FTmpStream = new TFileStream(FTmpFileName, fmCreate|fmShareDenyWrite);

	// 使用可能なのを探す

	// 標準 RIFF wave reader で試す
	bool res;
	FInputContext = new TRIFFWaveContext();
	try
	{
		res = FInputContext->Start(filename);
	}
	catch(...)
	{
		res = false;
	}

	if(!res)
	{
		delete FInputContext;
		FInputContext = NULL;

		// プラグインから読み込み可能なモノを探す
		for(int i = 0; i < FPlugins->Count; i++)
		{
			FInputContext = new TTSSWaveContext(FPlugins->Strings[i]);

			try
			{
				res = FInputContext->Start(filename);
			}
			catch(...)
			{
				res = false;
			}
			if(res) break;
			delete FInputContext, FInputContext = NULL;
		}
	}

	if(!FInputContext)
	{
		delete FTmpStream;
		FTmpStream = NULL;
		throw Exception(filename + " : この形式のファイルは読み込めないか、ファイルが異常です");
	}

	FNumSamples = FInputContext->TotalSamples;
	if(FNumSamples >= (0x80000000ul/8))
	{
		throw Exception(filename + " は 大きすぎるため、扱うことができません");
	}

	// Format に格納
	// 16bit への変換はinputcontextが行うため、Format には 16bit を指定する
	DWORD speakerconfig = FInputContext->SpeakerConfig;
	int channels = FInputContext->Channels;
	int frequency = FInputContext->Frequency;

	Format.Format.nChannels = channels;
	Format.Format.nSamplesPerSec = frequency;
	Format.Format.wBitsPerSample = 16;
	Format.SubFormat = __KSDATAFORMAT_SUBTYPE_PCM;
	Format.Samples.wValidBitsPerSample = 16;

	if(speakerconfig == 0 && channels <= 2)
	{
		// 普通の WAVE_FORMAT_PCM
		Format.Format.wFormatTag = WAVE_FORMAT_PCM;
		Format.Format.cbSize = 0;
	}
	else if(speakerconfig == 0)
	{
		// channels が 4 あるいは 6 の場合
		if(channels == 4)
		{
			// 左前、右前、左後、右後であると仮定する
			Format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			Format.dwChannelMask =
				SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT |
				SPEAKER_BACK_RIGHT;
			Format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		}
		else if(channels == 6)
		{
			// 5.1 チャンネル用データであると仮定する
			Format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			Format.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			Format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
		}
		else
		{
			throw Exception(filename + " : 対応できないチャンネル数です");
		}
	}
	else
	{
		// speakerconfig がある
		Format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		Format.dwChannelMask = speakerconfig;
		Format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	}

	Format.Format.nBlockAlign =
		Format.Format.wBitsPerSample * Format.Format.nChannels / 8;
	Format.Format.nAvgBytesPerSec =
		Format.Format.nSamplesPerSec * Format.Format.nBlockAlign;

	FReaderThread = new TWaveReaderThread(this);

	// この関数は読み込みの終了を待たずに戻ります
}
//---------------------------------------------------------------------------
void TWaveReader::GetPeak(int &high, int &low, int pos, int channel, int range)
{
	if(!FReadDone) return;

	if(range < PEAK_RANGE)
	{
		int i;
		i = pos;
		int lim = i + range;
		if(lim >= FNumSamples) lim = FNumSamples;
		high = low = FData[i * Format.Format.nChannels + channel];
		i++;
		while(i<lim)
		{
			int n = FData[i * Format.Format.nChannels + channel];
			if(low > n) low = n;
			if(high < n) high = n;
			i++;
		}
	}
	else
	{

		int i;
		i = pos / PEAK_RANGE;
		int lim = i + range / PEAK_RANGE;
		if(i >= FNumSamples / PEAK_RANGE) i = FNumSamples / PEAK_RANGE -1;
		if(lim >= FNumSamples / PEAK_RANGE) lim = FNumSamples / PEAK_RANGE -1;
		low = FPeaks[i*Format.Format.nChannels*2 + channel*2];
		high = FPeaks[i*Format.Format.nChannels*2 + channel*2 + 1];
		i++;
		while(i<lim)
		{
			int nlow = FPeaks[i*Format.Format.nChannels*2 + channel*2];
			int nhigh = FPeaks[i*Format.Format.nChannels*2 + channel*2 + 1];
			if(low > nlow) low = nlow;
			if(high < nhigh) high = nhigh;
			i++;
		}
	}
}
//---------------------------------------------------------------------------
int TWaveReader::GetSampleAt(int pos, int channel)
{
	if(pos < 0) return 0;
	if(pos >= FNumSamples) return 0; // out of the range
	if(channel < 0) return 0;
	if(channel >= Format.Format.nChannels) return 0;
	return FData[pos * Format.Format.nChannels + channel];
}
//---------------------------------------------------------------------------
int TWaveReader::GetData(__int16 *buf, int ofs, int num)
{
	if(!FReadDone) return 0;

	if(ofs >= FNumSamples) return 0;
	if(ofs + num >= FNumSamples) num = FNumSamples - ofs;
	if(num <=0 ) return 0;

	memcpy(buf, FData + ofs * Format.Format.nChannels, num * Format.Format.nChannels * sizeof(__int16));

	return num;
}
//---------------------------------------------------------------------------
int TWaveReader::SamplePosToTime(DWORD samplepos)
{
	if(!FReadDone) return -1;

	return (int)((__int64)samplepos*1000L / (__int64)Format.Format.nSamplesPerSec);
}
//---------------------------------------------------------------------------
AnsiString TWaveReader::GetChannelLabel(int ch)
{
	static AnsiString chlabels[] =
	{"FL", "FR", "FC", "LF", "BL", "BR", "FLC", "FRC", "BC", "SL", "SR", "TC",
		"TFL", "TFC", "TFR", "TBL", "TBC", "TBR"};

	if(Format.Format.wFormatTag == WAVE_FORMAT_PCM)
	{
		if(Format.Format.nChannels == 1)
		{
			if(ch == 0) return "Mono";
		}
		else if(Format.Format.nChannels == 2)
		{
			if(ch == 0) return "L";
			else if(ch == 1) return "R";
		}
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		DWORD mask = Format.dwChannelMask;
		DWORD m = 1;
		int index = 0;
		int nmasks = 0;
		int count = ch;
		if(!(mask & 0x80000000))
		{
			while(m)
			{
				if(mask & m)
				{
					if(count == 0)
					{
						if(index >= sizeof(chlabels) / sizeof(chlabels[0])) break;
						return chlabels[index];
					}
					count --;
					nmasks ++;
				}
				m <<= 1;
				index ++;
			}
		}
		return AnsiString(ch - count);
	}

	return "-";
}
//---------------------------------------------------------------------------
void TWaveReader::GetFormat(tTVPWaveFormat & format)
{
	format.SamplesPerSec = Format.Format.nSamplesPerSec;
	format.Channels = Format.Format.nChannels;
	format.BitsPerSample = Format.Samples.wValidBitsPerSample;
	format.BytesPerSample = Format.Format.wBitsPerSample / 8;
	format.TotalSamples = FNumSamples;
	format.TotalTime = SamplePosToTime(FNumSamples);
	format.SpeakerConfig = Format.dwChannelMask;
	format.IsFloat = false; // always 16bit integer
	format.Seekable = true;
}
//---------------------------------------------------------------------------
bool TWaveReader::Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered)
{
	// write bufsamplelen samples to buf from current FDecodePoint
	if(FDecodePoint >= FNumSamples)
	{
		// already finished
		rendered = 0;
		return false;
	}

	tjs_uint remain = FNumSamples - FDecodePoint;
	rendered = bufsamplelen < remain ? bufsamplelen : remain;
	memcpy(buf, FData + FDecodePoint * Format.Format.nChannels,
		sizeof(FData[0]) * Format.Format.nChannels * rendered);
	FDecodePoint += rendered;
	return FDecodePoint < FNumSamples;
}
//---------------------------------------------------------------------------
bool TWaveReader::SetPosition(tjs_uint64 samplepos)
{
	// set FDecodePoint
	if(samplepos >= 0x80000000ui64) return false;
	if(samplepos >= FNumSamples) return false;
	FDecodePoint = (int) samplepos;
	return true;
}
//---------------------------------------------------------------------------
AnsiString TWaveReader::SamplePosToTimeString(int pos)
{
	// returns HH:MM:SS.nnn string
	int t = SamplePosToTime(pos);

	AnsiString ret;

	ret.sprintf("%02d:%02d:%02d.%03d",
						t / (1000 * 60 * 60),
						t / (1000 * 60) % 60,
						t / 1000 % 60,
						t % 1000);

	return ret;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
