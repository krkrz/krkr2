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
#include "tjsCommHead.h"

#include "WaveSegmentQueue.h"


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Clear()
{
	Labels.clear();
	Segments.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveSegmentQueue & queue)
{
	Enqueue(queue.Labels); // Labels ���G���L���[(���������ɂ��Ȃ��Ƃ���)
	Enqueue(queue.Segments); // segments ���L���[(�������͌�)
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveSegment & segment)
{
	if(Segments.size() > 0)
	{
		// ��̃Z�O�����g�� 1 �ȏ゠��
		tTVPWaveSegment & last = Segments.back();
		// �Ō�̃Z�O�����g�Ƃ��ꂩ��ǉ����悤�Ƃ���Z�O�����g���A�����Ă邩�H
		if(last.Start + last.Length == segment.Start &&
			(double)last.FilteredLength / last.Length ==
			(double)segment.FilteredLength / segment.Length)
		{
			// �A�����Ă��āA���A�䗦�����S�ɓ����Ȃ̂�
			// ��̍Ō�̃Z�O�����g����������
			// (���Ȃ݂ɂ����Ŕ䗦�̔�r�̍ۂɌ덷���������Ƃ��Ă�
			//  �傫�Ȗ��Ƃ͂Ȃ�Ȃ�)
			last.FilteredLength += segment.FilteredLength;
			last.Length += segment.Length;
			return ; // �����
		}
	}

	// �P���ɍŌ�ɗv�f��ǉ�
	Segments.push_back(segment);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const tTVPWaveLabel & Label)
{
	Labels.push_back(Label);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const std::deque<tTVPWaveSegment> & segments)
{
	// segment �̒ǉ�
	for(std::deque<tTVPWaveSegment>::const_iterator i = segments.begin();
		i != segments.end(); i++)
		Enqueue(*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Enqueue(const std::deque<tTVPWaveLabel> & Labels)
{
	// �I�t�Z�b�g�ɉ��Z����l�𓾂�
	tjs_int64 Label_offset = GetFilteredLength();

	// Label �̒ǉ�
	for(std::deque<tTVPWaveLabel>::const_iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		tTVPWaveLabel one_Label(*i);
		one_Label.Offset += Label_offset; // offset �̏C��
		Enqueue(one_Label);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Dequeue(tTVPWaveSegmentQueue & dest, tjs_int64 length)
{
	tjs_int64 remain;
	// dest ���N���A
	dest.Clear();

	// Segments ��؂�o��
	remain = length;
	while(Segments.size() > 0 && remain > 0)
	{
		if(Segments.front().FilteredLength <= remain)
		{
			// Segments.front().FilteredLength �� remain �ȉ�
			// �� ���̗v�f�� dest �ɃG���L���[���� this ����폜
			remain -= Segments.front().FilteredLength;
			dest.Enqueue(Segments.front());
			Segments.pop_front();
		}
		else
		{
			// Segments.front().FilteredLength �� remain �����傫��
			// �� �v�f��r���łԂ��������� dest �ɃG���L���[
			// FilteredLength �����ɐ؂�o�����s���Ă�̂�
			// Length �� ��`��Ԃ��s��
			tjs_int64 newlength =
				static_cast<tjs_int64>(
					(double)Segments.front().Length / (double)Segments.front().FilteredLength * remain);
			if(newlength > 0)
				dest.Enqueue(tTVPWaveSegment(Segments.front().Start, newlength, remain));

			// Segments.front() �� Start, Length �� FilteredLength ���C��
			Segments.front().Start += newlength;
			Segments.front().Length -= newlength;
			Segments.front().FilteredLength -= remain;
			if(Segments.front().Length == 0 || Segments.front().FilteredLength == 0)
			{
				// �Ԃ����؂������� (��`�⊮�������ʂ̌덷��)
				// ������0�ɂȂ��Ă��܂���
				Segments.pop_front(); // �Z�O�����g���̂Ă�
			}
			remain = 0; // ���[�v�𔲂���
		}
	}

	// Labels ��؂�o��
	size_t Labels_to_dequeue = 0;
	for(std::deque<tTVPWaveLabel>::iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		tjs_int64 newoffset = i->Offset - length;
		if(newoffset < 0)
		{
			// newoffset ���� �Ȃ̂� dest �ɓ���
			dest.Enqueue(*i);
			Labels_to_dequeue ++; // ���Ƃ� dequeue
		}
		else
		{
			// *i �̃I�t�Z�b�g���C��
			i->Offset = newoffset;
		}
	}

	while(Labels_to_dequeue--) Labels.pop_front(); // �R�s�[����Labels ���폜
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tjs_int64 tTVPWaveSegmentQueue::GetFilteredLength() const
{
	// �L���[�̒����� ���ׂĂ� Segments ��FilteredLength�̍��v
	tjs_int64 length = 0;
	for(std::deque<tTVPWaveSegment>::const_iterator i = Segments.begin();
		i != Segments.end(); i++)
		length += i->FilteredLength;

	return length;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPWaveSegmentQueue::Scale(tjs_int64 new_total_filtered_length)
{
	// �L���[�� FilteredLength ��ω�������
	tjs_int64 total_length_was = GetFilteredLength(); // �ω��O�̒���

	if(total_length_was == 0) return; // ���̒������Ȃ��̂ŃX�P�[���o���Ȃ�

	// Segments �̏C��
	tjs_int64 offset_was = 0; // �ω��O�̃I�t�Z�b�g
	tjs_int64 offset_is = 0; // �ω���̃I�t�Z�b�g

	for(std::deque<tTVPWaveSegment>::iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		tjs_int64 old_end = offset_was + i->FilteredLength;
		offset_was += i->FilteredLength;

		// old_end �͑S�̂��猩�Ăǂ̈ʒu�ɂ���H
		double ratio = static_cast<double>(old_end) /
						static_cast<double>(total_length_was);

		// �V���� old_end �͂ǂ̈ʒu�ɂ���ׂ��H
		tjs_int64 new_end = static_cast<tjs_int64>(ratio * new_total_filtered_length);

		// FilteredLength �̏C��
		i->FilteredLength = new_end - offset_is;

		offset_is += i->FilteredLength;
	}

	// ������ۂ�Segments �̏���
	for(std::deque<tTVPWaveSegment>::iterator i = Segments.begin();
		i != Segments.end() ; )
	{
		if(i->FilteredLength == 0 || i->Length == 0)
			i = Segments.erase(i);
		else
			i++;
	}

	// Labels �̏C��
	double ratio = (double)new_total_filtered_length / (double)total_length_was;
	for(std::deque<tTVPWaveLabel>::iterator i = Labels.begin();
		i != Labels.end(); i++)
	{
		i->Offset = static_cast<tjs_int64>(i->Offset * ratio);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tjs_int64 tTVPWaveSegmentQueue::FilteredPositionToDecodePosition(tjs_int64 pos) const
{
	// Segments �̏C��
	tjs_int64 offset_filtered = 0;

	for(std::deque<tTVPWaveSegment>::const_iterator i = Segments.begin();
		i != Segments.end(); i++)
	{
		if(offset_filtered <= pos && pos < offset_filtered + i->FilteredLength)
		{
			// �Ή������Ԃ����������̂Ő�`�ŕ⊮���ĕԂ�
			return (tjs_int64)(i->Start + (pos - offset_filtered) *
				(double)i->Length / (double)i->FilteredLength );
		}

		offset_filtered += i->FilteredLength;
	}

	// �Ή������Ԃ�������Ȃ��̂ŁA���炩�ɕ��ł���� 0 ���A
	// �����łȂ���΍Ō�̈ʒu��Ԃ�
	if(pos<0) return 0;
	if(Segments.size() == 0) return 0;
	return Segments[Segments.size()-1].Start + Segments[Segments.size()-1].Length;
}
//---------------------------------------------------------------------------


