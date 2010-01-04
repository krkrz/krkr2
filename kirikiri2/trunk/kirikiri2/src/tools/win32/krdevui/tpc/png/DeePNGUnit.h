//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef DeePNGUnitH
#define DeePNGUnitH

#include <vcl.h>

//---------------------------------------------------------------------------
extern void __fastcall SetGrayscalePalette(Graphics::TBitmap *dib, int bit);
//---------------------------------------------------------------------------
// 例外クラス
class EDeePNG : public Exception
{
public:
	__fastcall EDeePNG(const System::AnsiString Msg) : Sysutils::Exception(
		Msg) { }
	__fastcall EDeePNG(const System::AnsiString Msg, const System::TVarRec
		* Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	__fastcall EDeePNG(int Ident, Extended Dummy) : Sysutils::Exception(
		Ident, Dummy) { }
	__fastcall EDeePNG(int Ident, const System::TVarRec * Args, const
		int Args_Size) : Sysutils::Exception(Ident, Args, Args_Size) { }
	__fastcall EDeePNG(const System::AnsiString Msg, int AHelpContext
		) : Sysutils::Exception(Msg, AHelpContext) { }
	__fastcall EDeePNG(const System::AnsiString Msg, const System::TVarRec
		* Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg,
		 Args, Args_Size, AHelpContext) { }
	__fastcall EDeePNG(int Ident, int AHelpContext) : Sysutils::
		Exception(Ident, AHelpContext) { }
	__fastcall EDeePNG(int Ident, const System::TVarRec * Args
		, const int Args_Size, int AHelpContext) : Sysutils::Exception(Ident,
		 Args, Args_Size, AHelpContext) { }

	__fastcall virtual ~EDeePNG(void) { }

};
//---------------------------------------------------------------------------
// TDeePNG クラス
class TDeePNG : public Graphics::TBitmap
{
private:
protected:
	bool ofs_set;
	int ofs_x;
	int ofs_y;
	int ofs_unit;
	bool vpag_set;
	int vpag_w;
	int vpag_h;
	int vpag_unit;

public:
	// コンストラクタとデストラクタ
	__fastcall TDeePNG(void);
	__fastcall ~TDeePNG(void);

	// Stream I/O  のオーバーライド
	void __fastcall LoadFromStream(Classes::TStream * Stream);
	void __fastcall SaveToStream(Classes::TStream * Stream);

	// オフセット
	void SetOffset(int ofs_x, int ofs_y, int ofs_unit)
	{
		this->ofs_x = ofs_x;
		this->ofs_y = ofs_y;
		this->ofs_unit = ofs_unit;
		this->ofs_set = true;
	}
	bool GetOffset(int &ofs_x, int &ofs_y, int &ofs_unit)
	{
		if(!this->ofs_set) return false;
		ofs_x = this->ofs_x;
		ofs_y = this->ofs_y;
		ofs_unit = this->ofs_unit;
		return true;
	}
	// 仮想ページサイズ
	void SetVirtualPage(int vpag_w, int vpag_h, int vpag_unit)
	{
		this->vpag_w = vpag_w;
		this->vpag_h = vpag_h;
		this->vpag_unit = vpag_unit;
		this->vpag_set = true;
	}
	bool GetVirtualPage(int &vpag_w, int &vpag_h, int &vpag_unit)
	{
		if(!this->vpag_set) return false;
		vpag_w = this->vpag_w;
		vpag_h = this->vpag_h;
		vpag_unit = this->vpag_unit;
		return true;
	}
	// タグ
	void SetTags(TStringList *tags); // set tag information from tags
	void AppendTags(TStringList *tags); // append tag information to tags

public:
};
//---------------------------------------------------------------------------
#endif
