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

#include "LayerIntf.h"
#include "LayerManager.h"
#include "ComplexRect.h"

class iTVPWindow;
class tTJSNI_BaseLayer;

/*[*/
//---------------------------------------------------------------------------
//! @brief		�`��f�o�C�X�C���^�[�t�F�[�X
//---------------------------------------------------------------------------
class iTVPDrawDevice
{
public:
//---- �I�u�W�F�N�g������Ԑ���
	//! @brief		(Window��DrawDevice) �`��f�o�C�X��j���
	//! @note		�E�B���h�E���j����Ƃ��A���邢�͂ق��̕`��f�o�C�X��
	//!				�ݒ肳�ꂽ���߂ɂ��̕`��f�o�C�X���K�v�Ȃ��Ȃ����ۂɌĂ΂��B
	//!				�ʏ�A�����ł� delete this �����s���A�`��f�o�C�X��j��邪�A���̑O��
	//!				AddLayerManager() �ł��̕`��f�o�C�X�̊Ǘ����ɓ���Ă���
	//!				���C���}�l�[�W�������ׂ� Release ����B
	//!				���C���}�l�[�W���� Release ���� RemoveLayerManager() ���Ă΂��
	//!				�\�������邱�Ƃɒ��ӂ��邱�ƁB
	virtual void TJS_INTF_METHOD Destruct() = 0;

//---- window interface �֘A
	//! @brief		(Window��DrawDevice) �E�B���h�E�C���^�[�t�F�[�X��ݒ肷��
	//! @param		window		�E�B���h�E�C���^�[�t�F�[�X
	//! @note		(TJS����) Window.drawDevice �v���p�e�B��ݒ肵������ɌĂ΂��B
	virtual void TJS_INTF_METHOD SetWindowInterface(iTVPWindow * window) = 0;

//---- LayerManager �̊Ǘ��֘A
	//! @brief		(Window��DrawDevice) ���C���}�l�[�W����ǉ�����
	//! @note		�v���C�}�����C�����E�B���h�E�ɒǉ������ƁA�����I�Ƀ��C���}�l�[�W����
	//!				�쐬����A���ꂪ�`��f�o�C�X�ɂ����̃��\�b�h�̌Ăяo���ɂĒʒm�����B
	//!				�`��f�o�C�X�ł� iTVPLayerManager::AddRef() ���Ăяo���āA�ǉ����ꂽ
	//!				���C���}�l�[�W�������b�N���邱�ƁB
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager) = 0;

	//! @brief		(Window��DrawDevice) ���C���}�l�[�W�����폜����
	//! @note		�v���C�}�����C���� invalidate �����ۂɌĂяo�����B
	//TODO: �v���C�}�����C������A���邢�̓E�B���h�E�j��̏I�����������������H
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager) = 0;

