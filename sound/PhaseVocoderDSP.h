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
#ifndef RisaPhaseVocoderH
#define RisaPhaseVocoderH

#include "RingBuffer.h"

//---------------------------------------------------------------------------
//! @brief Phase Vocoder DSP �N���X
//---------------------------------------------------------------------------
class tRisaPhaseVocoderDSP
{
protected:
	float ** AnalWork; //!< ���(Analyze)�p�o�b�t�@(FrameSize��) ���O�ŏ΂�Ȃ��悤��
	float ** SynthWork; //!< �����p��ƃo�b�t�@(FrameSize)
	float ** LastAnalPhase; //!< �O���͎��̊e�t�B���^�o���h�̈ʑ� (�e�`�����l�����Ƃ�FrameSize/2��)
	float ** LastSynthPhase; //!< �O�񍇐����̊e�t�B���^�o���h�̈ʑ� (�e�`�����l�����Ƃ�FrameSize/2��)

	int * FFTWorkIp; //!< rdft �ɓn�� ip �p�����[�^
	float * FFTWorkW; //!< rdft �ɓn�� w �p�����[�^
	float * InputWindow; //!< ��͗p���֐�
	float * OutputWindow; //!< �o�͗p���֐�

	unsigned int FrameSize; //!< FFT�T�C�Y
	unsigned int OverSampling; //!< �I�[�o�[�E�T���v�����O�W��
	unsigned int Frequency; //!< PCM �T���v�����O��g��
	unsigned int Channels; //!< PCM �`�����l����
	unsigned int InputHopSize; //!< FrameSize/OverSampling
	unsigned int OutputHopSize; //!< InputHopSize * TimeScale (SetTimeScale���ɍČv�Z�����)

	float	TimeScale; //!< ���Ԏ����̃X�P�[��(�o��/���)
	float	FrequencyScale; //!< ��g�����̃X�P�[��(�o��/���)

	// �ȉ��ARebuildParams ���^�̎��ɍč\�z�����p�����[�^
	// �����ɂ��郁���o�ȊO�ł́AInputWindow �� OutputWindow ���č\�z�����
	float OverSamplingRadian; //!< (2.0*M_PI)/OverSampling
	float OverSamplingRadianRecp; //!< OverSamplingRadian �̋t��
	float FrequencyPerFilterBand; //!< Frequency/FrameSize
	float FrequencyPerFilterBandRecp; //!< FrequencyPerFilterBand �̋t��
	float ExactTimeScale; //!< ������TimeScale = OutputHopSize / InputHopSize
	// �č\�z�����p�����[�^�A�����܂�

	tRisaRingBuffer<float> InputBuffer; //!< ��͗p�����O�o�b�t�@
	tRisaRingBuffer<float> OutputBuffer; //!< �o�͗p�����O�o�b�t�@

	bool	RebuildParams; //!< �����I�ȃp�����[�^�Ȃǂ��č\�z���Ȃ���΂Ȃ�Ȃ��Ƃ��ɐ^

	unsigned long LastSynthPhaseAdjustCounter; //!< LastSynthPhase ��␳��������͂��邽�߂̃J�E���^
	const static unsigned long LastSynthPhaseAdjustIncrement = 0x03e8a444; //!< LastSynthPhaseAdjustCounter�ɉ��Z����l
	const static unsigned long LastSynthPhaseAdjustInterval  = 0xfa2911fe; //!< LastSynthPhase ��␳������


public:
	//! @brief Process ���Ԃ��X�e�[�^�X
	enum tStatus
	{
		psNoError, //!< ���Ȃ�
		psInputNotEnough, //!< ��͂������Ȃ� (GetInputBuffer�œ����|�C���^�ɏ����Ă���Ď��s����)
		psOutputFull //!< �o�̓o�b�t�@�������ς� (GetOutputBuffer�œ����|�C���^����ǂݏo���Ă���Ď��s����)
	};

public:
	//! @brief		�R���X�g���N�^
	//! @param		framesize		�t���[���T�C�Y(2�̗ݏ�, 16�`)
	//! @param		frequency		���PCM�̃T���v�����O���[�g
	//! @param		channels		���PCM�̃`�����l����
	//! @note		���y�p�ł�framesize=4096,oversamp=16���炢���悭�A
	//! @note		�{�C�X�p�ł�framesize=256,oversamp=8���炢���悢�B
	tRisaPhaseVocoderDSP(unsigned int framesize,
					unsigned int frequency, unsigned int channels);

	//! @brief		�f�X�g���N�^
	~tRisaPhaseVocoderDSP();

	float GetTimeScale() const { return TimeScale; } //!< ���Ԏ����̃X�P�[���𓾂�

	//! @brief		���Ԏ����̃X�P�[����ݒ肷��
	//! @param		v     �X�P�[��
	void SetTimeScale(float v);

	float GetFrequencyScale() const { return FrequencyScale; } //!< ��g�������̃X�P�[���𓾂�

