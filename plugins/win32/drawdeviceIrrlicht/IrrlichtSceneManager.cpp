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

void
IrrlichtSceneManager::addCameraSceneNode(vector3df position, vector3df lookat, int id)
{
	if (smgr) {
		smgr->addCameraSceneNode(NULL, position, lookat, id);
	}
}

