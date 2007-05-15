#include "vcl.h"
#include "SysUtils.hpp"

#include <wx/dir.h>

//----------------------------------------------------------------------------
PACKAGE AnsiString __fastcall ExtractFileDir(const AnsiString & FileName)
{
	char drive[MAX_PATH], dir[MAX_PATH];
	_splitpath( FileName.c_str(), drive, dir, NULL, NULL );

	// ExtractFileDirは、終端 の \ を含まない。splitpath は含んでしまうらしいので、取り除く
	int n = strlen(dir);
	if ( dir[n - 1] == '\\' ) dir[n - 1] = '\0';
	return AnsiString(drive) + AnsiString(dir);
}

//----------------------------------------------------------------------------
PACKAGE AnsiString __fastcall IncludeTrailingBackslash(const AnsiString & FileName)
{
	int n = FileName.Length();

	if ( n == 0 )
		return AnsiString("\\");
	if ( FileName.c_str()[n - 1] != '\\' )
		return FileName + '\\';
	else
		return FileName;
}

//----------------------------------------------------------------------------
// ケツの￥を削除する
PACKAGE AnsiString __fastcall ExcludeTrailingBackslash(const AnsiString & FileName)
{
	int n = FileName.Length();

	if ( n == 0 )
		return FileName;
	if ( FileName.c_str()[n - 1] != '\\' )
		return FileName;

	return FileName.SubString(0, n-1);
}

//----------------------------------------------------------------------------
// ファイル名の拡張子を変更したファイル名を返す
// 第二引数は拡張子（ピリオドを含む）
PACKAGE AnsiString __fastcall ChangeFileExt(const AnsiString & FileName, const AnsiString & Extension)
{
	AnsiString str = FileName;

	char *p = strrchr(str.c_str(), '.');
	if ( p != NULL ) *p = '\0';

	return str + Extension;
}
//----------------------------------------------------------------------------
//フルパスからファイル名だけを返す
PACKAGE AnsiString __fastcall ExtractFileName(const AnsiString & FileName)
{
	AnsiString str = FileName;

	char *p = strrchr(str.c_str(), '\\');
	if ( p != NULL )
		return *(p + 1);
	return FileName;
}
//----------------------------------------------------------------------------
//フルパスからパス部を返す。最後尾の "\" を含む
PACKAGE AnsiString __fastcall ExtractFilePath(const AnsiString & FileName)
{
	AnsiString str = FileName;

	char *p = strrchr(str.c_str(), '\\');
	if ( p != NULL )
		*(p + 1) = '\0';
	return str;
}

//----------------------------------------------------------------------------
// UNC形式から絶対パスに変換する
AnsiString __fastcall ExpandUNCFileName(const AnsiString & FileName)
{
	// 未実装
	return "";
}

//----------------------------------------------------------------------------
//指定のディレクトリが存在するかどうかを返す
bool DirectoryExists(const AnsiString& Name )
{
	return wxDir::Exists( Name.c_str() );
}
//----------------------------------------------------------------------------
//指定のファイルが存在するかどうかを調べる
bool FileExists(const AnsiString& Name)
{
	return wxFile::Exists( Name.c_str() );
}
//----------------------------------------------------------------------------
//ディスクからファイルを削除する
bool DeleteFile(const AnsiString& Name)
{
	if ( remove( Name.c_str() ) == 0 )
		return true;
	else
		return false;
}
//----------------------------------------------------------------------------
// 文字列の先頭および末尾部分のスペースと制御文字を削除する。全角スペースは削除できない
AnsiString Trim(const AnsiString& str)
{
	const char * p = str.c_str();
	while( *p > '\0' && *p < 0x20 )
		p++;

	AnsiString _str(p);
	char * p0 = _str.c_str();
	char * p1 = _str.c_str() + _str.Length() - 1;
	while( p0 < p1 && *p1 >= '\0' && *p1 < 0x20 )
		*p1-- = '\0';

	return _str;
}

