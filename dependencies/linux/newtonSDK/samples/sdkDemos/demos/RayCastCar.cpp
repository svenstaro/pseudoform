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
#include "dBone.h"
#include "RenderPrimitive.h"
#include "CustomDGRayCastCar.h"
#include "dAnimationClip.h"
#include "dPoseGenerator.h"
#include "dBonesToPoseBinding.h"
#include "../OGLMesh.h"
#include "../SceneManager.h"
#include "../PhysicsUtils.h"
#include "../toolBox/MousePick.h"
#include "../toolBox/OpenGlUtil.h"
#include "../toolBox/DebugDisplay.h"
#include "../toolBox/LevelPrimitive.h"
#include "../toolBox/PlaneCollision.h"
#include "../toolBox/HeightFieldPrimitive.h"
#include "../toolBox/UserHeightFieldCollision.h"



dFloat cameraDist = 7.0f;
dFloat cameraUpPos = 1.95f;
int vId = 0; // jeep
// vId = 1; // monstertruck  
// vId = 2; // f1
static NewtonWorld* world;
static class BasicCar* player;
static void CarCameraKeyboard (SceneManager& me);
static void NoPlayerAutoSleep (SceneManager& me, int mode);
static void CamFollow ();


//#define MAX_STEER_ANGLE			(30.0f * 3.1416f / 180.0f)
//#define MAX_STEER_ANGLE_RATE		(0.1f)
//#define MAX_TORQUE				(18000.0f)

#define F1_MASS					    (800.0f)
#define JEEP_MASS					(900.0f)
#define TRUCK_MASS				    (1180.0f)

#define F1_CENTRE_OF_MASS_OFFSET	(0.002f)
#define F1_ENGINE_CENTRE_OF_MASS_OFFSET  (0.001f)
#define JEEP_TIRE_MASS				(30.0f)
#define JEEP_CENTRE_OF_MASS_OFFSET	(0.4f)
#define JEEP_ENGINE_CENTRE_OF_MASS_OFFSET  (0.25f)
#define JEEP_SUSPENSION_LENGTH		(0.2f)
#define JEEP_SUSPENSION_SPRING		(175.0f)
#define JEEP_SUSPENSION_DAMPER		(6.0f)
#define MONST_CENTRE_OF_MASS_OFFSET	(0.5f)
#define MONST_ENGINE_CENTRE_OF_MASS_OFFSET  (0.25f)

//#define JEEP_USE_CONVEX_CAST		1
#define JEEP_USE_CONVEX_CAST		1


#define CUSTOM_VEHICLE_JOINT_ID		0xF4ED


#define F1_FRICTION					2.5f
#define JEEP_FRICTION				2.5f
#define TRUCK_FRICTION				2.5f




static void SetDemoCallbacks (SceneManager& system)
{
	system.m_control = CarCameraKeyboard;
	system.m_autoSleep = NoPlayerAutoSleep;
	system.m_showIslands = SetShowIslands;
	system.m_showContacts = SetShowContacts; 
	system.m_setMeshCollision = SetShowMeshCollision;
}

static LevelPrimitive* LoadLevelAndSceneRoot (SceneManager& system, const char* levelName, int optimized)
{
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

	//InitEyePoint (dVector (1.0f, 0.0f, 0.0f), dVector (-40.0f, 10.0f, 0.0f));

	return level;
}




struct CAR_CONFIG
{
	struct TIRE
	{
		const char* m_boneName;
		const char* m_animName;
	};
	const char* m_name;

	TIRE front_right;
	TIRE front_left;
	TIRE rear_right;
	TIRE rear_left;
};


static CAR_CONFIG jeep = 
{
	"jeep.dae",
	{"FR_tire", NULL},
	{"FL_tire", NULL},
	{"RR_tire", NULL},
	{"RL_tire", NULL},
};

/*
static CAR_CONFIG formulaOne = 
{
	"f1.dae",
	{"FR_tire", "f1_fr.ani"},
	{"FL_tire", "f1_fl.ani"},
	{"RR_tire", "f1_rr.ani"},
	{"RL_tire", "f1_rl.ani"},
};
*/

