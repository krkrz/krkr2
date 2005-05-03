#include <vcl.h>
#pragma hdrstop

#include "tjsTypes.h"
#include "WaveLoopManager.h"


//---------------------------------------------------------------------------
// Wave Sample Types
//---------------------------------------------------------------------------
#ifndef TJS_HOST_IS_BIG_ENDIAN
	#define TJS_HOST_IS_BIG_ENDIAN 0
#endif

#ifdef __WIN32__
	// for assembler compatibility
	#pragma pack(push,1)
#endif
struct tTVPPCM8
{
	tjs_uint8 value;
	tTVPPCM8(tjs_int v) { value = (tjs_uint8)(v + 0x80); }
	void operator = (tjs_int v) { value = (tjs_uint8)(v + 0x80); }
	operator tjs_int () const { return (tjs_int)value - 0x80; }
};
struct tTVPPCM24
{
	tjs_uint8 value[3];
	tTVPPCM24(tjs_int v)
	{
		operator = (v);
	}
	void operator =(tjs_int v)
	{
#if TJS_HOST_IS_BIG_ENDIAN
		value[0] = (v & 0xff0000) >> 16;
		value[1] = (v & 0x00ff00) >> 8;
		value[2] = (v & 0x0000ff);
#else
		value[0] = (v & 0x0000ff);
		value[1] = (v & 0x00ff00) >> 8;
		value[2] = (v & 0xff0000) >> 16;
#endif
	}
	operator tjs_int () const
	{
#if TJS_HOST_IS_BIG_ENDIAN
		return (value[0] << 16) + (value[1] << 8) + (value[2]);
#else
		return (value[2] << 16) + (value[1] << 8) + (value[0]);
#endif
	}
};
#ifdef __WIN32__
	#pragma pack(pop)
