//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Font Selector Form
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "FontSelectFormUnit.h"
#include "tvpfontstruc.h"
#include "WindowFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTVPFontSelectForm *TVPFontSelectForm;
//---------------------------------------------------------------------------
static TTVPFontSelectForm *TVPCurrentFontSelectForm = NULL;
//---------------------------------------------------------------------------
HDWP TVPShowFontSelectFormTop(HDWP hdwp)
{
	if(TVPCurrentFontSelectForm)
		hdwp = TVPCurrentFontSelectForm->ShowTop(hdwp);
	return hdwp;
}
//---------------------------------------------------------------------------
void TVPShowFontSelectFormAtAppActivate()
{
	if(TVPCurrentFontSelectForm && TVPFullScreenedWindow != NULL)
	{
		TVPCurrentFontSelectForm->InvokeSetVisible();
		TVPCurrentFontSelectForm->InvokeShowTop();
	}
}
//---------------------------------------------------------------------------
void TVPHideFontSelectFormAtAppDeactivate()
{
	if(TVPCurrentFontSelectForm && TVPFullScreenedWindow != NULL)
	{
		TVPCurrentFontSelectForm->Visible = false;
	}
}
//---------------------------------------------------------------------------
int CALLBACK TVPFSFEnumFontsProc(LOGFONT *lplf, TEXTMETRIC *lptm, DWORD type,
	LPARAM data)
{
	// enumerate fonts

	TTVPFontSelectForm *form = reinterpret_cast<TTVPFontSelectForm*>(data);
	if(form->Flags & TVP_FSF_FIXEDPITCH)
	{
		// fixed pitch only ?
		if(lptm->tmPitchAndFamily & TMPF_FIXED_PITCH) return 1;
	}

	if(form->Flags & TVP_FSF_SAMECHARSET)
	{
		// sama character set only ?
		if(lplf->lfCharSet != form->RefFont.lfCharSet) return 1;
	}

	if(form->Flags & TVP_FSF_NOVERTICAL)
	{
		// not to list vertical fonts up ?
		if(lplf->lfFaceName[0] == '@') return 1;
	}

	if(form->Flags & TVP_FSF_TRUETYPEONLY)
	{
		// true type only ?
		if(!(type & TRUETYPE_FONTTYPE)) return 1;
	}

	form->ListBox->Items->Add(lplf->lfFaceName);

	return 1;
}
//---------------------------------------------------------------------------
__fastcall TTVPFontSelectForm::TTVPFontSelectForm(TComponent* Owner, TCanvas *RefCanvas,
	int flags, AnsiString caption, AnsiString prompt, AnsiString samplestring)
	: TForm(Owner)
{
	Caption = caption;
	Label1->Caption = prompt;
	Flags = flags;
	Memo->Text = samplestring;
	GetObject(RefCanvas->Font->Handle, sizeof(LOGFONT), &RefFont);

	int itemheight = RefCanvas->Font->Height;
	if(itemheight < 0) itemheight = -itemheight;
	if(flags & TVP_FSF_USEFONTFACE)
	{
		ListBox->Style = lbOwnerDrawFixed;
		ListBox->ItemHeight = itemheight; // listbox item height
	}

	Memo->Font->Height = -itemheight;

	::EnumFonts(RefCanvas->Handle, NULL, (int (__stdcall *)())TVPFSFEnumFontsProc,
		reinterpret_cast<LPARAM>(this));


	int n = ListBox->Items->IndexOf(RefFont.lfFaceName);
	if(n!=-1)
	{
		ListBox->ItemIndex = n;
	}

	TVPCurrentFontSelectForm = this;
		// the application won't have multiple instance of this class
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::FormDestroy(TObject *Sender)
{
	TVPCurrentFontSelectForm = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::WMShowTop(TMessage &Msg)
{
	if(Visible)
	{
		SetZOrder(true);
		SetWindowPos(Handle, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREPOSITION|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::WMSetVisible(TMessage &Msg)
{
	Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::ListBoxDrawItem(TWinControl *Control,
	  int Index, TRect &Rect, TOwnerDrawState State)
{
	TListBox *listbox = (TListBox*)Control;
	listbox->Canvas->FillRect(Rect);

	// draw letter as font name
	listbox->Canvas->Font->Name = listbox->Items->Strings[Index];
	listbox->Canvas->Font->Height = listbox->ItemHeight;
	listbox->Canvas->TextOut( Rect.Left, Rect.Top, listbox->Items->Strings[Index]);
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::OKButtonClick(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::ListBoxClick(TObject *Sender)
{
	if(ListBox->ItemIndex != -1)
	{
		Memo->Font->Name = ListBox->Items->Strings[ListBox->ItemIndex];
		OKButton->Enabled=true;
		FontName = Memo->Font->Name;
	}
	else
	{
		OKButton->Enabled=false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::ListBoxDblClick(TObject *Sender)
{
	OKButton->Click();
}
//---------------------------------------------------------------------------
void __fastcall TTVPFontSelectForm::FormShow(TObject *Sender)
{
	// to invoke WMShowTop
	InvokeShowTop();
}
//---------------------------------------------------------------------------
void TTVPFontSelectForm::InvokeShowTop()
{
	::PostMessage(Handle, WM_USER + 0x30, 0, 0);
}
//---------------------------------------------------------------------------
void TTVPFontSelectForm::InvokeSetVisible()
{
	::PostMessage(Handle, WM_USER + 0x31, 0, 0);
}
//---------------------------------------------------------------------------
HDWP TTVPFontSelectForm::ShowTop(HDWP hdwp)
{
	if(Visible)
	{
		hdwp = DeferWindowPos(hdwp, Handle, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOMOVE|
			SWP_NOREPOSITION|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
	return hdwp;
}
//---------------------------------------------------------------------------