static CAR_CONFIG formulaOne = 
{
	"f1.dae",
	{"FR_tire", NULL},
	{"FL_tire", NULL},
	{"RR_tire", NULL},
	{"RL_tire", NULL},
};


static CAR_CONFIG monstertruck = 
{
	"monstertruck.dae",
	{"FR_tire", NULL},
	{"FL_tire", NULL},
	{"RR_tire", NULL},
	{"RL_tire", NULL},
};

class BasicCar: public CustomDGRayCastCar
{
	class TireAnimation
	{
	public:
		TireAnimation ()
		{
			m_tire = NULL;
			m_animation = NULL;
		}

		~TireAnimation ()
		{
			if (m_animation) {
				m_animation->Release();
			}
		}

		dBone* m_tire;	
		dBonesToPoseBinding* m_animation;
	};

public:

	static BasicCar* Create (const CAR_CONFIG& config, SceneManager* system, NewtonWorld* nWorld, const dMatrix& matrix, dInt32 materailID)
	{
		dFloat Ixx;
		dFloat Iyy;
		dFloat Izz;
		dFloat mass;
		dBone* carRoot;
		OGLModel* carModel;
		NewtonBody* rigidBody;
		NewtonCollision* chassisCollision;

		// open the level data
		char fullPathName[2048];
		GetWorkingFileName (config.m_name, fullPathName);

		// load the model in a temp scene node
		carModel = new OGLModel;

		// set the matrix
		carModel->SetMatrix(matrix);
		system->AddModel (carModel);

		// load the model
		OGLLoaderContext context;
		carModel->LoadCollada(fullPathName, context);

		// create a scene node to make a single body
		carRoot = carModel->FindBone("body");


		// make the collision shape
		OGLMesh* geo;
		geo = (OGLMesh*) carModel->FindMesh (carRoot->GetName());
		chassisCollision = NewtonCreateConvexHull(nWorld, geo->m_vertexCount, geo->m_vertex, 3 * sizeof (dFloat), 0.1f, 0, &carRoot->m_localMatrix[0][0]); 

		//create the rigid body
		rigidBody = NewtonCreateBody (nWorld, chassisCollision);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (rigidBody, carModel);

		// set the material group id for vehicle
		//	NewtonBodySetMaterialGroupID (m_vehicleBody, vehicleID);
		NewtonBodySetMaterialGroupID (rigidBody, materailID);

		// set a destructor for this rigid body
		NewtonBodySetDestructorCallback (rigidBody, PhysicsBodyDestructor);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (rigidBody, PhysicsApplyGravityForce);

		// set the transform call back function
		NewtonBodySetTransformCallback (rigidBody, SetTransform);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (rigidBody, &matrix[0][0]);

		dVector origin(0, 0, 0, 1);
		dVector inertia(0, 0, 0, 1);

		// calculate the moment of inertia and the relative center of mass of the solid
		NewtonConvexCollisionCalculateInertialMatrix (chassisCollision, &inertia[0], &origin[0]);
		mass = JEEP_MASS;
		// Set the vehicle Center of mass
		// the rear spoilers race the center of mass by a lot for a race car
		// we need to lower some more for the geometrical value of the y axis
		//origin.m_y *= JEEP_CENTRE_OF_MASS_OFFSET;

		// Some value is so high here
		if (strcmp(config.m_name,"jeep.dae")==0) { 
			mass = JEEP_MASS;
			origin.m_x += JEEP_ENGINE_CENTRE_OF_MASS_OFFSET;
			origin.m_y += -JEEP_CENTRE_OF_MASS_OFFSET; 
		} else if (strcmp(config.m_name,"monstertruck.dae")==0) { 
			mass = TRUCK_MASS;
			origin.m_x += MONST_ENGINE_CENTRE_OF_MASS_OFFSET;
			origin.m_y += -MONST_CENTRE_OF_MASS_OFFSET;
		} else if (strcmp(config.m_name,"f1.dae")==0) { 
			mass = F1_MASS;
			origin.m_x += -F1_ENGINE_CENTRE_OF_MASS_OFFSET;
			origin.m_y += -F1_CENTRE_OF_MASS_OFFSET;
		}

		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];

