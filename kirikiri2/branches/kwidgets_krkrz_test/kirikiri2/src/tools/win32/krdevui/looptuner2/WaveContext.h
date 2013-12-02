//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef WaveContextH
#define WaveContextH

#include <vcl.h>

//---------------------------------------------------------------------------
class TCustomWaveContext
{
protected:
	virtual int __fastcall GetChannels() = 0;
	virtual int __fastcall GetFrequency() = 0;
	virtual int __fastcall GetTotalSamples() = 0;
	virtual DWORD __fastcall GetSpeakerConfig() = 0;

public:
	__fastcall TCustomWaveContext() { };
	virtual __fastcall ~TCustomWaveContext() { };

	virtual bool __fastcall Start(AnsiString filename) = 0;
	virtual int __fastcall Read(__int16 * dest, int destsize) = 0;

	__property int Channels = {read = GetChannels};
	__property int Frequency = {read = GetFrequency};
	__property DWORD SpeakerConfig = {read = GetSpeakerConfig};
	__property int TotalSamples = {read = GetTotalSamples};
};
//---------------------------------------------------------------------------


#endif
