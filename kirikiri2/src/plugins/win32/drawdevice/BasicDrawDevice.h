//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------
#ifndef DRAWDEVICE_H
#define DRAWDEVICE_H

#include "tp_stub.h"
#include <vector>


//---------------------------------------------------------------------------
//! @brief		�`��f�o�C�X�C���^�[�t�F�[�X�̊�{�I�Ȏ���
//---------------------------------------------------------------------------
class tTVPDrawDevice : public iTVPDrawDevice
{
protected:
	iTVPWindow * Window;
	size_t PrimaryLayerManagerIndex; //!< �v���C�}�����C���}�l�[�W��
	std::vector<iTVPLayerManager *> Managers; //!< ���C���}�l�[�W���̔z��
	tTVPRect DestRect; //!< �`���ʒu

protected:
	tTVPDrawDevice(); //!< �R���X�g���N�^
protected:
	virtual ~tTVPDrawDevice(); //!< �f�X�g���N�^

public:
	//! @brief		�w��ʒu�ɂ��郌�C���}�l�[�W���𓾂�
	//! @param		index		�C���f�b�N�X(0�`)
	//! @return		�w��ʒu�ɂ��郌�C���}�l�[�W��(AddRef����Ȃ��̂Œ���)�B
	//!				�w��ʒu�Ƀ��C���}�l�[�W�����Ȃ����NULL���Ԃ�
	iTVPLayerManager * GetLayerManagerAt(size_t index)
	{
		if(Managers.size() <= index) return NULL;
		return Managers[index];
	}

	//! @brief		Device��LayerManager���̍��W�̕ϊ����s��
	//! @param		x		X�ʒu
	//! @param		y		Y�ʒu
	//! @return		�ϊ��ɐ����ΐ^�B�����Ȃ���΋U�BPrimaryLayerManagerIndex�ɊY������
	//!				���C���}�l�[�W�����Ȃ���΋U���Ԃ�
	//! @note		x, y �� DestRect�� (0,0) �����_�Ƃ�����W�Ƃ��ēn�����ƌ��Ȃ�
	bool TransformToPrimaryLayerManager(tjs_int &x, tjs_int &y);

	//! @brief		LayerManager��Device���̍��W�̕ϊ����s��
	//! @param		x		X�ʒu
	//! @param		y		Y�ʒu
	//! @return		�ϊ��ɐ����ΐ^�B�����Ȃ���΋U�BPrimaryLayerManagerIndex�ɊY������
	//!				���C���}�l�[�W�����Ȃ���΋U���Ԃ�
	//! @note		x, y �� ���C���� (0,0) �����_�Ƃ�����W�Ƃ��ēn�����ƌ��Ȃ�
	bool TransformFromPrimaryLayerManager(tjs_int &x, tjs_int &y);

//---- �I�u�W�F�N�g������Ԑ���
	virtual void TJS_INTF_METHOD Destruct();

//---- window interface �֘A
	virtual void TJS_INTF_METHOD SetWindowInterface(iTVPWindow * window);

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager);

//---- ���[�U�[�C���^�[�t�F�[�X�֘A
	// window �� drawdevice
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnReleaseCapture();
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow();
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD RecheckInputState();

	// layer manager �� drawdevice
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD SetMouseCursor(iTVPLayerManager * manager, tjs_int cursor);
	virtual void TJS_INTF_METHOD GetCursorPos(iTVPLayerManager * manager, tjs_int &x, tjs_int &y);
	virtual void TJS_INTF_METHOD SetCursorPos(iTVPLayerManager * manager, tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD SetHintText(iTVPLayerManager * manager, const ttstr & text);
	virtual void TJS_INTF_METHOD WindowReleaseCapture(iTVPLayerManager * manager);

	virtual void TJS_INTF_METHOD SetAttentionPoint(iTVPLayerManager * manager, tTJSNI_BaseLayer *layer,
							tjs_int l, tjs_int t);
	virtual void TJS_INTF_METHOD DisableAttentionPoint(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD SetImeMode(iTVPLayerManager * manager, tTVPImeMode mode);
	virtual void TJS_INTF_METHOD ResetImeMode(iTVPLayerManager * manager);

//---- �v���C�}�����C���֘A
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer();
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer();
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer);

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD Update();
	virtual void TJS_INTF_METHOD Show() = 0;

//---- �f�o�b�O�x��
	virtual void TJS_INTF_METHOD DumpLayerStructure();
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b);

// �ق��̃��\�b�h�ɂ��Ă͎������Ȃ�
};
//---------------------------------------------------------------------------
#endif
