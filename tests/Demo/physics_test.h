/*

A Physics Test

*/

#ifndef PHYSICS_TEST_H
#define PHYSICS_TEST_H

#include "scene.h"

class PhysicsTest : public NYX::Scene
{
    typedef NYX::Scene super;
    
public:

	PhysicsTest(NYX::ApplicationPtr application);
	~PhysicsTest();

	virtual void HandleUIEvent(UIEvent &event) override;
};
    
#endif // PHYSICS_TEST_H