	//! @brief		��g�������̃X�P�[����ݒ肷��
	//! @param		v     �X�P�[��
	void SetFrequencyScale(float v);

	//! @brief		�I�[�o�[�T���v�����O�W�����擾����
	//! @return		�I�[�o�[�T���v�����O�W��
	unsigned int GetOverSampling() const { return OverSampling; }

	//! @brief		�I�[�o�[�T���v�����O�W����ݒ肷��
	//! @param		v		�W�� ( 0 = ���Ԏ����̃X�P�[���ɏ]���Ď����I�ɐݒ� )
	void SetOverSampling(unsigned int v);

	unsigned int GetInputHopSize() const { return InputHopSize; } //!< InputHopSize�𓾂�
	unsigned int GetOutputHopSize() const { return OutputHopSize; } //!< OutputHopSize �𓾂�

private:
	//! @brief		�N���A
	void Clear();

public:
	//! @brief		��̓o�b�t�@�̋󂫃T���v���O���j���[�����𓾂�
	//! @return		��̓o�b�t�@�̋󂫃T���v���O���j���[����
	size_t GetInputFreeSize();

	//! @brief		��̓o�b�t�@�̏������݃|�C���^�𓾂�
	//! @param		numsamplegranules �������݂����T���v���O���j���[����
	//! @param		p1		�u���b�N1�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�
	//! @param		p1size	p1�̕\���u���b�N�̃T���v���O���j���[����
	//! @param		p2		�u���b�N2�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�(NULL�����蓾��)
	//! @param		p2size	p2�̕\���u���b�N�̃T���v���O���j���[����(0�����蓾��)
	//! @return		�󂫗e�ʂ�����Ȃ���΋U�A�󂫗e�ʂ�����A�|�C���^���Ԃ����ΐ^
	//! @note		p1 �� p2 �̂悤�ɂQ�̃|�C���^�Ƃ��̃T�C�Y���Ԃ����̂́A
	//!				���̃o�b�t�@�����ۂ̓����O�o�b�t�@�ŁA�����O�o�b�t�@�����̃��j�A�ȃo�b�t�@
	//!				�̏I�[���܂����\�������邽�߁B�܂����Ȃ��ꍇ��p2��NULL�ɂȂ邪�A�܂���
	//!				�ꍇ�� p1 �̂��Ƃ� p2 �ɑ����ď������܂Ȃ���΂Ȃ�Ȃ��B
	bool GetInputBuffer(size_t numsamplegranules,
		float * & p1, size_t & p1size,
		float * & p2, size_t & p2size);

	//! @brief		�o�̓o�b�t�@�̏���ς݃T���v���O���j���[�����𓾂�
	//! @return		�o�̓o�b�t�@�̏���ς݃T���v���O���j���[����
	size_t GetOutputReadySize();

	//! @brief		�o�̓o�b�t�@�̓ǂݍ��݃|�C���^�𓾂�
	//! @param		numsamplegranules �ǂݍ��݂����T���v���O���j���[����
	//! @param		p1		�u���b�N1�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�
	//! @param		p1size	p1�̕\���u���b�N�̃T���v���O���j���[����
	//! @param		p2		�u���b�N2�̐擪�ւ̃|�C���^���i�[���邽�߂̕ϐ�(NULL�����蓾��)
	//! @param		p2size	p2�̕\���u���b�N�̃T���v���O���j���[����(0�����蓾��)
	//! @return		����ꂽ�T���v��������Ȃ���΋U�A�T���v��������A�|�C���^���Ԃ����ΐ^
	//! @note		p1 �� p2 �̂悤�ɂQ�̃|�C���^�Ƃ��̃T�C�Y���Ԃ����̂́A
	//!				���̃o�b�t�@�����ۂ̓����O�o�b�t�@�ŁA�����O�o�b�t�@�����̃��j�A�ȃo�b�t�@
	//!				�̏I�[���܂����\�������邽�߁B�܂����Ȃ��ꍇ��p2��NULL�ɂȂ邪�A�܂���
	//!				�ꍇ�� p1 �̂��Ƃ� p2 �𑱂��ēǂݏo���Ȃ���΂Ȃ�Ȃ��B
	bool GetOutputBuffer(size_t numsamplegranules,
		const float * & p1, size_t & p1size,
		const float * & p2, size_t & p2size);

	//! @brief		������1�X�e�b�v�s��
	//! @return		�������ʂ�\��enum
	tStatus Process();

	//! @brief		���Z�̍�����������������
	//! @param		ch			�������s���`�����l��
	//! @note		�����̕����͊eCPU���ƂɍœK������邽�߁A
	//!				������ opt_default �f�B���N�g�����Ȃǂɒu�����B
	//!				(PhaseVocoderDSP.cpp���ɂ͂���̎����͂Ȃ�)
	void ProcessCore(int ch);
};
//---------------------------------------------------------------------------

#endif
