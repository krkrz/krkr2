//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef WaveReaderUnitH
#define WaveReaderUnitH

#include "tjsTypes.h"
#include "WaveContext.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>


//---------------------------------------------------------------------------
// PCM data format (internal use)
//---------------------------------------------------------------------------
struct tTVPWaveFormat
{
	tjs_uint SamplesPerSec; // sample granule per sec
	tjs_uint Channels;
	tjs_uint BitsPerSample; // per one sample
	tjs_uint BytesPerSample; // per one sample
	tjs_uint64 TotalSamples; // in sample granule; unknown for zero
	tjs_uint64 TotalTime; // in ms; unknown for zero
	tjs_uint32 SpeakerConfig; // bitwise OR of SPEAKER_* constants
	bool IsFloat; // true if the data is IEEE floating point
	bool Seekable;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPWaveDecoder interface
//---------------------------------------------------------------------------
class tTVPWaveDecoder
{
public:
	virtual ~tTVPWaveDecoder() {};

	virtual void GetFormat(tTVPWaveFormat & format) = 0;
		/* Retrieve PCM format, etc. */

	virtual bool Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered) = 0;
		/*
			Render PCM from current position.
			where "buf" is a destination buffer, "bufsamplelen" is the buffer's
			length in sample granule, "rendered" is to be an actual number of
			written sample granule.
			returns whether the decoding is to be continued.
			because "redered" can be lesser than "bufsamplelen", the player
			should not end until the returned value becomes false.
		*/

	virtual bool SetPosition(tjs_uint64 samplepos) = 0;
		/*
			Seek to "samplepos". "samplepos" must be given in unit of sample granule.
			returns whether the seeking is succeeded.
		*/
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class TWaveReader : public tTVPWaveDecoder
{
	friend class TWaveReaderThread;

public:
	TWaveReader();
	~TWaveReader();

private:
	bool FReadDone;
	int FPrevRange;

	int FNumSamples;
	int FSamplesRead;

	WAVEFORMATEXTENSIBLE Format;

	int GetChannels() { return Format.Format.nChannels; }
	int GetNumSamples(void) {return FNumSamples;}
	int GetFrequency(void) {return Format.Format.nSamplesPerSec; }
	int GetBitsPerSample(void) {return Format.Format.wBitsPerSample; }

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

	bool ReadBlock();

	void Map();

protected:

public:
	void Clear();
	void LoadWave(AnsiString filename);

	void GetPeak(int &high, int &low, int pos, int channel, int range);

	int GetSampleAt(int pos, int channel);

	int GetData(__int16 *buf, int ofs, int num);

	int SamplePosToTime(DWORD samplepos);

	__property TNotifyEvent OnReadProgress={read=FOnReadProgress, write=FOnReadProgress};

	const WAVEFORMATEXTENSIBLE * GetWindowsFormat() const { return &Format; }

	AnsiString GetChannelLabel(int ch);

	__property bool ReadDone = {read=FReadDone};
	__property int NumSamples = {read=FNumSamples};
	__property int SamplesRead = {read=FSamplesRead};
	__property int Channels = {read=GetChannels};
	__property int Frequency = {read=GetFrequency};
	__property int BitsPerSample = {read=GetBitsPerSample};

	__property AnsiString FilterString = {read=FFilterString};

public:
	// tTVPWaveDecoder stuff
	int FDecodePoint;
	void GetFormat(tTVPWaveFormat & format);
	bool Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered);
	bool SetPosition(tjs_uint64 samplepos);

	AnsiString SamplePosToTimeString(int pos); // returns HH:MM:SS.nnn string
};
//---------------------------------------------------------------------------


#endif
