//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <FileCtrl.hpp>

#include "RelFolderSelectorUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ShellCtrls"
#pragma resource "*.dfm"
TRelFolderSelectorForm *RelFolderSelectorForm;
//---------------------------------------------------------------------------
#define SPACER 6
__fastcall TRelFolderSelectorForm::TRelFolderSelectorForm(TComponent* Owner)
	: TForm(Owner)
{
	ShellTreeView->Path = ExtractFileDir(ParamStr(0));

	// adjust components
	Label1->Left = SPACER;
	Label1->Top = SPACER;
	ShellComboBox->Left = SPACER;
	ShellComboBox->Top = SPACER + Label1->Height + 2;
	ShellComboBox->Width = ClientWidth - SPACER * 2;
	ShellTreeView->Left = SPACER;
	ShellTreeView->Top = ShellComboBox->Height + ShellComboBox->Top;
	ShellTreeView->Width = ClientWidth - SPACER * 2;
	OKButton->Top = ClientHeight - OKButton->Height - SPACER;
	CancelButton->Top = ClientHeight - CancelButton->Height - SPACER;
	OKButton->Left = ClientWidth - (StatusBar->Width + SPACER + CancelButton->Width
		+ SPACER + OKButton->Width);
	CancelButton->Left = ClientWidth - ( StatusBar->Width + SPACER + CancelButton->Width);
	ShellTreeView->Height = CancelButton->Top - SPACER - ShellTreeView->Top;

	if(ShellTreeView->Selected)
	{
		ShellTreeView->Selected->Expanded = true;
		TTreeNode * topnode = ShellTreeView->Selected;
		if(topnode->Count >= 1)
		{
			TTreeNode * longestitem = NULL;
			int longesttextsize = -1;
			for(int i = 0; i < topnode->Count; i++)
			{
				TTreeNode *subnode = topnode->Item[i];
				TRect rect = subnode->DisplayRect(true);
				if(rect.right > longesttextsize)
				{
					longesttextsize = rect.right;
					longestitem = topnode->Item[i];
				}
			}
			if(longestitem)
				longestitem->MakeVisible();
		}
		ShellTreeView->TopItem = topnode;
		topnode->MakeVisible();
	}

}
//---------------------------------------------------------------------------
void __fastcall TRelFolderSelectorForm::ShellTreeViewChange(
	  TObject *Sender, TTreeNode *Node)
{
	if(ShellTreeView->Path != "" && DirectoryExists(ShellTreeView->Path))
		OKButton->Enabled = true;
	else
		OKButton->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TRelFolderSelectorForm::SelectMenuItemClick(
	  TObject *Sender)
{
	if(OKButton->Enabled) OKButton->Click();
}
//---------------------------------------------------------------------------
void __fastcall TRelFolderSelectorForm::FormDestroy(TObject *Sender)
{
	delete ShellTreeView;
	delete ShellComboBox;
}
//---------------------------------------------------------------------------



