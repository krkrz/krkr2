#include "vcl.h"
#include "Dialogs.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TCommonDialog::TCommonDialog(TComponent* owner) : TComponent(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TOpenDialog::TOpenDialog(TComponent* owner) : TCommonDialog(owner)
{
}

//----------------------------------------------------------------------------
// Windows のファイル選択用コモンダイアログボックスを操作するコンポーネント
bool TOpenDialog::Execute(void)
{
	wxFileDialog dlg(NULL,
		"Choose a file",
		"",
		m_FileName.c_str(),
		"*.*",
		wxOPEN);
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_FileName = dlg.GetPath().c_str();
		return true;
	}
	return false;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TSaveDialog::TSaveDialog(TComponent* owner) : TOpenDialog(owner)
{
}

//----------------------------------------------------------------------------
bool TSaveDialog::Execute(void)
{
	wxFileDialog dlg(NULL,
		"Choose a file",
		"",
		FileName.c_str(),
		"*.*",
		wxSAVE);
	if ( dlg.ShowModal() == wxID_OK )
	{
		FileName = dlg.GetPath().c_str();
		return true;
	}
	return false;
}