		NewtonBodySetCentreOfMass (rigidBody, &origin[0]);
		// set the mass matrix
		NewtonBodySetMassMatrix (rigidBody, mass, Ixx, Iyy, Izz);


		// release the collision 
		NewtonReleaseCollision (nWorld, chassisCollision);	


		// set the vehicle local coordinate system 
		dMatrix chassisMatrix;
		// if you vehicle move along the z direction you can use 
//		chassisMatrix.m_front = dVector (0.0f, 0.0f, 1.0f, 0.0);
		chassisMatrix.m_front = dVector (1.0f, 0.0f, 0.0f, 0.0f);			// this is the vehicle direction of travel
		chassisMatrix.m_up	  = dVector (0.0f, 1.0f, 0.0f, 0.0f);			// this is the downward vehicle direction
		chassisMatrix.m_right = chassisMatrix.m_front * chassisMatrix.m_up;	// this is in the side vehicle direction (the plane of the wheels)
		chassisMatrix.m_posit = dVector (0.0f, 0.0f, 0.0f, 1.0f);

		// create a vehicle joint with 4 tires
		BasicCar* carJoint;
		carJoint = new BasicCar (chassisMatrix, rigidBody, 4); 
		carJoint->m_tireOffsetMatrix = carRoot->CalcGlobalMatrix();

		// get radio and width of the tire
		dFloat witdh;
		dFloat radius;
		carJoint->CalculateTireDimensions ("FR_tire", witdh, radius);

		// add all tire to car
		carJoint->AddTire (config, config.front_right.m_boneName, witdh, radius, config.front_right.m_animName); 
		carJoint->AddTire (config, config.front_left.m_boneName, witdh, radius, config.front_left.m_animName); 
		carJoint->AddTire (config, config.rear_right.m_boneName, witdh, radius, config.rear_right.m_animName); 
		carJoint->AddTire (config, config.rear_left.m_boneName, witdh, radius, config.rear_left.m_animName); 
		return carJoint;

	}

	void ApplySteering (dFloat value)
	{
		dFloat vEngineSteerAngle = GenerateTiresSteerAngle( value );
		dFloat vEngineSteerForce = GenerateTiresSteerForce( vEngineSteerAngle );
		// Set the both generate steer angle and force value's
		SetTireSteerAngleForce( 0, vEngineSteerAngle, vEngineSteerForce );
		SetTireSteerAngleForce( 1, vEngineSteerAngle, vEngineSteerForce );
		if (vId==1){
			SetTireSteerAngleForce( 2, -vEngineSteerAngle, -(vEngineSteerForce/2) );
			SetTireSteerAngleForce( 3, -vEngineSteerAngle, -(vEngineSteerForce/2) );
		}
	}

	void ApplyBrake (dFloat value)
	{
		// Set the generate brake value
		SetTireBrake( 2, value);
		SetTireBrake( 3, value);
	}

	void ApplyTorque (dFloat value)
	{

		dFloat vEngineTorque;
//		vEngineTorque = GenerateTiresTorque( value );
		vEngineTorque = GenerateEngineTorque (value);

//		// Set the generate torque value
		SetTireTorque( 2, vEngineTorque );
		SetTireTorque( 3, vEngineTorque );
		switch (vId) 
		{
			case 0: 
			{
				_ASSERTE (0);
//				SetCustomTireTorque( 0, vEngineTorque );
//				SetCustomTireTorque( 1, vEngineTorque );
				break;
			}
			case 1:
			{
				_ASSERTE (0);
//				SetCustomTireTorque( 0, vEngineTorque );
//				SetCustomTireTorque( 1, vEngineTorque );
				dFloat speed;
				speed = dAbs( GetSpeed() );
				if (speed<5.0f) {
					_ASSERTE (0);
					// Can make a little wheely (:
//					SetVarTireMovePointForceUp(2,-1.35f);
//					SetVarTireMovePointForceUp(3,-1.35f);
				} else {
					_ASSERTE (0);
//					SetVarTireMovePointForceUp(2,0.0f);
//					SetVarTireMovePointForceUp(3,0.0f);
				}
				break;
			}
			case 2:
			{
//				dFloat speed;
//				speed = dAbs( GetSpeed() );
//				if ((speed<25) && (speed>20)) {
//					_ASSERTE (0);
//					SetVarTireMovePointForceUp(2,0.2f);
//					SetVarTireMovePointForceUp(3,0.2f);	
//				} else if (speed<20) {
//					_ASSERTE (0);
//					SetVarTireMovePointForceUp(2,0.25f);
//					SetVarTireMovePointForceUp(3,0.25f);
//				} else {
//					_ASSERTE (0);
//					SetVarTireMovePointForceUp(2,0.0f);
//					SetVarTireMovePointForceUp(3,0.0f);
//				}
			}
		}
	}

