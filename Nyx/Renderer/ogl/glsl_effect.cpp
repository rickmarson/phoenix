/*
 
 GLSL Shader Backend
 
 */

#include "glsl_effect.h"
#include "glsl_shader_utils.h"
#include "gl_utils.h"
#include "Cache/resource_cache.h"
#include "hash.h"

#include <sstream>
#include <streambuf>

using namespace std;

namespace NYX {

GLSLEffect::GLSLEffect(string name) :
	Effect(name)
{

}

GLSLEffect::~GLSLEffect()
{
	//should probably make sure that all related programs are cleaned from the gpu
}

bool GLSLEffect::CreateShader(GLenum shaderType, const char* source)
{
    bool ret = false;
    GLint compiled;
	GLuint tmpId;

    switch (shaderType)
    {
    case GL_VERTEX_SHADER:
        tmpId = glCreateShader(GL_VERTEX_SHADER);
        if (tmpId == 0)
        {
            LogManager::GetInstance()->LogMessage("Problem Creating Shaders!");
            ret = false;
        }
        glShaderSource( tmpId, 1, &source, NULL );
        glCompileShader( tmpId );
        glGetShaderiv( tmpId, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE)
        {
            LogManager::GetInstance()->LogMessage("Problem Compiling Shaders!");
			DumpProgramInfoLog(tmpId);
			ret = false;
        }
        else
            ret = true;

		mVertexShader = tmpId;
        break;
    case GL_GEOMETRY_SHADER:
        tmpId = glCreateShader(GL_GEOMETRY_SHADER_EXT);
        if (tmpId == 0)
        {
            LogManager::GetInstance()->LogMessage("Problem Creating Shaders!");
            ret = false;
        }
        glShaderSource( tmpId, 1, &source, NULL );
        glCompileShader( tmpId );
        glGetShaderiv( tmpId, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE)
        {
            LogManager::GetInstance()->LogMessage("Problem Compiling Shaders!");
			DumpProgramInfoLog(tmpId);
            ret = false;
        }
        else
            ret = true;
        
		mGeometryShader = tmpId;
        break;
    case GL_FRAGMENT_SHADER:
        tmpId = glCreateShader(GL_FRAGMENT_SHADER);
        if (tmpId == 0)
        {
            LogManager::GetInstance()->LogMessage("Problem Creating Shaders!");
            ret = false;
        }
        glShaderSource( tmpId, 1, &source, NULL );
        glCompileShader( tmpId );
        glGetShaderiv( tmpId, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE)
        {
            LogManager::GetInstance()->LogMessage("Problem Compiling Shaders!");
			DumpProgramInfoLog(tmpId);
            ret = false;
        }
        else
            ret = true;
       
		mFragmentShader = tmpId;
        break;
    }

    return ret;

}

bool GLSLEffect::CreateProgram(string progName)
{
    GLuint tmpProgId = glCreateProgram();

    if (tmpProgId == 0)
    {
        LogManager::GetInstance()->LogMessage("Problem Creating GLSL Program!");
        return false;
    }

    if ( mVertexShader != 0 )
		glAttachShader(tmpProgId, mVertexShader);
    else
    {
        LogManager::GetInstance()->LogMessage("Vertex Shader Missing! Failed to create program.");
        return false;
    }

    if ( mGeometryShader != 0 )
		glAttachShader(tmpProgId, mGeometryShader);

    if ( mFragmentShader != 0 )
		glAttachShader(tmpProgId, mFragmentShader);
    else
    {
        LogManager::GetInstance()->LogMessage("Fragment Shader Missing! Failed to create program.");
        return false;
    }

    glLinkProgram(tmpProgId);

    GLint linked;
    glGetProgramiv(tmpProgId, GL_LINK_STATUS, &linked);


    if (linked == GL_FALSE)
    {
        LogManager::GetInstance()->LogMessage("Failed to link program!");

		DumpProgramInfoLog(tmpProgId);

		return false;
    }

	mProgramID = tmpProgId;

    return true;
}

bool GLSLEffect::LoadEffectFromFile(string shaderName)
{
    string file_name = shaderName + ".shader";
    
    GLSLShaderHelper shaderHelper(shaderName);
    
    if ( shaderHelper.LoadEffectFromFile(file_name) )
    {
        bool ok = false;
        
        mShaderUniforms = std::move(shaderHelper.GetShaderUniforms());
        
        ok = CreateShader(GL_VERTEX_SHADER, shaderHelper.GetVertexShaderSource().c_str());
        ok |= CreateShader(GL_FRAGMENT_SHADER, shaderHelper.GetFragmentShaderSource().c_str());
        if ( !shaderHelper.GetGeometryShaderSource().empty() )
            ok |= CreateShader(GL_GEOMETRY_SHADER, shaderHelper.GetGeometryShaderSource().c_str());
        
        ok |= CreateProgram(shaderName);
        
        return ok;
    }
    else
    {
        return false;
    }
}

GLint GLSLEffect::GetAttributeLocation(std::string attribName)
{
	//enables a given attrtibute and returns its location.
	GLint loc = glGetAttribLocation(mProgramID, attribName.c_str());

	return loc;
}

void GLSLEffect::LoadUniforms( void )
{
    for (auto it = mShaderUniforms.begin(); it != mShaderUniforms.end(); ++it)
    {
        GLint loc = glGetUniformLocationARB(mProgramID, (*it).Name().c_str());
        
        switch ( (*it).Type() )
        {
            case UNF_DIFFUSE_TEX0:
            case UNF_DIFFUSE_TEX1:
            case UNF_DIFFUSE_TEX2:
            case UNF_SPECULAR_TEX0:
            case UNF_NORMAL_MAP:
            case UNF_ALPHA_MAP:
            case UNF_CUBE_MAP:
                glUniform1i( loc, (*it).AsInt() );
                break;
            default:
                switch ((*it).GetLenght())
                {
                    case 1:
                        glUniform1f(loc, (*it).AsFloat());
                        break;
                    case 2:
                        glUniform2fv(loc, 1, (*it).AsVec3()); // not used
                        break;
                    case 3:
                        glUniform3fv(loc, 1, (*it).AsVec3());
                        break;
                    case 4:
                        glUniform4fv(loc, 1, (*it).AsVec4());
                        break;
                    default:
                        glUniformMatrix4fv(loc, 1, GL_FALSE, (*it).AsMat4());
                        break;
                }
        }
        
    }
}

void GLSLEffect::Begin( void )
{
    ValidateProgram();
	glUseProgram(mProgramID);
}

void GLSLEffect::End( void )
{
	glUseProgram(0);
}
    
    
bool GLSLEffect::ValidateProgram( void )
{
#if DEBUG
    glValidateProgram(mProgramID);
     
     GLint validated;
     glGetProgramiv(mProgramID, GL_VALIDATE_STATUS, &validated);
     
     if (validated == GL_FALSE)
     {
         LogManager::GetInstance()->LogMessage("Failed to validate program!");
     
		 DumpProgramInfoLog(mProgramID);
     
         return false;
     }
#endif
    
    return true;
}

}
