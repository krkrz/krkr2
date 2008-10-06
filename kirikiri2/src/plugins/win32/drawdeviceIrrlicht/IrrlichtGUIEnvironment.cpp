#include "IrrlichtGUIEnvironment.h"

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
