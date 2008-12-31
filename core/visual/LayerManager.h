//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Layer Management
//---------------------------------------------------------------------------
#ifndef LayerManagerH
#define LayerManagerH


#include "LayerIntf.h"
#include "drawable.h"

/*[*/
class tTJSNI_BaseLayer;
//---------------------------------------------------------------------------
// abstract class of Layer Manager 
//---------------------------------------------------------------------------
class iTVPLayerManager
{
public:
//-- object lifetime management
	//! @brief	�Q�ƃJ�E���^���C���N�������g����
	virtual void TJS_INTF_METHOD AddRef() = 0;

	//! @brief	�Q�ƃJ�E���^���f�N�������g����
	virtual void TJS_INTF_METHOD Release() = 0;

//-- draw device specific information
	//! @brief	�`��f�o�C�X�ŗL�̏���ݒ肷��
	//! @param	data	�`��f�o�C�X�ŗL�̏��
	//! @note	�`��f�o�C�X�ŗL�̏������C���}�l�[�W���ɐݒ肷��B
	//!			���C���}�l�[�W���ł͂��̏��̒��g�ɂ��Ă͊֒m���Ȃ��B
	//!			�`��f�o�C�X���Ŗڈ�Ɏg������A����̏��ƌ��т��ĊǗ�����B
	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) = 0;

	//! @brief	�`��f�o�C�X�ŗL�̏����擾����
	//! @return	�`��f�o�C�X�ŗL�̏��
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const = 0;

//-- layer metrics
	//! @brief	�v���C�}�����C���̃T�C�Y���擾����
	//! @param	w	���C���̉���(�s�N�Z���P��)
	//! @param	h	���C���̏c��(�s�N�Z���P��)
	//! @return	�擾�ɐ����ΐ^�A���s����΋U
	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const = 0;

//-- layer structure information
	//! @brief	�v���C�}�����C���̎擾
	//! @return	�v���C�}�����C��
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const = 0;

	//! @brief	�t�H�[�J�X�̂��郌�C���̎擾
	//! @return	�t�H�[�J�X�̂��郌�C��
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const = 0;

	//! @brief	�t�H�[�J�X�̂��郌�C���̐ݒ�
	//! @param	layer	�t�H�[�J�X�̂��郌�C��
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;

