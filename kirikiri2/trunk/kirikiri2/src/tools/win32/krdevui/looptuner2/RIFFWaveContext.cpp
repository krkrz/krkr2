//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RIFFWaveContext.h"

//---------------------------------------------------------------------------
const GUID __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT =
		{ 0x00000003 , 0x0000, 0x0010, { 0x80,0x00,0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }};
const GUID __KSDATAFORMAT_SUBTYPE_PCM =
		{ 0x00000001 , 0x0000, 0x0010, { 0x80,0x00,0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
static void ConvertFloatTo16bits(short int *output, const float *input, int channels, int count)
{
	// �����_�ł� 32bit float �݂̂�ϊ�����

	// float PCM �� +1.0 �` 0 �` -1.0 �͈̔͂ɂ���
	// �͈͊O�̃f�[�^�̓N���b�v����
	int total = channels * count;
	while(total--)
	{
		float t = *(input ++) * 32768.0;
		if(t > 0)
		{
			int i = (int)(t + 0.5);
			if(i > 32767) i = 32767;
			*(output++) = (short int)i;
		}
		else
		{
			int i = (int)(t - 0.5);
			if(i < -32768) i = -32768;
			*(output++) = (short int)i;
		}
	}

}
//---------------------------------------------------------------------------
static void ConvertIntTo16bits(short int *output, const void *input, int bytespersample,
	int validbits, int channels, int count)
{
	// �{���� int16 �Ȃǂ̃T�C�Y�̌��܂����^�������ׂ�

	// �����\���� PCM �� 16bit �ɕϊ�����
	int total = channels * count;

	if(bytespersample == 1)
	{
		// �ǂݍ��ݎ��̃`�F�b�N�ł��ł� 8bit ��͂̃f�[�^�� �L��r�b�g���� 8 �r�b�g
		// �ł��邱�Ƃ��ۏ؂���Ă���̂ŒP�� 16bit �Ɋg�����邾���ɂ���
		const char *p = (const char *)input;
		while(total--) *(output++) = (short int)( ((int)*(p++)-0x80) * 0x100);
	}
	else if(bytespersample == 2)
	{
		// �L��r�b�g���ȉ��̐��l�� �}�X�N���Ď�苎��
		short int mask =  ~( (1 << (16 - validbits)) - 1);
		const short int *p = (const short int *)input;
		while(total--) *(output++) = (short int)(*(p++) & mask);
	}
	else if(bytespersample == 3)
	{
		long int mask = ~( (1 << (24 - validbits)) - 1);
		const unsigned char *p = (const unsigned char *)input;
		while(total--)
		{
			int t = p[0] + (p[1] << 8) + (p[2] << 16);
			p += 3;
			t |= -(t&0x800000); // �����g��
			t &= mask; // �}�X�N
			t >>= 8;
			*(output++) = (short int)t;
		}
	}
	else if(bytespersample == 4)
	{
		long int mask = ~( (1 << (32 - validbits)) - 1);
		const unsigned __int32 *p = (const unsigned __int32 *)input;
		while(total--)
		{
			*(output++) = (short int)((*(p++) & mask) >> 16);
		}
	}

}
//---------------------------------------------------------------------------
static void ConvertTo16bits(short int *output, const void *input, int bytespersample,
	bool isfloat, int validbits, int channels, int count)
{
	// �w�肳�ꂽ�t�H�[�}�b�g�� 16bit �t�H�[�}�b�g�ɕϊ�����
	// channels �� -6 �̏ꍇ�͓��ʂ�
	// FL FC FR BL BR LF �ƕ���ł���f�[�^ (AC3�Ɠ�������) ��
	// FL FR FC LF BL BR (WAVEFORMATEXTENSIBLE �̊�҂��鏇��) �ɕ��ёւ���
	int cns = channels;
	if(cns<0) cns = -cns;

	if(isfloat)
		ConvertFloatTo16bits(output, (const float *)input, cns, count);
	else
		ConvertIntTo16bits(output, input, bytespersample, validbits, cns, count);

	if(channels == -6)
	{
		// �`�����l���̓��ւ�
		short int *op = output;
		for(int i = 0; i < count; i++)
		{
			short int fc = op[1];
			short int bl = op[3];
			short int br = op[4];
			op[1] = op[2];
			op[2] = fc;
			op[3] = op[5];
			op[4] = bl;
			op[5] = br;
			op += 6;
		}
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
__fastcall TRIFFWaveContext::TRIFFWaveContext()
{
	FInputStream = NULL;
}
//---------------------------------------------------------------------------
__fastcall TRIFFWaveContext::~TRIFFWaveContext()
{
	if(FInputStream) delete FInputStream;
}
//---------------------------------------------------------------------------
bool __fastcall TRIFFWaveContext::Start(AnsiString filename)
{
	try
	{
		FInputStream = new TFileStream(filename, fmOpenRead | fmShareDenyWrite);
	}
	catch(...)
	{
		return false;
	}

	// �w�b�_��ǂ�
	BYTE buf[8];
	FInputStream->Position=0;
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "RIFF", 4)) return false; // RIFF ���Ȃ�
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "WAVEfmt ", 4)) return false; // WAVE �łȂ��� fmt �`�����N���������Ȃ�

	DWORD fmtlen;
	FInputStream->ReadBuffer(&fmtlen,4);

	FInputStream->ReadBuffer(&Format, sizeof(WAVEFORMATEX));
	if(Format.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE &&
		Format.Format.wFormatTag != WAVE_FORMAT_PCM &&
		Format.Format.wFormatTag != WAVE_FORMAT_IEEE_FLOAT) return false; // �Ή��ł��Ȃ��t�H�[�}�b�g

	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		FInputStream->ReadBuffer(
			(char*)&Format + sizeof(Format.Format),
			sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX));
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_PCM)
	{
		Format.Samples.wValidBitsPerSample = Format.Format.wBitsPerSample;
		Format.SubFormat = __KSDATAFORMAT_SUBTYPE_PCM;
		Format.dwChannelMask = 0;
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		Format.Samples.wValidBitsPerSample = Format.Format.wBitsPerSample;
		Format.SubFormat = __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		Format.dwChannelMask = 0;
	}

	// �e��`�F�b�N
	if(Format.Format.wBitsPerSample & 0x07) return false; // 8 �̔{���ł͂Ȃ�
	if(Format.Format.wBitsPerSample > 32) return false; // 32bit ���傫���T�C�Y�͈����Ȃ�
	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
		Format.Samples.wValidBitsPerSample < 8) return false; // �L��r�b�g�� 8 �����͈����Ȃ�
	if(Format.Format.wBitsPerSample < Format.Samples.wValidBitsPerSample)
		return false; // �L��r�b�g�������ۂ̃r�b�g�����傫��
	if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 16))
		IsFloat = true;
	else if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_PCM, 16))
		IsFloat = false;
	else
		return false; // ���m�̃T�u�t�H�[�}�b�g

	if(IsFloat && Format.Format.wBitsPerSample != Format.Samples.wValidBitsPerSample)
		return false; // float �̗L��r�b�g�����i�[�r�b�g���Ɠ������Ȃ�
	if(IsFloat && Format.Format.wBitsPerSample != 32)
		return false; // float �̃r�b�g���� 32 �łȂ�


	FGranuleSize = Format.Format.wBitsPerSample / 8 * Format.Format.nChannels;

	// data �`�����N��T��
	FInputStream->Position= fmtlen + 0x14;

	while(1)
	{
		FInputStream->ReadBuffer(buf,4); // data ?
		FInputStream->ReadBuffer(&FDataSize,4); // �f�[�^�T�C�Y
		if(memcmp(buf,"data",4)==0) break;
		FInputStream->Position=FInputStream->Position+FDataSize;

		if(FInputStream->Position >= FInputStream->Size) return false;
	}

	FDataStart=FInputStream->Position;

	return true;
}
//---------------------------------------------------------------------------
int __fastcall TRIFFWaveContext::Read(__int16 * dest, int destgranules)
{
	int samplesize = FGranuleSize;
	destgranules *= samplesize; // data (in bytes) to read
	int bytestowrite;
	int remain = FDataStart + FDataSize - FInputStream->Position;
	int readsamples;

	char *buf = new char[destgranules];

	try
	{
		bytestowrite = (destgranules>remain) ? remain : destgranules;
		bytestowrite = FInputStream->Read(buf, bytestowrite);

		readsamples = bytestowrite / samplesize;

		// convert to the destination format
		ConvertTo16bits(dest, buf, Format.Format.wBitsPerSample / 8, IsFloat,
			Format.Samples.wValidBitsPerSample, Format.Format.nChannels, readsamples);
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}
	delete [] buf;

	return readsamples;
}
//---------------------------------------------------------------------------