protected:
	BasicCar (const dMatrix& chassisMatrix, NewtonBody* carBody, int maxTiresCount)
		:CustomDGRayCastCar (maxTiresCount, chassisMatrix, carBody) 
	{
		m_steerAngle = 0.0f;
		// assign a type information to locate the joint in the callbacks
		SetJointID (CUSTOM_VEHICLE_JOINT_ID);
	}

	~BasicCar()
	{
		for (int i = 0; i < GetTiresCount(); i ++) {
			TireAnimation* tireAnim;
			const CustomDGRayCastCar::Tire& tire = GetTire (i);
			tireAnim = (TireAnimation*) tire.m_userData;
			delete tireAnim;
		}
	}

private:
	void AddTire (const CAR_CONFIG& config, const char* boneName, dFloat width, dFloat radius, const char* animName)
	{
		OGLModel* carRoot;
		TireAnimation* tire;

		tire = new TireAnimation;

		carRoot = (OGLModel*) NewtonBodyGetUserData(GetBody0());
		tire->m_tire = carRoot->FindBone(boneName);

		// if the tire have an anomation...
		if (animName) {
			char fullPathName[2048];
			GetWorkingFileName (animName, fullPathName);
			dBonesToPoseBinding* bind;
			bind = new dBonesToPoseBinding (fullPathName);

			bind->SetUpdateCallback (UdateSuspentionParts);


			// set the user data to the binding pose;
			for (dBonesToPoseBinding::dListNode* node = bind->GetFirst(); node; node = node->GetNext()) {
				const char* name;
				dBindFrameToNode& poseBind = node->GetInfo();
				name = poseBind.m_sourceTranform->m_source->m_bindName;
				poseBind.m_userData = tire->m_tire->GetParent()->Find(name);
			}

			tire->m_animation = bind;
		}

		// add this tire, get local position and rise it by the suspension length 
		dMatrix matrix (tire->m_tire->CalcGlobalMatrix());
		dVector tirePosition (matrix.m_posit);
		// not need anymore because it is done internally already now...
		//tirePosition += matrix .m_up.Scale (JEEP_SUSPENSION_LENGTH);
		if (strcmp(config.m_name,"jeep.dae")==0) { 
			AddSingleSuspensionTire (tire, tirePosition, JEEP_TIRE_MASS, radius, width, JEEP_FRICTION, JEEP_SUSPENSION_LENGTH, JEEP_SUSPENSION_SPRING, JEEP_SUSPENSION_DAMPER, JEEP_USE_CONVEX_CAST);
		} else if (strcmp(config.m_name,"monstertruck.dae")==0) { 
			AddSingleSuspensionTire (tire, tirePosition, 50, radius, width, TRUCK_FRICTION, 0.6f, 65.0f, 2.0f, JEEP_USE_CONVEX_CAST);
		} else if (strcmp(config.m_name,"f1.dae")==0) {  
			AddSingleSuspensionTire (tire, tirePosition, 25, radius, width, F1_FRICTION, 0.2f, 200.0f, 6.0f, JEEP_USE_CONVEX_CAST);
		}
	}


	void CalculateTireDimensions (const char* tireName, dFloat& witdh, dFloat& radius) const
	{
		dVector extremePoint;
		NewtonWorld* world;
		//dSceneNode* carRoot;
		OGLModel* carModel;
		dBone* tirePart;
		OGLMesh* geo;
		NewtonCollision* collision;

		// find the the tire visual mesh 
		world = NewtonBodyGetWorld(GetBody0());
		carModel = (OGLModel*) NewtonBodyGetUserData(GetBody0());

		tirePart = carModel->FindBone (tireName);
		geo = (OGLMesh*) carModel->FindMesh(tireName);

		// make a convex hull collition shape to assist in calculation of the tire shape size
		collision = NewtonCreateConvexHull(world, geo->m_vertexCount, geo->m_vertex, 3 * sizeof (dFloat), 0.0f, 0, NULL); 

		dMatrix tireMatrix (tirePart->CalcGlobalMatrix() * carModel->m_matrix);
		//		dMatrix tireMatrix (tirePart->CalcGlobalMatrix());

		// fin the support points tha can be use to define the tire collision mesh
		dVector upDir (tireMatrix.UnrotateVector(dVector (0.0f, 1.0f, 0.0f, 0.0f)));
		NewtonCollisionSupportVertex (collision, &upDir[0], &extremePoint[0]);
		radius = upDir % extremePoint;

		dVector withdDir (tireMatrix.UnrotateVector(carModel->m_matrix.m_right));
		NewtonCollisionSupportVertex (collision, &withdDir[0], &extremePoint[0]);
		witdh = withdDir % extremePoint;

		withdDir = withdDir.Scale (-1.0f);
		NewtonCollisionSupportVertex (collision, &withdDir[0], &extremePoint[0]);
		witdh += withdDir % extremePoint;

		NewtonReleaseCollision (world, collision);	
	}

	static void UdateSuspentionParts (void* userData, dFloat* posit, dFloat* rotation)
	{
		dBone* node;
		node = (dBone*) userData;

		dVector p (posit[0], posit[1], posit[2], 1.0f);
		dQuaternion r (rotation[0], rotation[1], rotation[2], rotation[3]); 
		node->SetMatrix (dMatrix (r, p));
	}


	static void SetTransform  (const NewtonBody* body, const dFloat* matrix, int threadIndex)
	{
		BasicCar* car;
		NewtonJoint* joint;


		// set the transform of the main body
		PhysicsSetTransform (body, matrix, threadIndex);

		// find the car joint attached to the body
		car = NULL;
		for (joint = NewtonBodyGetFirstJoint(body); joint; joint = NewtonBodyGetNextJoint(body, joint)) {
			car = (BasicCar*) NewtonJointGetUserData(joint);
			if (car->GetJointID() == CUSTOM_VEHICLE_JOINT_ID) {
				break;
			}
		}

		if (car) {
			// position all visual tires matrices

			int count;
			const dMatrix& carMatrix = *((dMatrix*)matrix);
			dMatrix rootMatrixInv (car->m_tireOffsetMatrix * carMatrix);
			rootMatrixInv = rootMatrixInv.Inverse();
			count = car->GetTiresCount();

			// for each tire get the global matrix postion, and calculate the local matrix relative to the main body   
			for (int i = 0; i < count; i ++) {

				dBone* tireNode;
				TireAnimation* tireAnim;

				const CustomDGRayCastCar::Tire& tire = car->GetTire (i);

				tireAnim = (TireAnimation*) tire.m_userData;
				tireNode = tireAnim->m_tire;

				// if the tire has an animation, update the animation matrices
				if (tireAnim->m_animation) {
					dFloat param;
					param = (1.0f - car->GetTireParametricPosition(i));

					if (param > 1.0f) {
						param = 1.0f;
					}
					if (param < 0.0f) {
						param = 0.0f;
					}

					tireAnim->m_animation->GeneratePose (param);
					tireAnim->m_animation->UpdatePose();
				}

				// calculate the tire local matrix
				dMatrix matrix (car->CalculateTireMatrix(i) * rootMatrixInv);
				tireNode->SetMatrix(matrix);

				if (DebugDisplayOn()) {
					// if debug display show the contact and tire collsion shape in debug mode 
					NewtonWorldCriticalSectionLock(world);
					dMatrix tireBaseMatrix (car->CalculateSuspensionMatrix(i, tire.m_posit) * car->GetChassisMatrixLocal() * carMatrix);
					DebugDrawCollision (tire.m_shape, tireBaseMatrix, dVector (1.0f, 1.0f, 0.0f, 1.0f));

					dVector span (tire.m_contactPoint + tire.m_contactNormal.Scale (1.0f));
					DebugDrawLine (tire.m_contactPoint, span, dVector (1.0f, 0.0f, 0.0f, 1.0f));

					//DebugDrawLine (tire.m_contactPoint, car->dbpos, dVector (0.5f, 0.0f, 1.0f, 1.0f));
					NewtonWorldCriticalSectionUnlock(world);
				}
			}
		}
	}

	dFloat m_steerAngle;
	dMatrix m_tireOffsetMatrix;
};

