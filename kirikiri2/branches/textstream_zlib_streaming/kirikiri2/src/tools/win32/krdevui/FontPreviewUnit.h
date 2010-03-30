//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef FontPreviewUnitH
#define FontPreviewUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFontPreviewForm;
class TFontPreviewBox : public TCustomControl
{
	typedef TCustomControl inherited;

	TFontPreviewForm *Form;
	TList *RedrawList;

	int Cols;

public:
	__fastcall TFontPreviewBox(TWinControl *Owner);
	__fastcall ~TFontPreviewBox();


protected:
	int __fastcall GetOneSize();
	void __fastcall CreateParams(TCreateParams &params);
	void __fastcall Paint(void);
	void __fastcall RedrawGlyphs();
	void __fastcall ApplicationIdle(System::TObject* Sender, bool &Done);
	void __fastcall SetScrollRange(void);
	void __fastcall WMVScroll(TWMVScroll &msg);
	void __fastcall WMMouseMove(TWMMouseMove &msg);
	DYNAMIC void __fastcall Resize(void);
	int __fastcall GetTopIndex();

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_VSCROLL, TWMVScroll, WMVScroll)
	VCL_MESSAGE_HANDLER(WM_MOUSEMOVE, TWMMouseMove, WMMouseMove)
END_MESSAGE_MAP(TCustomControl)

	bool CanResizeUpdate;

public:
	void __fastcall NotifyFontChanged();
};
//---------------------------------------------------------------------------
class TFontPreviewForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPanel *UpperPanel;
	TComboBox *ZoomComboBox;
	TButton *CloseButton;
	TStatusBar *StatusBar;
	TPanel *PreviewPanel;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ZoomComboBoxChange(TObject *Sender);
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
          TPoint &MousePos, bool &Handled);
private:	// ユーザー宣言
	TFontPreviewBox * PreviewBox;
	TCanvas *RefCanvas;
	TCanvas *PrivRefCanvas; // for private (GAIJI) area
	bool Antialiased;

public:		// ユーザー宣言
	__fastcall TFontPreviewForm(TComponent* Owner, TCanvas *refcanvas,
		TCanvas *privrefcanvas);


public:
	void __fastcall NotifyFontChanged(bool antialiased);
	int __fastcall GetZoom() const;

	TCanvas * GetRefCanvas() const { return RefCanvas; }
	TCanvas * GetPrivRefCanvas() const { return PrivRefCanvas; }
	bool GetAntialiased() const { return Antialiased; }
	const wchar_t * GetCharacterList() const;
	int GetCharacterListCount() const;

private:
};
//---------------------------------------------------------------------------
extern PACKAGE TFontPreviewForm *FontPreviewForm;
//---------------------------------------------------------------------------
#endif
