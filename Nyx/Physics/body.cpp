/*
 
 Generic Physics Body
 
 */

#include "body.h"
#include "Utils/log_manager.h"

using namespace std;

namespace NYX {

PhysicsBody::PhysicsBody(std::string name, uint id) :
	mBulletCollisionShape(NULL),
	mBulletConvexHull(NULL),
	mBulletMotionState(NULL),
	mMass(0.0),
	mFriction(0.0),
	mDamping(0.0),
	mAngularDamping(0.0),
	mIsInitialized(false),
	mName(name),
	mID(id),
	mHasMesh(false)
{
    

}

PhysicsBody::~PhysicsBody()
{
	delete mBulletCollisionShape;

	mBulletMotionState = NULL;
	mBulletCollisionShape = NULL;
}

void PhysicsBody::SetMesh(float* rawVertices, int totalVertices, int stride, int *indices)
{
	mBulletConvexHull = new btConvexHullShape(rawVertices, totalVertices, stride);
	if (mBulletConvexHull != NULL)
		mHasMesh = true;
}

void PhysicsBody::SetCollisionShape(eCollisionShape collisionShape, void* dim) 
{ 
	if (collisionShape != CS_MESH && dim == NULL)
	{
		LogManager::GetInstance()->LogMessage("Error! NULL pointer passed with collision shape other than CS_MESH!");
		return;
	}
	else if (collisionShape == CS_MESH && !mHasMesh)
	{
		LogManager::GetInstance()->LogMessage("Error! Attempted to create a mesh collision shape without initializing mesh data first!");
		return;
	}

	mCollisionShape = collisionShape; 

	switch (mCollisionShape)
	{
	case CS_PLANE:
		mBulletCollisionShape = new btStaticPlaneShape(btVector3(static_cast<float*>(dim)[0],
																static_cast<float*>(dim)[1],
																static_cast<float*>(dim)[2]),
														static_cast<float*>(dim)[3]);
		break;
	case CS_BOX:
		mBulletCollisionShape = new btBoxShape(btVector3(static_cast<float*>(dim)[0],
														static_cast<float*>(dim)[1],
														static_cast<float*>(dim)[2]));
		break;
	case CS_SPHERE:
		mBulletCollisionShape = new btSphereShape(static_cast<float*>(dim)[0]);
		break;
	case CS_MESH:
		mBulletCollisionShape = mBulletConvexHull;
		break;
	default:
		break;
	}
}

void PhysicsBody::SetMassAndInertia(float mass, Vector3 localInertia) 
{
	if (mBulletCollisionShape == NULL)
	{
		LogManager::GetInstance()->LogMessage("Error! A Bullet collision shape must be initialized first!");
		return;
	}

	mMass = mass;
	mIsDynamic = (mass != 0.f);
	mLocalInertia = localInertia;

	mBulletInertia.setValue(mLocalInertia.X(), mLocalInertia.Y(), mLocalInertia.Z());
	mBulletCollisionShape->calculateLocalInertia(mMass, mBulletInertia);
}

void PhysicsBody::SetInitialState(Vector3 origin, Matrix3x3 transform) 
{
	mPosition = origin;
	mTransform = transform;

	float *rotdata = transform.Data();
	btScalar xx(rotdata[0]); btScalar xy(rotdata[1]); btScalar xz(rotdata[2]);
	btScalar yx(rotdata[3]); btScalar yy(rotdata[4]); btScalar yz(rotdata[5]);
	btScalar zx(rotdata[6]); btScalar zy(rotdata[7]); btScalar zz(rotdata[8]);

	btVector3 bulletOrigin(mPosition.X(), mPosition.Y(), mPosition.Z());
	btMatrix3x3 bulletRotation(xx, xy, xz,
							yx, yy, yz,
							zx, zy, zz);
	
	mBulletTransform.setOrigin(bulletOrigin);
	mBulletTransform.setBasis(bulletRotation);
}

void PhysicsBody::SetVelocity(Vector3 velocity)
{
	mVelocity = velocity;
}

void PhysicsBody::SetFriction(float friction)
{
	mFriction = friction;
}

void PhysicsBody::SetDamping(float damping, float angDamping)
{
	mDamping = damping;
	mAngularDamping = angDamping;
}

}


