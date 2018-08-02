/*
 
 Shader Uniforms
 
 */

#ifndef SHADER_UNIFORM_H
#define SHADER_UNIFORM_H

#include "log_manager.h"


using namespace std;

namespace NYX {
// Required Classes
class Matrix3x3;
class Matrix4x4;
    
    
enum eShaderUniform
{
	//matrices and camera properties
	UNF_MVP_MATRIX, //modelviewprojection 
	UNF_MV_MATRIX, //modelview
	UNF_PROJ_MATRIX, //projection
	UNF_MODEL_MATRIX, //model
	UNF_VIEW_MATRIX, //view (inverse of model)
	UNF_CAM_LOOKAT, //eye vector
	//material properties
	UNF_COLOR,
	UNF_DIFFUSE_COLOR,
	UNF_AMBIENT_COLOR,
	UNF_SPECULAR_COLOR,
	UNF_SHININESS,
	//light properties
	UNF_LIGHT_POS,
	UNF_LIGHT_DIR,
	UNF_LIGHT_DIFFUSE,
	UNF_LIGHT_AMBIENT,
	UNF_LIGHT_SPECULAR,
	//textures
	UNF_DIFFUSE_TEX0,
	UNF_DIFFUSE_TEX1,
	UNF_DIFFUSE_TEX2,
	UNF_SPECULAR_TEX0,
	UNF_NORMAL_MAP,
	UNF_ALPHA_MAP,
	UNF_CUBE_MAP,
	NUM_UNIFORMS,
	UNF_INVALID = 0xffff
};

class NYX_EXPORT ShaderUniform
{
public:

	ShaderUniform() = delete;
	ShaderUniform(std::string name_in);
	ShaderUniform(const ShaderUniform& in) = delete;
	ShaderUniform(ShaderUniform&& in);
    ~ShaderUniform( void );

    ShaderUniform& operator= ( const ShaderUniform& ) = delete;
    ShaderUniform& operator= ( ShaderUniform&& in );
    
	const std::string&	Name( void ) const			{ return mName;  }
	eShaderUniform		Type( void ) const			{ return mType;  }

	size_t				GetLenght(void) const;
	size_t				GetSizeInBytes(void) const;

    bool                IsMat3( void ) const;
    bool                IsMat4( void ) const;
    bool                IsVec3( void ) const;
    bool                IsVec4( void ) const;
    bool                IsFloat( void ) const;
    bool                IsInt( void ) const;
    
    const float*        AsMat3( void ) const;
    const float*        AsMat4( void ) const;
    const float*        AsVec3( void ) const;
    const float*        AsVec4( void ) const;
    float               AsFloat( void ) const;
    int                 AsInt( void ) const;
	void*				AsRawPointer( void ) const;
    
    void				SetValue(const Matrix3x3& mat3);
    void				SetValue(const Matrix4x4& mat4);
    void				SetValue(const float* vec, size_t length);
    void                SetValue(float value_in);
    void                SetValue(int value_in);

    
private:

	eShaderUniform	mType;
	std::string		mName;

    ubyte *mStorage;
    void *mValue;
	static const char * aUniformEnumToName[NUM_UNIFORMS];

private:

	static eShaderUniform	UniformNameToEnum(std::string uniformName);
	static std::string		UniformEnumToName(eShaderUniform uniform);
};

}

#endif // SHADER_UNIFORM_H
