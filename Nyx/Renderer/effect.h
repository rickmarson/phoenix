/*

Shader

*/

#ifndef EFFECT_H
#define EFFECT_H

#include "shader_uniform.h"
#include "irenderer.h"

#include <list>
#include <typeinfo>
#include <typeindex>

using namespace std;

namespace NYX {

class NYX_EXPORT Effect
{
public:

	static Effect* CreateEffect(std::string name, eRenderer backend);
    virtual ~Effect();

    uint ProgramId();
	const std::string& GetName(void) const						{ return mName; }

	std::list<ShaderUniform>& GetShaderUniforms(void)			{ return mShaderUniforms; }

    virtual bool LoadEffectFromFile(std::string shaderName) = 0;
	
	virtual void LoadUniforms( void ) = 0;

	virtual void Begin( void ) = 0;
	virtual void End( void ) = 0;

protected:

	uint			mID;
	std::string		mName;

	std::list<ShaderUniform> mShaderUniforms; 

protected:

	Effect(std::string name);
};


}

#endif // EFFECT_H
