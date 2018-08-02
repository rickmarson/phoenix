/*

Shader Utilities, Generic code

*/

#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include "shader_uniform.h"
#include <list>
#include <sstream>

using namespace std;

namespace NYX {

class NYX_EXPORT ShaderHelper
{
public:

    ShaderHelper( const string& shader_name ) :
        mShaderName( shader_name )
    {}
    virtual ~ShaderHelper() = default;
    
    std::list<ShaderUniform>& GetShaderUniforms(void)		{ return mShaderUniformNames; }
    
protected:
	
    enum class SourceType
    {
        SRC_VS,
        SRC_GS,
        SRC_FS
    };
    
    std::string mShaderName;
    
	std::list<ShaderUniform> mShaderUniformNames;

    std::list<std::string> mUniformStrings;
    std::list<std::string> mAttributeStrings; // vertex data
    std::list<std::string> mVaryingStrings; // vertex shader output -> fragment shader input
    std::string mVertexShaderSource;
    std::string mFragmentShaderSource;
    std::string mGeometryShaderSource;
	uint mGeometryShaderMaxVertexOut = 4;
    
protected:
    
    bool ParseShaderFile(std::string shader_file);
    bool ParseBlock( stringstream& stream );
    bool ParseAttributes( stringstream& stream );
    bool ParseUniforms( stringstream& stream );
    bool ParseVaryings( stringstream& stream );
    bool ParseSource( stringstream& stream, SourceType src_type );
    void EndBlock( stringstream& stream );
    
    void TrimWhitespaces( stringstream& stream );
    void CleanupString( string& str );
    void RemoveComment( string& str );

};


}

#endif // SHADER_UTILS_H
