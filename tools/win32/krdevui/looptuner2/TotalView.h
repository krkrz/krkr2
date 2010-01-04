//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef TotalViewH
#define TotalViewH

#include "WaveLoopManager.h"

#include <vector>
#include <deque>


//---------------------------------------------------------------------------
class TWaveReader;
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TTotalViewPointEvent)(TObject *Sender, int pos);
//---------------------------------------------------------------------------
class TThumbnailThread;
class TTotalView : public TCustomControl
{
	typedef TCustomControl inherited;

//-- constructor and destructor
public:
	__fastcall TTotalView(Classes::TComponent* AOwner);
	__fastcall ~TTotalView();

private:
	double FMagnify;

	TWaveReader * FReader;
	void __fastcall SetReader(TWaveReader * reader);

	TThumbnailThread *FThumbnailer;
	Graphics::TBitmap * WaveCache;

public:
	void __fastcall ResetAll();
	__property TWaveReader * Reader = { read = FReader, write = SetReader };

protected:
	void __fastcall Paint(void);
	DYNAMIC void __fastcall Resize();
	void __fastcall RecreateWaveCache();
	void __fastcall RecreateWaveCacheBitmap();


//-- view range
	int FViewStart;
	int FViewLength;

protected:
	void DrawViewRange();
	void InvalidateViewRange();

public:
	void SetViewRange(int start, int length);


//-- caret
	int FCaretPos;
	bool FShowCaret;

protected:
	void DrawCaret();
	void InvalidateCaret();

public:
	void SetCaretState(int caretpos, bool showcaret);

//-- marker
	int FMarkerPos;
	bool FShowMarker;

protected:
	void DrawMarker();
	void InvalidateMarker();

public:
	void SetMarkerState(int markerpos, bool showmarker);


//-- links and labels
	std::vector<tTVPWaveLoopLink> Links;
	std::vector<tTVPWaveLabel> Labels;

protected:
	void DrawLinksAndLabels();

public:
	void SetLinks(const std::vector<tTVPWaveLoopLink> & links);
	void SetLabels(const std::vector<tTVPWaveLabel> & labels);

//-- input events
private:
	TTotalViewPointEvent FOnPoint;
	TTotalViewPointEvent FOnDoubleClick;

	bool FMouseDowned;
	int LastClickedPos;

protected:
	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseUp(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall DblClick(void);

public:
	__property TTotalViewPointEvent OnPoint = { read = FOnPoint, write = FOnPoint };
	__property TTotalViewPointEvent OnDoubleClick = { read = FOnDoubleClick, write = FOnDoubleClick };


};
//---------------------------------------------------------------------------
#endif
