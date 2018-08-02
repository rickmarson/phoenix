/*
    Main Demo
*/


#include "config.h"
#include "application.h"
#include "space_scene.h"
#include "physics_test.h"
#include "particle_test.h"

using namespace NYX;

int main(int argc, char **argv)
{
	ApplicationPtr pApplication( new Application(argv[0]) ) ;

	if (argc > 1)
	{
		// process command line arguments
	}
    
    pApplication->RegisterCacheSearchPath("/", "Resources/Shaders");
    pApplication->RegisterCacheSearchPath("/", "Resources/Models");
    pApplication->RegisterCacheSearchPath("/", "Resources/Textures");
    pApplication->RegisterCacheSearchPath("/", "Resources/FXKernels");
    pApplication->RegisterCacheSearchPath("/", "Resources/Scripts");
    pApplication->RegisterCacheSearchPath("/", "Resources/Fonts");
    
    // Demo-only data
    pApplication->RegisterCacheSearchPath("/", "Resources/Data");
    
    pApplication->Initialise( "config" );
    
    std::string test_names[3];
    test_names[0] = "space scene";
    test_names[1] = "physics test";
    test_names[2] = "particle test";
    
    ScenePtr space_scene = ScenePtr( new SpaceScene(pApplication) );
    ScenePtr test_bullet = ScenePtr( new PhysicsTest(pApplication) );
    ScenePtr test_particles = ScenePtr( new ParticleFXTest(pApplication) );
    
    space_scene->SetupScene(test_names[0]);
    test_bullet->SetupScene(test_names[1]);
    test_particles->SetupScene(test_names[2]);
    
    pApplication->ShowWindow(true);
    
    pApplication->Run();

	return 0;
}