static BasicCar* car1; // jeep
static BasicCar* car2; // monstertruck
static BasicCar* car3; // f1

static void CamFollow ()
{
	if (!player) {
		return;
	}
	dFloat gSpeed;
	dMatrix target;
	gSpeed = dAbs(player->GetSpeed());
	NewtonBodyGetMatrix (player->GetBody0(), &target[0][0]);
	GetDGCamera()->PointToMatrix(target,dVector(0.0f,1.0f,0.0f,0.0f));
	// 4000 is the desired distance.
	GetDGCamera()->m_Matrix.m_posit += GetDGCamera()->m_Matrix.m_right.Scale(gSpeed/3250);
	GetDGCamera()->m_Matrix.m_posit.m_y = target.m_posit.m_y + cameraUpPos;

}


static void NoPlayerAutoSleep (SceneManager& me, int mode)
{
	// set auto sleep mode 
	AutoSleep (me, mode);

	// make sure the player do no go to auto sleep node
	if (player) {
		NewtonBodySetAutoSleep (player->GetBody0(), 0);
	}
}

static void CarCameraKeyboard (SceneManager& me)
{

	// read the mouse position and set the camera direction

	dMOUSE_POINT mouse1;
	dInt32 mouseLeftKey;
	static dMOUSE_POINT mouse0;

	GetCursorPos(mouse1);

	// this section control the camera object picking
	mouseLeftKey = dGetKeyState (KeyCode_L_BUTTON);

	if (!MousePick (world, mouse1, mouseLeftKey, 0.125f, 1.0f)) {
		// we are not in mouse pick mode, then we are in camera tracking mode
		if (mouseLeftKey) {
			// when click left mouse button the first time, we reset the camera
			// convert the mouse x position to delta yaw angle
			if (mouse1.x < (mouse0.x - 1)) {
				yawAngle += 1.0f * 3.1416f / 180.0f;
				if (yawAngle > (360.0f * 3.1416f / 180.0f)) {
					yawAngle -= (360.0f * 3.1416f / 180.0f);
				}
			} else if (mouse1.x > (mouse0.x + 1)) {
				yawAngle -= 1.0f * 3.1416f / 180.0f;
				if (yawAngle < 0.0f) {
					yawAngle += (360.0f * 3.1416f / 180.0f);
				}
			}

			if (mouse1.y < (mouse0.y - 1)) {
				rollAngle += 1.0f * 3.1416f / 180.0f;
				if (rollAngle > (80.0f * 3.1416f / 180.0f)) {
					rollAngle = 80.0f * 3.1416f / 180.0f;
				}
			} else if (mouse1.y > (mouse0.y + 1)) {
				rollAngle -= 1.0f * 3.1416f / 180.0f;
				if (rollAngle < -(80.0f * 3.1416f / 180.0f)) {
					rollAngle = -80.0f * 3.1416f / 180.0f;
				}
			}
		}
	}

	// save mouse position and left mouse key state for next frame
	mouse0 = mouse1;

	// apply hand brakes torque
	if (dGetKeyState (KeyCode_SPACE)) {
		player->ApplyBrake(1.0f);
	}

	// apply tire torque
	if (dGetKeyState (KeyCode_w)) {
		player->ApplyTorque(1.0f);
	} else if (dGetKeyState (KeyCode_s)) {
		player->ApplyTorque(-1.0f);
	} else {
		player->ApplyTorque(0.0f);
//		player->ApplyTorque(0.1f);
	}


	// apply steering
	if (dGetKeyState (KeyCode_d)) {
		player->ApplySteering (-1.0f);
	} else if (dGetKeyState (KeyCode_a)) {
		player->ApplySteering (1.0f);
	} else {
		player->ApplySteering (0.0f);
	}

	// attach the Camera to the selected player car
	if (dGetKeyState (KeyCode_F1)) {
		vId = 0;
		player = car1;
		dMatrix target;
		NewtonBodyGetMatrix (player->GetBody0(), &target[0][0]);
		GetDGCamera()->Position(target.m_posit.m_x,target.m_posit.m_y+5.0f,target.m_posit.m_z+20.0f);
		NewtonBodySetAutoSleep (player->GetBody0(), 0);
		NewtonBodySetAutoSleep (car2->GetBody0(), 1);
		NewtonBodySetAutoSleep (car3->GetBody0(), 1);
		cameraUpPos = 2.5f;

	} else if (dGetKeyState (KeyCode_F2)) {
		vId = 1;
		player = car2;
		dMatrix target;
		NewtonBodyGetMatrix (player->GetBody0(), &target[0][0]);
		GetDGCamera()->Position(target.m_posit.m_x,target.m_posit.m_y+5.0f,target.m_posit.m_z+20.0f);
		NewtonBodySetAutoSleep (player->GetBody0(), 0);
		NewtonBodySetAutoSleep (car1->GetBody0(), 1);
		NewtonBodySetAutoSleep (car3->GetBody0(), 1);
		cameraUpPos = 3.0f;

	} else if (dGetKeyState (KeyCode_F3)) {
		vId = 2;
		player = car3;
		dMatrix target;
		NewtonBodyGetMatrix (player->GetBody0(), &target[0][0]);
		GetDGCamera()->Position(target.m_posit.m_x,target.m_posit.m_y+5.0f,target.m_posit.m_z+20.0f);
		NewtonBodySetAutoSleep (player->GetBody0(), 0);
		NewtonBodySetAutoSleep (car1->GetBody0(), 1);
		NewtonBodySetAutoSleep (car2->GetBody0(), 1);
		cameraUpPos = 1.95f;
	}
	CamFollow();
} 



