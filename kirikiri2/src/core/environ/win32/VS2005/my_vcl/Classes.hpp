#pragma once

#include "vcl_base.h"

typedef unsigned short Word;

#include "Messages.hpp"

typedef boost::function1<void, Messages::TMessage &> TWndMethod;
extern HWND AllocateHWnd( TWndMethod Method );// メッセージを処理する TWndMethod 型
extern void DeallocateHWnd(HWND);

typedef Set<int,ssShift,ssDouble> TShiftState;

#include "Component.h"

//----------------------------------------------------------------------------
class TStrings : public System::TObject
{
protected:
	std::vector<AnsiString> m_Strings;
public:
	PROPERTY(int, Count);
	PROPERTY(AnsiString, Text);
	PROPERTY_ARY(AnsiString, Strings);

	virtual int Add(const AnsiString & s);
	virtual void Assign(TObject* Source );  // コピー元の TStringList オブジェクト
	virtual void Delete(int Index );
	virtual void Clear();
	virtual int IndexOf(const AnsiString & s);
	virtual void Move(int CurIndex, int NewIndex);
	virtual void SaveToFile(const AnsiString &);
	virtual void LoadFromFile(const AnsiString & FileName);

	void BeginUpdate(); // リストビューの更新を抑制する, 再描画を行うときは、 EndUpdate メソッドを実行する
	void EndUpdate();
};

//----------------------------------------------------------------------------
//TStringList オブジェクトは、TStrings オブジェクトとほとんど同じ機能だが、以下にリストアップする機能が追加されている。
//  リストが保持する文字列データをソートする
//  ソートするとき、重複する文字列データを削除できる
//  リストの内容を変更するときにイベントが発生する
class TStringList : public TStrings
{
public:
	std::map<AnsiString,AnsiString> Values;
	PROPERTY_ARY_R(AnsiString,Names);
	PROPERTY_VAR0(bool, Sorted);
	PROPERTY_VAR0(TDuplicates, Duplicates);
};

#include "Stream.h"

//----------------------------------------------------------------------------
class TCollectionItem : public System::TObject
{
};

class TCollection : public System::TObject
{
};


