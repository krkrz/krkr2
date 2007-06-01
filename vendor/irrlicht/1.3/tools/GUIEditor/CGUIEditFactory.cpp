#include "CGUIEditFactory.h"
#include "IGUIEnvironment.h"
#include "irrString.h"

#include "CGUIEditWorkspace.h"
#include "CGUIEditWindow.h"
#include "CGUIAttributeEditor.h"
#include "CGUIStringAttribute.h"
#include "CGUIBoolAttribute.h"
#include "CGUIEnumAttribute.h"
#include "CGUIColorAttribute.h"

namespace irr
{
namespace gui
{

enum EGUIEDIT_ELEMENT_TYPES
{
	EGUIEDIT_GUIEDIT=0,
	EGUIEDIT_GUIEDITWINDOW,
	EGUIEDIT_ATTRIBUTEEDITOR,
	EGUIEDIT_STRINGATTRIBUTE,
	EGUIEDIT_BOOLATTRIBUTE,
	EGUIEDIT_ENUMATTRIBUTE,
	EGUIEDIT_COLORATTRIBUTE,
	EGUIEDIT_COLORFATTRIBUTE,
	EGUIEDIT_COUNT
};

const c8* const GUIEditElementTypeNames[] =
{
	"GUIEditor",
	"GUIEditWindow",
	"attributeEditor",
	"string_attribute",
	"bool_attribute",
	"enum_attribute",
	"color_attribute",
	"colorf_attribute",
	0
};



CGUIEditFactory::CGUIEditFactory(IGUIEnvironment* env)
: Environment(env)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditFactory");
	#endif

	// don't grab the gui environment here to prevent cyclic references
}


CGUIEditFactory::~CGUIEditFactory()
{
}


//! adds an element to the environment based on its type name
IGUIElement* CGUIEditFactory::addGUIElement(const c8* typeName, IGUIElement* parent)
{
	/*
		here we create elements, add them to the manager, and then drop them
	*/

	core::stringc elementType(typeName);
	IGUIElement* ret=0;
	if (parent == 0)
	{
		parent = Environment->getRootGUIElement();
	}

	// editor workspace
	if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDIT]))
		ret = new CGUIEditWorkspace(Environment, -1, parent);
	// editor window
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDITWINDOW]))
		ret = new CGUIEditWindow(Environment, core::rect<s32>(0,0,100,100), parent);
	// block of attribute editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ATTRIBUTEEDITOR]))
		ret = new CGUIAttributeEditor(Environment, -1, parent);
	//! single attribute editors
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_STRINGATTRIBUTE]))
		ret = new CGUIStringAttribute(Environment, parent);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_BOOLATTRIBUTE]))
		ret = new CGUIBoolAttribute(Environment, parent);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ENUMATTRIBUTE]))
		ret = new CGUIEnumAttribute(Environment, parent);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORATTRIBUTE]))
		ret = new CGUIColorAttribute(Environment, parent);
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_COLORFATTRIBUTE]))
		ret = new CGUIColorAttribute(Environment, parent);


	// the environment now has the reference, so we can drop the element
	if (ret)
		ret->drop();

	return ret;
}


//! returns amount of element types this factory is able to create
s32 CGUIEditFactory::getCreatableGUIElementTypeCount()
{
	return EGUIEDIT_COUNT;
}


//! returns type name of a createable element type 
const c8* CGUIEditFactory::getCreateableGUIElementTypeName(s32 idx)
{
	if (idx>=0 && idx<EGUIEDIT_COUNT)
		return GUIEditElementTypeNames[idx];

	return 0;
}



} // end namespace gui
} // end namespace irr

