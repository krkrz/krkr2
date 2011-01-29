//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <shlobj.h>
#include <FileCtrl.hpp>
#include <Jpeg.hpp>
#include <inifiles.hpp>
#include <memory>

#pragma link "vclx50.lib"

#include "TPCMainUnit.h"
#include "DeePNGUnit.h"
#include "DeePSDUnit.h"
#include "JPEGQualityUnit.h"
#include "LogViewerUnit.h"
#include "ProgressUnit.h"
#include "TLG5Saver.h"
#include "TLG6Saver.h"
#include "TLGMetaInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTPCMainForm *TPCMainForm;
//---------------------------------------------------------------------------
unsigned long MMXReady = 0;
extern "C"
{
	void __fastcall CheckMMX(void)
	{
		MMXReady = false; // umm...
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
static AnsiString SelectFolder(HWND owner)
{
	BROWSEINFO bi;
	bi.hwndOwner = owner;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = "フォルダを選択してください";
	bi.lpszTitle = "フォルダの選択";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	PItemIDList id;

	id = SHBrowseForFolder(&bi);

	if(id != NULL)
	{
		char path[MAX_PATH];
		SHGetPathFromIDList(id, path);
		return AnsiString(path);
	}
	return "";
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// SaveImage
//---------------------------------------------------------------------------
static AnsiString AddImageExtension(AnsiString base, int type)
{
	// type : 0(BMP) 1(JPEG) 2(PNG)
	switch(type)
	{
	case 0: return base + ".bmp";
	case 1: return base + ".jpg";
	case 2: return base + ".png";
	default: return base;
	}
}
//---------------------------------------------------------------------------
static void SaveImage(Graphics::TBitmap *bmp, AnsiString filename, int type, int jpegq, TStringList *tags)
{
	// save the image
	// type : 0(BMP) 1(JPEG) 2(PNG)
	// jpeg : JPEG quality
	// tags : meta information (for PNG)
	// the extension is to be automatically added
	if(type == 0)
	{
		bmp->SaveToFile(filename + ".bmp");
	}
	else if(type == 1)
	{
		TJPEGImage *image = NULL;
		try
		{
			image = new TJPEGImage();
			if(bmp->PixelFormat == pf8bit)
				image->Grayscale = true;
			image->Assign(bmp);
			image->CompressionQuality = jpegq;
			image->SaveToFile(filename + ".jpg");
		}
		catch(...)
		{
			if(image) delete image;
			throw;
		}
		if(image) delete image;
	}
	else if(type == 2)
	{
		TDeePNG *image = new TDeePNG();
		try
		{
			image->Assign(bmp);
			image->SetTags(tags);
			image->SaveToFile(filename + ".png");
		}
		catch(...)
		{
			delete image;
			throw;
		}
		delete image;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// SeparateMainAndMask
//---------------------------------------------------------------------------
static Graphics::TBitmap * SeparateMask(Graphics::TBitmap *bmp)
{
	bmp->PixelFormat = pf32bit;
	Graphics::TBitmap * mask = new Graphics::TBitmap();
	try
	{
		int w, h;
		mask->Width = w = bmp->Width;
		mask->Height = h = bmp->Height;

		SetGrayscalePalette(mask, 8);

		for(int y = 0; y < h; y++)
		{
			unsigned char * dest = (unsigned char *)mask->ScanLine[y];
			DWORD * src = (DWORD *)bmp->ScanLine[y];
			for(int x = 0; x < w; x++) dest[x] = src[x] >> 24;
		}
	}
	catch(...)
	{
		delete mask;
	}

	bmp->PixelFormat = pf24bit;

	return mask;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// ClearFullTransparentColor
//---------------------------------------------------------------------------
static void ClearFullTransparentColor(Graphics::TBitmap *bmp)
{
	// clear the color where the transparency is fully transparent
	// bmp->PixelFormat must be pf32bit

	int w, h;
	w = bmp->Width;
	h = bmp->Height;

	for(int y = 0; y < h; y++)
	{
		DWORD * p = (DWORD *)bmp->ScanLine[y];
		for(int x = 0; x < w; x++)
		{
			DWORD c = p[x];
			if(!(c & 0xff000000)) c = 0;
			p[x] = c;
		}
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// CheckTransparentImage
//---------------------------------------------------------------------------
static bool CheckTransparentImage(Graphics::TBitmap *bmp)
{
	// check whether given image has transparent color
	// bmp must be 32bpp image.

	for(int y = 0; y < bmp->Height; y++)
	{
		DWORD *pixel = (DWORD*)bmp->ScanLine[y];
		DWORD *pixellim = pixel + bmp->Width;
		while(pixel < pixellim)
		{
			if((*pixel & 0xff000000) != 0xff000000) return true;
			pixel ++;
		}
	}
	return false;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// ExpandOpaqueColor
//---------------------------------------------------------------------------
static void ExpandOpaqueColor(Graphics::TBitmap *bmp, int n)
{
	// Expand the opaque color to fully transparent color
	// this algorithm is *very* slow

	// basically, for fully transparent pixel, find the nearest opaque pixel
	// (that is, not fully transparent pixel) and set opaue pixel's color to
	// the fully transparent pixel.

	int nd_start = n*n * 2;
	int w, h;
	w = bmp->Width;
	h = bmp->Height;
	int pitch;

	if(bmp->Height > 1)
		pitch = (DWORD*)bmp->ScanLine[1] - (DWORD*)bmp->ScanLine[0];
	else
		pitch = 0;

	DWORD* pixel = (DWORD*)bmp->ScanLine[0];

	for(int y = 0; y < h; y++)
	{
		DWORD *scl = pixel + y * pitch;
		int yystart = y - n < 0 ? 0 : y - n;
		int yyend = y + n >= h ? h - 1 : y + n;
		for(int x = 0; x < w; x++)
		{
			if(!(scl[x] & 0xff000000))
			{
				// the pixel is fully transparent
				int nearestdist = nd_start;
				int r, g, b;
				int npix;
				int xxstart = x - n < 0 ? 0 : x - n;
				int xxend = x + n >= w ? w - 1 : x + n;
				for(int yy = yystart; yy <= yyend; yy++)
				{
					int dy = yy - y;
					dy *= dy;
					if(dy > nearestdist) continue;
					DWORD *scl = pixel + yy * pitch;
					for(int xx = xxstart; xx <= xxend; xx++)
					{
						DWORD px = scl[xx];
						if(px & 0xff000000)
						{
							// opaque
							int dx = xx - x;
							int dist = dx*dx + dy;
							if(nearestdist > dist)
							{
								npix = 1;
								r = (px&0xff0000) >> 16;
								g = (px&0x00ff00) >> 8;
								b = (px&0x0000ff);
								nearestdist = dist;
							}
							else if(nearestdist == dist)
							{
								npix++;
								r += ((px&0xff0000) >> 16);
								g += ((px&0x00ff00) >> 8 );
								b += ((px&0x0000ff)      );
							}
						}
					}
				}
				if(nearestdist != nd_start)
				{
					// nearest opaque pixel found
					r /= npix;
					g /= npix;
					b /= npix;
					scl[x] = (r << 16) + (g << 8) + b;
				}
				else
				{
					// not found
					scl[x] = 0;
				}
			}

		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// LoadImage
//---------------------------------------------------------------------------
# define COMPOSE_GRAYSCALE(r,g,b) (((b)*19 + (g)*183 + (r)*54)>>8)
static void ToGrayscaleImage(Graphics::TBitmap **in)
{
	// do grayscale

	(*in)->PixelFormat = pf24bit; // once convert to 24bpp bitmap

	Graphics::TBitmap *out = new Graphics::TBitmap();
	try
	{
		int w, h;
		out->PixelFormat = pf8bit;
		out->Width = w = (*in)->Width;
		out->Height = h = (*in)->Height;
		SetGrayscalePalette(out, 8);

		for(int y = 0; y < h; y++)
		{
			const unsigned char * src = (const unsigned char *)(*in)->ScanLine[y];
			unsigned char * dest = (unsigned char *)out->ScanLine[y];
			for(int x = 0; x < w; x++)
			{
				dest[x] = COMPOSE_GRAYSCALE(src[2], src[1], src[0]);
				src += 3;
			}
		}
	}
	catch(...)
	{
		delete out;
		throw;
	}

	delete (*in);
	(*in) = out;
}
//---------------------------------------------------------------------------
static void BindMaskToMain(Graphics::TBitmap * main, Graphics::TBitmap * mask)
{
	// bind 8bpp mask to 32bpp main

	if(main->Width != mask->Width || mask->Height != mask->Height)
		throw Exception("メイン画像とマスク画像のサイズが異なります");

	if(main->PixelFormat != pf32bit) main->PixelFormat = pf32bit;
	if(mask->PixelFormat != pf8bit) mask->PixelFormat = pf8bit;

	int w = main->Width;
	int h = main->Height;

	for(int y = 0; y < h; y++)
	{
		DWORD * m = (DWORD *) main->ScanLine[y];
		const unsigned char * g = (const unsigned char*) mask->ScanLine[y];
		for(int x = 0; x < w; x++)
		{
			DWORD c = m[x];
			c &= 0xffffff;
			c |= (g[x] << 24);
			m[x] = c;
		}
	}
}
//---------------------------------------------------------------------------
static Graphics::TBitmap *LoadSingleImage(AnsiString infile,
	AnsiString & input_type, AnsiString & output_type, TStringList *tags)
{
	// load image
	AnsiString ext = ExtractFileExt(infile).LowerCase();
	Graphics::TBitmap * bmp = new Graphics::TBitmap();
	try
	{
		if(ext == ".bmp")
		{
			bmp->LoadFromFile(infile);
			// all input files is to be assumed as alpha except if there is not specification
			if(input_type == "") input_type = "alpha";
		}
		else if(ext == ".png")
		{
			TDeePNG *png = new TDeePNG();
			try
			{
				png->LoadFromFile(infile);
				png->AppendTags(tags);
				bmp->Assign(png);
			}
			catch(...)
			{
				delete png;
				throw;
			}
			delete png;
			// all input files is to be assumed as alpha except if there is not specification
			if(input_type == "") input_type = "alpha";
		}
		else if(ext == ".jpg" || ext == ".jpeg" || ext == ".jfif")
		{
			TJPEGImage *jpg = new TJPEGImage();
			try
			{
				jpg->Performance = jpBestQuality;
				jpg->LoadFromFile(infile);
				bmp->Assign(jpg);
			}
			catch(...)
			{
				delete jpg;
				throw;
			}
			delete jpg;
			// all input files is to be assumed as alpha except if there is not specification
			if(input_type == "") input_type = "alpha";
		}
		else if(ext == ".psd" || ext == ".pdd")
		{
			TDeePSD *psd = new TDeePSD();
			try
			{
				psd->LayerMode = output_type;
				psd->LoadFromFile(infile);
				bmp->Assign(psd);
				input_type = psd->LayerMode;
			}
			catch(...)
			{
				delete psd;
				throw;
			}
			delete psd;
		}
		else
		{
			throw Exception("認識できない画像形式です");
		}
	}
	catch(...)
	{
		delete bmp;
		throw;
	}

	return bmp;
}
//---------------------------------------------------------------------------
static Graphics::TBitmap * LoadImage(AnsiString infile,
	AnsiString & input_type, AnsiString & output_type, TStringList *tags)
{
	// load image
	Graphics::TBitmap * bmp = LoadSingleImage(infile,
		input_type, output_type, tags);
	Graphics::TBitmap * mask = NULL;

	try
	{
		Graphics::TBitmap *mask = NULL;

		AnsiString maskfile = ChangeFileExt(infile, "") + "_m";
		AnsiString _input_type; // dummy
		AnsiString _output_type; // dummy
		if(FileExists(maskfile + ".bmp"))
			mask = LoadSingleImage(maskfile + ".bmp", _input_type, _output_type, NULL);
		else if(FileExists(maskfile + ".png"))
			mask = LoadSingleImage(maskfile + ".png", _input_type, _output_type, NULL);
		else if(FileExists(maskfile + ".jpg"))
			mask = LoadSingleImage(maskfile + ".jpg", _input_type, _output_type, NULL);
		else if(FileExists(maskfile + ".jpeg"))
			mask = LoadSingleImage(maskfile + ".jpeg", _input_type, _output_type, NULL);

		if(mask)
		{
			ToGrayscaleImage(&mask);
			BindMaskToMain(bmp, mask);
		}

		if(bmp->PixelFormat != pf32bit)
		{
			bmp->PixelFormat = pf24bit;
		}
		else
		{
			if(input_type == "alpha" || input_type == "addalpha")
			{
				if(!CheckTransparentImage(bmp))
				{
					bmp->PixelFormat = pf24bit;
					input_type = "opaque";
				}
			}
		}
		if(output_type == "") output_type = input_type;

	}
	catch(...)
	{
		delete bmp;
		delete mask;
		throw;
	}

	delete mask;

	return bmp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
__fastcall TTPCMainForm::TTPCMainForm(TComponent* Owner)
	: TForm(Owner)
{
	OrgClientHeight = ClientHeight;
	OrgCloseButtonTop = CloseButton->Top;

	TranspMainJPEGQuality = 90;
	TranspMaskJPEGQuality = 90;
	OpaqueJPEGQuality = 90;
	TranspMainFormatComboBox->ItemIndex = 1;
	TranspMaskFormatComboBox->ItemIndex = 1;
	TranspFullTranspColorMethodComboBox->ItemIndex = 6;
	FileList = new TStringList();
	LoadFromIni();
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::FormDestroy(TObject *Sender)
{
	SaveToIni();
	delete FileList;
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::UpdateState()
{
	bool b = TranspSeparatedFormatRadioButton->Checked;
	TranspMainFormatLabel->Font->Color = b ? clWindowText : clGrayText;
	TranspMaskFormatLabel->Font->Color = b ? clWindowText : clGrayText;
	TranspMainFormatComboBox->Enabled = b;
	TranspMaskFormatComboBox->Enabled = b;
	TranspMainJPEGOptionButton->Enabled = b && TranspMainFormatComboBox->ItemIndex == 1;
	TranspMaskJPEGOptionButton->Enabled = b && TranspMaskFormatComboBox->ItemIndex == 1;

	b = SpecifyFolderRadioButton->Checked;
	OutputFolderEdit->Color = b ? clWindow : clBtnFace;
	OutputFolderEdit->Enabled = b;
	OutputFolderRefButton->Enabled = b;

	OpaqueJPEGOptionButton->Enabled = OpaqueJPEGFormatRadioButton->Checked;


	b = TranspAssumeInputIsAddAlphaCheckBox->Checked;
	TranspOutputAddAlphaFormatCheckBox->Enabled = !b;
	if(b)
		TranspOutputAddAlphaFormatCheckBox->Checked = b;
	else
		TranspOutputAddAlphaFormatCheckBox->Checked =
			TranspOutputAddAlphaFormatCheckBox->Tag;

	b = !TranspOutputAddAlphaFormatCheckBox->Checked && !TranspAssumeInputIsAddAlphaCheckBox->Checked;
	TranspFullTranspColorMethodLabel->Font->Color = b ? clWindowText : clGrayText;
	TranspFullTranspColorMethodComboBox->Enabled = b;



	if(!ExpandButton->Checked)
	{
		ClientHeight = ExpandButton->Top + ExpandButton->Height + ExpandButton->Top;
		CloseButton->Top = (ClientHeight - CloseButton->Height)/2;
	}
	else
	{
		CloseButton->Top = OrgCloseButtonTop;
		ClientHeight = OrgClientHeight;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::LoadFromIni()
{
	AnsiString inifilename = ChangeFileExt(ParamStr(0), ".ini");

	if(!FileExists(inifilename)) return;

	TMemIniFile * ini = new TMemIniFile(inifilename);

	ExpandButton->Checked = ini->ReadBool("Window", "Expand", true);

	OpaqueBMPFormatRadioButton->Checked = ini->ReadBool("Format", "OpaqueBMP", false);
	OpaquePNGFormatRadioButton->Checked = ini->ReadBool("Format", "OpaquePNG", true);
	OpaqueTLG5FormatRadioButton->Checked = ini->ReadBool("Format", "OpaqueTLG5", false);
	OpaqueTLG6FormatRadioButton->Checked = ini->ReadBool("Format", "OpaqueTLG6", false);
	OpaqueJPEGFormatRadioButton->Checked = ini->ReadBool("Format", "OpaqueJPEG", false);
	OpaqueJPEGQuality = ini->ReadInteger("Format", "OpaqueJPEGQuality", 90);

	TranspBMPFormatRadioButton->Checked = ini->ReadBool("Format", "TranspBMP", false);
	TranspPNGFormatRadioButton->Checked = ini->ReadBool("Format", "TranspPNG", true);
	TranspTLG5FormatRadioButton->Checked = ini->ReadBool("Format", "TranspTLG5", true);
	TranspTLG6FormatRadioButton->Checked = ini->ReadBool("Format", "TranspTLG6", true);
	TranspSeparatedFormatRadioButton->Checked = ini->ReadBool("Format", "TranspSeparated", false);
	TranspMainFormatComboBox->ItemIndex = ini->ReadInteger("Format", "TranspMainFormat", 1);
	TranspMaskFormatComboBox->ItemIndex = ini->ReadInteger("Format", "TranspMaskFormat", 1);
	TranspMainJPEGQuality = ini->ReadInteger("Format", "TranspMainJPEGQuality", 90);
	TranspMaskJPEGQuality = ini->ReadInteger("Format", "TranspMaskJPEGQuality", 90);
	TranspAssumeInputIsAddAlphaCheckBox->Checked = ini->ReadBool("Format",
		"TranspAssumeInputIsAddAlpha", false);
	TranspOutputAddAlphaFormatCheckBox->Checked = ini->ReadBool("Format",
		"TranspOutputAddAlphaFormat", false);
	TranspFullTranspColorMethodComboBox->ItemIndex = ini->ReadInteger("Format",
		"TranspFullTranspColorMethod", 0);

	SameFolderRadioButton->Checked = ini->ReadBool("Output", "SameFolder", true);
	SpecifyFolderRadioButton->Checked = ini->ReadBool("Output", "SpecifyFolder", false);
	OutputFolderEdit->Text = ini->ReadString("Output", "OutputFolder", "");
	OverwriteCheckBox->Checked = ini->ReadBool("Output", "Overwrite", false);

	DoNotShowLogWhenNoErrorCheckBox->Checked = ini->ReadBool("Log",
		"DoNotShowLogWhenNoError", false);

	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::SaveToIni()
{
	TMemIniFile * ini = new TMemIniFile(ChangeFileExt(ParamStr(0), ".ini"));

	ini->WriteBool("Window", "Expand", ExpandButton->Checked);

	ini->WriteBool("Format", "OpaqueBMP", OpaqueBMPFormatRadioButton->Checked);
	ini->WriteBool("Format", "OpaquePNG", OpaquePNGFormatRadioButton->Checked);
	ini->WriteBool("Format", "OpaqueTLG5", OpaqueTLG5FormatRadioButton->Checked);
	ini->WriteBool("Format", "OpaqueTLG6", OpaqueTLG6FormatRadioButton->Checked);
	ini->WriteBool("Format", "OpaqueJPEG", OpaqueJPEGFormatRadioButton->Checked);
	ini->WriteInteger("Format", "OpaqueJPEGQuality", OpaqueJPEGQuality);

	ini->WriteBool("Format", "TranspBMP", TranspBMPFormatRadioButton->Checked);
	ini->WriteBool("Format", "TranspPNG", TranspPNGFormatRadioButton->Checked);
	ini->WriteBool("Format", "TranspTLG5", TranspTLG5FormatRadioButton->Checked);
	ini->WriteBool("Format", "TranspTLG6", TranspTLG6FormatRadioButton->Checked);
	ini->WriteBool("Format", "TranspSeparated", TranspSeparatedFormatRadioButton->Checked);
	ini->WriteInteger("Format", "TranspMainFormat", TranspMainFormatComboBox->ItemIndex);
	ini->WriteInteger("Format", "TranspMaskFormat", TranspMaskFormatComboBox->ItemIndex);
	ini->WriteInteger("Format", "TranspMainJPEGQuality", TranspMainJPEGQuality);
	ini->WriteInteger("Format", "TranspMaskJPEGQuality", TranspMaskJPEGQuality);
	ini->WriteBool("Format", "TranspAssumeInputIsAddAlpha", TranspAssumeInputIsAddAlphaCheckBox->Checked);
	ini->WriteBool("Format", "TranspOutputAddAlphaFormat", TranspOutputAddAlphaFormatCheckBox->Checked);
	ini->WriteInteger("Format", "TranspFullTranspColorMethod", TranspFullTranspColorMethodComboBox->ItemIndex);

	ini->WriteBool("Output", "SameFolder", SameFolderRadioButton->Checked);
	ini->WriteBool("Output", "SpecifyFolder", SpecifyFolderRadioButton->Checked);
	ini->WriteString("Output", "OutputFolder", OutputFolderEdit->Text);
	ini->WriteBool("Output", "Overwrite", OverwriteCheckBox->Checked);

	ini->WriteBool("Log", "DoNotShowLogWhenNoError",
		DoNotShowLogWhenNoErrorCheckBox->Checked);

	ini->UpdateFile();
	delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TTPCMainForm::ExpandButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::OpaqueBMPFormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TTPCMainForm::OpaquePNGFormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TTPCMainForm::OpaqueTLG5FormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::OpaqueTLG6FormatRadioButtonClick(
	  TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TTPCMainForm::OpaqueJPEGFormatRadioButtonClick(
      TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspBMPFormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspPNGFormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspTLG5FormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspTLG6FormatRadioButtonClick(
	  TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspSeparatedFormatRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspMainFormatComboBoxChange(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TTPCMainForm::TranspMaskFormatComboBoxChange(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspAssumeInputIsAddAlphaCheckBoxClick(
	  TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspOutputAddAlphaFormatCheckBoxClick(TObject *Sender)
{
	if(TranspOutputAddAlphaFormatCheckBox->Enabled)
	{
		TranspOutputAddAlphaFormatCheckBox->Tag =
			TranspOutputAddAlphaFormatCheckBox->Checked;
		UpdateState();
	}
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::SameFolderRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TTPCMainForm::SpecifyFolderRadioButtonClick(TObject *Sender)
{
	UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::OpaqueJPEGOptionButtonClick(TObject *Sender)
{
	ShowJPEGQualityForm(this, OpaqueJPEGQuality);
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspMainJPEGOptionButtonClick(TObject *Sender)
{
	ShowJPEGQualityForm(this, TranspMainJPEGQuality);
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::TranspMaskJPEGOptionButtonClick(TObject *Sender)
{
	ShowJPEGQualityForm(this, TranspMaskJPEGQuality);
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::CloseButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::FormShow(TObject *Sender)
{
	DragAcceptFiles(Handle, true);
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::OutputFolderRefButtonClick(TObject *Sender)
{
	AnsiString newfolder = SelectFolder(Handle);
	if(newfolder != "") OutputFolderEdit->Text = newfolder;
}
//---------------------------------------------------------------------------
void __fastcall TTPCMainForm::WMDropFiles(TMessage &Msg)
{
	HDROP hd = (HDROP)Msg.WParam;

	char filename[ MAX_PATH ];
	int filecount=
		DragQueryFile(hd, 0xFFFFFFFF, NULL, MAX_PATH);

	FileList->Clear();
	Log = "";

	try
	{
		for(int i = filecount-1; i>=0; i--)
		{
			DragQueryFile(hd, i, filename, MAX_PATH);

			WIN32_FIND_DATA fd;
			HANDLE h;

			// existence checking
			if((h = FindFirstFile(filename, &fd)) != INVALID_HANDLE_VALUE)
			{
				FindClose(h);

				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;


				AnsiString base = ChangeFileExt(ExtractFileName(filename), "");
				int baselen = base.Length();
				if(baselen < 3 || strcmpi(base.c_str() + baselen - 2, "_m"))
					FileList->Add(filename);
			}

		}
		DragFinish(hd);

		Application->BringToFront();
		BringToFront();

		TProgressForm *progform = new TProgressForm(this, FileList);
		progform->ShowModal();
		bool success = progform->GetSucceeded();
		delete progform;

		if(!success || !DoNotShowLogWhenNoErrorCheckBox->Checked)
		{
			TLogViewerForm *logviewer = new TLogViewerForm(this);
			logviewer->LogEdit->Text = Log;
			logviewer->ShowModal();
			delete logviewer;
		}
	}
	catch(...)
	{
		throw;
	}
}
//---------------------------------------------------------------------------
const char * NotOverwritten = "同名のファイルがあるために上書きしませんでした : ";
bool __fastcall TTPCMainForm::ProcessFile(AnsiString infile)
{
	// process a file
	Graphics::TBitmap *inbmp = NULL;
	bool error = false;

	try
	{
		// change current directory
		SetCurrentDir(ExtractFileDir(infile));

		Log += "変換中 " + ExtractFileName(infile) + " ...\n";

		// check for output files
		AnsiString out_base = ChangeFileExt(ExtractFileName(infile), "");
		AnsiString out_dir;
		if(SameFolderRadioButton->Checked)
		{
			// current directory
			out_dir = ".";
		}
		else
		{
			// specified output directory
			out_dir = ExcludeTrailingBackslash(OutputFolderEdit->Text);
			ForceDirectories(ExpandFileName(out_dir));
		}

		out_base = out_dir + "/" + out_base;



		// read input
		AnsiString input_type   = TranspAssumeInputIsAddAlphaCheckBox->Checked ? "addalpha" : "";
		AnsiString output_type  = TranspOutputAddAlphaFormatCheckBox->Checked ? "addalpha" : "";
		std::auto_ptr<TStringList> tags(new TStringList());
		inbmp = LoadImage(infile, input_type, output_type, tags.get());

		// overwrite check
		if(!OverwriteCheckBox->Checked)
		{
			if(inbmp->PixelFormat == pf32bit)
			{
				if(TranspBMPFormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".bmp"))
						throw Exception(NotOverwritten + out_base + ".bmp");
				}
				else if(TranspPNGFormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".png"))
						throw Exception(NotOverwritten + out_base + ".png");
				}
				else if(TranspTLG5FormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".tlg"))
						throw Exception(NotOverwritten + out_base + ".tlg");
				}
				else if(TranspTLG6FormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".tlg"))
						throw Exception(NotOverwritten + out_base + ".tlg");
				}
				else if(TranspSeparatedFormatRadioButton->Checked)
				{
					// main/mask separated type
					if(FileExists(AddImageExtension(out_base, TranspMainFormatComboBox->ItemIndex)))
						throw Exception(NotOverwritten +
							AddImageExtension(out_base, TranspMainFormatComboBox->ItemIndex));

					if(FileExists(AddImageExtension(out_base + "_m", TranspMaskFormatComboBox->ItemIndex)))
						throw Exception(NotOverwritten +
							AddImageExtension(out_base + "_m", TranspMaskFormatComboBox->ItemIndex));
				}
			}
			else
			{
				if(OpaqueBMPFormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".bmp"))
						throw Exception(NotOverwritten + out_base + ".bmp");
				}
				else if(OpaquePNGFormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".png"))
						throw Exception(NotOverwritten + out_base + ".png");
				}
				else if(OpaqueTLG5FormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".tlg"))
						throw Exception(NotOverwritten + out_base + ".tlg");
				}
				else if(OpaqueTLG6FormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".tlg"))
						throw Exception(NotOverwritten + out_base + ".tlg");
				}
				else if(OpaqueJPEGFormatRadioButton->Checked)
				{
					if(FileExists(out_base + ".jpg"))
						throw Exception(NotOverwritten + out_base + ".jpg");
				}
			}
		}

		// expand full transparent color
		if(inbmp->PixelFormat == pf32bit && output_type == "alpha" && 
			!TranspOutputAddAlphaFormatCheckBox->Checked)
		{
			switch(TranspFullTranspColorMethodComboBox->ItemIndex)
			{
			case 0: // remove
				ClearFullTransparentColor(inbmp);
				break;
			case 1: // intact
				break;
			case 2: // expand(1)
				ExpandOpaqueColor(inbmp, 1);
				break;
			case 3: // expand(2)
				ExpandOpaqueColor(inbmp, 2);
				break;
			case 4: // expand(3)
				ExpandOpaqueColor(inbmp, 3);
				break;
			case 5: // expand(5)
				ExpandOpaqueColor(inbmp, 5);
				break;
			case 6: // expand(8)
				ExpandOpaqueColor(inbmp, 8);
				break;
			}
		}

		// convert alpha to addivive alpha
		if(inbmp->PixelFormat == pf32bit &&
			input_type == "alpha" && output_type == "addalpha")
		{
			ConvertAlphaToAddAlpha(inbmp);
		}

		// append 'mode' tag
		tags->Append("mode=" + output_type);

		// write output
		if(inbmp->PixelFormat == pf32bit)
		{
			// transparent image
			if(TranspBMPFormatRadioButton->Checked)
			{
				inbmp->SaveToFile(out_base + ".bmp");
			}
			else if(TranspPNGFormatRadioButton->Checked)
			{
				TDeePNG *png = new TDeePNG();
				try
				{
					png->Assign(inbmp);
					png->SetTags(tags.get());
					png->SaveToFile(out_base + ".png");
				}
				catch(...)
				{
					delete png;
					throw;
				}
				delete png;
			}
			else if(TranspTLG5FormatRadioButton->Checked)
			{
				SaveTLGWithMetaInfo(tags.get(), inbmp, out_base + ".tlg", SaveTLG5);
			}
			else if(TranspTLG6FormatRadioButton->Checked)
			{
				SaveTLGWithMetaInfo(tags.get(), inbmp, out_base + ".tlg", SaveTLG6);
			}
			else if(TranspSeparatedFormatRadioButton->Checked)
			{
				// main/mask separated type
				Graphics::TBitmap *mask = SeparateMask(inbmp);
				try
				{
					SaveImage(inbmp, out_base, TranspMainFormatComboBox->ItemIndex, TranspMainJPEGQuality, tags.get());
					SaveImage(mask, out_base + "_m", TranspMaskFormatComboBox->ItemIndex, TranspMaskJPEGQuality, tags.get());
				}
				catch(...)
				{
					delete mask;
					throw;
				}
				delete mask;
			}
		}
		else
		{
			// opaque image
			if(OpaqueBMPFormatRadioButton->Checked)
			{
				inbmp->SaveToFile(out_base + ".bmp");
			}
			else if(OpaquePNGFormatRadioButton->Checked)
			{
				TDeePNG *png = new TDeePNG();
				try
				{
					png->Assign(inbmp);
					png->SetTags(tags.get());
					png->SaveToFile(out_base + ".png");
				}
				catch(...)
				{
					delete png;
					throw;
				}
				delete png;
			}
			else if(OpaqueTLG5FormatRadioButton->Checked)
			{
				SaveTLGWithMetaInfo(tags.get(), inbmp, out_base + ".tlg", SaveTLG5);
			}
			else if(OpaqueTLG6FormatRadioButton->Checked)
			{
				SaveTLGWithMetaInfo(tags.get(), inbmp, out_base + ".tlg", SaveTLG6);
			}
			else if(OpaqueJPEGFormatRadioButton->Checked)
			{
				SaveImage(inbmp, out_base, 1, OpaqueJPEGQuality, NULL);
			}
		}
	}
	catch(Exception &e)
	{
		Log += "エラー: " + e.Message + "\n";
		error = true;
	}
	catch(...)
	{
		Log += "不明なエラー\n";
		error = true;
	}

	if(!error)
		Log += "OK\n";

	Log += "\n";

	if(inbmp) delete inbmp;

	return !error;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
extern "C" void _export PASCAL UIExecTPC()
{
	try
	{
		TPCMainForm = new TTPCMainForm(Application);
		TPCMainForm->ShowModal();
		delete TPCMainForm, TPCMainForm = NULL;
	}
	catch(Exception &e)
	{
		Application->ShowException(&e);
	}
	ExitProcess(0);
}
//---------------------------------------------------------------------------














