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

#ifndef __NEWTON_DEMOS__
#define __NEWTON_DEMOS__
#include "dList.h"
#include "OGLModel.h"

class SceneManager;
typedef void (*LaunchSDKDemoCallback) (SceneManager& system);

class SceneManager: public dList <OGLModel*>
{
	public:
	SceneManager ();
	~SceneManager ();

	void CloseScene ();
	void LoadScene (LaunchSDKDemoCallback sceneLoader);


	typedef void (*Create) (SceneManager& me);
	typedef void (*KeyboardControl) (SceneManager& me);
	typedef void (*AutoSleep) (SceneManager& me, int mode);
	typedef void (*SetShowInslandMode) (SceneManager& me, int mode);
	typedef void (*SetShowContacts) (SceneManager& me, int mode);
	typedef void (*SetShowMeshCollision) (SceneManager& me, int mode);

	void Render ();
	void ApplyControl ();
	void SetAutoSleep (int mode);
	void CleanUp();

	void AddModel (OGLModel* model);
	void RemoveModel (OGLModel* model);
	NewtonWorld* m_world;

	AutoSleep m_autoSleep;
	KeyboardControl m_control;
	SetShowContacts m_showContacts;
	SetShowInslandMode m_showIslands;
	SetShowMeshCollision m_setMeshCollision;
};


void BasicSphereSpin (SceneManager& system);
void Restitution (SceneManager& system);
/*
void ClosestDistance(SceneManager& system);
void ConvexCast(SceneManager& system);
void BasicBoxStacks (SceneManager& system);
void BasicCylinderStacks (SceneManager& system);
void BasicConvexStacks (SceneManager& system);

void CreateWalls (SceneManager& system);

void CreatePyramid (SceneManager& system);
void CreatePyramidTall (SceneManager& system);

void CreateTower (SceneManager& system);
void CreateTowerTall (SceneManager& system);

void Jenga (SceneManager& system);
void JengaTall (SceneManager& system);

void UnstableStacks (SceneManager& system);
void UnstableStruture (SceneManager& system);

void Friction (SceneManager& system);

void PrecessingTops (SceneManager& system);
void ScaledCollision (SceneManager& system);
void CompoundCollision (SceneManager& system);


void SimplePlaneCollision (SceneManager& system);
void SimpleMeshLevelCollision (SceneManager& system);
void OptimizedMeshLevelCollision (SceneManager& system);
void SimpleHeighMapColliion (SceneManager& system);
void UserHeighMapColliion (SceneManager& system);
void SceneCollision (SceneManager& system);

void Bomb (SceneManager& system);
void Magnets (SceneManager& system);

void ArchimedesBuoyancy (SceneManager& system);


void LegacyJoints (SceneManager& system);
void BasicCustomJoints (SceneManager& system);
void BasicRobots (SceneManager& system);
void MotorizedRobots (SceneManager& system);
void TracktionJoints (SceneManager& system);

void SkinRagDoll (SceneManager& system);
void DescreteRagDoll (SceneManager& system);

void RayCastCar (SceneManager& system);
void PlayerController(SceneManager& system);
void RealTimeSimpleDestruction(SceneManager& system);
void PrefabSimpleDestruction(SceneManager& system);
void BuildingDestruction(SceneManager& system);
*/

#endif
