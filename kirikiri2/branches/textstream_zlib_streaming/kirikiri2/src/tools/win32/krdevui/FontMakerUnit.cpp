//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FontMakerUnit.h"
#include "FontMakerWriteUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFontMakerForm *FontMakerForm;

DWORD WINAPI (*procGetFontUnicodeRanges)(HDC, LPGLYPHSET);
	// GetFontUnicodeRanges function ( only supporter on Win2000 or later )

DWORD WINAPI (*procGetGlyphIndicesW)(HDC, LPCWSTR, int, LPWORD, DWORD);
	// GetGlyphIndicesW

DWORD WINAPI (*procGetGlyphOutlineW)(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2 *);
	// GetGlyphOutlineW function ( WinNT or Win9x ( but 95 is not supported ) )

BOOL  WINAPI (*procTextOutW)(HDC, int, int, LPCWSTR, int);
	// TextOutW

BOOL  APIENTRY (*procGetTextExtentPoint32W)(HDC, LPCWSTR, int, LPSIZE);
	// GetTextExtentPoint32W

//---------------------------------------------------------------------------
int CALLBACK FontEnumProc(
			ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
			NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
			int FontType,             // type of font
			LPARAM userdata)
{
	TFontMakerForm *form = reinterpret_cast<TFontMakerForm*>(userdata);

	// enumerate fonts
	// true type only
	bool is_outline =
		(lpntme->ntmTm.ntmFlags &  NTM_PS_OPENTYPE) ||
		(lpntme->ntmTm.ntmFlags &  NTM_TT_OPENTYPE) ||
		(FontType & TRUETYPE_FONTTYPE);
	if(!is_outline) return 1;

	if(!form->ShowAllFontsCheckBox->Checked)
	{
		if(lpelfe->elfLogFont.lfCharSet != SHIFTJIS_CHARSET) return 1;
	}

	if(form->FontSelectComboBox->Items->IndexOf(lpelfe->elfLogFont.lfFaceName) == -1)
		form->FontSelectComboBox->Items->Add(lpelfe->elfLogFont.lfFaceName);

	return 1;
}