//---- �`��ʒu�E�T�C�Y�֘A
	//! @brief		(Window��DrawDevice) �`���E�B���h�E�̐ݒ�
	//! @param		wnd		�E�B���h�E�n���h��
	//! @param		is_main	���C���E�B���h�E�̏ꍇ�ɐ^
	//! @note		�E�B���h�E����`���ƂȂ�E�B���h�E�n���h�����w�肷�邽�߂ɌĂ΂��B
	//!				���΂��΁AWindow.borderStyle �v���p�e�B���ύX���ꂽ��A�t���X�N���[����
	//!				�ڍs����Ƃ���t���X�N���[������߂鎞�ȂǁA�E�B���h�E���č쐬�����
	//!				���Ƃ����邪�A���̂悤�ȏꍇ�ɂ́A�E�B���h�E����������j���钼�O��
	//!				wnd = NULL �̏�Ԃł��̃��\�b�h���Ă΂�邱�Ƃɒ��ӁB�E�B���h�E���쐬
	//!				���ꂽ���ƁA�ĂїL��ȃE�B���h�E�n���h���𔺂��Ă��̃��\�b�h���Ă΂��B
	//!				���̃��\�b�h�́A�E�B���h�E���쐬���ꂽ����ɌĂ΂��ۏ؂͂Ȃ��B
	//!				�����Ă��A��ԍŏ��ɃE�B���h�E���\�����ꂽ����ɌĂ΂��B
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main) = 0;

	//! @brief		(Window->DrawDevice) �`���`�̐ݒ�
	//! @note		�E�B���h�E����A�`���ƂȂ��`��ݒ肷�邽�߂ɌĂ΂��B
	//!				�`��f�o�C�X�́ASetTargetWindow() �Ŏw�肳�ꂽ�E�B���h�E�̃N���C�A���g�̈�́A
	//!				���̃��\�b�h�Ŏw�肳�ꂽ��`�ɕ\�����s���K�v������B
	//!				���̋�`�́AGetSrcSize �ŕԂ����l�ɑ΂��AWindow.zoomNumer �� Window.zoomDenum
	//!				�v���p�e�B�ɂ��g�嗦��AWindow.layerLeft �� Window.layerTop ���������ꂽ
	//!				��`�ł���B
	//!				���̃��\�b�h�ɂ���ĕ`���`���ς�����Ƃ��Ă��A���̃^�C�~���O��
	//!				�`��f�o�C�X���ōĕ`����s���K�v�͂Ȃ�(�K�v������Εʃ��\�b�h�ɂ��
	//!				�ĕ`��̕K�v�����ʒm����邽��)�B
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect) = 0;

	//! @brief		(Window->DrawDevice) ���摜�̃T�C�Y�𓾂�
	//! @note		�E�B���h�E����A�`���`�̃T�C�Y�����肷�邽�߂Ɍ��摜�̃T�C�Y��
	//!				�K�v�ɂȂ����ۂɌĂ΂��B�E�B���h�E�͂�������Ƃ� SetDestRectangle()
	//!				���\�b�h�ŕ`���`��ʒm���Ă��邾���Ȃ̂ŁA
	//!				�Ȃ�炩�̈Ӗ��̂���T�C�Y�ł���K�v�͕K�������Ȃ��B
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h) = 0;

	//! @brief		(LayerManager��DrawDevice) ���C���T�C�Y�ύX�̒ʒm
	//! @param		manager		���C���}�l�[�W��
	//! @note		���C���}�l�[�W���ɃA�^�b�`����Ă���v���C�}�����C���̃T�C�Y���ς����
	//!				�ۂɌĂяo�����
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) ���C���̉摜�̕ύX�̒ʒm
	//! @param		manager		���C���}�l�[�W��
	//! @note		���C���̉摜�ɕω����������ۂɌĂяo�����B
	//!				���̒ʒm���󂯎������� iTVPLayerManager::UpdateToDrawDevice()
	//!				���Ăяo���΁A�Y��������`��f�o�C�X�ɑ΂��ĕ`�悳���邱�Ƃ��ł���B
	//!				���̒ʒm���󂯎���Ă��������邱�Ƃ͉\�B���̏ꍇ�́A
	//!				���� iTVPLayerManager::UpdateToDrawDevice() ���Ă񂾍ۂɁA
	//!				����܂ł̕ύX�������ׂĕ`�悳���B
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager) = 0;

