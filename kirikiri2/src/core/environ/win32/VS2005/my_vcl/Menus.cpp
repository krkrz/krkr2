#include "Menus.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// 個々のメニュー項目を操作するオブジェクト
//class TMenuItem : public TComponent
TMenuItem::TMenuItem(TComponent* owner) : TComponent(owner)
{
}
TMenuItem::~TMenuItem()
{
}

/*	PROPERTY(int, AutoHotkeys);
	PROPERTY(AnsiString, Caption );
	PROPERTY(bool, Checked);
	PROPERTY(int, Count);
	PROPERTY(bool,Enabled);
	PROPERTY(int, GroupIndex);
	PROPERTY(HMENU, Handle);
	PROPERTY(int, MenuIndex);
	PROPERTY(TMenuItem*, Parent);
	PROPERTY(bool, RadioItem);
	PROPERTY(TShortCut, ShortCut);
	PROPERTY(bool,Visible);
	TMenuItem* Items[10];
*/

	// メソッド
void TMenuItem::Add(TMenuItem*)
{
}
void TMenuItem::Click()
{
}
void TMenuItem::Delete(int index)
{
}
void TMenuItem::Insert(int index, TMenuItem*)
{
}
int  TMenuItem::IndexOf(TMenuItem*)
{
	return 0;
}

	// イベント。クロージャー
//	boost::function1<void, System::TObject*> OnClick;
//};

//----------------------------------------------------------------------------
//	PROPERTY_ARY(TMenuItem*, Items);
void TMenuItem::setItems(int index, TMenuItem *)
{
}

TMenuItem* TMenuItem::getItems(int index) const
{
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TMenu : public TComponent
//{
//public:
TMenu::TMenu(TComponent* owner) : TComponent(owner)
{
}
TMenu::~TMenu()
{
}
//	PROPERTY_ARY(TMenuItem*, Items);
//	PROPERTY(bool, Visible);

//----------------------------------------------------------------------------
//WM_KEY メッセージがメニューの項目に設定したショートカットキーを含むとき、メニュー項目の Click メソッドを実行する
bool __fastcall TMenu::IsShortCut(Messages::TWMKey &Message)
{
	return false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TMainMenu::TMainMenu(TComponent* owner) : TMenu(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TPopupMenu::TPopupMenu(TComponent* owner) : TMenu(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// 関数
//----------------------------------------------------------------------------
//Menus.hpp
//テキスト文字列から TShortCut 型を返す
TShortCut  TextToShortCut(const AnsiString&)
{
	return TShortCut(0);
}
//----------------------------------------------------------------------------
//Menus.hpp
//メニューショートカットを説明する文字列に変換する
AnsiString ShortCutToText(TShortCut)
{
	return AnsiString();
}

//----------------------------------------------------------------------------
//Menus.hpp
//メニューショートカットの仮想キーコードとシフト状態を返す
void ShortCutToKey( TShortCut ShortCut, Word & Key, TShiftState &Shift )
{
	Key = 0;
	Shift = TShiftState();
}