//---------------------------------------------------------------------------
__fastcall TFontMakerForm::TFontMakerForm(TComponent* Owner)
	: TForm(Owner)
{
	FontPreviewForm = NULL;
	CharacterListInvalidated = true;
	CharacterList = new wchar_t [65536*2];
	PrivRefControl = new TPrivRefControl(this);
	PrivRefControl->Parent = this;
	PrivRefControl->Left = -1;
	PrivRefControl->Top = -1;
	PrivRefControl->Width = 1;
	PrivRefControl->Height = 1;
	PrivRefControl->Visible = true;

	HMODULE gdi32dll = GetModuleHandle("gdi32.dll");
	*(void**)&procGetFontUnicodeRanges =
		GetProcAddress(gdi32dll, "GetFontUnicodeRanges");
	*(void**)&procGetGlyphIndicesW =
		GetProcAddress(gdi32dll, "GetGlyphIndicesW");
	*(void**)&procGetGlyphOutlineW =
		GetProcAddress(gdi32dll, "GetGlyphOutlineW");
	*(void**)&procTextOutW =
		GetProcAddress(gdi32dll, "TextOutW");
	*(void**)&procGetTextExtentPoint32W =
		GetProcAddress(gdi32dll, "GetTextExtentPoint32W");

	if(Win32Platform != VER_PLATFORM_WIN32_NT)
		procGetGlyphOutlineW = NULL; // ?? this cannot be used ...

	if(!procTextOutW || !procGetTextExtentPoint32W)
		throw Exception("Windows95 (or OS which does not support wide character code API) is not supported on this tool!");

	BoldCheckBox->Enabled = (Win32Platform == VER_PLATFORM_WIN32_NT);
	IncludeAllFontCharsCheckBox->Enabled =
		(procGetFontUnicodeRanges && procGetGlyphOutlineW && procGetGlyphIndicesW);
	IncludePrivateCharsCheckBox->Enabled =
		(procGetFontUnicodeRanges && procGetGlyphOutlineW && procGetGlyphIndicesW);

	EnumFonts();

	LOGFONT font;
	SetFontToCanvas(font);
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::FormDestroy(TObject *Sender)
{
	if(FontPreviewForm) delete FontPreviewForm;
	delete [] CharacterList;
	delete PrivRefControl;
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::EnumFonts()
{
	FontSelectComboBox->Items->Clear();

	LOGFONT l;
	l.lfHeight = -12;
	l.lfWidth = 0;
	l.lfEscapement = 0;
	l.lfOrientation = 0;
	l.lfWeight = 400;
	l.lfItalic = FALSE;
	l.lfUnderline = FALSE;
	l.lfStrikeOut = FALSE;
	l.lfCharSet = DEFAULT_CHARSET;
	l.lfOutPrecision = OUT_DEFAULT_PRECIS;
	l.lfQuality = DEFAULT_QUALITY;
	l.lfPitchAndFamily = 0;
	l.lfFaceName[0] = '\0';

	::EnumFontFamiliesEx(Canvas->Handle, &l, (FONTENUMPROC)FontEnumProc,
		reinterpret_cast<LPARAM>(this), 0);

	TStringList *list = new TStringList();
	list->Assign(FontSelectComboBox->Items);
	list->Sort();
	FontSelectComboBox->Items->Assign(list);
	delete list;

	int n = FontSelectComboBox->Items->IndexOf(Canvas->Font->Name);
	if(n!=-1)
	{
		FontSelectComboBox->ItemIndex = n;
	}
	else
	{
		int n = FontSelectComboBox->Items->IndexOf(CreateButton->Font->Name);
		if(n != -1)
			FontSelectComboBox->ItemIndex = n;
		else
			FontSelectComboBox->ItemIndex = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::OutputRefButtonClick(TObject *Sender)
{
	if(SaveDialog->Execute())
	{
		OutputEdit->Text = SaveDialog->FileName;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::CreateButtonClick(TObject *Sender)
{
	if(FontSelectComboBox->ItemIndex == -1) return;

	if(OutputEdit->Text == "")
	{
		OutputRefButtonClick(this);
		if(OutputEdit->Text == "") return;
	}

	// create font
	LOGFONT font;
	SetFontToCanvas(font);

	// call process dialog
	TFontMakerWriteForm *form = new
		TFontMakerWriteForm(Application, OutputEdit->Text, Canvas, PrivRefControl->Canvas,
			AntialiasedCheckBox->Checked, font.lfFaceName[0] == '@',
				GetCharacterList(), GetCharacterListCount());
	form->ShowModal();
	delete form;
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::CancelButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::ShowAllFontsCheckBoxClick(TObject *Sender)
{
	EnumFonts();
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::ShowSampleButtonClick(TObject *Sender)
{
	if(!FontPreviewForm)
	{
		FontPreviewForm = new TFontPreviewForm(this, Canvas,
			PrivRefControl->Canvas);
		NotifyFontChanged();
		FontPreviewForm->Visible = true;
	}
	else
	{
		NotifyFontChanged();
		FontPreviewForm->Visible = true;
		FontPreviewForm->BringToFront();
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::FontSelectComboBoxChange(TObject *Sender)
{
	CharacterListInvalidated = true;
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::SizeEditChange(TObject *Sender)
{
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::BoldCheckBoxClick(TObject *Sender)
{
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::AntialiasedCheckBoxClick(TObject *Sender)
{
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::IncludeAllFontCharsCheckBoxClick(
	  TObject *Sender)
{
	CharacterListInvalidated = true;
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::IncludePrivateCharsCheckBoxClick(
	  TObject *Sender)
{
	CharacterListInvalidated = true;
	NotifyFontChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::SetFontToCanvas(LOGFONT & font)
{
	// create font object
	SetPrivFontToPrivCanvas();

	::GetObject(Canvas->Font->Handle, sizeof(font), &font);
	font.lfHeight = -SizeUpDown->Position;
	AnsiString fontname = FontSelectComboBox->Items->Strings[FontSelectComboBox->ItemIndex];
	strcpy(font.lfFaceName, fontname.c_str());
	font.lfEscapement = font.lfOrientation = font.lfFaceName[0] == '@' ? 2700:0;
	font.lfWeight = BoldCheckBox->Checked ? 700:400;
	font.lfCharSet = DEFAULT_CHARSET;
	Canvas->Font->Handle = CreateFontIndirect(&font);
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::SetPrivFontToPrivCanvas()
{
	// create font object for private (GAIJI) area
	LOGFONT font;
	::GetObject(PrivRefControl->Canvas->Font->Handle, sizeof(font), &font);
	font.lfHeight = -SizeUpDown->Position;
	AnsiString fontname = FontSelectComboBox->Items->Strings[FontSelectComboBox->ItemIndex];
	strcpy(font.lfFaceName,  (fontname.c_str()[0] == '@' ? AnsiString('@') + CreateButton->Font->Name :
		CreateButton->Font->Name).c_str());
	font.lfEscapement = font.lfOrientation = font.lfFaceName[0] == '@' ? 2700:0;
	font.lfWeight = BoldCheckBox->Checked ? 700:400;
	font.lfCharSet = DEFAULT_CHARSET;
	PrivRefControl->Canvas->Font->Handle = CreateFontIndirect(&font);
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::NotifyFontChanged()
{
	LOGFONT font;
	SetFontToCanvas(font);
	if(FontPreviewForm) FontPreviewForm->NotifyFontChanged(AntialiasedCheckBox->Checked);
}
//---------------------------------------------------------------------------
int __cdecl TFontMakerForm::CodeComparison(const void *s1, const void *s2)
{
	const wchar_t *c1 = (const wchar_t *)s1;
	const wchar_t *c2 = (const wchar_t *)s2;
	if(*c1 < *c2) return -1;
	if(*c1 == *c2) return 0;
	return 1;
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::PushDBCSCharacter(int code)
{
	// "code" is in DBCS(MBCS) character code
	if(CharacterListCount >= 65536*2) return;

	char ch[3];
	int insize;
	if(code < 256)
		ch[0] = code, ch[1] = 0, insize = 1;
	else
		ch[0] = code >> 8, ch[1] = code, ch[2] = 0, insize = 2;

	wchar_t wch[10];
	int res = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS|MB_PRECOMPOSED,
		ch, insize, wch, 10);

	if(res == 1)
	{
		if(!(!IncludePrivateCharsCheckBox->Checked && (wch[0] >= 0xe000 && wch[0] <= 0xf8ff)))
			CharacterList[CharacterListCount++] = wch[0];
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::PushCharacterRange(int base, int count)
{
	int i;
	int lim = count + base;
	for(i = base; i < lim; i++)
	{
		if(CharacterListCount >= 65536*2) return;
		if(i >= 0x20 && i <= 0xfffd)
			CharacterList[CharacterListCount++] = i;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFontMakerForm::CreateCharacterList()
{
	if(!CharacterListInvalidated) return;

	LOGFONT lf;
	SetFontToCanvas(lf);

	CharacterListCount = 0;

	// DBCS
	for(int i = 0x20; i <= 0xffff; i++) PushDBCSCharacter(i);

	// UNICODE
	if(IncludeAllFontCharsCheckBox->Checked && procGetFontUnicodeRanges)
	{
		GLYPHSET * gs;
		DWORD size = procGetFontUnicodeRanges(Canvas->Handle, NULL);
		gs = (GLYPHSET*) new char[size];
		ZeroMemory(gs, size);
		gs->cbThis = size;
		procGetFontUnicodeRanges(Canvas->Handle, gs);
		if(gs->cGlyphsSupported<=65536 && gs->cRanges >= 1)
		{
			for(unsigned int i = 0; i < gs->cRanges; i++)
			{
				PushCharacterRange(gs->ranges[i].wcLow, gs->ranges[i].cGlyphs);
			}
		}
		delete [] (char*)gs;
	}

	// sort
	qsort(CharacterList, CharacterListCount, sizeof(wchar_t), CodeComparison);

	// purge duplicates
	wchar_t *dp = CharacterList;
	wchar_t *sp = CharacterList;
	wchar_t *splim = CharacterList + CharacterListCount;
	int current = -1;
	while(sp < splim)
	{
		if(*sp != current && *sp && *sp <= 0xfffd)
			*(dp++) = current = *(sp++);
		else
			sp++;
	}
	CharacterListCount = dp - CharacterList;

	// Check existence of glyphs and purge non-existence glyphs
	if(procGetGlyphIndicesW)
	{
		WORD *indices = new WORD[CharacterListCount];
		int cnt = procGetGlyphIndicesW(Canvas->Handle, CharacterList,
			CharacterListCount, indices, GGI_MARK_NONEXISTING_GLYPHS);

		if(cnt != -1)
		{
			WORD *ip = indices;
			sp = CharacterList;
			dp = CharacterList;
			splim = CharacterList + cnt;

			while(sp < splim)
			{
				if(*sp >= 0xe000 && *sp <= 0xf8ff && IncludePrivateCharsCheckBox->Checked)
				{
					// private area
					*(dp++) = *sp;
				}
				else
				{
					// other unicodes
					if(*ip != 0xffff && *sp >= 0x20 && *sp <= 0xfffd) *(dp++) = *sp;
				}
				ip++;
				sp++;
			}

			delete [] indices;

			CharacterListCount = dp - CharacterList;
		}
	}

	// down the flag
	CharacterListInvalidated = false;
}
//---------------------------------------------------------------------------
int __fastcall TFontMakerForm::GetCharacterListCount()
{
	CreateCharacterList();
	return CharacterListCount;
}
//---------------------------------------------------------------------------
const wchar_t * __fastcall TFontMakerForm::GetCharacterList()
{
	CreateCharacterList();
	return CharacterList;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
extern "C" void _export PASCAL UIExecFontMaker()
{
	try
	{
		TFontMakerForm *form = new TFontMakerForm(Application);
		form->ShowModal();
		delete form;
	}
	catch(Exception &e)
	{
		Application->ShowException(&e);
	}
	ExitProcess(0);
}
//---------------------------------------------------------------------------














