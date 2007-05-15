#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "Controls.hpp"
#include "Application.h"
#include "Messages.hpp"

class TClipboard : public System::TObject //TPersistent ñlÇÃTObjectÇÕ PersistentçûÇ›
{
private:
//  int FOpenRefCount;
//  HWND FClipboardWindow;
//  bool FAllocated;
//  bool FEmptied;
//  void Adding(void);
//  void AssignGraphic(Graphics::TGraphic* Source);
//  void AssignPicture(Graphics::TPicture* Source);
//  void AssignToBitmap(Graphics::TBitmap* Dest);
//  void AssignToMetafile(Graphics::TMetafile* Dest);
//  void AssignToPicture(Graphics::TPicture* Dest);
//  AnsiString GetAsText();
//  HWND GetClipboardWindow(void);
//  int GetFormatCount(void);
//  Word GetFormats(int Index);
//  void SetAsText(const AnsiString Value);

protected:
//  virtual void AssignTo(Classes::TPersistent* Dest);
//  void SetBuffer(Word Format, void *Buffer, int Size);
//  virtual void WndProc(Messages::TMessage &Message);
//  void MainWndProc(Messages::TMessage &Message);

//  __property HWND Handle = {read=GetClipboardWindow, nodefault};
//  __property int OpenRefCount = {read=FOpenRefCount, nodefault};

public:
//  TClipboard(void) : Classes::TPersistent() { }
//  inline virtual ~TClipboard(void) { }

//  virtual void Assign(Classes::TPersistent* Source);
//  virtual void Clear(void);
	virtual void Close(void);
//  Classes::TComponent* GetComponent(Classes::TComponent* Owner, Classes::TComponent* Parent);
//  unsigned GetAsHandle(Word Format);
//  int GetTextBuf(char * Buffer, int BufSize);
	bool HasFormat(Word Format);
	virtual void Open(void);
//  void SetComponent(Classes::TComponent* Component);
	void SetAsHandle(Word Format, unsigned Value);
//  void SetTextBuf(char * Buffer);

	PROPERTY(AnsiString, AsText);//  __property AnsiString AsText = {read=GetAsText, write=SetAsText};
//  __property int FormatCount = {read=GetFormatCount, nodefault};
//  __property Word Formats[int Index] = {read=GetFormats};
};

extern TClipboard* Clipboard();