//-- HID releted
	//! @brief		�N���b�N���ꂽ
	//! @param		x		�v���C�}�����C�����W��ɂ����� x �ʒu
	//! @param		y		�v���C�}�����C�����W��ɂ����� y �ʒu
	virtual void TJS_INTF_METHOD NotifyClick(tjs_int x, tjs_int y) = 0;

	//! @brief		�_�u���N���b�N���ꂽ
	//! @param		x		�v���C�}�����C�����W��ɂ����� x �ʒu
	//! @param		y		�v���C�}�����C�����W��ɂ����� y �ʒu
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y) = 0;

	//! @brief		�}�E�X�{�^�����������ꂽ
	//! @param		x		�v���C�}�����C�����W��ɂ����� x �ʒu
	//! @param		y		�v���C�}�����C�����W��ɂ����� y �ʒu
	//! @param		mb		�ǂ̃}�E�X�{�^����
	//! @param		flags	�t���O(TVP_SS_*�萔�̑g�ݍ��킹)
	virtual void TJS_INTF_METHOD NotifyMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		�}�E�X�{�^���������ꂽ
	//! @param		x		�v���C�}�����C�����W��ɂ����� x �ʒu
	//! @param		y		�v���C�}�����C�����W��ɂ����� y �ʒu
	//! @param		mb		�ǂ̃}�E�X�{�^����
	//! @param		flags	�t���O(TVP_SS_*�萔�̑g�ݍ��킹)
	virtual void TJS_INTF_METHOD NotifyMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		�}�E�X���ړ�����
	//! @param		x		�v���C�}�����C�����W��ɂ����� x �ʒu
	//! @param		y		�v���C�}�����C�����W��ɂ����� y �ʒu
	//! @param		flags	�t���O(TVP_SS_*�萔�̑g�ݍ��킹)
	virtual void TJS_INTF_METHOD NotifyMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;

	//! @brief		�}�E�X�L���v�`��������
	//! @note		�}�E�X�L���v�`�������ׂ��ꍇ�ɃE�B���h�E����Ă΂��B
	virtual void TJS_INTF_METHOD ReleaseCapture() = 0;

	//! @brief		�}�E�X���v���C�}�����C���O�Ɉړ�����
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow() = 0;

	//! @brief		�L�[�������ꂽ
	//! @param		key		���z�L�[�R�[�h
	//! @param		shift	�V�t�g�L�[�̏��
	virtual void TJS_INTF_METHOD NotifyKeyDown(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		�L�[�������ꂽ
	//! @param		key		���z�L�[�R�[�h
	//! @param		shift	�V�t�g�L�[�̏��
	virtual void TJS_INTF_METHOD NotifyKeyUp(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		�L�[�ɂ����
	//! @param		key		�����R�[�h
	virtual void TJS_INTF_METHOD NotifyKeyPress(tjs_char key) = 0;

	//! @brief		�}�E�X�z�C�[������]����
	//! @param		shift	�V�t�g�L�[�̏��
	//! @param		delta	��]�p
	//! @param		x		�v���C�}�����C�����W��ɂ����� x �ʒu
	//! @param		y		�v���C�}�����C�����W��ɂ����� y �ʒu
	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;

	//! @brief		��͏�Ԃ̃`�F�b�N
	//! @note		�E�B���h�E�����1�b�����ɁA���C���}�l�[�W�������[�U����̓�͂̏�Ԃ�
	//!				�ă`�F�b�N���邽�߂ɌĂ΂��B���C����Ԃ̕ω������[�U�̓�͂Ƃ�
	//!				�񓯊�ɍs��ꂽ�ꍇ�A���Ƃ��΃}�E�X�J�[�\���̉��Ƀ��C�����o������
	//!				�̂ɂ�������炸�A�}�E�X�J�[�\�������̃��C���̎w�肷��`��ɕύX����Ȃ�
	//!				�Ƃ������󋵂�����������B���̂悤�ȏ󋵂ɑΏ����邽�߁A�E�B���h�E����
	//!				���̃��\�b�h����1�b�����ɌĂ΂��B
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;

//-- invalidation/update
	//! @brief		�`��f�o�C�X���]�ރ��C���̏o�͌`����ݒ肷��
	//! @param		type	���C���`��
	//! @note		�f�t�H���g�� ltOpaque �B�`��f�o�C�X�����̌`���̉摜���o�͂Ƃ���
	//!				�]�ނȂ�΂��̌`�����w�肷��B�������A�v���C�}�����C���� type
	//!				�v���p�e�B�����l�ɕύX���邱�ƁB
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) = 0;

	//! @brief		����̋�`�̍ĕ`���v������
	//! @param		r		�v���C�}�����C�����W��ɂ������`
	//! @note		����̋�`�̍ĕ`������C���}�l�[�W���ɑ΂��ėv������B
	//!				�v���͋L�^����邾���ł��̃��\�b�h�͂����ɖ߂�B���ۂɂ��ꂪ
	//!				���Z�����̂� UpdateToDrawDevice() ���Ă񂾂Ƃ��ł���B
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r) = 0; // draw device -> layer

	//! @brief		���e�̍ĕ`����s��
	//! @note		���e�̍ĕ`����s���ۂɌĂԁB���̃��\�b�h���ł́A���C���}�l�[�W����
	//!				iTVPDrawDevice::StartBitmapCompletion()
	//!				iTVPDrawDevice::NotifyBitmapCompleted()
	//!				iTVPDrawDevice::EndBitmapCompletion() �̊e���\�b�h��p���A
	//!				���܂܂łɕύX���s��ꂽ�̈�Ȃǂ������`��f�o�C�X�ɑ���B
	virtual void TJS_INTF_METHOD UpdateToDrawDevice() = 0;

//-- debug assist
	//! @brief		(Window->DrawDevice) ���C���\�����R���\�[���Ƀ_���v����
	virtual void TJS_INTF_METHOD DumpLayerStructure() = 0;
};
//---------------------------------------------------------------------------
/*]*/


//---------------------------------------------------------------------------
// tTVPLayerManager
//---------------------------------------------------------------------------
// layer mamager which is to be connected to draw device
//---------------------------------------------------------------------------
class tTVPLayerManager : public iTVPLayerManager, public tTVPDrawable
{
	tjs_int RefCount; //!< reference count
	tTJSNI_BaseWindow * Window;

	void * DrawDeviceData; //!< draw device specific information

	tTVPBaseBitmap * DrawBuffer;
	tTVPLayerType DesiredLayerType; //!< desired layer type by the draw device for this layer manager

	tTJSNI_BaseLayer * CaptureOwner;
	tTJSNI_BaseLayer * LastMouseMoveSent;

	std::vector<tTJSNI_BaseLayer *> ModalLayerVector;
		// pointer to modal layer vector
	tTJSNI_BaseLayer * FocusedLayer; // pointer to current focused layer
	tTJSNI_BaseLayer * Primary; // primary layer
	bool OverallOrderIndexValid;
	std::vector<tTJSNI_BaseLayer*> AllNodes;
		// hold overall nodes;
		// use GetAllNodes to retrieve the newest information of this
	tTVPComplexRect UpdateRegion; // window update region

	bool FocusChangeLock;

	bool VisualStateChanged;
		// flag for visual
		// state changing ( see tTJSNI_BaseLaye::NotifyChildrenVisualStateChanged)

	tjs_int LastMouseMoveX;
	tjs_int LastMouseMoveY;

	bool ReleaseCaptureCalled;

	bool InNotifyingHintOrCursorChange;

public:
	tTVPLayerManager(tTJSNI_BaseWindow *window);

private:
	virtual ~tTVPLayerManager();
public:
	virtual void TJS_INTF_METHOD AddRef();
	virtual void TJS_INTF_METHOD Release();

	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) { DrawDeviceData = data; }
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const { return DrawDeviceData; }

public:
	void RegisterSelfToWindow();
	void UnregisterSelfFromWindow();

public:
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) { DesiredLayerType = type; }

