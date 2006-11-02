//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Phase Vocoder Filter
//---------------------------------------------------------------------------

#ifndef PhaseVocoderFilterH
#define PhaseVocoderFilterH

#include "WaveLoopManager.h"
#include "WaveIntf.h"





class tRisaPhaseVocoderDSP;
//---------------------------------------------------------------------------
// Phase Vocoder Filter
//---------------------------------------------------------------------------
class tTVPPhaseVocoderFilter : public tTVPSampleAndLabelSource
{
	tTVPSampleAndLabelSource * Source; // source filter

	tRisaPhaseVocoderDSP * PhaseVocoder; // Phase Vocoder DSP instance
	void * FormatConvertBuffer; // buffer for converting PCM formats internally
	size_t FormatConvertBufferSize;

	tTVPWaveFormat InputFormat;
	tTVPWaveFormat OutputFormat;

public:
	tTVPPhaseVocoderFilter(tTVPSampleAndLabelSource * source);
	~tTVPPhaseVocoderFilter();

	void Decode(void *dest, tjs_uint samples, tjs_uint &written,
		std::vector<tTVPWaveLoopSegment> &segments,
		std::vector<tTVPWaveLabel> &labels); // from tTVPSampleAndLabelSource

	const tTVPWaveFormat & GetFormat() const { return OutputFormat; }
			// from tTVPSampleAndLabelSource
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNI_PhaseVocoder
//---------------------------------------------------------------------------
class tTJSNI_PhaseVocoder : public tTJSNativeInstance, public iTVPBasicWaveFilter
{
	typedef tTJSNativeInstance inherited;

public:
	tTJSNI_PhaseVocoder();
	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

private:
	tTVPPhaseVocoderFilter * Filter; // phase vocoder filter

private:
	tTVPSampleAndLabelSource * Recreate(tTVPSampleAndLabelSource * source);
		 // from iTVPBasicWaveFilter
	void Clear(void); // from iTVPBasicWaveFilter

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_PhaseVocoder
//---------------------------------------------------------------------------
class tTJSNC_PhaseVocoder : public tTJSNativeClass
{
public:
	tTJSNC_PhaseVocoder();

	static tjs_uint32 ClassID;

private:
	iTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------

#endif

