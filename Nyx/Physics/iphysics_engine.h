/*

Iterfaces for Physics Engine and Physics Bodies

*/

#ifndef I_PHYSICS_ENGINE_H
#define I_PHYSICS_ENGINE_H

#include "vector3.h"
#include "Math/matrix3x3.h"
#include <memory>

// required classes
class btRigidBody;

// interface list
namespace NYX {

class IPhysicsBody;
class IPhysicsEngine;

typedef std::shared_ptr< IPhysicsBody > PhysicsBodyPtr;
typedef std::shared_ptr< IPhysicsEngine > PhysicsEnginePtr;

// Enums
enum ePhysicsWorld
{
    PHYS_RIGID_ONLY,
    PHYS_RIGID_AND_SOFT
};
    
enum ePhysicsBody
{
    BDY_RIGID,
    BDY_SOFT
};
    
//keep it simple for the moment
enum eCollisionShape
{
    CS_PLANE, //static objects only
    CS_BOX,
    CS_SPHERE,
    CS_MESH
};
    
// physics subsystem

class NYX_EXPORT IPhysicsBody
{
public:
	virtual ~IPhysicsBody() {}

	virtual uint GetID() = 0;
	virtual std::string GetName() = 0;
	virtual ePhysicsBody BodyType() = 0;

	virtual void SetCollisionShape(eCollisionShape collisionShape, void* dim) = 0;
	virtual void SetMassAndInertia(float mass, Vector3 localInertia) = 0;
	virtual void SetInitialState(Vector3 origin, Matrix3x3 transform) = 0; //origin and initial transform
	virtual void SetVelocity(Vector3 velocity) = 0;
	virtual void SetFriction(float friction) = 0;
	virtual void SetDamping(float damping, float angDamping) = 0;
	
	virtual void Finilize() = 0;
	virtual eCollisionShape GetCollisionShape() = 0;
	virtual Vector3 GetPosition() = 0;
	virtual Matrix3x3 GetTransform() = 0;
	virtual float GetMass() = 0;
	virtual Vector3 GetLocalInertia() = 0;
	virtual bool IsInitialized() = 0;
	virtual bool IsDynamic() = 0;

	virtual void CalculateTotalForce() = 0;
};

class NYX_EXPORT ISoftBody
{
public:
	virtual ~ISoftBody() {}
	//TODO
};

class NYX_EXPORT IPhysicsEngine
{
public:
    virtual ~IPhysicsEngine() {}

	virtual void InitPhysics(ePhysicsWorld ePhysics) = 0;

    /*Gravity is the only global force that is "visible" to the entire world, and is applied to every object.
	  Every other force is applied per-object.
	  Note, however, that while the main purpose of the "gravity" vector is to apply gravity, it can be used
	  to "smuggle" other forces in by passing the total sum of all forces instead of just gravity. In this way,
	  the total force will be applied to all objects.
	  Note also that this works well only for a "flat" world, i.e. shooters, rpgs. For space sims, the gravity must
	  be calculated and applied per object along with all other forces. 
	*/
    virtual void SetGravity(Vector3 gravity) = 0;

	virtual void AddBody(PhysicsBodyPtr body) = 0;

	virtual void StepSimulation(float timeStep) = 0;
};

};

#endif // I_PHYSICS_ENGINE_H
