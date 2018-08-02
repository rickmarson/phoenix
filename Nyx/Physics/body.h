#ifndef BODY_H
#define BODY_H

/*

Generic Physics Body
 
*/

#include "iphysics_engine.h"
#include "Bullet/btBulletDynamicsCommon.h"

namespace NYX {

/*
Wrapper for Bullet Rigid bodies
*/
class PhysicsBody : public IPhysicsBody
{
public:

    PhysicsBody(std::string name, uint id);
    ~PhysicsBody();

	void SetCollisionShape(eCollisionShape collisionShape, void* dim = NULL);
	void SetMassAndInertia(float mass, Vector3 localInertia);
	void SetInitialState(Vector3 origin, Matrix3x3 transform); //origin and initial transform
	void SetVelocity(Vector3 velocity);
	void SetFriction(float friction);
	void SetDamping(float damping, float angDamping);
	virtual void SetMesh(float* rawVertices, int totalVertices, int stride, int* indices = NULL);

	uint GetID();
	std::string GetName();
	ePhysicsBody BodyType();

	eCollisionShape GetCollisionShape();
	Vector3 GetPosition();
	Matrix3x3 GetTransform();
	float GetMass();
	Vector3 GetLocalInertia();
	bool HasMesh();
	bool IsInitialized();
	bool IsDynamic();

	virtual	void Finilize() = 0;
	virtual void CalculateTotalForce() = 0;

protected:

	btCollisionShape* mBulletCollisionShape;
	btConvexHullShape* mBulletConvexHull;
	btDefaultMotionState* mBulletMotionState;
	btTransform mBulletTransform;
	btVector3 mBulletInertia;

	eCollisionShape mCollisionShape;
	float mMass;
	float mFriction;
	float mDamping;
	float mAngularDamping;
	Vector3 mLocalInertia;
	Vector3 mPosition;
	Vector3 mVelocity;
	Matrix3x3 mTransform;

	bool mIsDynamic;
	bool mIsInitialized;
	bool mHasMesh;
	std::string mName; //the name of the relative scene node
	uint mID; //the id of the relative scene node
	ePhysicsBody mBodyType;
};

inline uint PhysicsBody::GetID() { return mID; }
inline std::string PhysicsBody::GetName() { return mName; }
inline ePhysicsBody PhysicsBody::BodyType() { return mBodyType; }

inline eCollisionShape PhysicsBody::GetCollisionShape() { return mCollisionShape; }
inline Vector3 PhysicsBody::GetPosition() { return mPosition; }
inline Matrix3x3 PhysicsBody::GetTransform() { return mTransform; }
inline float PhysicsBody::GetMass() { return mMass; }
inline Vector3 PhysicsBody::GetLocalInertia() { return mLocalInertia; }
inline bool PhysicsBody::IsInitialized() { return mIsInitialized; }
inline bool PhysicsBody::IsDynamic() { return mIsDynamic; }
inline bool PhysicsBody::HasMesh() { return mHasMesh; }

}

#endif // BODY_H