public: // methods from tTVPDrawable
	virtual tTVPBaseBitmap * GetDrawTargetBitmap(const tTVPRect &rect,
		tTVPRect &cliprect);

	virtual tTVPLayerType GetTargetLayerType();

	virtual void DrawCompleted(const tTVPRect &destrect,
		tTVPBaseBitmap *bmp, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity);

public:
	void AttachPrimary(tTJSNI_BaseLayer *pri); // attach primary layer to the manager
	void DetachPrimary(); // detach primary layer from the manager

	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const { return Primary; }
	bool IsPrimaryLayerAttached() const { return Primary != NULL; } 

	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const;

	void NotifyPart(tTJSNI_BaseLayer *lay); // notifies layer parting from its parent

	tTVPComplexRect & GetUpdateRegionForCompletion() { return UpdateRegion; }

private:

	void _RecreateOverallOrderIndex(tjs_uint& index,
		std::vector<tTJSNI_BaseLayer*>& nodes);

public:
	void InvalidateOverallIndex();
	void RecreateOverallOrderIndex();

	std::vector<tTJSNI_BaseLayer*> & GetAllNodes();

	void NotifyVisualStateChanged() { VisualStateChanged = true; }
	bool GetVisualStateChanged() { return VisualStateChanged; }
	void QueryUpdateExcludeRect();


public:

	void NotifyMouseCursorChange(tTJSNI_BaseLayer *layer, tjs_int cursor);
	void SetMouseCursor(tjs_int cursor);

	void GetCursorPos(tjs_int &x, tjs_int &y);
		// get cursor position in primary coordinates
	void SetCursorPos(tjs_int x, tjs_int y);
		// set cursor position in primary coordinates

	void NotifyHintChange(tTJSNI_BaseLayer *layer, const ttstr & hint);
	void SetHint(const ttstr &hint);
		// set layer hint to current window
	void NotifyLayerResize();  // layer -> window
	void NotifyWindowInvalidation(); // layer -> window

