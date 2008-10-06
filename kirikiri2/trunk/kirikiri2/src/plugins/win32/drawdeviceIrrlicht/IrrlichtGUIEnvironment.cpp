#include "IrrlichtGUIEnvironment.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;
 
IrrlichtGUIEnvironment::IrrlichtGUIEnvironment() : gui(NULL)
{
}

IrrlichtGUIEnvironment::IrrlichtGUIEnvironment(irr::gui::IGUIEnvironment *gui) : gui(gui)
{
	if (gui) {
		gui->grab();
	}
}

IrrlichtGUIEnvironment::IrrlichtGUIEnvironment(const IrrlichtGUIEnvironment &orig) : gui(orig.gui)
{
	if (gui) {
		gui->grab();
	}
}

IrrlichtGUIEnvironment::~IrrlichtGUIEnvironment()
{
	if (gui) {
		gui->drop();
		gui = NULL;
	}
}
