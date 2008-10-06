#ifndef IRRLICHTSCENEMANAGER_H
#define IRRLICHTSCENEMANAGER_H

#include <irrlicht.h>

/**
 * シーンマネージャ操作用クラス
 */
class IrrlichtSceneManager {

protected:
	irr::scene::ISceneManager *smgr;
	
public:
	IrrlichtSceneManager();
	IrrlichtSceneManager(irr::scene::ISceneManager *smgr);
	IrrlichtSceneManager(const IrrlichtSceneManager &orig);
	~IrrlichtSceneManager();

	// ------------------------------------------------------------
	// シーン制御
	// ------------------------------------------------------------
public:	
	/**
	 * シーンデータファイルの読み込み
	 * @param filename シーンデータファイル(irrファイル)
	 * @return 読み込みに成功したら true
	 */
	bool loadScene(const char *filename);

	/**
	 * カメラの設定
	 */
	void addCameraSceneNode(irr::core::vector3df position, irr::core::vector3df lookat, int id);
};

#endif
