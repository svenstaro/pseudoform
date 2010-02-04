/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include <toolbox_stdafx.h>
#include "SkyBox.h"
#include "RenderPrimitive.h"
#include "../OGLMesh.h"
#include "../SceneManager.h"
#include "../PhysicsUtils.h"
#include "../toolBox/MousePick.h"
#include "../toolBox/OpenGlUtil.h"
#include "../toolBox/DebugDisplay.h"
#include "../toolBox/LevelPrimitive.h"
#include "../toolBox/PlaneCollision.h"
#include "../toolBox/ScenePrimitive.h"
#include "../toolBox/HeightFieldPrimitive.h"
#include "../toolBox/UserHeightFieldCollision.h"


static void SetDemoCallbacks (SceneManager& system)
{
	system.m_control = Keyboard;
	system.m_autoSleep = AutoSleep;
	system.m_showIslands = SetShowIslands;
	system.m_showContacts = SetShowContacts; 
	system.m_setMeshCollision = SetShowMeshCollision;
}

static LevelPrimitive* LoadLevelAndSceneRoot (SceneManager& system, const char* levelName, int optimized)
{
	NewtonWorld* world;
	NewtonBody* floorBody;
	LevelPrimitive* level;

	world = system.m_world;
	// /////////////////////////////////////////////////////////////////////
	//
	// create the sky box,
	system.AddModel (new SkyBox ());

	// Load a level geometry
	level = new LevelPrimitive (levelName, world, optimized);
	system.AddModel(level);
	floorBody = level->m_level;

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (floorBody, PhysicsBodyDestructor);

	// get the default material ID
	int defaultID;
	defaultID = NewtonMaterialGetDefaultGroupID (world);

	// set default material properties
	NewtonMaterialSetDefaultSoftness (world, defaultID, defaultID, 0.05f);
	NewtonMaterialSetDefaultElasticity (world, defaultID, defaultID, 0.4f);
	NewtonMaterialSetDefaultCollidable (world, defaultID, defaultID, 1);
	NewtonMaterialSetDefaultFriction (world, defaultID, defaultID, 1.0f, 0.5f);
	NewtonMaterialSetCollisionCallback (world, defaultID, defaultID, NULL, NULL, GenericContactProcess); 

//	NewtonMaterialSetSurfaceThickness(world, materialID, materialID, 0.1f);
//	NewtonMaterialSetSurfaceThickness(world, defaultID, defaultID, 0.0f);

	// set the island update callback
	NewtonSetIslandUpdateEvent (world, PhysicsIslandUpdate);

	// save th3 callback
	SetDemoCallbacks (system);

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), dVector (-40.0f, 10.0f, 0.0f));

	return level;
}


static ScenePrimitive* LoadSceneAndSceneRoot (SceneManager& system, const char* levelName, int optimized)
{
	// create scene collsion mesh;
	NewtonWorld* world;
	NewtonBody* floorBody;
	ScenePrimitive* scene;

	world = system.m_world;
	// /////////////////////////////////////////////////////////////////////
	//
	// create the sky box,
	system.AddModel (new SkyBox ());

	// Load a level geometry
	scene = new ScenePrimitive (world, levelName, optimized);
	system.AddModel(scene);
	floorBody = scene->m_level;

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (floorBody, PhysicsBodyDestructor);

	// get the default material ID
	int defaultID;
	defaultID = NewtonMaterialGetDefaultGroupID (world);

	// set default material properties
	NewtonMaterialSetDefaultSoftness (world, defaultID, defaultID, 0.05f);
	NewtonMaterialSetDefaultElasticity (world, defaultID, defaultID, 0.4f);
	NewtonMaterialSetDefaultCollidable (world, defaultID, defaultID, 1);
	NewtonMaterialSetDefaultFriction (world, defaultID, defaultID, 1.0f, 0.5f);
	NewtonMaterialSetCollisionCallback (world, defaultID, defaultID, NULL, NULL, GenericContactProcess); 

//	NewtonMaterialSetSurfaceThickness(world, materialID, materialID, 0.1f);
//	NewtonMaterialSetSurfaceThickness(world, defaultID, defaultID, 0.0f);

	// set the island update callback
	NewtonSetIslandUpdateEvent (world, PhysicsIslandUpdate);

	// save th3 callback
	SetDemoCallbacks (system);

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), dVector (-40.0f, 10.0f, 0.0f));

	return scene;
}





void SimplePlaneCollision (SceneManager& system)
{
	NewtonWorld* world;
	LevelPrimitive *level;
	NewtonCollision* planeColl;

	world = system.m_world;

	// create a material carrier to collision with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	// create the sky box and the floor,
	level = LoadLevelAndSceneRoot (system, "flatplane.dae", 1);

	// Find the world mesh and replace the collision for a custom plane collision mesh
	dMatrix matrix;
	NewtonBodyGetMatrix (level->m_level, &matrix[0][0]);
	dVector plane (matrix.m_up);
	plane.m_w = - (plane % matrix.m_posit);
	planeColl = CreatePlaneCollidion (world, plane);
	NewtonBodySetCollision(level->m_level, planeColl);
	NewtonReleaseCollision(world, planeColl);


	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}


void SimpleHeighMapColliion (SceneManager& system)
{
	NewtonWorld* world;
	LevelPrimitive *level;
	HeightFieldPrimitive* map;

	world = system.m_world;

	// create the sky box and the floor,
	level = LoadLevelAndSceneRoot (system, "flatplane.dae", 0);

	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	// delete the existing levelMesh
	NewtonDestroyBody (world, level->m_level);
	system.RemoveModel (level);

	// add a height map mesh 
	map = new HeightFieldPrimitive (world);
	system.AddModel (map);


	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}

void UserHeighMapColliion (SceneManager& system)
{
	NewtonWorld* world;
	LevelPrimitive *level;
	UserHeightFieldCollision* map;

	world = system.m_world;

	// create the sky box and the floor,
	level = LoadLevelAndSceneRoot (system, "flatplane.dae", 0);

	// delete the existing levelMesh
	NewtonDestroyBody (world, level->m_level);
	system.RemoveModel (level);


	// add a height map mesh 
//	new HeightFieldPrimitive (scene, world);
	map = new UserHeightFieldCollision (world);
	system.AddModel (map);

	// create a material to collision with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);


	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}




void SceneCollision (SceneManager& system)
{
	NewtonWorld* world;
//	dSceneNode* scene;

	world = system.m_world;

	// create the sky box and the floor,
	LoadSceneAndSceneRoot (system, "pitpool.dae", 0);

	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;


	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));
	location.m_y += 20.0f;

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}


void OptimizedMeshLevelCollision (SceneManager& system)
{
	NewtonWorld* world;

	world = system.m_world;

	// create the sky box and the floor,
	//	LoadLevelAndSceneRoot (system, "flatplane.dae", 1);
	//	LoadLevelAndSceneRoot (system, "dungeon.dae", 1);
	LoadLevelAndSceneRoot (system, "pitpool.dae", 1);
	//	LoadLevelAndSceneRoot (system, "dungeon.dae", 1);


	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}


void SimpleMeshLevelCollision (SceneManager& system)
{
	NewtonWorld* world;
//	dSceneNode* scene;

	world = system.m_world;

	// create the sky box and the floor,
//	LoadLevelAndSceneRoot (system, "flatplane.dae", 0);
//	LoadLevelAndSceneRoot (system, "dungeon.dae", 0);
	LoadLevelAndSceneRoot (system, "pitpool.dae", 0);
//	LoadLevelAndSceneRoot (system, "playground.dae", 0);
	

	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;


	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}