public:
	tTJSNI_BaseWindow * GetWindow() const { return Window; }
	void SetWindow(tTJSNI_BaseWindow *window);
	void NotifyResizeFromWindow(tjs_uint w, tjs_uint h); // draw device -> layer
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r); // draw device -> layer

	virtual void TJS_INTF_METHOD NotifyClick(tjs_int x, tjs_int y) { PrimaryClick(x, y); }
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y) { PrimaryDoubleClick(x, y); }
	virtual void TJS_INTF_METHOD NotifyMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
		{ PrimaryMouseDown(x, y, mb, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
		{ PrimaryMouseUp(x, y, mb, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
		{ PrimaryMouseMove(x, y, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow()
		{ MouseOutOfWindow(); }
	virtual void TJS_INTF_METHOD NotifyKeyDown(tjs_uint key, tjs_uint32 shift)
		{ PrimaryKeyDown(key, shift); }
	virtual void TJS_INTF_METHOD NotifyKeyUp(tjs_uint key, tjs_uint32 shift)
		{ PrimaryKeyUp(key, shift); }
	virtual void TJS_INTF_METHOD NotifyKeyPress(tjs_char key)
		{ PrimaryKeyPress(key); }
	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)
		{ PrimaryMouseWheel(shift, delta, x, y); }

	tTJSNI_BaseLayer * GetMostFrontChildAt(tjs_int x, tjs_int y,
		tTJSNI_BaseLayer *except = NULL, bool get_disabled = false);

	void PrimaryClick(tjs_int x, tjs_int y);
	void PrimaryDoubleClick(tjs_int x, tjs_int y);

	void PrimaryMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	void PrimaryMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);

	void PrimaryMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	void ForceMouseLeave();
	void ForceMouseRecheck();
	void MouseOutOfWindow();
	void LeaveMouseFromTree(tTJSNI_BaseLayer *root); // force to leave mouse

	virtual void TJS_INTF_METHOD ReleaseCapture();
	void ReleaseCaptureFromTree(tTJSNI_BaseLayer * layer);

	bool BlurTree(tTJSNI_BaseLayer *root); // remove focus from "root"
	tTJSNI_BaseLayer *SearchFirstFocusable(bool ignore_chain_focusable = true); // search first focusable layer


	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const { return FocusedLayer; }
	void CheckTreeFocusableState(tTJSNI_BaseLayer *root);
		// check newly added tree's focusable state
	bool SetFocusTo(tTJSNI_BaseLayer *layer, bool direction = true);
		// set focus to layer
	void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) { SetFocusTo(layer, false); }
	tTJSNI_BaseLayer *FocusPrev(); // focus to previous layer
	tTJSNI_BaseLayer *FocusNext(); // focus to next layer
	void ReleaseAllModalLayer(); // release all modal layer on invalidation
	void SetModeTo(tTJSNI_BaseLayer *layer); // set mode to layer
	void RemoveModeFrom(tTJSNI_BaseLayer *layer); // remove mode from layer
	void RemoveTreeModalState(tTJSNI_BaseLayer *root);
		// remove modal state from given tree
	tTJSNI_BaseLayer *GetCurrentModalLayer() const;

private:
	bool SearchAttentionPoint(tTJSNI_BaseLayer *target, tjs_int &x, tjs_int &y);
	void SetAttentionPointOf(tTJSNI_BaseLayer *layer);
	void DisableAttentionPoint();

public:
	void NotifyAttentionStateChanged(tTJSNI_BaseLayer *from);

private:
	void SetImeModeOf(tTJSNI_BaseLayer *layer);
	void ResetImeMode();

public:
	void NotifyImeModeChanged(tTJSNI_BaseLayer *from);

private:
	tjs_int EnabledWorkRefCount;
public:
	void SaveEnabledWork();
	void NotifyNodeEnabledState();
	void PrimaryKeyDown(tjs_uint key, tjs_uint32 shift);
	void PrimaryKeyUp(tjs_uint key, tjs_uint32 shift);
	void PrimaryKeyPress(tjs_char key);
	void PrimaryMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);

	void AddUpdateRegion(const tTVPComplexRect &rects);
	void AddUpdateRegion(const tTVPRect & rect);
	void PrimaryUpdateByWindow(const tTVPRect &rect);
	virtual void TJS_INTF_METHOD UpdateToDrawDevice();
	void NotifyUpdateRegionFixed();

public:
	void TJS_INTF_METHOD RecheckInputState();
		// To re-check current layer under current mouse position
		// and update hint, cursor type and process layer enter/leave.
		// This can be reasonably slow, about 1 sec interval.

public:
	void TJS_INTF_METHOD DumpLayerStructure();
};
//---------------------------------------------------------------------------



#endif
