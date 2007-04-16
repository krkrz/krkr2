#include <windows.h>

#include "IrrlichtDrawDevice.h"

/**
 * コンストラクタ
 */
tTVPIrrlichtDrawDevice::tTVPIrrlichtDrawDevice()
{
	device = NULL;
	driver = NULL;
	smgr   = NULL;
	texture  = NULL;
	destBuffer = NULL;
}

/**
 * デストラクタ
 */
tTVPIrrlichtDrawDevice::~tTVPIrrlichtDrawDevice()
{
	detach();
}

IrrlichtDevice *
tTVPIrrlichtDrawDevice::create(HWND hwnd, video::E_DRIVER_TYPE type)
{
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<s32>(hwnd);
	params.DriverType    = type;
	params.WindowSize    = core::dimension2d<s32>(width, height);
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	return createDeviceEx(params);
}

/**
 * ウインドウの再設定
 */
void
tTVPIrrlichtDrawDevice::attach(HWND hwnd)
{
	// ウインドウサイズの取得
	RECT rect;
	GetClientRect(hwnd, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;

	device = create(hwnd, video::EDT_DIRECT3D9);
	if (device == NULL) {
		device = create(hwnd, video::EDT_DIRECT3D8);
		if (device == NULL) {
			device = create(hwnd, video::EDT_OPENGL);
		}
	}
	if (device == NULL) {
		TVPThrowExceptionMessage(L"Irrlicht デバイスの初期化に失敗しました");
	}

	//device->setWindowCaption(title.c_str());
	driver  = device->getVideoDriver();
	smgr    = device->getSceneManager();
	texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
	
	init();
	
	// 駆動開始
	start();
}

/**
 * ウインドウの解除
 */
void
tTVPIrrlichtDrawDevice::detach()
{
	stop();
	if (device) {
		device->drop();
		device = NULL;
		driver = NULL;
		smgr   = NULL;
		texture = NULL;
	}
}

/***
 * ウインドウの指定
 */

void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::SetTargetWindow(HWND wnd)
{
	detach();
	if (wnd != NULL) {
		attach(wnd);
	}
}

/**
 * ビットマップコピー処理開始
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::StartBitmapCompletion(iTVPLayerManager * manager)
{
	// bitmap処理開始
	if (device && texture) {
		destBuffer = (unsigned char *)texture->lock();
		core::dimension2d<s32> size = texture->getSize();
		destWidth  = size.Width;
		destHeight = size.Height;
		destPitch  = texture->getPitch();
	}
}

/**
 * ビットマップコピー処理
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo で表されるビットマップの cliprect の領域を、x, y に描画する。
	if (device && texture && destBuffer) {
		int srcPitch = -bitmapinfo->bmiHeader.biWidth * 4; // XXX きめうち
		unsigned char *srcBuffer = (unsigned char *)bits - srcPitch * (bitmapinfo->bmiHeader.biHeight - 1);
		int srcx   = cliprect.left;
		int srcy   = cliprect.top;
		int width  = cliprect.get_width();
		int height = cliprect.get_height();
		// クリッピング
		if (x < 0) {
			srcx  += x;
			width += x;
			x = 0;
		}
		if (x + width > destWidth) {
			width -= ((x + width) - destWidth);
		}
		if (y < 0) {
			srcy += y;
			height += y;
			y = 0;
		}
		if (y + height > destHeight) {
			height -= ((y + height) - destHeight);
		}
		unsigned char *src  = srcBuffer  + srcy * srcPitch  + srcx * 4;
		unsigned char *dest = destBuffer +    y * destPitch +    x * 4;
		for (int i=0;i<height;i++) {
			memcpy(dest, src, width * 4);
			src  += srcPitch;
			dest += destPitch;
		}
	}
}

/**
 * ビットマップコピー処理終了
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	// bitmap 処理終了
	if (device && texture) {
		texture->unlock();
		destBuffer = NULL;
	}
}

//---------------------------------------------------------------------------

/**
 * テスト用初期化処理
 */
void
tTVPIrrlichtDrawDevice::init()
{
#if 0
	// 以下サンプルデータのロード処理
	ttstr dataPath = "sydney.md2";
	dataPath = TVPNormalizeStorageName(dataPath);
	TVPGetLocalName(dataPath);
	int len = dataPath.GetNarrowStrLen() + 1;
	char *str = new char[len];
	dataPath.ToNarrowStr(str, len);
	IAnimatedMesh *mesh = smgr->getMesh(str);
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node) {
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation ( scene::EMAT_STAND );
	}
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
#else
	ttstr dataPath = "map-20kdm2.pk3";
	dataPath = TVPNormalizeStorageName(dataPath);
	TVPGetLocalName(dataPath);
	int len = dataPath.GetNarrowStrLen() + 1;
	char *str = new char[len];
	dataPath.ToNarrowStr(str, len);
	device->getFileSystem()->addZipFileArchive(str);
	
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;
		
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
	
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));
	
	smgr->addCameraSceneNodeFPS();
	device->getCursorControl()->setVisible(false);
#endif
}

//---------------------------------------------------------------------------

/**
 * Ogre 呼び出し処理開始
 */
void
tTVPIrrlichtDrawDevice::start()
{
	stop();
	TVPAddContinuousEventHook(this);
}

/**
 * Ogre 呼び出し処理停止
 */
void
tTVPIrrlichtDrawDevice::stop()
{
	TVPRemoveContinuousEventHook(this);
}

/**
 * Continuous コールバック
 * 吉里吉里が暇なときに常に呼ばれる
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::OnContinuousCallback(tjs_uint64 tick)
{
	if (device) {
		device->getTimer()->tick();
		driver->beginScene(true, true, video::SColor(255,100,101,140));
		smgr->drawAll();
		if (texture) {
			driver->draw2DImage(texture, core::position2d<s32>(0,0),
								core::rect<s32>(0,0,width,height), 0, 
								video::SColor(255,255,255,255), true);
		}
		driver->endScene();
	}
};

