#include "IrrlichtSceneManager.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

IrrlichtSceneManager::IrrlichtSceneManager() : smgr(NULL)
{
}

IrrlichtSceneManager::IrrlichtSceneManager(irr::scene::ISceneManager *smgr) : smgr(smgr)
{
	if (smgr) {
		smgr->grab();
	}
}

IrrlichtSceneManager::IrrlichtSceneManager(const IrrlichtSceneManager &orig) : smgr(orig.smgr)
{
	if (smgr) {
		smgr->grab();
	}
}

IrrlichtSceneManager::~IrrlichtSceneManager()
{
	if (smgr) {
		smgr->drop();
		smgr = NULL;
	}
}

// ------------------------------------------------------------
// シーン制御
// ------------------------------------------------------------

/**
 * シーンデータファイルの読み込み
 * @param filename シーンデータファイル(irrファイル)
 * @return 読み込みに成功したら true
 */
bool
IrrlichtSceneManager::loadScene(const char *filename)
{
	return smgr ? smgr->loadScene(filename) : false;
}

/**
 * 環境光の設定
 */
void 
IrrlichtSceneManager::setAmbientLight(irr::video::SColorf ambientColor)
{
	if (smgr) {
		return smgr->setAmbientLight(ambientColor);
	}
}

/**
 * 環境光の取得
 */
irr::video::SColorf
IrrlichtSceneManager::getAmbientLight()
{
	return smgr ? smgr->getAmbientLight() : SColorf();
}

/**
 * 光源の設定
 */
void
IrrlichtSceneManager::addLightSceneNode(irr::core::vector3df position, irr::video::SColorf color, float radius, int id)
{
	if (smgr) {
		smgr->addLightSceneNode(NULL, position, color, radius, id);
	}
}

/**
 * カメラの設定
 */
void
IrrlichtSceneManager::addCameraSceneNode(vector3df position, vector3df lookat, int id)
{
	if (smgr) {
		smgr->addCameraSceneNode(NULL, position, lookat, id);
	}
}

