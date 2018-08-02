#ifndef RIGIDBODY_H
#define RIGIDBODY_H

/*
 
 A Rigid Body
 
 */

#include "body.h"

namespace NYX {

/*
Wrapper for Bullet Rigid bodies
*/
class RigidBody : public PhysicsBody
{
public:

    RigidBody(std::string name, uint id);
    ~RigidBody();

	void Finilize();

	btRigidBody* GetBulletRigidBody();

	void CalculateTotalForce();

private:

	btRigidBody* mBulletRigidBody;
};

inline btRigidBody* RigidBody::GetBulletRigidBody() { return mBulletRigidBody; }

}

#endif // RIGIDBODY_H
