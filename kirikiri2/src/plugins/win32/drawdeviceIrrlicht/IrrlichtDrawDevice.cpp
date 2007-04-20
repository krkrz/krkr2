#include <windows.h>

#include "IrrlichtDrawDevice.h"

/**
 * コンストラクタ
 */
LayerManagerInfo::LayerManagerInfo(ITexture *texture) : texture(texture)
{
	destBuffer = NULL;
};

/**
 * デストラクタ
 */
LayerManagerInfo::~LayerManagerInfo()
{
}


/**
 * テクスチャをロックして描画領域情報を取得する
 */
void
LayerManagerInfo::lock()
{
	if (texture) {
		destBuffer = (unsigned char *)texture->lock();
		core::dimension2d<s32> size = texture->getSize();
		destWidth  = size.Width;
		destHeight = size.Height;
		destPitch  = texture->getPitch();
	} else {
		destBuffer = NULL;
	}
}

/**
 * ロックされたテクスチャにビットマップ描画を行う
 */
void
LayerManagerInfo::copy(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
					   const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo で表されるビットマップの cliprect の領域を、x, y に描画する。

	if (destBuffer) {
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
 * テクスチャのロックの解除
 */
void
LayerManagerInfo::unlock()
{
	if (texture) {
		texture->unlock();
		destBuffer = NULL;
	}
}

/**
 * コンストラクタ
 */
tTVPIrrlichtDrawDevice::tTVPIrrlichtDrawDevice()
{
	device = NULL;
}

/**
 * デストラクタ
 */
tTVPIrrlichtDrawDevice::~tTVPIrrlichtDrawDevice()
{
	detach();
}

/**
 * テクスチャの割り当て
 * @param manager レイヤマネージャ
 */
void 
tTVPIrrlichtDrawDevice::allocInfo(iTVPLayerManager * manager)
{
	if (device) {
		video::IVideoDriver* driver = device->getVideoDriver();

		// テクスチャ割り当てXXX サイズ判定が必要…
		ITexture *texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);
		if (texture == NULL) {
			TVPThrowExceptionMessage(L"テクスチャの割り当てに失敗しました");
		}

		manager->SetDrawDeviceData((void*)new LayerManagerInfo(texture));
	}
}

/**
 *  テクスチャの解放
 * @param manager レイヤマネージャ
 */
void
tTVPIrrlichtDrawDevice::freeInfo(iTVPLayerManager * manager)
{
	if (device) {
		LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
		if (info != NULL) {
			video::IVideoDriver* driver = device->getVideoDriver();
			driver->removeTexture(info->texture);
			manager->SetDrawDeviceData(NULL);
			delete info;
		}
	}
}

/**
 * デバイスの生成
 * @param hwnd ハンドル
 * @param ドライバの種類
 */
IrrlichtDevice *
tTVPIrrlichtDrawDevice::create(HWND hwnd, video::E_DRIVER_TYPE type)
{
	SIrrlichtCreationParameters params;
	params.WindowId     = reinterpret_cast<s32>(hwnd);
	params.DriverType    = type;
//	params.WindowSize    = core::dimension2d<s32>(width, height);
	params.Bits          = 32;
	params.Stencilbuffer = true;
	params.Vsync = true;
	params.EventReceiver = this;
	return createDeviceEx(params);
}

/**
 * ウインドウの再設定
 * @param hwnd ハンドル
 */
void
tTVPIrrlichtDrawDevice::attach(HWND hwnd)
{
	// ウインドウサイズの取得 XXX
	RECT rect;
	GetClientRect(hwnd, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// デバイス生成
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
	
	// マネージャに対するテクスチャの割り当て
	for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
		allocInfo(*i);
	}

	// XXX テスト用
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
	if (device) {
		stop();
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			freeInfo(*i);
		}
		device->drop();
		device = NULL;
	}
}

/**
 * レイヤマネージャの登録
 * @param manager レイヤマネージャ
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::AddLayerManager(iTVPLayerManager * manager)
{
	allocInfo(manager);
	tTVPDrawDevice::AddLayerManager(manager);
}

/**
 * レイヤマネージャの削除
 * @param manager レイヤマネージャ
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::RemoveLayerManager(iTVPLayerManager * manager)
{
	freeInfo(manager);
	tTVPDrawDevice::RemoveLayerManager(manager);
}


/***
 * ウインドウの指定
 * @param wnd ウインドウハンドラ
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
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->lock();
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
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->copy(x, y, bits, bitmapinfo, cliprect, type, opacity);
	}
}

/**
 * ビットマップコピー処理終了
 */
void TJS_INTF_METHOD
tTVPIrrlichtDrawDevice::EndBitmapCompletion(iTVPLayerManager * manager)
{
	LayerManagerInfo *info = (LayerManagerInfo*)manager->GetDrawDeviceData();
	if (info) {
		info->unlock();
	}
}

//---------------------------------------------------------------------------

/**
 * テスト用初期化処理
 */
void
tTVPIrrlichtDrawDevice::init()
{
	/// シーンマネージャ
	ISceneManager* smgr = device->getSceneManager();

#if 1
	// 以下サンプルデータのロード処理
	ttstr dataPath = "../../../media/sydney.md2";
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
	ttstr dataPath = "../../../media/map-20kdm2.pk3";
	dataPath = TVPNormalizeStorageName(dataPath);
	TVPGetLocalName(dataPath);
	int len = dataPath.GetNarrowStrLen() + 1;
	char *str = new char[len];
	dataPath.ToNarrowStr(str, len);
	device->getFileSystem()->addZipFileArchive(str);
	
	scene::IAnimatedMesh* mesh = smgr->getMesh("../../../media/20kdm2.bsp");
	scene::ISceneNode* node = 0;
		
	if (mesh)
		node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
	
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));
	
	smgr->addCameraSceneNodeFPS();
//	device->getCursorControl()->setVisible(false);
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

		/// ドライバ
		video::IVideoDriver* driver = device->getVideoDriver();

		// 描画開始
		driver->beginScene(true, true, video::SColor(255,0,0,0));

		/// シーンマネージャ
		ISceneManager* smgr = device->getSceneManager();
		smgr->drawAll();

		// 個別レイヤマネージャの描画
		for (std::vector<iTVPLayerManager *>::iterator i = Managers.begin(); i != Managers.end(); i++) {
			LayerManagerInfo *info = (LayerManagerInfo*)(*i)->GetDrawDeviceData();
			if (info && info->texture) {
				driver->draw2DImage(info->texture, core::position2d<s32>(0,0),
									core::rect<s32>(0,0,width,height), 0, 
									video::SColor(255,255,255,255), true);
			}
		}
		// 描画完了
		driver->endScene();
	}
};

/**
 * イベント受理
 * @param event イベント情報
 * @return 処理したら true
 */
bool
tTVPIrrlichtDrawDevice::OnEvent(SEvent event)
{
	switch (event.EventType) {
	case EET_KEY_INPUT_EVENT:
		int shift = 0;
		if (event.KeyInput.Shift) {
			shift |= 0x01;
		}
		if (event.KeyInput.Control) {
			shift |= 0x04;
		}
		if (event.KeyInput.PressedDown){
			OnKeyDown(event.KeyInput.Key, shift); 
	    } else {
			OnKeyUp(event.KeyInput.Key, shift);
        }
        return true;
	}
	return false;
}
