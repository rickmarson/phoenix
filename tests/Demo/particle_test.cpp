/*
 
 A Particle Effects Test
 
 */

#include "particle_test.h"
#include "camera_tp.h"
#include "camera_fp.h"
#include "particle_node.h"

using namespace NYX;

ParticleFXTest::ParticleFXTest(ApplicationPtr application) :
	Scene(application)
{

}

ParticleFXTest::~ParticleFXTest()
{

}

void ParticleFXTest::HandleUIEvent(UIEvent &event)
{
     super::HandleUIEvent(event);
}
