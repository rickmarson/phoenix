/*

Shader

*/

#include "effect.h"
#include "ogl/glsl_effect.h"
#if !PLATFORM_MAC && !DISABLE_D3D
#include "d3d/d3d11_effect.h"
#endif
#include "hash.h"

using namespace std;

namespace NYX {


Effect* Effect::CreateEffect(std::string name, eRenderer backend)
{
	Effect *fx = nullptr;

#if defined(__WIN32__) && !DISABLE_D3D 
	switch (backend)
	{
	case OPENGL:
	{
		fx = new GLSLEffect(name);
		break;
	}
	case D3D:
	{
		fx = new D3D11Effect(name);
	}
	default:
		break;
	}
#else
	fx = new GLSLEffect(name);
#endif

	return fx;

}

Effect::Effect(string name) :
	mName(name)
{
	mID = GenerateHash(name.c_str(), name.size() );
}

Effect::~Effect()
{

}

uint Effect::ProgramId()
{
    return mID;
}

}
