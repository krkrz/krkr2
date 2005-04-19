#ifndef WaveLoopManagerH
#define WaveLoopManagerH
//---------------------------------------------------------------------------

#include "tjsTypes.h"
#include "WaveReader.h"
#include <vector>

#define TVP_WL_MAX_FLAGS 16

#define TVP_WL_SMOOTH_TIME 50
#define TVP_WL_SMOOTH_TIME_HALF (TVP_WL_SMOOTH_TIME/2)

//---------------------------------------------------------------------------
struct tTVPWaveLoopLink
{
	tjs_int64 From;		// 'From' in sample position
	tjs_int64 To;		// 'To' in sample position
	bool Smooth;		// Smooth transition (uses short 50ms crossfade)
	bool Condition;		// Condition / false:jump if not set   true:jump if set
	tjs_int CondVar;	// Condition variable / -1:none   >=0:variables

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
	tTVPWaveFormat Format;
	tTVPWaveDecoder * Decoder;

	tjs_int ShortCrossFadeHalfSamples;
		// TVP_WL_SMOOTH_TIME_HALF in sample unit

	tjs_int64 Position; // decoding position

	tjs_uint8 *CrossFadeSamples; // sample buffer for crossfading
	tjs_int CrossFadeLen;
	tjs_int CrossFadePosition;

	bool IsLinksSorted; // false if links is not yet sorted


public:
	tTVPWaveLoopManager(tTVPWaveDecoder * decoder);
	virtual ~tTVPWaveLoopManager();

	bool GetFlag(tjs_int index);
	void SetFlag(tjs_int index, bool f);
	void ClearFlags();

	tjs_int64 GetPosition() const;
	void SetPosition(tjs_int64 pos);

	void Decode(void *dest, tjs_uint samples, tjs_uint &written,
		std::vector<tTVPWaveLoopSegment> &segments);

private:
	bool GetNearestEvent(tjs_int64 current,
		tTVPWaveLoopLink & link, bool ignore_conditions);

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

