#pragma once

#include "vcl_base.h"
#include "Classes.hpp"

typedef Word TShortCut;

//----------------------------------------------------------------------------
class TMenuItem : public TComponent
{
protected:
//	std::vector<TMenuItem> m_Strings;
public:
	TMenuItem(TComponent* Owner);
	virtual ~TMenuItem();
	TShortCut ShortCut;

	PROPERTY_VAR0(int, AutoHotkeys);
	PROPERTY_VAR0(AnsiString, Caption );
	PROPERTY_VAR0(bool, Checked);
	PROPERTY_VAR0(int, Count);
	PROPERTY_VAR0(bool,Enabled);
	PROPERTY_VAR0(int, GroupIndex);
	PROPERTY_VAR0(HMENU, Handle);
	PROPERTY_VAR0(int, MenuIndex);
	PROPERTY_VAR0(TMenuItem*, Parent);
	PROPERTY_VAR0(bool, RadioItem);
	PROPERTY_VAR0(TShortCut, ShortCut);
	PROPERTY_VAR0(bool,Visible);

	PROPERTY_ARY(TMenuItem*, Items);

	// メソッド
	void Add(TMenuItem*);
	void Click();
	void Delete(int index);
	void Insert(int index, TMenuItem*);
	int  IndexOf(TMenuItem*);

	// イベント。クロージャー
	boost::function1<void, System::TObject*> OnClick;
};

//----------------------------------------------------------------------------
class TMenu : public TComponent
{
public:
	TMenu(TComponent* owner);
	virtual ~TMenu();
	PROPERTY_VAR0(TMenuItem*, Items);
	PROPERTY_VAR0(bool, Visible);
	bool __fastcall IsShortCut(Messages::TWMKey &Message);
};
//----------------------------------------------------------------------------
class TMainMenu : public TMenu
{
public:
	TMainMenu(TComponent* owner);
};
//----------------------------------------------------------------------------
class TPopupMenu : public TMenu
{
public:
	TPopupMenu(TComponent* owner);
};

//----------------------------------------------------------------------------
extern TShortCut  TextToShortCut(const AnsiString&);
extern AnsiString ShortCutToText(TShortCut);
extern void ShortCutToKey( TShortCut ShortCut, Word & Key, TShiftState &Shift );

