#ifndef WaveLoopManagerH
#define WaveLoopManagerH
//---------------------------------------------------------------------------

#include "tjsTypes.h"
#include "WaveReader.h"
#include <vector>
#include <string>

#define TVP_WL_MAX_FLAGS 16

#define TVP_WL_SMOOTH_TIME 50
#define TVP_WL_SMOOTH_TIME_HALF (TVP_WL_SMOOTH_TIME/2)

//---------------------------------------------------------------------------
#ifdef TVP_IN_LOOP_TUNER
	#define LABEL_STRING_TYPE  AnsiString
#else
	#define LABEL_STRING_TYPE  ttstr
#endif
//---------------------------------------------------------------------------
struct tTVPWaveLoopLink
{
	tjs_int64 From;		// 'From' in sample position
	tjs_int64 To;		// 'To' in sample position
	bool Smooth;		// Smooth transition (uses short 50ms crossfade)
	bool Condition;		// Condition / false:jump if not set   true:jump if set
	tjs_int CondVar;	// Condition variable / -1:none   >=0:variables
#ifdef TVP_IN_LOOP_TUNER
	// these are only used by the loop tuner
	tjs_int FromTier;	// display tier of vertical 'from' line
	tjs_int LinkTier;	// display tier of horizontal link
	tjs_int ToTier;		// display tier of vertical 'to' allow line
#endif
};

bool inline operator < (const tTVPWaveLoopLink & lhs, const tTVPWaveLoopLink & rhs)
{
	if(lhs.From < rhs.From) return true;
	if(lhs.From == rhs.From)
	{
		// give priority to conditional expression
		return lhs.CondVar > rhs.CondVar;
	}
	return false;
}
//---------------------------------------------------------------------------
struct tTVPWaveLabel
{
	tjs_int64 Position; // label position
	LABEL_STRING_TYPE Name; // label name
};

bool inline operator < (const tTVPWaveLabel & lhs, const tTVPWaveLabel & rhs)
{
	return lhs.Position < rhs.Position;
}

//---------------------------------------------------------------------------
struct tTVPWaveLoopSegment
{
	tTVPWaveLoopSegment(tjs_int64 start, tjs_int64 length)
		{ Start = start; Length = length; }
	tjs_int64 Start;
	tjs_int64 Length;
};

//---------------------------------------------------------------------------
class tTVPWaveLoopManager
{
	bool Flags[TVP_WL_MAX_FLAGS];
	std::vector<tTVPWaveLoopLink> Links;
	std::vector<tTVPWaveLabel> Labels;
	tTVPWaveFormat Format;
	tTVPWaveDecoder * Decoder;

	tjs_int ShortCrossFadeHalfSamples;
		// TVP_WL_SMOOTH_TIME_HALF in sample unit

	tjs_int64 Position; // decoding position

	tjs_uint8 *CrossFadeSamples; // sample buffer for crossfading
	tjs_int CrossFadeLen;
	tjs_int CrossFadePosition;

	bool IsLinksSorted; // false if links are not yet sorted
	bool IsLabelsSorted; // false if labels are not yet sorted

public:
	tTVPWaveLoopManager(tTVPWaveDecoder * decoder);
	virtual ~tTVPWaveLoopManager();

	bool GetFlag(tjs_int index);
	void SetFlag(tjs_int index, bool f);
	void ClearFlags();

	tjs_int64 GetPosition() const;
	void SetPosition(tjs_int64 pos);

	void Decode(void *dest, tjs_uint samples, tjs_uint &written,
		std::vector<tTVPWaveLoopSegment> &segments,
		std::vector<tTVPWaveLabel> &labels);

private:
	bool GetNearestEvent(tjs_int64 current,
		tTVPWaveLoopLink & link, bool ignore_conditions);

	void GetLabelAt(tjs_int64 from, tjs_int64 to,
		std::vector<tTVPWaveLabel> & labels);

	void DoCrossFade(void *dest, void *src1, void *src2, tjs_int samples,
		tjs_int ratiostart, tjs_int ratioend);

	void ClearCrossFadeInformation();
};
//---------------------------------------------------------------------------


/*
new .sli format

SLI2
Link   828843   203405   Smooth
Link   345394   134234
Link   823985   752653   Smooth,Cond:A=0


*/
#endif

