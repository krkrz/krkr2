//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Text Editor
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "PadImpl.h"
#include "PadIntf.h"
#include "PadFormUnit.h"


//---------------------------------------------------------------------------
// tTJSNI_Pad : Pad Class C++ Native Instance
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNI_Pad::Construct(tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 * dsp)
{
	HRESULT hr = tTJSNI_BasePad::Construct(numparams, param, dsp);
	if(TJS_FAILED(hr)) return hr;

	Form = new TTVPPadForm(Application);
	Form->SetExecButtonEnabled(false);

	return S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD
tTJSNI_Pad::Invalidate()
{
	if(Form) delete Form, Form = NULL;
	tTJSNI_BasePad::Invalidate();
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::OpenFromStorage(const ttstr & name)
{
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::SaveToStorage(const ttstr & name)
{
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::SetText(const ttstr &content)
{
	Form->SetLines(content);
}
//---------------------------------------------------------------------------
ttstr tTJSNI_Pad::GetText() const
{
	return Form->GetLines();
}
//---------------------------------------------------------------------------
tjs_uint32 tTJSNI_Pad::GetColor() const
{
	return Form->GetEditColor();
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::SetColor(tjs_uint32 color)
{
	Form->SetEditColor(color);
}
//---------------------------------------------------------------------------
bool tTJSNI_Pad::GetVisible() const
{
	return Form->Visible;
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::SetVisible(bool state)
{
	Form->Visible = state;
}
//---------------------------------------------------------------------------
ttstr tTJSNI_Pad::GetFileName() const
{
	return Form->GetFileName();
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::SetFileName(const ttstr & name)
{
	Form->SetFileName(name);
}
//---------------------------------------------------------------------------
void tTJSNI_Pad::SetTitle(const ttstr &title)
{
	Form->Caption = title.AsAnsiString();
}
//---------------------------------------------------------------------------
ttstr tTJSNI_Pad::GetTitle() const
{
	return Form->Caption;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTJSNC_Pad
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_Pad::CreateNativeInstance()
{
	return new tTJSNI_Pad();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPCreateNativeClass_Pad
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Pad()
{
	return new tTJSNC_Pad();
}
//---------------------------------------------------------------------------

