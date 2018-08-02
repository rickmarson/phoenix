/*

Shader Uniforms

*/

#include "shader_uniform.h"
#include "Math/matrix3x3.h"
#include "Math/matrix4x4.h"

using namespace std;

namespace NYX {


const char * ShaderUniform::aUniformEnumToName[NUM_UNIFORMS] = { "mMVP",
		"mMV",
		"mProj",
		"mModel",
		"mView",
		"vEye",
		"vColor",
		"vDiffuseColor",
		"vAmbientColor",
		"vSpecularColor",
		"sShininess",
		"vLightPos",
		"vLightDir",
		"vLightDiffuse",
		"vLightAmbient",
		"vLightSpecular",
// samplers, remember to always place samplers after all other uniforms
		"diffuseTexture0",
		"diffuseTexture1",
		"diffuseTexture2",
		"specularTexture0",
		"normalMap",
		"skyMap"
};


ShaderUniform::ShaderUniform(std::string name_in) :
	mName(name_in),
	mStorage(nullptr),
    mValue(nullptr)
{
	mType = UniformNameToEnum(name_in);
    mStorage = new ubyte[sizeof(Matrix4x4)];
    ASSERT(mStorage);
}

ShaderUniform::ShaderUniform(ShaderUniform&& in)
{
	mType = in.mType;
	mName = in.mName;
	mStorage = in.mStorage;
    mValue = in.mValue;
	in.mStorage = nullptr;
    in.mValue = nullptr;
}

ShaderUniform::~ShaderUniform( void )
{
    if (mValue)
    {
        if ( IsMat3() )
            ((Matrix3x3*)mValue)->~Matrix3x3();
        else if ( IsMat4() )
            ((Matrix4x4*)mValue)->~Matrix4x4();
        else if ( IsFloat() || IsVec3() || IsVec4() )
            delete [] (float*)mValue;
        else if ( IsInt() )
            delete [] (int*)mValue;
    }
    
    if ( mStorage )
        delete [] mStorage;
}
    
ShaderUniform& ShaderUniform::operator= ( ShaderUniform&& in )
{
    mType = in.mType;
    mName = in.mName;
    mStorage = in.mStorage;
    mValue = in.mValue;
    in.mStorage = nullptr;
    in.mValue = nullptr;
    return *this;
}

const float* ShaderUniform::AsMat3( void ) const
{
    ASSERT(mValue);
    
    return ((Matrix3x3*)mValue)->Data();
}
    
const float* ShaderUniform::AsMat4( void ) const
{
    ASSERT(mValue);
    
    return ((Matrix4x4*)mValue)->Data();
}
    
const float* ShaderUniform::AsVec3( void ) const
{
    ASSERT(mValue);
    
    return ((float*)mValue);
}
    
const float* ShaderUniform::AsVec4( void ) const
{
    ASSERT(mValue);
    
    return ((float*)mValue);
}

float ShaderUniform::AsFloat( void ) const
{
    ASSERT(mValue);
    
    return ((float*)mValue)[0];
}
    
int ShaderUniform::AsInt( void ) const
{
    ASSERT(mValue);
    
    return ((int*)mValue)[0];
}

void* ShaderUniform::AsRawPointer(void) const
{
	ASSERT(mValue);

	return (void*)mValue;
}
    
void ShaderUniform::SetValue(const Matrix3x3& mat3)
{
    if ( !mValue)
        mValue = (new(mStorage) Matrix3x3(mat3));
    else
        *((Matrix3x3*)mValue) = mat3;
}
    
void ShaderUniform::SetValue(const Matrix4x4& mat4)
{
    if ( !mValue )
        mValue = (new(mStorage) Matrix4x4(mat4));
    else
        *((Matrix4x4*)mValue) = mat4;
}
    
void ShaderUniform::SetValue(const float* vec, size_t length)
{
    if ( !mValue )
        mValue = (new(mStorage) float[length]);
    
    memcpy(mValue, vec, sizeof(float)*length);
}
    
void ShaderUniform::SetValue(float value_in)
{
    if ( !mValue )
        mValue = (new(mStorage) float[1]);
    
    ((float*)mValue)[0] = value_in;
}
    
void ShaderUniform::SetValue(int value_in)
{
    if ( !mValue )
        mValue = (new(mStorage) int[1]);
    
    ((int*)mValue)[0] = value_in;
}

    
bool ShaderUniform::IsMat3( void ) const
{
    return (GetLenght() == 9);
}
    
bool ShaderUniform::IsMat4( void ) const
{
    return (GetLenght() == 16);
}
    
bool ShaderUniform::IsVec3( void ) const
{
    return (GetLenght() == 3);
}
    
bool ShaderUniform::IsVec4( void ) const
{
    return (GetLenght() == 4);
}
    
bool ShaderUniform::IsFloat( void ) const
{
    return (GetLenght() == 1 && mType == UNF_SHININESS);
}
    
bool ShaderUniform::IsInt( void ) const
{
    switch (mType)
    {
        case UNF_DIFFUSE_TEX0:
        case UNF_DIFFUSE_TEX1:
        case UNF_DIFFUSE_TEX2:
        case UNF_SPECULAR_TEX0:
        case UNF_NORMAL_MAP:
        case UNF_ALPHA_MAP:
        case UNF_CUBE_MAP:
            return true;
            break;
        default:
            return false;
            break;
    }
}

eShaderUniform ShaderUniform::UniformNameToEnum(std::string uniformName)
{
	//convert uniform names read from the xml effect file to enum values, 
	//so that we can use a switch in methods that get called each frame (which is faster). 
	if (uniformName == "mMVP") //premultiplied modelviewprojection matrix
		return UNF_MVP_MATRIX;
	else if (uniformName == "mView") //view matrix
		return UNF_VIEW_MATRIX;
	else if (uniformName == "mProj") //projection matrix
		return UNF_PROJ_MATRIX;
	else if (uniformName == "mModel") //model matrix
		return UNF_MODEL_MATRIX;
	else if (uniformName == "mMV") //modelview matrix
		return UNF_MV_MATRIX;
	else if (uniformName == "vEye")
		return UNF_CAM_LOOKAT;
	else if (uniformName == "vColor")
		return UNF_COLOR;
	else if (uniformName == "vDiffuseColor")
		return UNF_DIFFUSE_COLOR;
	else if (uniformName == "vAmbientColor")
		return UNF_AMBIENT_COLOR;
	else if (uniformName == "vSpecularColor")
		return UNF_SPECULAR_COLOR;
	else if (uniformName == "sShininess")
		return UNF_SHININESS;
	else if (uniformName == "vLightPos")
		return UNF_LIGHT_POS;
	else if (uniformName == "vLightDir")
		return UNF_LIGHT_DIR;
	else if (uniformName == "vLightDiffuse")
		return UNF_LIGHT_DIFFUSE;
	else if (uniformName == "vLightAmbient")
		return UNF_LIGHT_AMBIENT;
	else if (uniformName == "vLightSpecular")
		return UNF_LIGHT_SPECULAR;
	else if (uniformName == "diffuseTexture0")
		return UNF_DIFFUSE_TEX0;
	else if (uniformName == "diffuseTexture1")
		return UNF_DIFFUSE_TEX1;
	else if (uniformName == "diffuseTexture2")
		return UNF_DIFFUSE_TEX2;
	else if (uniformName == "specularTexture0")
		return UNF_SPECULAR_TEX0;
	else if (uniformName == "normalMap")
		return UNF_NORMAL_MAP;
	else if (uniformName == "skyMap")
		return UNF_CUBE_MAP;
	else
		return UNF_INVALID;
}


std::string	ShaderUniform::UniformEnumToName(eShaderUniform uniform)
{
	uint uniform_int = static_cast<uint>(uniform);
	if (uniform_int >= NUM_UNIFORMS)
		return std::string();

	return std::string(aUniformEnumToName[uniform_int]);
}

size_t ShaderUniform::GetLenght(void) const
{
	switch (mType)
	{
	case UNF_MVP_MATRIX:
	case UNF_VIEW_MATRIX:
	case UNF_PROJ_MATRIX:
	case UNF_MODEL_MATRIX:
	case UNF_MV_MATRIX:
		return 16;
		break;
	case UNF_CAM_LOOKAT:
	case UNF_COLOR:
	case UNF_DIFFUSE_COLOR:
	case UNF_AMBIENT_COLOR:
	case UNF_SPECULAR_COLOR:
	case UNF_LIGHT_POS:
	case UNF_LIGHT_DIR:
	case UNF_LIGHT_DIFFUSE:
	case UNF_LIGHT_AMBIENT:
	case UNF_LIGHT_SPECULAR:
		return 4;
		break;
	case UNF_SHININESS:
		return 1;
		break;
    case UNF_DIFFUSE_TEX0:
    case UNF_DIFFUSE_TEX1:
    case UNF_DIFFUSE_TEX2:
    case UNF_SPECULAR_TEX0:
    case UNF_NORMAL_MAP:
    case UNF_ALPHA_MAP:
    case UNF_CUBE_MAP:
            return 1;
            break;
	default:
		return 0;
		break;
	}
}

size_t ShaderUniform::GetSizeInBytes(void) const
{
	switch (mType)
	{
	case UNF_MVP_MATRIX:
	case UNF_VIEW_MATRIX:
	case UNF_PROJ_MATRIX:
	case UNF_MODEL_MATRIX:
	case UNF_MV_MATRIX:
		return 16 * sizeof(float);
		break;
	case UNF_CAM_LOOKAT:
	case UNF_COLOR:
	case UNF_DIFFUSE_COLOR:
	case UNF_AMBIENT_COLOR:
	case UNF_SPECULAR_COLOR:
	case UNF_LIGHT_POS:
	case UNF_LIGHT_DIR:
	case UNF_LIGHT_DIFFUSE:
	case UNF_LIGHT_AMBIENT:
	case UNF_LIGHT_SPECULAR:
		return 4 * sizeof(float);
		break;
	case UNF_SHININESS:
		return 1 * sizeof(float);
		break;
    case UNF_DIFFUSE_TEX0:
    case UNF_DIFFUSE_TEX1:
    case UNF_DIFFUSE_TEX2:
    case UNF_SPECULAR_TEX0:
    case UNF_NORMAL_MAP:
    case UNF_ALPHA_MAP:
    case UNF_CUBE_MAP:
            return 1 * sizeof(int);
            break;
	default:
		return 0;
		break;
	}

}

}