void RayCastCar (SceneManager& system)
{
	NewtonWorld* world;
	dBone* startPoint;
	LevelPrimitive* scene;

	world = system.m_world;

	// create the sky box and the floor,
//	scene = LoadLevelAndSceneRoot (system, "flatplane.dae", 1);
	scene = LoadLevelAndSceneRoot (system, "newtontrack.dae", 1);

	// create a material 
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	// find the start point
	dMatrix start (GetIdentityMatrix());
	startPoint = scene->FindBone ("startPoint");
	if (startPoint) {
		start = startPoint->CalcGlobalMatrix() * scene->m_matrix;
	}

	start.m_posit.m_y = FindFloor (world, start.m_posit.m_x, start.m_posit.m_z) + 1.5f;
	dMatrix start2 = start;
	start2.m_posit.m_x -= 5;
	start2.m_posit.m_y += 1;
	dMatrix start3 = start2;
	//start3.m_posit.m_y += 1;
	start3.m_posit.m_x += 10;
	//InitEyePoint (start.m_front, start.m_posit - start.m_front.Scale (5.0f));

	yawAngle = 0.0f * 3.1416f/180.0f;
	rollAngle = -20.0f * 3.1416f/180.0f;

	/*
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	*/

	//	car = BasicCar::Create (formulaOne, &system, system.m_world, start, defaultMaterialID);
	//	player = car;
/*
	//	start.m_posit += dVector(0.0f, 0.0f, -3.0f, 0.0f);
	// The jeep is hard to configure because the mass for this model is more on the back.
	// It adding more pressure on the back suspenssion.
	// Maybe it can come more simple to make the back suspenssion more hard, I don't have test...
	// Normally this jeep can use better mass center, but I place it low because I like high speed hehe.
	// In reality this jeep can't go fast like in this demo hehe...
	car1 = BasicCar::Create (jeep, &system, system.m_world, start, defaultMaterialID);
	// all of this var can come changed in runtime...
	car1->SetVarChassisRotationLimit(0.98f);
	car1->SetVarMaxBrakeForce(165.0f);
	car1->SetVarMaxTorque(5000.0f);
	car1->SetVarMaxTorqueRate(500.0f);
	car1->SetVarMaxSteerRate(0.1f);
	car1->SetVarEngineSteerDiv(200.0f);
	// Make a wheely jeep simulation (: lol
	//car1->SetVarTireMovePointForceUp(2,-0.5);
	//car1->SetVarTireMovePointForceUp(3,-0.5);

	car2 = BasicCar::Create (monstertruck, &system, system.m_world, start2, defaultMaterialID);
	// all of this var can come changed in runtime...
	car2->SetVarChassisRotationLimit(0.985f);
	car2->SetVarMaxBrakeForce(175.0f);
	car2->SetVarMaxTorque(4750.0f);
	car2->SetVarMaxTorqueRate(600.0f);
	car2->SetVarMaxSteerRate(0.075f);
	car2->SetVarEngineSteerDiv(200.0f);
	car2->SetVarMaxSteerForceRate(0.02f);
	car2->SetVarMaxSteerAngle(25.0f);
	car2->SetVarMaxSteerSpeedRestriction(0.075f);
*/
//	car3 = BasicCar::Create (jeep, &system, system.m_world, start3, defaultMaterialID);
	car3 = BasicCar::Create (formulaOne, &system, system.m_world, start3, defaultMaterialID);
//	car3 = BasicCar::Create (monstertruck, &system, system.m_world, start3, defaultMaterialID);

/*
	// all of this var can come changed in runtime...
	car3->SetVarChassisRotationLimit(0.98f);
	car3->SetVarMaxTorque(12500.0f);
	car3->SetVarMaxTorqueRate(500.0f);
	car3->SetVarMaxSteerRate(0.1f);
	car3->SetVarEngineSteerDiv(100.0f);
	car3->SetVarMaxBrakeForce(165.0f);
	car3->SetVarMaxSteerAngle(20.0f);
*/
	player = car3;

	vId = 2;
	// Don't use the SetCamera in this demo because it make a target to the position.
	// This demo use a different target method named CamFollow.
	// Both command can make conflict in this demo.
	GetDGCamera()->m_FocalLength = 75.0f;
	GetDGCamera()->Position(start3.m_posit.m_x,start3.m_posit.m_y,start3.m_posit.m_z+20.0f);
	// Just to make sure the cam is present at the demo begin, because it's based on the vehicle speed.
	CamFollow ();

	// populate the world with few more cars
	float x0 = start3.m_posit.m_x;
	float z0 = start3.m_posit.m_z;
	for(int x = 0; x < 6; x ++) {
		for(int z = 0; z < 6; z ++) {
			int rnd;
			start3.m_posit = dVector (float (x0 + (x - 3)* 15 - 12.0f), 0.0f, z0 + float ((z - 3)* 15 - 12), 1.0f);   
			start3.m_posit.m_y = FindFloor (world, start3.m_posit.m_x, start3.m_posit.m_z) + 2.0f;
			rnd = rand () >> 3 ;
			if (rnd & 1) {
//				BasicCar::Create (monstertruck, &system, system.m_world, start3, defaultMaterialID);
			} else if (rnd & 2) {
//				BasicCar::Create (formulaOne, &system, system.m_world, start3, defaultMaterialID);
			} else {
//				BasicCar::Create (jeep, &system, system.m_world, start3, defaultMaterialID);
			}
		}
	}
}



