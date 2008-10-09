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
	 * 環境光の設定
	 */
	void setAmbientLight(irr::video::SColorf ambientColor);

	/**
	 * 環境光の取得
	 */
	irr::video::SColorf getAmbientLight();
	
	/**
	 * 光源の設定
	 * @param position 位置指定
	 * @param color 色指定
	 * @param radius
	 * @param id ID指定
	 */
	void addLightSceneNode(irr::core::vector3df position, irr::video::SColorf color, float radius, int id);

	/**
	 * カメラの設定
	 * @param position 位置指定
	 * @param lookat 参照位置
	 * @param id ID指定
	 */
	void addCameraSceneNode(irr::core::vector3df position, irr::core::vector3df lookat, int id);
};

#endif
