#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "Controls.hpp"
#include "ExtCtrls.hpp"
#include "Forms.hpp"
#include "Graphics.hpp"
#include "ImgList.hpp"
#include "Menus.hpp"
#include "Messages.hpp"
#include "StdCtrls.hpp"
#include "ToolWin.hpp"

class TToolBar : public TToolWindow
{
public:
	TToolBar(TComponent* owner);
};

//----------------------------------------------------------------------------
// TCustomMultiSelectListControl, TCustomListViewは略
class TListColumn;
class TListItems;
class TListItem;
class TListView : public TCustomListControl
{
	std::vector<TListColumn> m_Column;
	TListItems* m_ListItems;
	wxListCtrl* m_wxListCtrl;
public:
	PROPERTY_ARY_R(TListColumn*, Column);
	PROPERTY_R(TListItems*, Items);
	PROPERTY_R(TListItem*, Selected);

	TListItem* GetItemAt( int X, int Y);    // 位置を指定する X 座標、Y 座標
	bool IsEditing();
};

//----------------------------------------------------------------------------
class TListItem : public System::TObject
{
public:
	PROPERTY_VAR1(AnsiString, Caption);
	PROPERTY(bool,Selected);
	PROPERTY(TStrings*,SubItems);

	bool EditCaption(); // リスト項目のキャプションのインプレース編集を開始する
};
//----------------------------------------------------------------------------
class TListItems : public System::TObject
{
	friend TListView;
public:
	PROPERTY(int, Count);
	PROPERTY_ARY_R(TListItem*, Item);
	PROPERTY_VAR_R(TListView*, Owner);// 本当はTCustomListViewなんだけど、略

	TListItem* Add(void);
	void BeginUpdate(); // リストビューの更新を抑制する, 再描画を行うときは、 EndUpdate メソッドを実行する
	void EndUpdate();
	void Delete( int Index );    // 削除する項目のインデックス
};
//----------------------------------------------------------------------------
typedef int TWidth;
class TListColumn : public System::TObject
{
public:
	PROPERTY_VAR0(TWidth, Width);
};

//----------------------------------------------------------------------------
// TCustomRichEdit は略
class TRichEdit : public TCustomMemo
{
public:
	TRichEdit( TComponent* );
	PROPERTY(TStrings*, Lines);
	PROPERTY(int, SelLength);
	PROPERTY(AnsiString, SelText);
	PROPERTY(int, SelStart);
//	SelStart
};
