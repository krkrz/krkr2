//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Wave Player implementation
//---------------------------------------------------------------------------
#ifndef WaveImplH
#define WaveImplH

#define DIRECTSOUND_VERSION 0x0300

#include <mmsystem.h>
#include <dsound.h>
#include <ks.h>
#include <ksmedia.h>

#include "WaveIntf.h"

/*[*/
//---------------------------------------------------------------------------
// IDirectSound former declaration
//---------------------------------------------------------------------------
#ifndef __DSOUND_INCLUDED__
struct IDirectSound;
#endif



/*]*/

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------

#define TVP_WSB_ACCESS_FREQ (8)  // wave sound buffer access frequency (hz)

//---------------------------------------------------------------------------

TJS_EXP_FUNC_DEF(void, TVPReleaseDirectSound, ());
TJS_EXP_FUNC_DEF(IDirectSound *, TVPGetDirectSound, ());
extern void TVPResetVolumeToAllSoundBuffer();
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNI_WaveSoundBuffer : Wave Native Instance
//---------------------------------------------------------------------------
class tTVPWaveSoundBufferDecodeThread;
class tTJSNI_WaveSoundBuffer : public tTJSNI_BaseWaveSoundBuffer
{
	typedef  tTJSNI_BaseWaveSoundBuffer inherited;

public:
	tTJSNI_WaveSoundBuffer();
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

	//-- buffer management ------------------------------------------------
private:
	LPDIRECTSOUNDBUFFER SoundBuffer;

	void ThrowSoundBufferException(const ttstr &reason);
	void TryCreateSoundBuffer();
	void CreateSoundBuffer();
	void DestroySoundBuffer();
	void ResetSoundBuffer();
	void ResetSamplePositions();

	WAVEFORMATEXTENSIBLE Format;

	tTVPWaveFormat C_InputFormat;
	tTVPWaveFormat InputFormat;

	tjs_int BufferBytes;
	tjs_int AccessUnitBytes;
	tjs_int AccessUnitSamples;
	tjs_int L2AccessUnitBytes;

	tjs_int L2BufferUnits;
	tjs_int L1BufferUnits;

	tjs_int Level2BufferSize;
	tjs_uint8 *Level2Buffer;
public:
	void FreeDirectSoundBuffer(bool disableevent = true)
	{
		// called at exit ( system uninitialization )
		bool b = CanDeliverEvents;
		if(disableevent)
			CanDeliverEvents = false; // temporarily disables event derivering
		Stop();
		DestroySoundBuffer();
		CanDeliverEvents = b;
	}


	//-- playing stuff ----------------------------------------------------
private:
	tTJSCriticalSection BufferCS;
	tTJSCriticalSection L2BufferCS;
	tTJSCriticalSection L2BufferRemainCS;

public:
	tTJSCriticalSection & GetBufferCS() { return BufferCS; }

private:
	tTVPWaveDecoder * Decoder;
	tTVPWaveSoundBufferDecodeThread * Thread;
public:
	bool ThreadCallbackEnabled;
private:
	tjs_uint64 LoopStart;
	tjs_uint64 LoopLength;
	tjs_uint64 LoopEnd;


	bool BufferPlaying; // whether this sound buffer is playing
	bool DSBufferPlaying; // whether the DS buffer is 'actually' playing
	bool Paused;

	bool UseVisBuffer;

	tjs_uint64 CurrentPos; // current decoding ( not playing ) position in samples

	tjs_int SoundBufferPrevReadPos;
	tjs_int SoundBufferWritePos;
	tjs_int PlayStopPos; // in bytes

	tjs_int L2BufferReadPos; // in unit
	tjs_int L2BufferWritePos;
	tjs_int L2BufferRemain;
	bool L2BufferEnded;
	tjs_uint8 *VisBuffer; // buffer for visualization
	tjs_int *L2BufferDecodedSamplesInUnit;
	tjs_uint64 *L2BufferSamplePositions;
	tjs_uint64 *L1BufferSamplePositions;

	bool Looping;

	void Clear();

	tjs_uint _Decode(void *buffer, tjs_uint bufsamplelen);
	tjs_uint Decode(void *buffer, tjs_uint bufsamplelen);

public:
	bool FillL2Buffer(bool firstwrite, bool fromdecodethread);

private:
	void PrepareToReadL2Buffer(bool firstread);
	tjs_uint ReadL2Buffer(void *buffer, tjs_uint64 & pcmpos);

	void FillDSBuffer(tjs_int writepos, tjs_uint64 * pcmpos);
public:
	bool FillBuffer(bool firstwrite = false, bool allowpause = true);


private:
	void StartPlay();
	void StopPlay();

public:
	void Play();
	void Stop();

	bool GetPaused() const { return Paused; }
	void SetPaused(bool b);

	tjs_int GetFrequency() const { return Format.Format.nSamplesPerSec; }
	tjs_int GetBitsPerSample() const {
		if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
			return Format.Samples.wValidBitsPerSample;
		else
			return Format.Format.wBitsPerSample; }
	tjs_int GetChannels() const { return Format.Format.nChannels; }

protected:
	void TimerBeatHandler(); // override

public:
	void Open(const ttstr & storagename);

public:
	void SetLooping(bool b) { Looping = b; }
	bool GetLooping() const { return Looping; }

    tjs_uint64 GetPosition();
	void SetPosition(tjs_uint64 pos);

	tjs_uint64 GetTotalTime();

	//-- volume/pan/3D position stuff -------------------------------------
private:
	tjs_int Volume;
	tjs_int Volume2;
	static tjs_int GlobalVolume;
	static tTVPSoundGlobalFocusMode GlobalFocusMode;

	bool Use3D;
	bool BufferCanControlPan;
	tjs_int Pan; // -100000 .. 0 .. 100000

public:
	void SetVolumeToSoundBuffer();

public:
	void SetVolume(tjs_int v);
	tjs_int GetVolume() const { return Volume; }
	void SetVolume2(tjs_int v);
	tjs_int GetVolume2() const { return Volume2; }
	void SetPan(tjs_int v);
	tjs_int GetPan() const { return Pan; }
	static void SetGlobalVolume(tjs_int v);
	static tjs_int GetGlobalVolume() { return GlobalVolume; }
	static void SetGlobalFocusMode(tTVPSoundGlobalFocusMode b);
	static tTVPSoundGlobalFocusMode GetGlobalFocusMode()
		{ return GlobalFocusMode; }

	//-- visualization stuff ----------------------------------------------
public:
	void SetUseVisBuffer(bool b);
	bool GetUseVisBuffer() const { return UseVisBuffer; }

protected:
	void ResetVisBuffer(); // reset or recreate visualication buffer
	void DeallocateVisBuffer();

	void CopyVisBuffer(tjs_int16 *dest, const tjs_uint8 *src,
		tjs_int numsamples, tjs_int channels);
public:
	tjs_int GetVisBuffer(tjs_int16 *dest, tjs_int numsamples, tjs_int channels,
		tjs_int aheadsamples);
};
//---------------------------------------------------------------------------

#endif
