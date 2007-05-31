// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorRotation.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorRotation::CSceneNodeAnimatorRotation(u32 time, const core::vector3df& rotation)
: Rotation(rotation), StartTime(time), DiffTime(0)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorRotation");
	#endif
}


//! destructor
CSceneNodeAnimatorRotation::~CSceneNodeAnimatorRotation()
{
}



//! animates a scene node
void CSceneNodeAnimatorRotation::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node) // thanks to warui for this fix
	{
		if ((timeMs-StartTime) != 0)
			DiffTime=timeMs-StartTime;
		core::vector3df NewRotation = node->getRotation();
		NewRotation += Rotation* ((f32)DiffTime/10.0f);
		node->setRotation(NewRotation);
		StartTime=timeMs;
	}
}


//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorRotation::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	out->addVector3d("Rotation", Rotation);
}

//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorRotation::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Rotation = in->getAttributeAsVector3d("Rotation");
}


} // end namespace scene
} // end namespace irr

