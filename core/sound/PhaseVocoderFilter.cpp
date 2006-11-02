//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Phase Vocoder Filter
//---------------------------------------------------------------------------

#include "tjsCommHead.h"
#include "PhaseVocoderDSP.h"
#include "PhaseVocoderFilter.h"


//---------------------------------------------------------------------------
tTVPPhaseVocoderFilter::tTVPPhaseVocoderFilter(tTVPSampleAndLabelSource * source)
{
	Source = source;
	FormatConvertBuffer = NULL;
	FormatConvertBufferSize = 0;

	// output format を設定
	InputFormat = source->GetFormat();
	OutputFormat = InputFormat;
	OutputFormat.IsFloat = true; // 出力は float
	OutputFormat.BitsPerSample = 32; // ビットは 32 ビット
	OutputFormat.BytesPerSample = 4;

	// PhaseVocoder を作成
	PhaseVocoder = new tRisaPhaseVocoderDSP(4096, 16,
				InputFormat.SamplesPerSec, InputFormat.Channels);
	PhaseVocoder->SetTimeScale(1.5);
}
//---------------------------------------------------------------------------
tTVPPhaseVocoderFilter::~tTVPPhaseVocoderFilter()
{
	if(PhaseVocoder) delete PhaseVocoder, PhaseVocoder = NULL;
	if(FormatConvertBuffer) delete [] FormatConvertBuffer, FormatConvertBuffer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPPhaseVocoderFilter::Decode(void *dest, tjs_uint samples, tjs_uint &written,
	std::vector<tTVPWaveLoopSegment> &segments,
	std::vector<tTVPWaveLabel> &labels)
{
	float * dest_buf = (float*) dest;
	written = 0;
	while(samples > 0)
	{
		size_t inputfree = PhaseVocoder->GetInputFreeSize();
		if(inputfree > 0)
		{
			// 入力にデータを流し込む
			float *p1, *p2;
			size_t p1len, p2len;
			PhaseVocoder->GetInputBuffer(inputfree, p1, p1len, p2, p2len);
			tjs_uint filled = 0;
			tjs_uint total = 0;
			Source->Decode       (p1, p1len, filled, segments, labels), total += filled;
			if(p2) Source->Decode(p2, p2len, filled, segments, labels), total += filled;
			if(total == 0) {written = 0; return ; } // もうデータがない
		}

		// PhaseVocoderの処理を行う
		PhaseVocoder->Process();

		// 入力にデータを流し込んでおいて出力が無いことはないが
		// 要求したサイズよりも小さい場合はある
		size_t output_ready = PhaseVocoder->GetOutputReadySize();
		if(output_ready > 0)
		{
			// PhaseVocoder の出力から dest にコピーする
			if(output_ready > samples) output_ready = samples;
			const float *p1, *p2;
			size_t p1len, p2len;
			PhaseVocoder->GetOutputBuffer(output_ready, p1, p1len, p2, p2len);
			memcpy(dest_buf, p1, p1len * sizeof(float)*OutputFormat.Channels);
			if(p2) memcpy(dest_buf + p1len * OutputFormat.Channels, p2,
							p2len * sizeof(float)*OutputFormat.Channels);

			samples  -= output_ready;
			written  += output_ready;
			dest_buf += output_ready * OutputFormat.Channels;
		}
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// tTJSNC_PhaseVocoder : PhaseVocoder TJS native class
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_PhaseVocoder::ClassID = (tjs_uint32)-1;
tTJSNC_PhaseVocoder::tTJSNC_PhaseVocoder() :
	tTJSNativeClass(TJS_W("PhaseVocoder"))
{
	// register native methods/properties

	TJS_BEGIN_NATIVE_MEMBERS(PhaseVocoder)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
// constructor/methods
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var.name*/_this, /*var.type*/tTJSNI_PhaseVocoder,
	/*TJS class name*/PhaseVocoder)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/PhaseVocoder)
//----------------------------------------------------------------------
#if 0
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/loadScenario)
{
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PhaseVocoder);
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	_this->LoadScenario(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/loadScenario)
#endif
//----------------------------------------------------------------------

//---------------------------------------------------------------------------




//----------------------------------------------------------------------
// properties
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(interface)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PhaseVocoder);
		*result = reinterpret_cast<tjs_int64>((iTVPBasicWaveFilter*)_this);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(interface)
//---------------------------------------------------------------------------

//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
iTJSNativeInstance *tTJSNC_PhaseVocoder::CreateNativeInstance()
{
	return new tTJSNI_PhaseVocoder();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tTJSNI_PhaseVocoder::tTJSNI_PhaseVocoder()
{
	Filter = NULL;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNI_PhaseVocoder::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_PhaseVocoder::Invalidate()
{
	if(Filter) delete Filter, Filter = NULL;
}
//---------------------------------------------------------------------------
tTVPSampleAndLabelSource * tTJSNI_PhaseVocoder::Recreate(tTVPSampleAndLabelSource * source)
{
	if(Filter) delete Filter, Filter = NULL;

	Filter = new tTVPPhaseVocoderFilter(source);
	return Filter;
}
//---------------------------------------------------------------------------
void tTJSNI_PhaseVocoder::Clear(void)
{
	if(Filter) delete Filter, Filter = NULL;
}
//---------------------------------------------------------------------------



