//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------

#include "tjsCommHead.h"

#include <algorithm>
#include "DrawDevice.h"
#include "MsgIntf.h"
#include "LayerIntf.h"
#include "LayerManager.h"
#include "WindowIntf.h"


//---------------------------------------------------------------------------
tTVPDrawDevice::tTVPDrawDevice()
{
	// �R���X�g���N�^
	Window = NULL;
	PrimaryLayerManagerIndex = 0;
	DestRect.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPDrawDevice::~tTVPDrawDevice()
{
	// ���ׂĂ� managers ���J���
	//TODO: �v���C�}�����C������A���邢�̓E�B���h�E�j��̏I�����������������H
	// managers �� �J����ہA���g�̓o�^�������s�����߂�
	// RemoveLayerManager() ���ĂԂ�������Ȃ��̂Œ��ӁB
	// ���̂��߁A�����ł͂�������z����R�s�[���Ă��炻�ꂼ���
	// Release() ���ĂԁB
	std::vector<iTVPLayerManager *> backup = Managers;
	for(std::vector<iTVPLayerManager *>::iterator i = backup.begin(); i != backup.end(); i++)
		(*i)->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTVPDrawDevice::TransformToPrimaryLayerManager(tjs_int &x, tjs_int &y)
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return false;

	// �v���C�}�����C���}�l�[�W���̃v���C�}�����C���̃T�C�Y�𓾂�
	tjs_int pl_w, pl_h;
	if(!manager->GetPrimaryLayerSize(pl_w, pl_h)) return false;

	// x , y �� DestRect �� 0, 0 �����_�Ƃ������W�Ƃ��ēn����Ă��Ă���
	tjs_int w = DestRect.get_width();
	tjs_int h = DestRect.get_height();
	x = w ? (x * pl_w / w) : 0;
	y = h ? (y * pl_h / h) : 0;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTVPDrawDevice::TransformFromPrimaryLayerManager(tjs_int &x, tjs_int &y)
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return false;

	// �v���C�}�����C���}�l�[�W���̃v���C�}�����C���̃T�C�Y�𓾂�
	tjs_int pl_w, pl_h;
	if(!manager->GetPrimaryLayerSize(pl_w, pl_h)) return false;

	// x , y �� DestRect �� 0, 0 �����_�Ƃ������W�Ƃ��ēn����Ă��Ă���
	x = pl_w ? (x * DestRect.get_width()  / pl_w) : 0;
	y = pl_h ? (y * DestRect.get_height() / pl_h) : 0;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::Destruct()
{
	delete this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetWindowInterface(iTVPWindow * window)
{
	Window = window;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	// Managers �� manager �� push ����BAddRef����̂�Y��Ȃ����ƁB
	Managers.push_back(manager);
	manager->AddRef();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::RemoveLayerManager(iTVPLayerManager * manager)
{
	// Managers ���� manager ���폜����BRelease����B
	std::vector<iTVPLayerManager *>::iterator i = std::find(Managers.begin(), Managers.end(), manager);
	if(i == Managers.end())
		TVPThrowInternalError;
	(*i)->Release();
	Managers.erase(i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetDestRectangle(const tTVPRect & rect)
{
	DestRect = rect;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::GetSrcSize(tjs_int &w, tjs_int &h)
{
	w = 0;
	h = 0;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;
	if(!manager->GetPrimaryLayerSize(w, h))
	{
		w = 0;
		h = 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::NotifyLayerResize(iTVPLayerManager * manager)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(primary_manager == manager)
		Window->NotifySrcResize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::NotifyLayerImageChange(iTVPLayerManager * manager)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(primary_manager == manager)
		Window->RequestUpdate();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnClick(tjs_int x, tjs_int y)
{
	if(!TransformToPrimaryLayerManager(x, y)) return;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyClick(x, y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnDoubleClick(tjs_int x, tjs_int y)
{
	if(!TransformToPrimaryLayerManager(x, y)) return;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyDoubleClick(x, y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if(!TransformToPrimaryLayerManager(x, y)) return;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyMouseDown(x, y, mb, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
{
	if(!TransformToPrimaryLayerManager(x, y)) return;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyMouseUp(x, y, mb, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
{
	if(!TransformToPrimaryLayerManager(x, y)) return;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyMouseMove(x, y, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnReleaseCapture()
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->ReleaseCapture();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnMouseOutOfWindow()
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyMouseOutOfWindow();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnKeyDown(tjs_uint key, tjs_uint32 shift)
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyKeyDown(key, shift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnKeyUp(tjs_uint key, tjs_uint32 shift)
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyKeyUp(key, shift);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnKeyPress(tjs_char key)
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyKeyPress(key);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)
{
	if(!TransformToPrimaryLayerManager(x, y)) return;
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->NotifyMouseWheel(shift, delta, x, y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::RecheckInputState()
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;

	manager->RecheckInputState();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetDefaultMouseCursor(iTVPLayerManager * manager)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->SetDefaultMouseCursor();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetMouseCursor(iTVPLayerManager * manager, tjs_int cursor)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->SetMouseCursor(cursor);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::GetCursorPos(iTVPLayerManager * manager, tjs_int &x, tjs_int &y)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	Window->GetCursorPos(x, y);
	if(primary_manager != manager || !TransformToPrimaryLayerManager(x, y))
	{
		// �v���C�}�����C���}�l�[�W���ȊO�ɂ͍��W 0,0 �œn���Ă���
		 x = 0;
		 y = 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetCursorPos(iTVPLayerManager * manager, tjs_int x, tjs_int y)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		if(TransformFromPrimaryLayerManager(x, y))
			Window->SetCursorPos(x, y);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::WindowReleaseCapture(iTVPLayerManager * manager)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->WindowReleaseCapture();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetHintText(iTVPLayerManager * manager, const ttstr & text)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->SetHintText(text);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetAttentionPoint(iTVPLayerManager * manager, tTJSNI_BaseLayer *layer,
				tjs_int l, tjs_int t)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		if(TransformFromPrimaryLayerManager(l, t))
			Window->SetAttentionPoint(layer, l, t);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::DisableAttentionPoint(iTVPLayerManager * manager)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->DisableAttentionPoint();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetImeMode(iTVPLayerManager * manager, tTVPImeMode mode)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->SetImeMode(mode);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::ResetImeMode(iTVPLayerManager * manager)
{
	iTVPLayerManager * primary_manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!primary_manager) return;
	if(primary_manager == manager)
	{
		Window->ResetImeMode();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTJSNI_BaseLayer * TJS_INTF_METHOD tTVPDrawDevice::GetPrimaryLayer()
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return NULL;
	return manager->GetPrimaryLayer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTJSNI_BaseLayer * TJS_INTF_METHOD tTVPDrawDevice::GetFocusedLayer()
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return NULL;
	return manager->GetFocusedLayer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetFocusedLayer(tTJSNI_BaseLayer * layer)
{
	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;
	manager->SetFocusedLayer(layer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::RequestInvalidation(const tTVPRect & rect)
{
	tjs_int l = rect.left, t = rect.top, r = rect.right, b = rect.bottom;
	if(!TransformToPrimaryLayerManager(l, t)) return;
	if(!TransformToPrimaryLayerManager(r, b)) return;
	r ++; // �덷�̋z��(�{���͂���������ƌ����ɂ��Ȃ��ƂȂ�Ȃ������ꂪ���ɂȂ邱�Ƃ͂Ȃ�)
	b ++;

	iTVPLayerManager * manager = GetLayerManagerAt(PrimaryLayerManagerIndex);
	if(!manager) return;
	manager->RequestInvalidation(tTVPRect(l, t, r, b));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::Update()
{
	// ���ׂĂ� layer manager �� UpdateToDrawDevice ���Ă�
	for(std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++)
	{
		(*i)->UpdateToDrawDevice();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::Show()
{
	// �Ȃɂ����Ȃ�
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::DumpLayerStructure()
{
	// ���ׂĂ� layer manager �� DumpLayerStructure ���Ă�
	for(std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++)
	{
		(*i)->DumpLayerStructure();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPDrawDevice::SetShowUpdateRect(bool b)
{
	// �Ȃɂ����Ȃ�
}
//---------------------------------------------------------------------------
