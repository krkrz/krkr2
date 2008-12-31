//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Phase Vocoder �̎���
//---------------------------------------------------------------------------

/*
	Phase Vocoder (�t�F�[�Y �{�R�[�_ ; �ʑ��{�R�[�_)�̎���

	�Q�l����:

		http://www.panix.com/~jens/pvoc-dolson.par
			Phase Vocoder �̃`���[�g���A���B�u�~���[�W�V�����ɂ��킩��悤�Ɂv
			������Ă���A���w���s�t�����h���[�B

		http://www.dspdimension.com/
			����(�I�[�v���\�[�X�ł͂Ȃ�)�� Time Stretcher/Pitch Shifter��
			DIRAC��A�e��A���S���Y���̐����A
			Pitch Shifter �̐����I�ȃ\�[�X�R�[�h�ȂǁB

		http://soundlab.cs.princeton.edu/software/rt_pvc/
			real-time phase vocoder analysis/synthesis library + visualization
			�\�[�X����B
*/

#include "tjsCommHead.h"

#include <math.h>
#include "PhaseVocoderDSP.h"
#include <string.h>

#include "tjsUtils.h"

#include "tvpgl_ia32_intf.h"
extern tjs_uint32 TVPCPUType;

extern "C"
{
#ifdef _WIN32
#define TVP_CDECL __cdecl
#endif
void TVP_CDECL sse__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi(tRisaPhaseVocoderDSP *_this, int ch);
void TVP_CDECL def__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi(tRisaPhaseVocoderDSP *_this, int ch);
void TVP_CDECL sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj(float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len);
void TVP_CDECL def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj(float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len);
void TVP_CDECL sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj(float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len);
void TVP_CDECL def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj(float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len);
}

