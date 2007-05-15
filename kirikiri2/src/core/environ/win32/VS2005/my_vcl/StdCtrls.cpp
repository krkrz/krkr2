#include <StdCtrls.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TListBox::TListBox(TComponent* owner) : TCustomListControl(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TComboBox::TComboBox(TComponent* owner) : TCustomListControl(owner)
{
	Items = new TStrings();
//	PROPERTY_VAR0(int, SelLength );
//	PROPERTY_VAR0(int, SelStart );
}
//----------------------------------------------------------------------------
//class TCustomMemo : public TWinControl
//StdCtrls.hpp
//----------------------------------------------------------------------------
TCustomEdit::TCustomEdit( TComponent* owner ) : TWinControl( owner )
{
}
//----------------------------------------------------------------------------
TCustomEdit::~TCustomEdit()
{
}
//----------------------------------------------------------------------------
void TCustomEdit::CutToClipboard()
{
}
//----------------------------------------------------------------------------
void TCustomEdit::CopyToClipboard()
{
}
//----------------------------------------------------------------------------
void TCustomEdit::Undo()
{
}

//----------------------------------------------------------------------------
TEdit::TEdit( TComponent* owner ) : TCustomEdit( owner )
{
}

//----------------------------------------------------------------------------
TCustomMemo::TCustomMemo( TComponent* owner ) : TCustomEdit( owner )
{
}
//----------------------------------------------------------------------------
TCustomMemo::~TCustomMemo()
{
}

//----------------------------------------------------------------------------
TMemo::TMemo( TComponent* owner ) : TCustomMemo( owner )
{
}
