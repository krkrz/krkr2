//---------------------------------------------------------------------------

#ifndef WaveReaderUnitH
#define WaveReaderUnitH

#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>

//---------------------------------------------------------------------------
class TCustomWaveContext
{
protected:
	virtual int __fastcall GetChannels() = 0;
	virtual int __fastcall GetFrequency() = 0;
	virtual DWORD __fastcall GetSpeakerConfig() = 0;

public:
	__fastcall TCustomWaveContext() { };
	virtual __fastcall ~TCustomWaveContext() { };

	virtual bool __fastcall Start(AnsiString filename) = 0;
	virtual int __fastcall Read(__int16 * dest, int destsize) = 0;

	__property int Channels = {read = GetChannels};
	__property int Frequency = {read = GetFrequency};
	__property DWORD SpeakerConfig = {read = GetSpeakerConfig};
};
//---------------------------------------------------------------------------
class TRIFFWaveContext : public TCustomWaveContext
{
	int FGranuleSize;
	int FDataSize;
	int FDataStart;

	WAVEFORMATEXTENSIBLE Format;
	bool IsFloat;

	TStream *FInputStream;

protected:
	int __fastcall GetChannels() { return Format.Format.nChannels; }
	int __fastcall GetFrequency() { return Format.Format.nSamplesPerSec; }
	DWORD __fastcall GetSpeakerConfig() { return Format.dwChannelMask; }

public:
	__fastcall TRIFFWaveContext();
	__fastcall ~TRIFFWaveContext();

	bool __fastcall Start(AnsiString filename);
	int __fastcall Read(__int16 * dest, int destgranules);
};
//---------------------------------------------------------------------------
class ITSSModule;
class ITSSStorageProvider;
class ITSSWaveDecoder;
class TTSSWaveContext : public TCustomWaveContext
{
	typedef HRESULT _stdcall (*GetModuleInstanceProc)(ITSSModule **out,
		ITSSStorageProvider *provider, IStream * config, HWND mainwin);
	typedef ULONG _stdcall (*GetModuleThreadModelProc)(void);
	typedef HRESULT _stdcall (*ShowConfigWindowProc)(HWND parentwin, IStream * storage );
	typedef ULONG _stdcall (*CanUnloadNowProc)(void);

	GetModuleInstanceProc FGetModuleInstance;
	GetModuleThreadModelProc FGetModuleThreadModel;
	ShowConfigWindowProc FShowConfigWindow;
	CanUnloadNowProc FCanUnloadNow;

	ITSSModule *FModule;
	HMODULE FHandle;
	ITSSWaveDecoder *FDecoder;


	int FGranuleSize;
	int FChannels;
	int FFrequency;
	int FBitsPerSample;
	DWORD FSpeakerConfig;

protected:
	int __fastcall GetChannels() { return FChannels; }
	int __fastcall GetFrequency() { return FFrequency; }
	int __fastcall GetBitsPerSample() { return FBitsPerSample; }
	DWORD __fastcall GetSpeakerConfig() { return FSpeakerConfig; }

public:
	__fastcall TTSSWaveContext(AnsiString dllname);
	__fastcall ~TTSSWaveContext();

	bool __fastcall Start(AnsiString filename);
	int __fastcall Read(__int16 * dest, int destgranules);
};

//---------------------------------------------------------------------------
class TWaveReader : public TObject
{
	friend class TWaveReaderThread;

public:
	__fastcall TWaveReader();
	__fastcall ~TWaveReader();

private:
	bool FReadDone;
	int FPrevRange;

	int FNumSamples;

	WAVEFORMATEXTENSIBLE Format;

	int GetChannels() { return Format.Format.nChannels; }
	int __fastcall GetNumSamples(void) {return FNumSamples;}
	int __fastcall GetFrequency(void) {return Format.Format.nSamplesPerSec; }
	int __fastcall GetBitsPerSample(void) {return Format.Format.wBitsPerSample; }

	__int16 *FPeaks;
	TStream *FTmpStream;
	AnsiString FTmpFileName;
	TCustomWaveContext *FInputContext;
	TWaveReaderThread * FReaderThread;
	TNotifyEvent FOnReadProgress;
	HANDLE FMappingFile;
	__int16 *FData;
	HANDLE FMapping;

	TStringList *FPlugins;
	AnsiString FFilterString;

	bool __fastcall ReadBlock();

	void __fastcall Map();

protected:

public:
	void __fastcall Clear();
	void __fastcall LoadWave(AnsiString filename);

	void __fastcall GetPeak(int &high, int &low, int pos, int channel, int range);

	int __fastcall GetData(__int16 *buf, int ofs, int num);

	int __fastcall SamplePosToTime(DWORD samplepos);

	__property TNotifyEvent OnReadProgress={read=FOnReadProgress, write=FOnReadProgress};

	const WAVEFORMATEXTENSIBLE * GetFormat() const { return &Format; }

	AnsiString __fastcall GetChannelLabel(int ch);

	__property bool ReadDone = {read=FReadDone};
	__property int NumSamples = {read=FNumSamples};
	__property int Channels = {read=GetChannels};
	__property int Frequency = {read=GetFrequency};
	__property int BitsPerSample = {read=GetBitsPerSample};

	__property AnsiString FilterString = {read=FFilterString};
};
//---------------------------------------------------------------------------


#endif
