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
#include "SceneManager.h"
#include "RenderCanvas.h"

static dInt32 g_memory;

void *operator new (size_t size) 
{ 
	void* ptr;
	ptr = malloc (size);

//unsigned xxx = unsigned (ptr);
//xxx &= 0xffff;
//	_ASSERTE (xxx != 0xE650);
//	dTrace (("%d %x\n", xxxx, ptr))
	return ptr; 
}                                          

void operator delete (void *ptr) 
{ 
	free (ptr); 
}

// memory allocation for Newton
void* PhysicsAlloc (int sizeInBytes)
{
	g_memory += sizeInBytes;
	return malloc (sizeInBytes);
}

// memory de-allocation for Newton
void PhysicsFree (void *ptr, int sizeInBytes)
{
	g_memory -= sizeInBytes;
	free (ptr);
}



SceneManager::SceneManager ()
{
	m_world = NULL;

	// Set the memory allocation function before creation the world
	// this is the only function that can be called before the creation of the newton world.
	// it should be called once, and the the call is optional 
	NewtonSetMemorySystem (PhysicsAlloc, PhysicsFree);

}


SceneManager::~SceneManager ()
{
	// close the last scene on exit
	CloseScene ();
}


void SceneManager::AddModel (OGLModel* model)
{
	dListNode* node;
	node = Append (model);

	model->m_node = node;
}

void SceneManager::RemoveModel (OGLModel* model)
{
	//	for(dListNode* node = GetFirst(); node; node = node->GetNext()) {
	//		if (model == node->GetInfo()) {
	//			Remove (node);
	//			break;
	//		}
	//	}
	dListNode* node;

	node = (dListNode*)model->m_node;
	Remove (node);

	delete model;
}


void SceneManager::Render ()
{
	dFloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
		OGLModel* model;
		model = node->GetInfo();
		model->Render ();
	}
}



void SceneManager::LoadScene (LaunchSDKDemoCallback sceneLoader)
{
//	NewtonWorld* world;

//	TimeLastFrame = GetTickCount();

	// create the newton world
	m_world = NewtonCreate ();

	// attach a callback to handle body going out of the world 
//	NewtonSetBodyLeaveWorldEvent (world, BodyLeaveWorld); 

	// set a collision destructor call back
//	NewtonSetCollisionDestructor(world, CollisionDestructor);

	// set performance counter
//	NewtonSetPerformanceClock (world, CHiResTimer::GetTimeInMicrosenconds);

	// load this demo into world
//	demosArray[index].m_callback(demo);
	sceneLoader (*this);

	// select instruction set mode
//	hardwareMode->set_int_val (1);
	//	hardwareMode->set_int_val (0);
//	ChangeHardwareMode (0);

	// set the solver mode
	//setSolverMode->set_int_val (2);
//	ChangeSolverMode (0);

	// set the Max Thread
//	selectNumberOfthreads->set_int_val (1);
	//selectNumberOfthreads->set_int_val (4);
//	SetThreadCount (0);

	// set auto sleep mode on
	//autoSleep->set_int_val (1);
//	ChangeSleepMode (0);

	//showContacts->set_int_val(1);
//	ChangeShowContacts (0);

	//showCollision->set_int_val(1);
//	ChangeShowCollision (0);

//	ChangeChangeIslandTrading (0);

	// set the adaptive friction model for faster speed 
	//	NewtonSetFrictionModel (nWorld, 1);

	// organize the world before beginning updates (optional)
	NewtonInvalidateCache (m_world);

	// set the z axis as the vertical axis (1 is the default witch map y axis)
	//	NewtonSetBroadPhaseVerticalAxis (world, 2);

}

void SceneManager::CloseScene ()
{
	// destroy all remaining visual objects
//	demo.CleanUp();
	while (GetFirst()) {
		OGLModel* model;
		model = GetFirst()->GetInfo();
		delete model;
		Remove(GetFirst());
	}

	// destroy the Newton world
	if (m_world) {
		NewtonDestroy (m_world);
		m_world = NULL;
	}

//	memset (&demo, 0, sizeof (demo));
	// check that there are no memory leak on exit
	_ASSERTE (NewtonGetMemoryUsed () == 0);
}
