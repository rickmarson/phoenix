#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

/*
 
 Physics Engine, based on Bullet
 
 */

#include "iphysics_engine.h"
#include "rigid_body.h"
#include "soft_body.h"

#include "Bullet/btBulletDynamicsCommon.h"

#include "Bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "Bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "Bullet/BulletSoftBody/btSoftRigidCollisionAlgorithm.h"
#include "Bullet/BulletSoftBody/btSoftSoftCollisionAlgorithm.h"

//default cpu soft body solver. see bullet/multithreaded for GPGPU soft body solvers. 
#include "Bullet/BulletSoftBody/btDefaultSoftBodySolver.h"

#include <vector>

namespace NYX {
/*
Physics Engine
*/
class DefaultPhysicsEngine : public IPhysicsEngine
{
public:

    DefaultPhysicsEngine();
    ~DefaultPhysicsEngine();

	void InitPhysics(ePhysicsWorld ePhysics);
    void SetGravity(Vector3 gravity);
	void AddBody(PhysicsBodyPtr rigidBody);
	void StepSimulation(float timeStep);

	void SetMaxSubSteps(int stepNo); //default 10

	void SetSoftWorldInfo(float air_density, float water_density, float water_offset, Vector3 water_normal);
	btSoftBodyWorldInfo GetSoftWorldInfo();

private:

	ePhysicsWorld mWorldType;

	std::vector<PhysicsBodyPtr> mBodyList;

	btDefaultCollisionConfiguration* mBulletCollisionConfig;
	btCollisionDispatcher* mBulletCollisionDispatcher;
	btBroadphaseInterface* mBulletBroadphase;
	btConstraintSolver* mBulletConstraintSolver; //default, no multithreading

	btDynamicsWorld* mBulletDynamicsWorld;
	btSoftBodyWorldInfo mBulletSoftWorldInfo;
	btSoftBodySolver* mBulletSoftBodySolver;

	btCollisionAlgorithmCreateFunc*	mBulletCollisionFunc;
	btAlignedObjectArray<btSoftSoftCollisionAlgorithm*> mBulletSoftSoftAlgorithms;
	btAlignedObjectArray<btSoftRigidCollisionAlgorithm*> mBulletSoftRigidAlgorithms;

	int mMaxSubSteps;

	void AddRigidBody(PhysicsBodyPtr body);
	void AddSoftBody(PhysicsBodyPtr body);
};

inline void DefaultPhysicsEngine::SetMaxSubSteps(int stepNo) { mMaxSubSteps = stepNo; }
inline btSoftBodyWorldInfo DefaultPhysicsEngine::GetSoftWorldInfo() { return mBulletSoftWorldInfo; }
inline void DefaultPhysicsEngine::SetSoftWorldInfo(float air_density, float water_density, float water_offset, Vector3 water_normal)
{
	mBulletSoftWorldInfo.air_density = air_density;
	mBulletSoftWorldInfo.water_density = water_density;
	mBulletSoftWorldInfo.water_offset = water_offset;
	mBulletSoftWorldInfo.water_normal = btVector3(water_normal.X(), water_normal.Y(), water_normal.Z());
}

}
#endif // PHYSICSENGINE_H
