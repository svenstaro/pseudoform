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

#ifndef __PHYSICS_UTIL__
#define __PHYSICS_UTIL__



enum PrimitiveType
{
	_BOX_PRIMITIVE,
	_CONE_PRIMITIVE,
	_SPHERE_PRIMITIVE,
	_CYLINDER_PRIMITIVE,
	_CAPSULE_PRIMITIVE,
	_CHAMFER_CYLINDER_PRIMITIVE,
	_RANDOM_CONVEX_HULL_PRIMITIVE,
	_REGULAR_CONVEX_HULL_PRIMITIVE,
};


#define CAMERA_SPEED 10.0f

extern dVector cameraDir;
extern dVector cameraEyepoint;
extern dFloat yawAngle;
extern dFloat rollAngle;

class SceneManager;
class NewtonCustomJoint;


dFloat RandomVariable(dFloat amp);
void InitEyePoint (const dVector& dir, const dVector& origin);

void AutoSleep (SceneManager& me, int mode);
void SetShowIslands (SceneManager& me, int mode);
void SetShowContacts (SceneManager& me, int mode);
void SetShowMeshCollision (SceneManager& me, int mode);

void ShowBodyContacts (const NewtonBody* body);
void ShowJointContacts (const NewtonJoint* contactJoint);
void ShowJointInfo(const NewtonCustomJoint* joint);

void ConvexCastPlacement (NewtonBody* body);
dFloat FindFloor (const NewtonWorld* world, dFloat x, dFloat z);

//void CalculateAABB (const NewtonBody* body, dVector& minP, dVector& maxP);
void CalculateAABB (const NewtonCollision* collision, const dMatrix& matrix, dVector& minP, dVector& maxP);


void Keyboard(SceneManager& me);
void PhysicsBodyDestructor (const NewtonBody* body);
void PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix, int threadIndex);
void PhysicsApplyGravityForce (const NewtonBody* body, dFloat timestep, int threadIndex);
int  PhysicsIslandUpdate (const NewtonWorld* world, const void* islandHandle, int bodyCount);

NewtonBody* CreateGenericSolid (NewtonWorld* world, SceneManager* scene, dFloat mass, const dMatrix& matrix, const dVector& size, PrimitiveType type, int MatreialID);
void AddBoxes(SceneManager* scene, dFloat mass, const dVector& origin, const dVector& size, int xCount, int zCount, dFloat spacing, PrimitiveType type, int materialID);

// this callback is called for every contact between the two bodies
//int GenericContactProcess (const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, dFloat timestep, int threadIndex);
int OnAABBOverlap (const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex);
void GenericContactProcess (const NewtonJoint* contactJoint, dFloat timestep, int threadIndex);



#endif
