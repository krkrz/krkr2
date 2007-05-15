#include "ComCtrls.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TToolBar::TToolBar(TComponent* owner) : TToolWindow(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TRichEdit::TRichEdit( TComponent* owner ) : TCustomMemo(owner)
{
}

//----------------------------------------------------------------------------
void TRichEdit::setLines(class TStrings *)
{
}

TStrings * TRichEdit::getLines(void)const
{
	return NULL;
}

//----------------------------------------------------------------------------
void TRichEdit::setSelLength(int)
{
}

int TRichEdit::getSelLength(void)const
{
	return 0;
}

//----------------------------------------------------------------------------
void TRichEdit::setSelText(class AnsiString)
{
}

AnsiString TRichEdit::getSelText(void) const
{
	return "";
}

//----------------------------------------------------------------------------
void TRichEdit::setSelStart(int)
{
}

int TRichEdit::getSelStart(void)const
{
	return 0;
}

