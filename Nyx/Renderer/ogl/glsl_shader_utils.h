/*

Shader Utilities, GLSL

*/

#ifndef GLSL_SHADER_UTILS_H
#define GLSL_SHADER_UTILS_H

#include "shader_utils.h"

using namespace std;

namespace NYX {

class NYX_EXPORT GLSLShaderHelper : public ShaderHelper
{
public:

    GLSLShaderHelper(const string& shader_name) :
        ShaderHelper(shader_name)
    {}
    virtual ~GLSLShaderHelper() = default;
    
    bool LoadEffectFromFile(std::string shader_file);

    const std::string& GetVertexShaderSource( void ) const      { return mVertexShaderSourceGLSL; }
    const std::string& GetFragmentShaderSource( void ) const    { return mFragmentShaderSourceGLSL; }
    const std::string& GetGeometryShaderSource( void ) const    { return mGeometryShaderSourceGLSL; }
    
private:

    bool ProcessVertexShaderSource( void );
    bool ProcessFragmentShaderSource( void );
    bool ProcessGeometryShaderSource( void );

private:
    
    std::string mVertexShaderSourceGLSL;
    std::string mFragmentShaderSourceGLSL;
    std::string mGeometryShaderSourceGLSL;
    
};


}

#endif // GLSL_EFFECT_H
