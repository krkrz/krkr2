// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CDefaultSceneNodeFactory.h"
#include "ISceneManager.h"
#include "ITextSceneNode.h"
#include "ITerrainSceneNode.h"
#include "IDummyTransformationSceneNode.h"
#include "ICameraSceneNode.h"
#include "IBillboardSceneNode.h"
#include "IAnimatedMeshSceneNode.h"
#include "IParticleSystemSceneNode.h"
#include "ILightSceneNode.h"
#include "IMeshSceneNode.h"

namespace irr
{
namespace scene
{

//! Names for scene node types
const c8* const SceneNodeTypeNames[] =
{
	"cube",
	"sphere",
	"text",
	"waterSurface",
	"terrain",
	"skyBox",
	"shadowVolume",
	"octTree",
	"mesh",
	"light",
	"empty",
	"dummyTransformation",
	"camera",
	"cameraMaya",
	"cameraFPS",
	"billBoard",
	"animatedMesh",
	"particleSystem",
	0
};



CDefaultSceneNodeFactory::CDefaultSceneNodeFactory(ISceneManager* mgr)
: Manager(mgr)
{
	// don't grab the scene manager here to prevent cyclic references
}


CDefaultSceneNodeFactory::~CDefaultSceneNodeFactory()
{
}


//! adds a scene node to the scene graph based on its type id
ISceneNode* CDefaultSceneNodeFactory::addSceneNode(ESCENE_NODE_TYPE type, ISceneNode* parent)
{
	switch(type)
	{
	case ESNT_CUBE:
		return Manager->addCubeSceneNode(10, parent);
	case ESNT_SPHERE:
		return Manager->addSphereSceneNode(5, 16, parent);
	case ESNT_TEXT:
		return Manager->addTextSceneNode(0, L"example");
	case ESNT_WATER_SURFACE:
		return Manager->addWaterSurfaceSceneNode(0, 2.0f, 300.0f, 10.0f, parent);
	case ESNT_TERRAIN:
		return Manager->addTerrainSceneNode((const char*)0, parent);
	case ESNT_SKY_BOX:
		return Manager->addSkyBoxSceneNode(0,0,0,0,0,0, parent);
	case ESNT_SHADOW_VOLUME:
		return 0; 
	case ESNT_OCT_TREE:
		return Manager->addOctTreeSceneNode((IMesh*)0, parent, -1, 128, true);
	case ESNT_MESH:
		return Manager->addMeshSceneNode(0, parent, -1, core::vector3df(), 
										 core::vector3df(), core::vector3df(1,1,1), true);
	case ESNT_LIGHT:
		return Manager->addLightSceneNode(parent);
	case ESNT_EMPTY:
		return Manager->addEmptySceneNode(parent);
	case ESNT_DUMMY_TRANSFORMATION:
		return Manager->addDummyTransformationSceneNode(parent);
	case ESNT_CAMERA:
		return Manager->addCameraSceneNode(parent);
	case ESNT_CAMERA_MAYA:
		return Manager->addCameraSceneNodeMaya(parent);
	case ESNT_CAMERA_FPS:
		return Manager->addCameraSceneNodeFPS(parent);
	case ESNT_BILLBOARD:
		return Manager->addBillboardSceneNode(parent);
	case ESNT_ANIMATED_MESH:
		return Manager->addAnimatedMeshSceneNode(0, parent, -1, core::vector3df(),
												 core::vector3df(), core::vector3df(1,1,1), true);
	case ESNT_PARTICLE_SYSTEM:
		return Manager->addParticleSystemSceneNode(true, parent);
	default:
		break;
	}

	return 0;
}


//! adds a scene node to the scene graph based on its type name
ISceneNode* CDefaultSceneNodeFactory::addSceneNode(const c8* typeName, ISceneNode* parent)
{
	return addSceneNode( getTypeFromName(typeName), parent );
}


//! returns amount of scene node types this factory is able to create
s32 CDefaultSceneNodeFactory::getCreatableSceneNodeTypeCount()
{
	return ESNT_COUNT;
}


//! returns type of a createable scene node type
ESCENE_NODE_TYPE CDefaultSceneNodeFactory::getCreateableSceneNodeType(s32 idx)
{
	if (idx>=0 && idx<ESNT_COUNT)
		return (ESCENE_NODE_TYPE)idx;

	return ESNT_UNKNOWN;
}


//! returns type name of a createable scene node type 
const c8* CDefaultSceneNodeFactory::getCreateableSceneNodeTypeName(s32 idx)
{
	if (idx>=0 && idx<ESNT_COUNT)
		return SceneNodeTypeNames[idx];

	return 0;
}


//! returns type name of a createable scene node type 
const c8* CDefaultSceneNodeFactory::getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type)
{
	// for this factory, type == index

	if (type>=0 && type<ESNT_COUNT)
		return SceneNodeTypeNames[type];

	return 0;
}

ESCENE_NODE_TYPE CDefaultSceneNodeFactory::getTypeFromName(const c8* name)
{
	for ( int i=0; SceneNodeTypeNames[i]; ++i)
		if (!strcmp(name, SceneNodeTypeNames[i]) )
			return (ESCENE_NODE_TYPE)i;

	return ESNT_UNKNOWN;
}


} // end namespace scene
} // end namespace irr

