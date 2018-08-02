/*

A Particle Effects Test

*/

#ifndef PARTICLE_FX_TEST_H
#define PARTICLE_FX_TEST_H

#include "scene.h"

class ParticleFXTest : public NYX::Scene
{
    typedef NYX::Scene super;
    
public:

	ParticleFXTest(NYX::ApplicationPtr application);
	~ParticleFXTest();

	virtual void HandleUIEvent(UIEvent &event) override;

};

#endif // PARTICLE_FX_TEST_H
