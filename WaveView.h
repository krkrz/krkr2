//---------------------------------------------------------------------------

#ifndef WaveViewH
#define WaveViewH

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TWaveReader;
class TWaveDrawer : public TGraphicControl
{
private:
	TWaveReader * FReader;
	int FMagnify; // magnification level (in logarithm based 2)
	int FStart; // start sample position at left edge of the client
	int FMinRulerMajorWidth;
	int FMinRulerMajorHeight;
	int FRulerUnit; // time in msec/10
	bool FDrawRuler; // whether to draw time line
	int GetHeadSize() { return FDrawRuler ? (FMinRulerMajorHeight + 1) : 0; }


	int FMarkerPos; // marker position for current playing position, -1 for invisible

public:
	__fastcall TWaveDrawer(Classes::TComponent* AOwner);
	__fastcall ~TWaveDrawer();
public:
	int __fastcall PixelToSample(int pixel);
	int __fastcall SampleToPixel(int sample);

private:
	void __fastcall SetReader(TWaveReader * reader);
	void __fastcall SetStart(int n);
	void __fastcall SetMagnify(int m);
	void __fastcall SetMarkerPos(int p);

	void __fastcall DrawWave(int start, bool clear);
	void __fastcall Paint(void);


public:
	__property TWaveReader * Reader = { read = FReader, write = SetReader };
	__property int Start = { read = FStart, write = SetStart };
	__property int Magnify = { read = FMagnify, write = SetMagnify };
	__property int MarkerPos = { read = FMarkerPos, write = SetMarkerPos };

};
//---------------------------------------------------------------------------
#endif
