//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Text Editor
//---------------------------------------------------------------------------
#ifndef PadImplH
#define PadImplH
//---------------------------------------------------------------------------
#include "tjsNative.h"
#include "PadIntf.h"

class TTVPPadForm;
//---------------------------------------------------------------------------
// tTJSNI_Pad : Pad Class C++ Native Instance
//---------------------------------------------------------------------------
class tTJSNI_Pad : public tTJSNI_BasePad
{
	TTVPPadForm *Form;
public:
	virtual tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *dsp);
	virtual void TJS_INTF_METHOD Invalidate();

// methods
	virtual void OpenFromStorage(const ttstr & name);
	virtual void SaveToStorage(const ttstr & name);

// properties
	virtual tjs_uint32 GetColor() const;
	virtual void SetColor(tjs_uint32 color);
	virtual bool GetVisible() const;
	virtual void SetVisible(bool state);
	virtual ttstr GetFileName() const;
	virtual void SetFileName(const ttstr &name);
	virtual void SetText(const ttstr &content);
	virtual ttstr GetText() const;
	virtual void SetTitle(const ttstr &title);
	virtual ttstr GetTitle() const;

protected:
private:
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif

