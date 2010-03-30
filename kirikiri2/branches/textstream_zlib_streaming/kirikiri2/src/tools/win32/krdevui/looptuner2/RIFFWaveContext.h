//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef RIFFWaveContextH
#define RIFFWaveContextH

#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "WaveContext.h"


//---------------------------------------------------------------------------
extern const GUID __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
extern const GUID __KSDATAFORMAT_SUBTYPE_PCM;
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
	int __fastcall GetTotalSamples() { return FDataSize / FGranuleSize; }
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
#endif

