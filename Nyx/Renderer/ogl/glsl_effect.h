/*

GLSL Shader Backend

*/

#ifndef GLSL_EFFECT_H
#define GLSL_EFFECT_H

#include "glew.h"

#if PLATFORM_MAC
#include <OpenGL/OpenGL.h>
#else
#include "SDL_opengl.h"
#endif

#include "../effect.h"
#include <map>
#include <vector>

using namespace std;

namespace NYX {

class NYX_EXPORT GLSLEffect : public Effect
{
public:

    GLSLEffect(std::string name);
    virtual ~GLSLEffect();

	GLint GetAttributeLocation(std::string attribName);

    virtual bool LoadEffectFromFile(std::string shaderName) override;
	virtual void LoadUniforms( void ) override;
	
	virtual void Begin( void ) override;
	virtual void End( void ) override;

    bool ValidateProgram( void );
    
private:

    GLuint mVertexShader = 0;
    GLuint mGeometryShader = 0;
    GLuint mFragmentShader = 0;
	GLuint mProgramID = 0; //prog name, compiled prog id

	bool CreateShader(GLenum shaderType, const char* source);
    bool CreateProgram(std::string progName);

};


}

#endif // GLSL_EFFECT_H
