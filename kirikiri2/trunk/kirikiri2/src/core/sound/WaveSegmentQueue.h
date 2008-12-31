//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Wave�Z�O�����g/���x���L���[�Ǘ�
//---------------------------------------------------------------------------
#ifndef WAVESEGMENTH
#define WAVESEGMENTH


#include <deque>


//---------------------------------------------------------------------------
//! @brief �Đ��Z�O�����g���
//---------------------------------------------------------------------------
struct tTVPWaveSegment
{
	//! @brief �R���X�g���N�^
	tTVPWaveSegment(tjs_int64 start, tjs_int64 length)
		{ Start = start; Length = FilteredLength = length; }
	tTVPWaveSegment(tjs_int64 start, tjs_int64 length, tjs_int64 filteredlength)
		{ Start = start; Length = length; FilteredLength = filteredlength; }
	tjs_int64 Start; //!< �I���W�i���f�R�[�_��ł̃Z�O�����g�̃X�^�[�g�ʒu (PCM �T���v���O���j���[�����P��)
	tjs_int64 Length; //!< �I���W�i���f�R�[�_��ł̃Z�O�����g�̒��� (PCM �T���v���O���j���[�����P��)
	tjs_int64 FilteredLength; //!< �t�B���^��̒��� (PCM �T���v���O���j���[�����P��)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief �Đ����x�����
//---------------------------------------------------------------------------
struct tTVPWaveLabel
{
	//! @brief �R���X�g���N�^
	tjs_int64 Position; //!< �I���W�i���f�R�[�_��ł̃��x���ʒu (PCM �T���v���O���j���[�����P��)
	ttstr Name; //!< ���x����
	tjs_int Offset;
		/*!< �I�t�Z�b�g
			@note
			This member will be set in tTVPWaveLoopManager::Render,
			and will contain the sample granule offset from first decoding
			point at call of tTVPWaveLoopManager::Render().
		*/

#ifdef TVP_IN_LOOP_TUNER
	// these are only used by the loop tuner
	tjs_int NameWidth; // display name width
	tjs_int Index; // index
#endif

	struct tSortByPositionFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Position < rhs.Position;
		}
	};

	struct tSortByOffsetFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Offset < rhs.Offset;
		}
	};

#ifdef TVP_IN_LOOP_TUNER
	struct tSortByIndexFuncObj
	{
		bool operator()(
			const tTVPWaveLabel &lhs,
			const tTVPWaveLabel &rhs) const
		{
			return lhs.Index < rhs.Index;
		}
	};
#endif

	//! @brief �R���X�g���N�^
	tTVPWaveLabel()
	{
		Position = 0;
		Offset = 0;
#ifdef TVP_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}

	//! @brief �R���X�g���N�^
	tTVPWaveLabel(tjs_int64 position, const ttstr & name, tjs_int offset)
		: Position(position), Name(name), Offset(offset)
	{
#ifdef TVP_IN_LOOP_TUNER
		NameWidth = 0;
		Index = 0;
#endif
	}
};
//---------------------------------------------------------------------------
bool inline operator < (const tTVPWaveLabel & lhs, const tTVPWaveLabel & rhs)
{
	return lhs.Position < rhs.Position;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief Wave�̃Z�O�����g�E���x���̃L���[���Ǘ�����N���X
//---------------------------------------------------------------------------
class tTVPWaveSegmentQueue
{
	// deque �ɂ�� Segments �� Labels �̔z��B
	// ���p��́A�����̔z��ɑ�ʂ̃f�[�^����邱�Ƃ͂܂��Ȃ��̂�
	// vector �ŏ\���Ȃ̂�������Ȃ��� ...
	std::deque<tTVPWaveSegment> Segments; //!< �Z�O�����g�̔z��
	std::deque<tTVPWaveLabel> Labels; //!< ���x���̔z��

public:
	//! @brief		���e���N���A����
	void Clear();

	//! @brief		�Z�O�����g�̔z��𓾂�
	//! @return		�Z�O�����g�̔z��
	const std::deque<tTVPWaveSegment> & GetSegments() const { return Segments; }

	//! @brief		���x���̔z��𓾂�
	//! @return		���x���̔z��
	const std::deque<tTVPWaveLabel> & GetLabels() const { return Labels; }

	//! @brief		tTVPWaveSegmentQueue���G���L���[����
	//! @param		queue		�G���L���[������tTVPWaveSegmentQueue�I�u�W�F�N�g
	void Enqueue(const tTVPWaveSegmentQueue & queue);

	//! @brief		tTVPWaveSegment���G���L���[����
	//! @param		queue		�G���L���[������tTVPWaveSegment�I�u�W�F�N�g
	void Enqueue(const tTVPWaveSegment & segment);

	//! @brief		tTVPWaveLabel���G���L���[����
	//! @param		queue		�G���L���[������tTVPWaveLabel�I�u�W�F�N�g
	//! @note		Offset �͏C������Ȃ��̂Œ���
	void Enqueue(const tTVPWaveLabel & Label);

	//! @brief		tTVPWaveSegment�̔z����G���L���[����
	//! @param		queue		�G���L���[������ std::dequeue<tTVPWaveSegment>�I�u�W�F�N�g
	void Enqueue(const std::deque<tTVPWaveSegment> & segments);

	//! @brief		tTVPWaveLabel�̔z����G���L���[����
	//! @param		queue		�G���L���[������ std::dequeue<tTVPWaveLabel>�I�u�W�F�N�g
	void Enqueue(const std::deque<tTVPWaveLabel> & Labels);

	//! @brief		�擪����w�蒷�������f�L���[����
	//! @param		dest		�i�[��L���[(���e�̓N���A�����)
	//! @param		length		�؂�o������(�T���v���O���j���[���P��)
	void Dequeue(tTVPWaveSegmentQueue & dest, tjs_int64 length);

	//! @brief		���̃L���[�̑S�̂̒����𓾂�
	//! @return		���̃L���[�̒��� (�T���v���O���j���[���P��)
	tjs_int64 GetFilteredLength() const;

	//! @brief		���̃L���[�̒�����ω�������
	//! @param		new_total_filtered_length �V�����L���[�̒��� (�T���v���O���j���[���P��)
	//! @note		�L���[����Segments �Ȃǂ̒����� Labels�̈ʒu�͐�`��Ԃ����
	void Scale(tjs_int64 new_total_length);

	//! @brief		�t�B���^���ꂽ�ʒu����f�R�[�h�ʒu�֕ϊ����s��
	//! @param		pos �t�B���^���ꂽ�ʒu
	//! @note		�f�R�[�h�ʒu
	tjs_int64 FilteredPositionToDecodePosition(tjs_int64 pos) const;
};
//---------------------------------------------------------------------------

#endif

