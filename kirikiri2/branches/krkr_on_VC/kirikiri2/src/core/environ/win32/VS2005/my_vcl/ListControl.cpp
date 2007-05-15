#include "vcl.h"
#include "ComCtrls.hpp"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TListView : public TCustomListControl
//std::vector<TListColumn*> m_Column;
//PROPERTY_ARY_R(TListColumn*, Column);
TListColumn* TListView::getColumn(int index) const
{
	if( index >= 0 && index < m_Column.size() )
		return const_cast<TListColumn*>( &m_Column[index] );
	return NULL;
}
//----------------------------------------------------------------------------
//PROPERTY_R(TListItems*, Items);
TListItems* TListView::getItems() const
{
	return const_cast<TListItems*>( m_ListItems );
}
//----------------------------------------------------------------------------
//PROPERTY_R(TListItem*, Selected);
TListItem* TListView::getSelected() const
{
	return NULL;
}
//----------------------------------------------------------------------------
// 位置を指定する X 座標、Y 座標
TListItem* TListView::GetItemAt( int X, int Y)
{
	return NULL;
}

//----------------------------------------------------------------------------
bool TListView::IsEditing()
{
	return false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TListItem : public System::TObject
//PROPERTY_VAR1(AnsiString, Caption);
//PROPERTY(bool,Selected);
//PROPERTY(TStrings*,SubItems);

//----------------------------------------------------------------------------
// リスト項目のキャプションのインプレース編集を開始する
bool TListItem::EditCaption()
{
	return false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TListItems : public System::TObject
//PROPERTY(int, Count);
void TListItems::setCount(int val)
{
}
int TListItems::getCount() const
{
	return 0;
}
//----------------------------------------------------------------------------
//PROPERTY_ARY_R(TListItem*, Item);
//void TListItems::setCount(int index, int val)
//{
//}
TListItem* TListItems::getItem(int index) const
{
	return NULL;
}
//----------------------------------------------------------------------------
//PROPERTY_VAR_R(TListView*, Owner);// 本当はTCustomListViewなんだけど、略

//----------------------------------------------------------------------------
// 新しい項目をリストの最後に追加する
TListItem* TListItems::Add(void)
{
	return NULL;
}
//----------------------------------------------------------------------------
// リストビューの更新を抑制する, 再描画を行うときは、 EndUpdate メソッドを実行する
void TListItems::BeginUpdate() {}
//----------------------------------------------------------------------------
void TListItems::EndUpdate() {}
//----------------------------------------------------------------------------
// 削除する項目のインデックス
void TListItems::Delete( int Index )
{
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TListColumn : public System::TObject
//PROPERTY_VAR0(TWidth, Width);