//---- ���[�U�[�C���^�[�t�F�[�X�֘A
	//! @brief		(Window��DrawDevice) �N���b�N���ꂽ
	//! @param		x		�`���`���ɂ����� x �ʒu(�`���`�̍��オ���_)
	//! @param		y		�`���`���ɂ����� y �ʒu(�`���`�̍��オ���_)
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y) = 0;

	//! @brief		(Window��DrawDevice) �_�u���N���b�N���ꂽ
	//! @param		x		�`���`���ɂ����� x �ʒu(�`���`�̍��オ���_)
	//! @param		y		�`���`���ɂ����� y �ʒu(�`���`�̍��オ���_)
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y) = 0;

	//! @brief		(Window��DrawDevice) �}�E�X�{�^�����������ꂽ
	//! @param		x		�`���`���ɂ����� x �ʒu(�`���`�̍��オ���_)
	//! @param		y		�`���`���ɂ����� y �ʒu(�`���`�̍��オ���_)
	//! @param		mb		�ǂ̃}�E�X�{�^����
	//! @param		flags	�t���O(TVP_SS_*�萔�̑g�ݍ��킹)
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		(Window��DrawDevice) �}�E�X�{�^���������ꂽ
	//! @param		x		�`���`���ɂ����� x �ʒu(�`���`�̍��オ���_)
	//! @param		y		�`���`���ɂ����� y �ʒu(�`���`�̍��オ���_)
	//! @param		mb		�ǂ̃}�E�X�{�^����
	//! @param		flags	�t���O(TVP_SS_*�萔�̑g�ݍ��킹)
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		(Window��DrawDevice) �}�E�X���ړ�����
	//! @param		x		�`���`���ɂ����� x �ʒu(�`���`�̍��オ���_)
	//! @param		y		�`���`���ɂ����� y �ʒu(�`���`�̍��オ���_)
	//! @param		flags	�t���O(TVP_SS_*�萔�̑g�ݍ��킹)
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;

	//! @brief		(Window��DrawDevice) ���C���̃}�E�X�L���v�`��������
	//! @note		���C���̃}�E�X�L���v�`�������ׂ��ꍇ�ɃE�B���h�E����Ă΂��B
	//! @note		WindowReleaseCapture() �ƍ������Ȃ����ƁB
	virtual void TJS_INTF_METHOD OnReleaseCapture() = 0;

	//! @brief		(Window��DrawDevice) �}�E�X���`���`�O�Ɉړ�����
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow() = 0;

	//! @brief		(Window��DrawDevice) �L�[�������ꂽ
	//! @param		key		���z�L�[�R�[�h
	//! @param		shift	�V�t�g�L�[�̏��
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		(Window��DrawDevice) �L�[�������ꂽ
	//! @param		key		���z�L�[�R�[�h
	//! @param		shift	�V�t�g�L�[�̏��
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		(Window��DrawDevice) �L�[�ɂ����
	//! @param		key		�����R�[�h
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key) = 0;

	//! @brief		(Window��DrawDevice) �}�E�X�z�C�[������]����
	//! @param		shift	�V�t�g�L�[�̏��
	//! @param		delta	��]�p
	//! @param		x		�`���`���ɂ����� x �ʒu(�`���`�̍��オ���_)
	//! @param		y		�`���`���ɂ����� y �ʒu(�`���`�̍��オ���_)
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;

	//! @brief		(Window->DrawDevice) ��͏�Ԃ̃`�F�b�N
	//! @note		�E�B���h�E�����1�b�����ɁA���C���}�l�[�W�������[�U����̓�͂̏�Ԃ�
	//!				�ă`�F�b�N���邽�߂ɌĂ΂��B���C����Ԃ̕ω������[�U�̓�͂Ƃ�
	//!				�񓯊�ɍs��ꂽ�ꍇ�A���Ƃ��΃}�E�X�J�[�\���̉��Ƀ��C�����o������
	//!				�̂ɂ�������炸�A�}�E�X�J�[�\�������̃��C���̎w�肷��`��ɕύX����Ȃ�
	//!				�Ƃ������󋵂�����������B���̂悤�ȏ󋵂ɑΏ����邽�߁A�E�B���h�E����
	//!				���̃��\�b�h����1�b�����ɌĂ΂��B
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;

	//! @brief		(LayerManager��DrawDevice) �}�E�X�J�[�\���̌`����f�t�H���g�ɖ߂�
	//! @param		manager		���C���}�l�[�W��
	//! @note		�}�E�X�J�[�\���̌`����f�t�H���g�̕��ɖ߂������ꍇ�ɌĂ΂��
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) �}�E�X�J�[�\���̌`���ݒ肷��
	//! @param		manager		���C���}�l�[�W��
	//! @param		cursor		�}�E�X�J�[�\���`��ԍ�
	virtual void TJS_INTF_METHOD SetMouseCursor(iTVPLayerManager * manager, tjs_int cursor) = 0;

	//! @brief		(LayerManager��DrawDevice) �}�E�X�J�[�\���̈ʒu���擾����
	//! @param		manager		���C���}�l�[�W��
	//! @param		x			�v���C�}�����C����̍��W�ɂ�����}�E�X�J�[�\����x�ʒu
	//! @param		y			�v���C�}�����C����̍��W�ɂ�����}�E�X�J�[�\����y�ʒu
	//! @note		���W�̓v���C�}�����C����̍��W�Ȃ̂ŁA�K�v�Ȃ�Εϊ����s��
	virtual void TJS_INTF_METHOD GetCursorPos(iTVPLayerManager * manager, tjs_int &x, tjs_int &y) = 0;

	//! @brief		(LayerManager��DrawDevice) �}�E�X�J�[�\���̈ʒu��ݒ肷��
	//! @param		manager		���C���}�l�[�W��
	//! @param		x			�v���C�}�����C����̍��W�ɂ�����}�E�X�J�[�\����x�ʒu
	//! @param		y			�v���C�}�����C����̍��W�ɂ�����}�E�X�J�[�\����y�ʒu
	//! @note		���W�̓v���C�}�����C����̍��W�Ȃ̂ŁA�K�v�Ȃ�Εϊ����s��
	virtual void TJS_INTF_METHOD SetCursorPos(iTVPLayerManager * manager, tjs_int x, tjs_int y) = 0;

	//! @brief		(LayerManager��DrawDevice) �E�B���h�E�̃}�E�X�L���v�`��������
	//! @param		manager		���C���}�l�[�W��
	//! @note		�E�B���h�E�̃}�E�X�L���v�`�������ׂ��ꍇ�Ƀ��C���}�l�[�W������Ă΂��B
	//! @note		�E�B���h�E�̃}�E�X�L���v�`���� OnReleaseCapture() �ŊJ��ł��郌�C���̃}�E�X�L���v�`��
	//!				�ƈقȂ邱�Ƃɒ��ӁB�E�B���h�E�̃}�E�X�L���v�`���͎��OS�̃E�B���h�E�V�X�e����
	//!				�@�\�ł��邪�A���C���̃}�E�X�L���v�`���͋g���g�������C���}�l�[�W�����Ƃ�
	//!				�Ǝ��ɊǗ����Ă��镨�ł���B���̃��\�b�h�ł͊�{�I�ɂ� ::ReleaseCapture() �Ȃǂ�
	//!				�}�E�X�̃L���v�`�����J���B
	virtual void TJS_INTF_METHOD WindowReleaseCapture(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) �c�[���`�b�v�q���g��ݒ肷��
	//! @param		manager		���C���}�l�[�W��
	//! @param		text		�q���g�e�L�X�g(�󕶎���̏ꍇ�̓q���g�̕\�����L�����Z������)
	virtual void TJS_INTF_METHOD SetHintText(iTVPLayerManager * manager, const ttstr & text) = 0;

	//! @brief		(LayerManager��DrawDevice) �����|�C���g�̐ݒ�
	//! @param		manager		���C���}�l�[�W��
	//! @param		layer		�t�H���g���̊܂܂�郌�C��
	//! @param		x			�v���C�}�����C����̍��W�ɂ����钍���|�C���g��x�ʒu
	//! @param		y			�v���C�}�����C����̍��W�ɂ����钍���|�C���g��y�ʒu
	//! @note		�����|�C���g�͒ʏ�L�����b�g�ʒu�̂��ƂŁA������IME�̃R���|�W�b�g�E�E�B���h�E��
	//!				�\�����ꂽ��A���[�U�⏕�̊g�勾���������g�債���肷��BIME���R���|�W�b�g�E�B���h�E��
	//!				�\��������A���m��̕����������ɕ\�������肷��ۂ̃t�H���g�� layer �p�����[�^
	//!				�Ŏ�����郌�C���������ɂ�邪�A�v���O�C�����炻�̏��𓾂���ݒ肵����
	//!				����C���^�[�t�F�[�X�͍��̂Ƃ���Ȃ��B
	//! @note		���W�̓v���C�}�����C����̍��W�Ȃ̂ŁA�K�v�Ȃ�Εϊ����s���B
	virtual void TJS_INTF_METHOD SetAttentionPoint(iTVPLayerManager * manager, tTJSNI_BaseLayer *layer,
							tjs_int l, tjs_int t) = 0;

	//! @brief		(LayerManager��DrawDevice) �����|�C���g�̉���
	//! @param		manager		���C���}�l�[�W��
	virtual void TJS_INTF_METHOD DisableAttentionPoint(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) IME���[�h�̐ݒ�
	//! @param		manager		���C���}�l�[�W��
	//! @param		mode		IME���[�h
	virtual void TJS_INTF_METHOD SetImeMode(iTVPLayerManager * manager, tTVPImeMode mode) = 0;

	//! @brief		(LayerManager��DrawDevice) IME���[�h�̃��Z�b�g
	//! @param		manager		���C���}�l�[�W��
	virtual void TJS_INTF_METHOD ResetImeMode(iTVPLayerManager * manager) = 0;

//---- �v���C�}�����C���֘A
	//! @brief		(Window��DrawDevice) �v���C�}�����C���̎擾
	//! @return		�v���C�}�����C��
	//! @note		Window.primaryLayer ���ǂݏo���ꂽ�ۂɂ��̃��\�b�h���Ă΂��B
	//!				����ȊO�ɌĂ΂�邱�Ƃ͂Ȃ��B
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() = 0;

	//! @brief		(Window��DrawDevice) �t�H�[�J�X�̂��郌�C���̎擾
	//! @return		�t�H�[�J�X�̂��郌�C��(NULL=�t�H�[�J�X�̂��郌�C�����Ȃ��ꍇ)
	//! @note		Window.focusedLayer ���ǂݏo���ꂽ�ۂɂ��̃��\�b�h���Ă΂��B
	//!				����ȊO�ɌĂ΂�邱�Ƃ͂Ȃ��B
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() = 0;

	//! @brief		(Window��DrawDevice) �t�H�[�J�X�̂��郌�C���̐ݒ�
	//! @param		layer		�t�H�[�J�X�̂��郌�C��(NULL=�t�H�[�J�X�̂��郌�C�����Ȃ���Ԃɂ������ꍇ)
	//! @note		Window.focusedLayer ���������܂ꂽ�ۂɂ��̃��\�b�h���Ă΂��B
	//!				����ȊO�ɌĂ΂�邱�Ƃ͂Ȃ��B
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;


//---- �ĕ`��֘A
	//! @brief		(Window��DrawDevice) �`���`�̖���̒ʒm
	//! @param		rect		�`���`���̍��W�ɂ�����A����ɂȂ����̈�
	//!							(�`���`�̍��オ���_)
	//! @note		�`���`�̈ꕔ���邢�͑S��������ɂȂ����ۂɃE�B���h�E����ʒm�����B
	//!				�`��f�o�C�X�́A�Ȃ�ׂ���������ɖ���ɂȂ����������ĕ`�悷�ׂ��ł���B
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect) = 0;

	//! @brief		(Window��DrawDevice) �X�V�̗v��
	//! @note		�`���`�̓��e���ŐV�̏�ԂɍX�V���ׂ��^�C�~���O�ŁA�E�B���h�E����Ă΂��B
	//!				iTVPWindow::RequestUpdate() ���Ă񂾌�A�V�X�e�����`��^�C�~���O�ɓ�����ۂ�
	//!				�Ă΂��B�ʏ�A�`��f�o�C�X�͂��̃^�C�~���O�𗘗p���ăI�t�X�N���[��
	//!				�T�[�t�F�[�X�ɉ摜��`�悷��B
	virtual void TJS_INTF_METHOD Update() = 0;

	//! @brief		(Window->DrawDevice) �摜�̕\��
	//! @note		�I�t�X�N���[���T�[�t�F�[�X�ɕ`�悳�ꂽ�摜���A�I���X�N���[���ɕ\������
	//!				(���邢�̓t���b�v����) �^�C�~���O�ŌĂ΂��B�ʏ�� Update �̒����
	//!				�Ă΂�邪�AVSync �҂����L��ɂȂ��Ă���ꍇ�� Update ����ł͂Ȃ��A
	//!				VBlank ���ɌĂ΂��\��������B�I�t�X�N���[���T�[�t�F�[�X��
	//!				�g��Ȃ��ꍇ�͖������Ă��܂�Ȃ��B
	virtual void TJS_INTF_METHOD Show() = 0;

//---- LayerManager ����̉摜�󂯓n���֘A
	//! @brief		(LayerManager->DrawDevice) �r�b�g�}�b�v�̕`����J�n����
	//! @param		manager		�`����J�n���郌�C���}�l�[�W��
	//! @note		���C���}�l�[�W������`��f�o�C�X�։摜���]�������O�ɌĂ΂��B
	//!				���̂��ƁANotifyBitmapCompleted() ���C�ӂ̉񐔌Ă΂�A�Ō��
	//!				EndBitmapCompletion() ���Ă΂��B
	//!				�K�v�Ȃ�΁A���̃^�C�~���O�ŕ`��f�o�C�X���ŃT�[�t�F�[�X�̃��b�N�Ȃǂ�
	//!				�s�����ƁB
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager->DrawDevice) �r�b�g�}�b�v�̕`���ʒm����
	//! @param		manager		�摜�̒񋟌��̃��C���}�l�[�W��
	//! @param		x			�v���C�}�����C����̍��W�ɂ�����摜�̍��[�ʒu
	//! @param		y			�v���C�}�����C����̍��W�ɂ�����摜�̏�[�ʒu
	//! @param		bits		�r�b�g�}�b�v�f�[�^
	//! @param		bitmapinfo	�r�b�g�}�b�v�̌`�����
	//! @param		cliprect	bits �̂����A�ǂ̕������g���ė~�������̏��
	//! @param		type		�񋟂����摜���z�肷�鍇�����[�h
	//! @param		opacity		�񋟂����摜���z�肷��s�����x(0�`255)
	//! @note		���C���}�l�[�W�����������������A���ʂ�`��f�o�C�X�ɕ`�悵�Ă��炢�����ۂ�
	//!				�Ă΂��B��̍X�V�������̋�`�ō\�������ꍇ�����邽�߁A���̃��\�b�h��
	//!				StartBitmapCompletion() �� EndBitmapCompletion() �̊Ԃɕ�����Ă΂��\��������B
	//!				��{�I�ɂ́Abits �� bitmapinfo �ŕ\�����r�b�g�}�b�v�̂����Acliprect ��
	//!				��������`�� x, y �ʒu�ɓ]������΂悢���A�`���`�̑傫���ɍ��킹��
	//!				�g���k���Ȃǂ͕`��f�o�C�X���Ŗʓ|������K�v������B
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) = 0;

	//! @brief		(LayerManager->DrawDevice) �r�b�g�}�b�v�̕`����I������
	//! @param		manager		�`����I�����郌�C���}�l�[�W��
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager) = 0;

//---- �f�o�b�O�x��
	//! @brief		(Window->DrawDevice) ���C���\�����R���\�[���Ƀ_���v����
	virtual void TJS_INTF_METHOD DumpLayerStructure() = 0;

	//! @brief		(Window->DrawDevice) �X�V��`�̕\�����s�����ǂ�����ݒ肷��
	//! @param		b		�\�����s�����ǂ���
	//! @note		���C���\���@�\�������X�V���s���ۂ̋�`��\�����A
	//!				�����X�V�̍œK���ɖ𗧂Ă邽�߂̎x���@�\�B
	//!				��������K�v�͂Ȃ����A�������邱�Ƃ��]�܂����B
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b) = 0;
};
//---------------------------------------------------------------------------
/*]*/

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
