#include <irrlicht.h>
#include <iostream>

// include the gui creator element factory
#include "CGUIEditFactory.h"

using namespace irr;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")

int main()
{
	IrrlichtDevice *device =createDevice(video::EDT_OPENGL, core::dimension2d<s32>(800, 600));
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment *env = device->getGUIEnvironment();

	/*
		first we create the factory which can make new GUI elements
		and register it with the gui environment.
	*/

	IGUIElementFactory* factory = new CGUIEditFactory(env);
	env->registerGUIElementFactory(factory);
	// remember to drop since we created with a create call
	factory->drop();

	IGUISkin *skin = env->createSkin(EGST_WINDOWS_METALLIC);
	env->setSkin(skin);

	device->getFileSystem()->addFolderFileArchive ( "../../media/" );
	IGUIFont *font = env->getFont("lucida.xml");
	if (font)
		skin->setFont(font);
	skin->drop();

	// change transparency of skin
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = env->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(250);
		env->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}

	/*
		now we add the GUI Editor Workspace
	*/

	env->addGUIElement("GUIEditor");
	
	while(device->run()) 
	{
		device->sleep(10);

		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(0,200,200,200));
			smgr->drawAll();
			env->drawAll();
			driver->endScene();
		}
	}

	device->drop();
	
	return 0;
}


