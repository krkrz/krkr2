//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" �`��f�o�C�X�Ǘ�
//---------------------------------------------------------------------------
#ifndef PASSTHROUGHDRAWDEVICE_H
#define PASSTHROUGHDRAWDEVICE_H

#include "DrawDevice.h"

class tTVPDrawer;
//---------------------------------------------------------------------------
//! @brief		�uPass Through�v�f�o�C�X(�����Ƃ���{�I�ȕ`����s���݂̂̃f�o�C�X)
//---------------------------------------------------------------------------
class tTVPPassThroughDrawDevice : public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;
	HWND TargetWindow;
	bool IsMainWindow;
	tTVPDrawer * Drawer; //!< �`����s������

public:
	//! @brief	drawer�̃^�C�v
	enum tDrawerType
	{
		dtNone, //!< drawer �Ȃ�
		dtDrawDib, //!< �����Ƃ��P����drawer
		dtDBGDI, // GDI �ɂ��_�u���o�b�t�@�����O���s��drawer
		dtDBDD, // DirectDraw �ɂ��_�u���o�b�t�@�����O���s��drawer
		dtDBD3D // Direct3D �ɂ��_�u���o�b�t�@�����O���s��drawer
	};

private:
	tDrawerType DrawerType; //!< drawer �̃^�C�v
	tDrawerType PreferredDrawerType; //!< �g���ė~���� drawer �̃^�C�v

	bool DestSizeChanged; //!< DestRect �̃T�C�Y�ɕύX����������
	bool SrcSizeChanged; //!< SrcSize �ɕύX����������

public:
	tTVPPassThroughDrawDevice(); //!< �R���X�g���N�^
private:
	~tTVPPassThroughDrawDevice(); //!< �f�X�g���N�^

public:
	void SetToRecreateDrawer() { DestroyDrawer(); }
	void DestroyDrawer();
private:
	void CreateDrawer(tDrawerType type);
	void CreateDrawer(bool zoom_required, bool should_benchmark);

public:
	void EnsureDrawer();

	tDrawerType GetDrawerType() const { return DrawerType; }
	void SetPreferredDrawerType(tDrawerType type) { PreferredDrawerType = type; }
	tDrawerType GetPreferredDrawerType() const { return PreferredDrawerType; }

//---- LayerManager �̊Ǘ��֘A
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);

//---- �`��ʒu�E�T�C�Y�֘A
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);

//---- �ĕ`��֘A
	virtual void TJS_INTF_METHOD Show();

//---- LayerManager ����̉摜�󂯓n���֘A
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

//---- �f�o�b�O�x��
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTJSNI_PassThroughDrawDevice
//---------------------------------------------------------------------------
class tTJSNI_PassThroughDrawDevice :
	public tTJSNativeInstance
{
	typedef tTJSNativeInstance inherited;

	tTVPPassThroughDrawDevice * Device;

public:
	tTJSNI_PassThroughDrawDevice();
	~tTJSNI_PassThroughDrawDevice();
	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

public:
	tTVPPassThroughDrawDevice * GetDevice() const { return Device; }

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_PassThroughDrawDevice
//---------------------------------------------------------------------------
class tTJSNC_PassThroughDrawDevice : public tTJSNativeClass
{
public:
	tTJSNC_PassThroughDrawDevice();

	static tjs_uint32 ClassID;

private:
	iTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


#endif
