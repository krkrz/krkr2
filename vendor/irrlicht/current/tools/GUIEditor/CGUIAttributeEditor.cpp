
#include "CGUIAttributeEditor.h"
#include "IGUIEnvironment.h"

#include "IVideoDriver.h"
#include "IAttributes.h"
#include "IGUIFont.h"
#include "CGUIEditWorkspace.h"
#include "CGUIAttribute.h"
#include "CGUIStringAttribute.h"

using namespace irr;
using namespace core;
using namespace gui;
using namespace io;

CGUIAttributeEditor::CGUIAttributeEditor(IGUIEnvironment* environment, s32 id, IGUIElement *parent) :
	IGUIElement(EGUIET_ELEMENT, environment, parent, id, rect<s32>(0, 0, 100, 100)),
		Attribs(0), LastOffset(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIAttributeEditor");
	#endif

	// create attributes
	Attribs = environment->getFileSystem()->createEmptyAttributes(Environment->getVideoDriver());
	// add scrollbar
	ScrollBar = environment->addScrollBar(false, rect<s32>(84, 5, 99, 85), this);
	ScrollBar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	ScrollBar->grab();
	ScrollBar->setSubElement(true);

	refreshAttribs();
}

CGUIAttributeEditor::~CGUIAttributeEditor()
{
	u32 i;
	for (i=0; i<AttribList.size(); ++i)
	{
		AttribList[i]->remove();
		AttribList[i]->drop();
	}
	AttribList.clear();

	Attribs->drop();
	ScrollBar->drop();
}

bool CGUIAttributeEditor::OnEvent(SEvent e)
{

	switch (e.EventType)
	{

	case EET_GUI_EVENT:
		switch (e.GUIEvent.EventType)
		{
		case EGET_SCROLL_BAR_CHANGED:
			{
				// set the offset of every attribute
				s32 diff = LastOffset - ScrollBar->getPos();
				for (u32 i=0; i<AttribList.size(); ++i)
					AttribList[i]->setRelativePosition(AttribList[i]->getRelativePosition() + position2di(0, diff));

				LastOffset = ScrollBar->getPos();
				return true;
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch (e.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
			{
				ScrollBar->setPos(ScrollBar->getPos() - (s32)(e.MouseInput.Wheel)*50);
				
				s32 diff = LastOffset - ScrollBar->getPos();
				for (u32 i=0; i<AttribList.size(); ++i)
					AttribList[i]->setRelativePosition(AttribList[i]->getRelativePosition() + position2di(0, diff));

				LastOffset = ScrollBar->getPos();
				return true;
			}
		}
		break;
	}
	return Parent->OnEvent(e);
}

IAttributes* CGUIAttributeEditor::getAttribs()
{
	return Attribs;
}

void CGUIAttributeEditor::refreshAttribs()
{
	// clear the attribute list
	u32 i;
	for (i=0; i<AttribList.size(); ++i)
	{
		AttribList[i]->remove();
		AttribList[i]->drop();
	}
	AttribList.clear();
	position2di top(10, 5);
	rect<s32> r(top.X,
				top.Y,
				AbsoluteRect.getWidth() - Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH) * 2,
				5 + Environment->getSkin()->getFont()->getDimension(L"A").Height);

	// add attribute elements
	u32 c = Attribs->getAttributeCount();
	for (i=0; i<c; ++i)
	{

		// try to 
		stringc str = Attribs->getAttributeTypeString(i);
		str += "_attribute";
		CGUIAttribute* n = (CGUIAttribute*)Environment->addGUIElement(str.c_str(), this);

		if (n)
		{
			// add custom editor
			AttribList.push_back(n);
			n->grab();
		}
		else
		{
			// create a generic string editor
			AttribList.push_back(new CGUIStringAttribute(Environment, this));
			// dont grab it because we created it with new
		}

		AttribList[i]->setSubElement(true);
		AttribList[i]->setRelativePosition(r);
		AttribList[i]->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		AttribList[i]->setAttrib(Attribs, i);
		r += position2di(0, AttribList[i]->getRelativePosition().getHeight() + 5);
		
	}

	if (r.UpperLeftCorner.Y > RelativeRect.getHeight())
	{
		ScrollBar->setVisible(true);
		ScrollBar->setMax(r.UpperLeftCorner.Y - RelativeRect.getHeight());
		LastOffset = ScrollBar->getPos();
	}
	else
	{
		ScrollBar->setVisible(false);
		ScrollBar->setPos(0);
		LastOffset = 0;
	}

}
void CGUIAttributeEditor::updateAttribs()
{
	for (u32 i=0; i<AttribList.size(); ++i)
		AttribList[i]->updateAttrib(false);
}

void CGUIAttributeEditor::updateAbsolutePosition()
{
	// get real position from desired position
	IGUIElement::updateAbsolutePosition();

	s32 p=0;
	// get lowest position
	if (AttribList.size())
		p = AttribList[AttribList.size() - 1]->getRelativePosition().LowerRightCorner.Y + ScrollBar->getPos();

	p -= RelativeRect.getHeight();

	if (p > 1)
	{
		ScrollBar->setMax(p);
		ScrollBar->setVisible(true);
	}
	else
	{
		ScrollBar->setMax(0);
		ScrollBar->setVisible(false);
	}
}