//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::tRisaPhaseVocoderDSP(
				unsigned int framesize,
				unsigned int frequency, unsigned int channels) :
					InputBuffer(framesize * 4 * channels),
					OutputBuffer(framesize * 4 * channels)
		// InputBuffer �͍Œ�ł�
		// channels * (framesize + (framesize/oversamp)) �K�v�ŁA
		// OutputBuffer �͍Œ�ł�
		// channels * (framesize + (framesize/oversamp)*MAX_TIME_SCALE) �K�v
{
	// �t�B�[���h�̏���
	FFTWorkIp = NULL;
	FFTWorkW = NULL;
	InputWindow = NULL;
	OutputWindow = NULL;
	AnalWork = NULL;
	SynthWork = NULL;
	LastAnalPhase = NULL;
	LastSynthPhase = NULL;

	FrameSize = framesize;
	OverSampling = 8;
	Frequency = frequency;
	Channels = channels;
	InputHopSize = OutputHopSize = FrameSize / OverSampling;

	TimeScale = 1.0;
	FrequencyScale = 1.0;
	RebuildParams = true; // �K������Ƀp�����[�^���č\�z����悤�ɐ^

	LastSynthPhaseAdjustCounter = 0;

	try
	{
		// ���[�N�Ȃǂ̊m��
		AnalWork  = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		SynthWork = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			AnalWork[ch] = NULL, SynthWork[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			AnalWork[ch]  = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize), 4);
			SynthWork[ch] = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize), 4);
		}

		LastAnalPhase = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastAnalPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastAnalPhase[ch] = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
			memset(LastAnalPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 �ŃN���A
		}

		LastSynthPhase = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastSynthPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastSynthPhase[ch] = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
			memset(LastSynthPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 �ŃN���A
		}

		FFTWorkIp = (int *)TJSAlignedAlloc(sizeof(int) * (static_cast<int>(2+sqrt((double)FrameSize/4))), 4);
		FFTWorkIp[0] = FFTWorkIp[1] = 0;
		FFTWorkW = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
		InputWindow = (float *)TJSAlignedAlloc(sizeof(float) * FrameSize, 4);
		OutputWindow = (float *)TJSAlignedAlloc(sizeof(float) * FrameSize, 4);
	}
	catch(...)
	{
		Clear();
		throw;
	}

	// ��o�̓o�b�t�@�̓��e���N���A
	float *bufp1;
	size_t buflen1;
	float *bufp2;
	size_t buflen2;

	InputBuffer.GetWritePointer(InputBuffer.GetSize(),
							bufp1, buflen1, bufp2, buflen2);
	if(bufp1) memset(bufp1, 0, sizeof(float)*buflen1);
	if(bufp2) memset(bufp2, 0, sizeof(float)*buflen2);

	OutputBuffer.GetWritePointer(OutputBuffer.GetSize(),
							bufp1, buflen1, bufp2, buflen2);
	if(bufp1) memset(bufp1, 0, sizeof(float)*buflen1);
	if(bufp2) memset(bufp2, 0, sizeof(float)*buflen2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::~tRisaPhaseVocoderDSP()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetTimeScale(float v)
{
	if(TimeScale != v)
	{
		TimeScale = v;
		RebuildParams = true;
		InputHopSize = OutputHopSize = FrameSize / OverSampling;
		OutputHopSize = static_cast<unsigned int>(InputHopSize * TimeScale) & ~1;
			// �� �����ɃA���C��(�d�v)
			// ���f�� re,im, re,im, ... �̔z�񂪋tFFT�ɂ�蓯����(���f���̌��~2��)
			// PCM�T���v���ɕϊ�����邽�߁APCM�T���v�����Q���ň���Ȃ��ƂȂ�Ȃ�.
			// ���̎��ۂ� OutputHopSize �ɏ]���� ExactTimeScale ���v�Z�����.
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetFrequencyScale(float v)
{
	if(FrequencyScale != v)
	{
		FrequencyScale = v;
		RebuildParams = true;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetOverSampling(unsigned int v)
{
	if(v == 0)
	{
		// TimeScale �ɏ]���Ēl��ݒ�
		// ������臒l�͎��ۂ̃��X�j���O�ɂ�茈�肳�ꂽ���l�ł���A
		// �_���I�ȍ����͂Ȃ��B
		if(TimeScale <= 0.2) v = 2;
		else if(TimeScale <= 1.2) v = 4;
		else v = 8;
	}

	if(OverSampling != v)
	{
		OverSampling = v;
		InputHopSize = OutputHopSize = FrameSize / OverSampling;
		OutputHopSize = static_cast<unsigned int>(InputHopSize * TimeScale) & ~1;
		// ������OutputHopSize�̌v�Z�ɂ��Ă� tRisaPhaseVocoderDSP::SetTimeScale
		// ���Q�Ƃ̂���
		RebuildParams = true;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::Clear()
{
	// �S�Ẵo�b�t�@�Ȃǂ�����
	if(AnalWork)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(AnalWork[ch]), AnalWork[ch] = NULL;
		TJSAlignedDealloc(AnalWork), AnalWork = NULL;
	}
	if(SynthWork)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(SynthWork[ch]), SynthWork[ch] = NULL;
		TJSAlignedDealloc(SynthWork), SynthWork = NULL;
	}
	if(LastAnalPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(LastAnalPhase[ch]), LastAnalPhase[ch] = NULL;
		TJSAlignedDealloc(LastAnalPhase), LastAnalPhase = NULL;
	}
	if(LastSynthPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(LastSynthPhase[ch]), LastSynthPhase[ch] = NULL;
		TJSAlignedDealloc(LastSynthPhase), LastSynthPhase = NULL;
	}
	TJSAlignedDealloc(FFTWorkIp), FFTWorkIp = NULL;
	TJSAlignedDealloc(FFTWorkW), FFTWorkW = NULL;
	TJSAlignedDealloc(InputWindow), InputWindow = NULL;
	TJSAlignedDealloc(OutputWindow), OutputWindow = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetInputFreeSize()
{
	return InputBuffer.GetFreeSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaPhaseVocoderDSP::GetInputBuffer(
	size_t numsamplegranules,
	float * & p1, size_t & p1size,
	float * & p2, size_t & p2size)
{
	size_t numsamples = numsamplegranules * Channels;

	if(InputBuffer.GetFreeSize() < numsamples) return false; // �\���ȋ󂫗e�ʂ��Ȃ�

	InputBuffer.GetWritePointer(numsamples, p1, p1size, p2, p2size);

	p1size /= Channels;
	p2size /= Channels;

	InputBuffer.AdvanceWritePos(numsamples);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetOutputReadySize()
{
	return OutputBuffer.GetDataSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaPhaseVocoderDSP::GetOutputBuffer(
	size_t numsamplegranules,
	const float * & p1, size_t & p1size,
	const float * & p2, size_t & p2size)
{
	size_t numsamples = numsamplegranules * Channels;

	if(OutputBuffer.GetDataSize() < numsamples) return false; // �\���ȏ���ς݃T���v�����Ȃ�

	OutputBuffer.GetReadPointer(numsamples, p1, p1size, p2, p2size);

	p1size /= Channels;
	p2size /= Channels;

	OutputBuffer.AdvanceReadPos(numsamples);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::tStatus tRisaPhaseVocoderDSP::Process()
{
	bool use_sse =
			(TVPCPUType & TVP_CPU_HAS_MMX) &&
			(TVPCPUType & TVP_CPU_HAS_SSE) &&
			(TVPCPUType & TVP_CPU_HAS_CMOV);


	// �p�����[�^�̍Čv�Z�̕K�v������ꍇ�͍Čv�Z������
	if(RebuildParams)
	{
		// ���֐��̌v�Z(�����ł�Vorbis I ��)
		float recovery_of_loss_of_vorbis_window = 2.0;
				//         1            1         2
				//  2  =  ��  1dx  /   ��   vorbis (x) dx
				//         0            0
				// where vobis = vorbis I window function
		float output_volume =
			TimeScale / FrameSize  / sqrt(FrequencyScale) / OverSampling * 2 *
											recovery_of_loss_of_vorbis_window;
		for(unsigned int i = 0; i < FrameSize; i++)
		{
			double x = ((double)i+0.5)/FrameSize;
			double window = sin(M_PI/2*sin(M_PI*x)*sin(M_PI*x));
			InputWindow[i]  = (float)(window);
			OutputWindow[i] = (float)(window *output_volume);
		}

		// ���̂ق��̃p�����[�^�̍Čv�Z
		OverSamplingRadian = (float)((2.0*M_PI)/OverSampling);
		OverSamplingRadianRecp = (float)(1.0/OverSamplingRadian);
		FrequencyPerFilterBand = (float)((double)Frequency/FrameSize);
		FrequencyPerFilterBandRecp = (float)(1.0/FrequencyPerFilterBand);
		ExactTimeScale = (float)OutputHopSize / InputHopSize;

		// �t���O��|��
		RebuildParams = false;
	}

	// ��̓o�b�t�@���̃f�[�^�͏\�����H
	if(InputBuffer.GetDataSize() < FrameSize * Channels)
		return psInputNotEnough; // ����Ȃ�

	// �o�̓o�b�t�@�̋󂫂͏\�����H
	if(OutputBuffer.GetFreeSize() < FrameSize * Channels)
		return psOutputFull; // ����Ȃ�

	// ���ꂩ�珑���������Ƃ��� OutputBuffer �̗̈�̍Ō�� OutputHopSize �T���v��
	// �O���j���[���� 0 �Ŗ��߂� (�I�[�o�[���b�v���ɂ͂ݏo�������Ȃ̂�)
	{
		float *p1, *p2;
		size_t p1len, p2len;

		OutputBuffer.GetWritePointer(OutputHopSize*Channels,
				p1, p1len, p2, p2len, (FrameSize - OutputHopSize)*Channels);
		memset(p1, 0, p1len * sizeof(float));
		if(p2) memset(p2, 0, p2len * sizeof(float));
	}

	// ���֐���K�p���A��̓o�b�t�@���� AnalWork �ɓǂݍ���
	{
		const float *p1, *p2;
		size_t p1len, p2len;
		InputBuffer.GetReadPointer(FrameSize*Channels, p1, p1len, p2, p2len);
		p1len /= Channels;
		p2len /= Channels;
		(use_sse?
		sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj:
		def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj)
			(AnalWork, p1, InputWindow, Channels, 0, p1len);

		if(p2)
			(use_sse?
			sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj:
			def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj)
				(AnalWork, p2, InputWindow + p1len, Channels, p1len, p2len);
	}

	// �`�����l�����Ƃɏ���
	for(unsigned int ch = 0; ch < Channels; ch++)
	{
		//------------------------------------------------
		// ���
		//------------------------------------------------

		// ���Z�̍������������s����
			(use_sse?
			sse__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi:
			def__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi)
				(this, ch);
	}

	// ���֐���K�p���ASynthWork ����o�̓o�b�t�@�ɏ�������
	{
		float *p1, *p2;
		size_t p1len, p2len;

		OutputBuffer.GetWritePointer(FrameSize*Channels, p1, p1len, p2, p2len);
		p1len /= Channels;
		p2len /= Channels;
		(use_sse?
		sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj:
		def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj)
			(p1, SynthWork, OutputWindow, Channels, 0, p1len);
		if(p2)
			(use_sse?
			sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj:
			def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj)
				(p2, SynthWork, OutputWindow + p1len, Channels, p1len, p2len);
	}

	// LastSynthPhase ���Ē������邩
	LastSynthPhaseAdjustCounter += LastSynthPhaseAdjustIncrement;
	if(LastSynthPhaseAdjustCounter >= LastSynthPhaseAdjustInterval)
	{
		// LastSynthPhase ���Ē�������J�E���g�ɂȂ���
		LastSynthPhaseAdjustCounter = 0;

		// �����ōs�������� LastSynthPhase �� unwrapping �ł���B
		// LastSynthPhase �͈ʑ��̍����ݐς����̂ő傫�Ȑ��l�ɂȂ��Ă������A
		// �K���ȊԊu�ł���� unwrapping ���Ȃ��ƁA������(���l���傫������)���x
		// �������������A����ɍ������o���Ȃ��Ȃ��Ă��܂��B
		// �������A���x���ۂ����΂悢���߁A���񂱂� unwrapping ���s���K�v�͂Ȃ��B
		// �����ł� LastSynthPhaseAdjustInterval/LastSynthPhaseAdjustIncrement �񂲂Ƃɒ������s���B
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			unsigned int framesize_d2 = FrameSize / 2;
			for(unsigned int i = 0; i < framesize_d2; i++)
			{
				long int n = static_cast<long int>(LastSynthPhase[ch][i] / (2.0*M_PI));
				LastSynthPhase[ch][i] -= n * (2.0*M_PI);
			}
		}
	}

	// ��o�̓o�b�t�@�̃|�C���^��i�߂�
	OutputBuffer.AdvanceWritePos(OutputHopSize * Channels);
	InputBuffer.AdvanceReadPos(InputHopSize * Channels);

	// �X�e�[�^�X = no error
	return psNoError;
}
//---------------------------------------------------------------------------
