//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <shlobj.h>
#include <FileCtrl.hpp>

#include "FolderSelectorUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ShellCtrls"
#pragma resource "*.dfm"
TFolderSelectorForm *FolderSelectorForm;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define SPACER 6
__fastcall TFolderSelectorForm::TFolderSelectorForm(TComponent* Owner)
	: TForm(Owner)
{
	ShellComboBox->Align = alTop;

	// adjust components
	Label1->Left = SPACER;
	Label1->Top = SPACER;
	NoSelectButton->Left = SPACER;
	NoSelectButton->Top = ClientHeight - NoSelectButton->Height - SPACER;
	OKButton->Left = ClientWidth - OKButton->Width - CancelButton->Width -
		SPACER*2- StatusBar1->Width ;
	OKButton->Top = ClientHeight - OKButton->Height -SPACER;
	CancelButton->Left = ClientWidth - CancelButton->Width - SPACER
		- StatusBar1->Width ;
	CancelButton->Top = ClientHeight - CancelButton->Height - SPACER;
	ViewPanel->Left = SPACER;
	ViewPanel->Top = SPACER + Label1->Height + 2;
	ViewPanel->Width = ClientWidth - SPACER*2;
	ViewPanel->Height = ClientHeight - ( SPACER + Label1->Height + SPACER + SPACER +
		NoSelectButton->Height + Label2->Height + 2 );
	Label2->Left = SPACER;
	Label2->Top = ViewPanel->Top + ViewPanel->Height + 2;
	PageScroller->Top = Label2->Top;
	PageScroller->Left = Label2->Left + Label2->Width;
	PageScroller->Width = ClientWidth - (SPACER*2+Label2->Width);
	StatusBar1->Left = ClientWidth - StatusBar1->Width;
	StatusBar1->Top = ClientHeight - StatusBar1->Height;
}
//---------------------------------------------------------------------------

void __fastcall TFolderSelectorForm::FormResize(TObject *Sender)
{
	if(ShellTreeView->Width < 50)
	{
		ShellListView->Width = ViewPanel->Width - 50 - Splitter->Width;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFolderSelectorForm::FormDestroy(TObject *Sender)
{
	// delete some shell related components here to avoid undeleted threads'
	// mischief.
	delete ShellTreeView;
	delete ShellListView;
	delete ShellComboBox;
}
//---------------------------------------------------------------------------
void __fastcall TFolderSelectorForm::ShellTreeViewChange(TObject *Sender,
	  TTreeNode *Node)
{
	if(ShellTreeView->Path != "" && DirectoryExists(ShellTreeView->Path))
	{
		PlaceLabel->Caption = ShellTreeView->Path + FolderLabel->Caption;
		Selected = ShellTreeView->Path;
		OKButton->Enabled = true;
	}
	else
	{
		OKButton->Enabled = false;
		Selected = "";
		PlaceLabel->Caption = NothingLabel->Caption;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFolderSelectorForm::ShellTreeViewClick(TObject *Sender)
{
	ShellTreeViewChange(Sender, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TFolderSelectorForm::ShellListViewChange(TObject *Sender,
	  TListItem *Item, TItemChange Change)
{
	if(ShellListView->Selected)
	{
		TShellListView *slv = (TShellListView*)ShellListView;
		AnsiString fn =
			slv->FileList->FileItem[ShellListView->Selected->Index]->Path;
		if(fn!="")
		{
			if(FileExists(fn))
			{
				PlaceLabel->Caption = fn + FileLabel->Caption;
				Selected = fn;
				OKButton->Enabled= true;
				return;
			}
			if(DirectoryExists(fn))
			{
				PlaceLabel->Caption = fn + FolderLabel->Caption;
				Selected = fn;
				OKButton->Enabled= true;
				return;
			}
		}
	}

	if(ShellTreeView->Path != "" && DirectoryExists(ShellTreeView->Path))
	{
		PlaceLabel->Caption = ShellTreeView->Path + FolderLabel->Caption;
		Selected = ShellTreeView->Path;
		OKButton->Enabled = true;
	}
	else
	{
		OKButton->Enabled = false;
		Selected = "";
		PlaceLabel->Caption = NothingLabel->Caption;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFolderSelectorForm::ShellListViewClick(TObject *Sender)
{
	ShellListViewChange(Sender, NULL, TItemChange());
}
//---------------------------------------------------------------------------

void __fastcall TFolderSelectorForm::ShellListViewShExecute(
	  TObject *Sender, TShellFileItem *Item, TShiftState Shift,
	  int SubItem, int X, int Y, bool &DefaultAction)
{
	if(FileExists(Item->Path))
	{
		// selected item is a file
		DefaultAction =false;
		OKButton->Click();
	}
}
//---------------------------------------------------------------------------
void __fastcall TFolderSelectorForm::SelectItemMenuItemClick(
	  TObject *Sender)
{
	if(OKButton->Enabled) OKButton->Click();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern "C" int _export PASCAL UIShowFolderSelectorForm(void *reserved, char *buf)
{
	TFolderSelectorForm *form = new TFolderSelectorForm(Application);
	form->ShellTreeView->Path = buf; // initial directory
	if(form->ShellTreeView->Selected)
	{
		form->ShellTreeView->Selected->Expanded = true;
		TTreeNode * topnode = form->ShellTreeView->Selected;
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

//		form->ShellTreeView->Selected->MakeVisible();
// 		INT min, max;
//		::GetScrollRange(form->ShellTreeView->Handle, SB_HORZ, &min, &max);
//		::SetScrollPos(form->ShellTreeView->Handle, SB_HORZ, max, true);
//		form->ShellTreeView->Selected->MakeVisible();
//		TreeView_EnsureVisible(form->ShellTreeView->Selected->Handle,
//			form->ShellTreeView->Selected->ItemId);
		}
		form->ShellTreeView->TopItem = topnode;
		topnode->MakeVisible();
	}

	int res = form->ShowModal();
	if(res == mrOk)
	{
		if(DirectoryExists(form->Selected))
		{
			form->Selected = IncludeTrailingBackslash(form->Selected);
		}
		strcpy(buf, form->Selected.c_str());
	}
	delete form;
	return res;
}
//---------------------------------------------------------------------------






