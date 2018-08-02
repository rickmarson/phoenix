/*

Physics Engine, based on Bullet

*/

#include "physics_engine.h"
#include "Utils/log_manager.h"
#include "Events/event_manager.h"

using namespace std;

namespace NYX {

DefaultPhysicsEngine::DefaultPhysicsEngine() :
	mBulletCollisionConfig(NULL),
	mBulletCollisionDispatcher(NULL),
	mBulletBroadphase(NULL),
	mBulletConstraintSolver(NULL),
	mBulletDynamicsWorld(NULL),
	mBulletSoftBodySolver(NULL),
	mBulletCollisionFunc(NULL),
	mMaxSubSteps(10)
{
    

}

DefaultPhysicsEngine::~DefaultPhysicsEngine()
{
	//delete bullet rigid bodies, our rigid bodies will be deleted with their list.
    for (int i = mBulletDynamicsWorld->getNumCollisionObjects()-1; i>=0; i--)
	{
		btCollisionObject* obj = mBulletDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		mBulletDynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete dynamics world
	delete mBulletDynamicsWorld;
	//delete solver
	delete mBulletConstraintSolver;
	//delete broadphase
	delete mBulletBroadphase;
	//delete dispatcher
	delete mBulletCollisionDispatcher;
	//delete collidion config
	delete mBulletCollisionConfig;
	//delete soft body stuff
	if (mWorldType == PHYS_RIGID_AND_SOFT)
	{
		delete mBulletSoftBodySolver;
		delete mBulletCollisionFunc;
	}
}

void DefaultPhysicsEngine::InitPhysics(ePhysicsWorld ePhysics)
{
	mWorldType = ePhysics;

	switch (mWorldType)
	{
	case PHYS_RIGID_ONLY:
		mBulletCollisionConfig = new btDefaultCollisionConfiguration();
		mBulletCollisionDispatcher = new btCollisionDispatcher(mBulletCollisionConfig);
		mBulletBroadphase = new btDbvtBroadphase();
		//the default constraint solver. No multithreading.
		mBulletConstraintSolver = new btSequentialImpulseConstraintSolver();

		mBulletDynamicsWorld = new btDiscreteDynamicsWorld(mBulletCollisionDispatcher,
														mBulletBroadphase,
														mBulletConstraintSolver,
														mBulletCollisionConfig);
		break;
	case PHYS_RIGID_AND_SOFT:
		mBulletCollisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
		mBulletCollisionDispatcher = new btCollisionDispatcher(mBulletCollisionConfig);
		mBulletSoftWorldInfo.m_dispatcher = mBulletCollisionDispatcher;
		mBulletBroadphase = new btDbvtBroadphase();
		mBulletSoftWorldInfo.m_broadphase = mBulletBroadphase;
		mBulletConstraintSolver = new btSequentialImpulseConstraintSolver();
		mBulletSoftBodySolver = new btDefaultSoftBodySolver();
		mBulletDynamicsWorld = new btSoftRigidDynamicsWorld(mBulletCollisionDispatcher,
															mBulletBroadphase,
															mBulletConstraintSolver,
															mBulletCollisionConfig,
															mBulletSoftBodySolver);
		break;
	default:
		break;
	}
}

void DefaultPhysicsEngine::SetGravity(Vector3 gravity)
{
	btVector3 bulletGravity(gravity.X(), gravity.Y(), gravity.Z());
	mBulletDynamicsWorld->setGravity(bulletGravity);
}

void DefaultPhysicsEngine::AddBody(PhysicsBodyPtr body)
{
	if (!body->IsInitialized())
	{
		LogManager::GetInstance()->LogMessage("Error! RigidBody was not initialized properly!");
		return;
	}
	
	mBodyList.push_back(body);

	switch (body->BodyType())
	{
	case BDY_RIGID:
		AddRigidBody(body);
		break;
	case BDY_SOFT:
		AddSoftBody(body);
		break;
	default:
		break;
	}
}

void DefaultPhysicsEngine::AddRigidBody(PhysicsBodyPtr body)
{
	RigidBody* rigidBody = dynamic_cast<RigidBody*>(body.get());
	mBulletDynamicsWorld->addRigidBody(rigidBody->GetBulletRigidBody());
}

void DefaultPhysicsEngine::AddSoftBody(PhysicsBodyPtr body)
{
	if (mWorldType != PHYS_RIGID_AND_SOFT)
	{
		LogManager::GetInstance()->LogMessage("Error! Can't add a soft body to a rigid-only world!");
		return;
	}

	SoftBody* softBody = dynamic_cast<SoftBody*>(body.get());
	
	btSoftRigidDynamicsWorld *softWorld = dynamic_cast<btSoftRigidDynamicsWorld*>(mBulletDynamicsWorld);
	softWorld->addSoftBody(softBody->GetBulletSoftBody());
}

void DefaultPhysicsEngine::StepSimulation(float timeStep)
{
	EventManager* pEventMng = EventManager::GetInstance();

	//update total force for all objects
	for (uint i = 0; i < mBodyList.size(); i++)
		mBodyList[i]->CalculateTotalForce();

	mBulletDynamicsWorld->stepSimulation(timeStep,mMaxSubSteps);

	//create and send events, dynamic boies only
	/*Note that positions and rotations are in absolute coordinates (i.e. not relative to any parent 
	  the associated scene node might have). 
	  Bullet is not aware of scene nodes parenting associations, so everything must be exchanged in absolute (world) 
	  coordinates.
	*/
	for (uint i = 0; i < mBodyList.size(); i++)
	{
		if (!mBodyList[i]->IsDynamic())
			continue;

		btTransform trans;
		float* mesh = nullptr; //used for soft bodies only

		switch (mBodyList[i]->BodyType())
		{
		case BDY_RIGID:
			{
				RigidBody* rigidBody = dynamic_cast<RigidBody*>(mBodyList[i].get());
				rigidBody->GetBulletRigidBody()->getMotionState()->getWorldTransform(trans);
			}
			break;
		case BDY_SOFT:
			{
				SoftBody* softBody = dynamic_cast<SoftBody*>(mBodyList[i].get());
				trans = softBody->GetBulletSoftBody()->getWorldTransform();
				mesh = softBody->GetUpdatedMesh();
			}
			break;
		default:
			break;
		}

		Vector3 pos(trans.getOrigin().getX(), 
					trans.getOrigin().getY(),
					trans.getOrigin().getZ());
		Matrix3x3 attitude;
		btMatrix3x3 bAttitude = trans.getBasis();
		btVector3 row = bAttitude.getRow(0);
		attitude(0,0) = row.getX(); attitude(0,1) = row.getY(); attitude(0,2) = row.getZ();
		row = bAttitude.getRow(1);
		attitude(1,0) = row.getX(); attitude(1,1) = row.getY(); attitude(1,2) = row.getZ();
		row = bAttitude.getRow(2);
		attitude(2,0) = row.getX(); attitude(2,1) = row.getY(); attitude(2,2) = row.getZ();

		EventPtr event(new ObjectMovedEvent(mBodyList[i]->GetID(), pos, attitude));

		if (mBodyList[i]->BodyType() == BDY_SOFT)
		{
			ObjectMovedEvent* objEv = dynamic_cast<ObjectMovedEvent*>(event.get());
			objEv->AddSoftBodyMesh(mesh);
		}

		pEventMng->QueueEvent(event);
	}

	if (mWorldType == PHYS_RIGID_AND_SOFT)
		mBulletSoftWorldInfo.m_sparsesdf.GarbageCollect();
}

}

