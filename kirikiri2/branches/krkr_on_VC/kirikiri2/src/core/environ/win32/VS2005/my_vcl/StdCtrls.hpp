#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "Controls.hpp"
#include "Forms.hpp"
#include "Graphics.hpp"
#include "Menus.hpp"
#include "Messages.hpp"

//----------------------------------------------------------------------------
// TCustomListBox,TCustomMultiSelectListControlは略
class TListBox : public TCustomListControl
{
public:
	TListBox(TComponent* owner);
	PROPERTY_VAR0(TCanvas*,Canvas);
	PROPERTY_VAR0(int, Style);
	PROPERTY_VAR0(int, ItemHeight);

	TStrings* Items;
};
//----------------------------------------------------------------------------
// TCustomCombo, TCustomComboBox は略
class TComboBox : public TCustomListControl
{
public:
	TComboBox(TComponent* owner);
	TStrings* Items;
	PROPERTY_VAR0(int, SelLength );
	PROPERTY_VAR0(int, SelStart );
};

//----------------------------------------------------------------------------
class TCustomEdit : public TWinControl
{
public:
	TCustomEdit( TComponent* );
	virtual ~TCustomEdit();

	PROPERTY_VAR0(bool, CanUndo);
	PROPERTY_VAR0(bool, ReadOnly);
	void Undo();
	void CutToClipboard();
	void CopyToClipboard();
};
//----------------------------------------------------------------------------
class TEdit : public TCustomEdit
{
public:
	TEdit( TComponent* );
	PROPERTY_VAR0(int, MaxLength);
};

//----------------------------------------------------------------------------
// Windows 標準のエディットコントロールの複数行編集可能なコントロールを操作するコンポーネント
class TCustomMemo : public TCustomEdit
{
public:
	TCustomMemo( TComponent* );
	virtual ~TCustomMemo();

	PROPERTY_VAR0(TScrollStyle, ScrollBars);
	PROPERTY_VAR0(bool, WordWrap);
};

class TMemo : public TCustomMemo
{
public:
	TMemo( TComponent* );
};