#endif
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Crossfade Template function
//---------------------------------------------------------------------------
#define MULT32(a, b)  ((tjs_int32)(((tjs_int64)(a)*(tjs_int64)(b))>>32))
template <typename T>
static void TVPCrossFadeIntegerBlend(void *dest, void *src1, void *src2,
	tjs_int ratiostart, tjs_int ratioend,
	tjs_int samples, tjs_int channels)
{
	tjs_int blend_step = (tjs_int)(
		(
			(ratioend - ratiostart) * ((tjs_int64)1<<32) / 100
		) / samples);
	const T *s1 = (const T *)src1;
	const T *s2 = (const T *)src2;
	T *out = (T *)dest;
	tjs_int ratio = (tjs_int)(ratiostart * ((tjs_int64)1<<32) / 100);
	for(tjs_int i = 0; i < samples; i++)
	{
		for(tjs_int j = channels - 1; j >= 0; j--)
		{
			tjs_int si1 = (tjs_int)*s1;
			tjs_int si2 = (tjs_int)*s2;
			tjs_int o = si1 + MULT32((si2 - si1), ratio);
			*out = o;
			s1 ++;
			s2 ++;
			out ++;
		}
		ratio += blend_step;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPWaveLoopManager
//---------------------------------------------------------------------------
tTVPWaveLoopManager::tTVPWaveLoopManager(tTVPWaveDecoder * decoder)
{
	Position = 0;
	IsLinksSorted = false;
	IsLabelsSorted = false;
	CrossFadeSamples = NULL;
	CrossFadeLen = 0;
	CrossFadePosition = 0;
	Decoder = decoder;

	decoder->GetFormat(Format);
	ClearFlags();

	// compute ShortCrossFadeHalfSamples
	ShortCrossFadeHalfSamples =
		Format.SamplesPerSec * TVP_WL_SMOOTH_TIME_HALF / 1000;
}
//---------------------------------------------------------------------------
tTVPWaveLoopManager::~tTVPWaveLoopManager()
{
	ClearCrossFadeInformation();
}
//---------------------------------------------------------------------------
bool tTVPWaveLoopManager::GetFlag(tjs_int index)
{
	// note that this function is not protected by any critical sections
	return Flags[index];
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::SetFlag(tjs_int index, bool f)
{
	// note that this function is not protected by any critical sections
	Flags[index] = f;
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::ClearFlags()
{
	// note that this function is not protected by any critical sections
	for(tjs_int i = 0; i < TVP_WL_MAX_FLAGS; i++) Flags[i] = false;
}
//---------------------------------------------------------------------------
tjs_int64 tTVPWaveLoopManager::GetPosition() const
{
	return Position;
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::SetPosition(tjs_int64 pos)
{
	Position = pos;
	Decoder->SetPosition(pos);
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::Decode(void *dest, tjs_uint samples, tjs_uint &written,
		std::vector<tTVPWaveLoopSegment> &segments,
		std::vector<tTVPWaveLabel> &labels)
{
	// decode from current position
	segments.clear();
	labels.clear();
	written = 0;
	tjs_uint8 *d = (tjs_uint8*)dest;

	while(written != samples && Position < Format.TotalSamples)
	{
		// decide next operation
		tjs_int64 next_event_pos;
		bool next_not_found = false;
		tjs_int before_count;

		// check nearest link
		tTVPWaveLoopLink link;
		if(GetNearestEvent(Position, link, false))
		{
			// nearest event found ...
			if(link.From == Position)
			{
				// do jump
				Position = link.To;
				if(!CrossFadeSamples)
					Decoder->SetPosition(Position);
				continue;
			}

			if(link.Smooth)
			{
				// the nearest event is a smooth link
				// bofore_count is sample count before 50%-50% blend
				// after_count  is sample count after  50%-50% blend
				before_count = ShortCrossFadeHalfSamples;
				// adjust before count
				if(link.From - before_count < 0)
					before_count = (tjs_int)link.From;
				if(link.To - before_count < 0)
					before_count = (tjs_int)link.To;
				if(link.From - before_count > Position)
				{
					// Starting crossfade is the nearest next event,
					// but some samples must be decoded before the event comes.
					next_event_pos = link.From - before_count;
				}
				else if(!CrossFadeSamples)
				{
					// just position to start crossfade
					// or crossfade must already start
					next_event_pos = link.From;
					// adjust before_count
					before_count = link.From - Position;
					// adjust after count
					tjs_int after_count = ShortCrossFadeHalfSamples;
					if(Format.TotalSamples - link.From < after_count)
						after_count =
							(tjs_int)(Format.TotalSamples - link.From);
					if(Format.TotalSamples - link.To < after_count)
						after_count =
							(tjs_int)(Format.TotalSamples - link.To);
					tTVPWaveLoopLink over_to_link;
					if(GetNearestEvent(link.To, over_to_link, true))
					{
						if(over_to_link.From - link.To < after_count)
							after_count =
								(tjs_int)(over_to_link.From - link.To);
					}
					// prepare crossfade
					// allocate memory
					tjs_uint8 *src1 = NULL;
					tjs_uint8 *src2 = NULL;
					try
					{
						tjs_int alloc_size =
							(before_count + after_count) * 
								Format.BytesPerSample;
						CrossFadeSamples = new tjs_uint8[alloc_size];
						src1 = new tjs_uint8[alloc_size];
						src2 = new tjs_uint8[alloc_size];
					}
					catch(...)
					{
						// memory allocation failed. perform normal link.
						if(CrossFadeSamples)
							delete [] CrossFadeSamples,
								CrossFadeSamples = NULL;
						if(src1) delete [] src1;
						if(src2) delete [] src2;
						next_event_pos = link.From;
					}
					if(CrossFadeSamples)
					{
						// decode samples
						tjs_uint decoded1 = 0, decoded2 = 0;

						Decoder->Render((void*)src1,
							before_count + after_count, decoded1);

						Decoder->SetPosition(
							link.To - before_count);

						Decoder->Render((void*)src2,
							before_count + after_count, decoded2);

						// perform crossfade
						tjs_int after_offset =
							before_count * Format.BytesPerSample;
						DoCrossFade(CrossFadeSamples,
							src1, src2, before_count, 0, 50);
						DoCrossFade(CrossFadeSamples + after_offset,
							src1 + after_offset, src2 + after_offset,
								after_count, 50, 100);
						delete [] src1;
						delete [] src2;
						// reset CrossFadePosition and CrossFadeLen
						CrossFadePosition = 0;
						CrossFadeLen = before_count + after_count;
					}
				}
				else
				{
					next_event_pos = link.From;
				}
			}
			else
			{
				// normal jump
				next_event_pos = link.From;
			}
		}
		else
		{
			// event not found
			next_not_found = true;
		}

		tjs_int one_unit;

		if(next_not_found || next_event_pos - Position > samples)
			one_unit = samples;
		else
			one_unit = (tjs_int) (next_event_pos - Position);

		segments.push_back(tTVPWaveLoopSegment(Position, one_unit));
		GetLabelAt(Position, Position + one_unit, labels);

		// decode or copy
		if(!CrossFadeSamples)
		{
			// not crossfade
			// decode direct into destination buffer
			tjs_uint decoded;
			Decoder->Render((void *)d, one_unit, decoded);
			Position += decoded;
			written += decoded;
			if(decoded != (tjs_uint)one_unit)
				break; // must be an internal error but do nothing
			d += decoded * Format.BytesPerSample;
		}
		else
		{
			// in cross fade
			// copy prepared samples
			if(one_unit > CrossFadeLen - CrossFadePosition)
				one_unit = CrossFadeLen - CrossFadePosition;
			memcpy((void *)d,
				CrossFadeSamples +
					CrossFadePosition * Format.BytesPerSample,
				one_unit * Format.BytesPerSample);
			CrossFadePosition += one_unit;
			Position += one_unit;
			written += one_unit;
			d += one_unit * Format.BytesPerSample;
			if(CrossFadePosition == CrossFadeLen)
			{
				// crossfade has finished
				ClearCrossFadeInformation();
			}
		}
	}	// while 
}
//---------------------------------------------------------------------------
bool tTVPWaveLoopManager::GetNearestEvent(tjs_int64 current,
		tTVPWaveLoopLink & link, bool ignore_conditions)
{
	// search nearest event in future, from current.
	// this checks conditions unless ignore_conditions is true.

	if(Links.size() == 0) return false; // there are no event

	if(!IsLinksSorted)
	{
		std::sort(Links.begin(), Links.end());
		IsLinksSorted = true;
	}

	// search nearest event using binary search
	tjs_int s = 0, e = Links.size();
	while(e - s > 1)
	{
		tjs_int m = (s+e)/2;
		if(Links[m].From <= current)
			s = m;
		else
			e = m;
	}

	if((tjs_uint)s >= Links.size() || Links[s].From < current)
	{
		// no links available
		return false;
	}

	if(Links[s].From == current)
	{
		// just current
		;
	}
	else
	{
		// choose next one
		s++;
	}

	if(!ignore_conditions)
	{
		do
		{
			// check condition
			if(Links[s].CondVar != -1)
			{
				if(Links[s].Condition == Flags[Links[s].CondVar])
					break; // condition matched
			}
			else
			{
				break;
			}
			s++;
		} while((tjs_uint)s < Links.size());

		if((tjs_uint)s >= Links.size() || Links[s].From < current)
		{
			// no links available
			return false;
		}
	}

	link = Links[s];

	return true;
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::GetLabelAt(tjs_int64 from, tjs_int64 to,
		std::vector<tTVPWaveLabel> & labels)
{
	if(Labels.size() == 0) return; // no labels found

	if(!IsLabelsSorted)
	{
		std::sort(Labels.begin(), Labels.end());
		IsLabelsSorted = true;
	}

	// search nearest label using binary search
	tjs_int s = 0, e = Labels.size();
	while(e - s > 1)
	{
		tjs_int m = (s+e)/2;
		if(Labels[m].Position <= from)
			s = m;
		else
			e = m;
	}

	if((tjs_uint)s >= Labels.size() || Labels[s].Position < from)
	{
		// no labels available
		return;
	}

	// search labels
	for(; s < (int)Labels.size(); s++)
	{
		if(Labels[s].Position >= from && Labels[s].Position < to)
			labels.push_back(Labels[s]);
		else
			break;
	}
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::DoCrossFade(void *dest, void *src1,
	void *src2, tjs_int samples, tjs_int ratiostart, tjs_int ratioend)
{
	// do on-memory wave crossfade
	// using src1 (fading out) and src2 (fading in).

	if(Format.IsFloat)
	{
		float blend_step =
			(float)((ratioend - ratiostart) / 100.0 / samples);
		const float *s1 = (const float *)src1;
		const float *s2 = (const float *)src2;
		float *out = (float *)dest;
		float ratio = ratiostart / 100.0;
		for(tjs_int i = 0; i < samples; i++)
		{
			for(tjs_int j = Format.Channels - 1; j >= 0; j--)
			{
				*out = *s1 + (*s2 - *s1) * ratio;
				s1 ++;
				s2 ++;
				out ++;
			}
			ratio += blend_step;
		}
	}
	else
	{
		if(Format.BytesPerSample == 1)
		{
			TVPCrossFadeIntegerBlend<tTVPPCM8>(dest, src1, src2,
				samples, ratiostart, ratioend, Format.Channels);
		}
		else if(Format.BytesPerSample == 2)
		{
			TVPCrossFadeIntegerBlend<tjs_int16>(dest, src1, src2,
				samples, ratiostart, ratioend, Format.Channels);
		}
		else if(Format.BytesPerSample == 3)
		{
			TVPCrossFadeIntegerBlend<tTVPPCM24>(dest, src1, src2,
				samples, ratiostart, ratioend, Format.Channels);
		}
		else if(Format.BytesPerSample == 4)
		{
			TVPCrossFadeIntegerBlend<tjs_int32>(dest, src1, src2,
				samples, ratiostart, ratioend, Format.Channels);
		}
	}
}
//---------------------------------------------------------------------------
void tTVPWaveLoopManager::ClearCrossFadeInformation()
{
	if(CrossFadeSamples) delete [] CrossFadeSamples, CrossFadeSamples = NULL;
}
//---------------------------------------------------------------------------





