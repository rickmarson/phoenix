/*

Material

*/

#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include <iostream>
#include <sstream>

namespace NYX {

class Model;
class SceneNode;
class MeshNode;
class Texture;
class Effect;

class NYX_EXPORT Material
{
	friend class Model;
	friend class SceneNode;
	friend class MeshNode;

public:

    enum E_MATERIAL_CHANNEL
    {
        E_DIFFUSE,
        E_SPECULAR,
        E_ALPHA,
        E_AMBIENT,
        E_BUMP
    };
    
	Material(std::string name = "") :
		mTexturesLoaded(false),
		mName(name)
	{}
	
	~Material() {}
	
	void SetColor(float red, float green, float blue, float alpha = 1.0);
	void SetAmbient(float red, float green, float blue, float alpha = 1.0);
	void SetDiffuse(float red, float green, float blue, float alpha = 1.0);
	void SetSpecular(float red, float green, float blue, float alpha = 1.0);
	void SetShininess(float shine);
	void SetAlpha(float alpha);
	void AddTexture(Texture* texture, E_MATERIAL_CHANNEL channel);

	void SetShaderProg(Effect* shader);
	Effect* ShaderProg();

	std::string Name();
	std::string GetShaderName();
	float* Color();
	float* Diffuse();
	float* Ambient();
	float* Specular();
	float Shininess();
	float Alpha();

	std::vector<Texture*>& GetDiffuseTexList();
	std::vector<Texture*>& GetSpecularTexList();
	std::vector<Texture*>& GetAlphaTexList();
	std::vector<Texture*>& GetBumpTexList();

	//*************
	//Utility function. Loads a single material from a memory (ASCII) stream, formatted as an .mtl file, with the single limitation that it supports
	//just one material. If more than a material is present, only the first one will be loaded. 
	//Modified from method Model::importMaterialsFromMemory(). 
	bool ImportSingleMaterialFromMemory(char *matlib);

private:

	std::string mName;
	std::string mShaderName;
	Effect* mShaderProgram;

	//pair texture_name/texture_id for each channel
	std::vector<Texture*> mDiffuseTextures;
	std::vector<Texture*> mSpecularTextures;
	std::vector<Texture*> mAlphaTextures;
	std::vector<Texture*> mBumpTextures;
	bool mTexturesLoaded; //might be useless

	float mColor[4]; //use in alternative to using separate channels
	float mAmbient[4];
	float mDiffuse[4];
	float mSpecular[4];
	float mShininess;
	float mAlpha;

};

inline void Material::SetColor(float red, float green, float blue, float alpha) {
		mColor[0] = red;
		mColor[1] = green;
		mColor[2] = blue;
		mColor[3] = alpha;
}
	
inline void Material::SetAmbient(float red, float green, float blue, float alpha) {
		mAmbient[0] = red;
		mAmbient[1] = green;
		mAmbient[2] = blue;
		mAmbient[3] = alpha;
}
	
inline void Material::SetDiffuse(float red, float green, float blue, float alpha) {
		mDiffuse[0] = red;
		mDiffuse[1] = green;
		mDiffuse[2] = blue;
		mDiffuse[3] = alpha;
}
	
inline void Material::SetSpecular(float red, float green, float blue, float alpha) {
		mSpecular[0] = red;
		mSpecular[1] = green;
		mSpecular[2] = blue;
		mSpecular[3] = alpha;
}

inline void Material::SetShininess(float shine) {mShininess = shine;}
inline void Material::SetAlpha(float alpha) {mAlpha = alpha;}

inline void Material::AddTexture(Texture* texture, E_MATERIAL_CHANNEL channel) {

	switch (channel)
	{
	case E_DIFFUSE:
		mDiffuseTextures.push_back(texture);
		break;
	case E_SPECULAR:
		mSpecularTextures.push_back(texture);
		break;
	case E_ALPHA:
		mAlphaTextures.push_back(texture);
		break;
	case E_BUMP:
		mBumpTextures.push_back(texture);
		break;
	case E_AMBIENT:
		break;
	default:
		break;
	}
}

inline void Material::SetShaderProg(Effect* shader) { mShaderProgram = shader; }
inline Effect* Material::ShaderProg() { return mShaderProgram; }

inline std::string Material::Name() { return mName; }
inline std::string Material::GetShaderName() { return mShaderName; }
inline float* Material::Color() { return mColor; }
inline float* Material::Diffuse() { return mDiffuse; }
inline float* Material::Ambient() { return mAmbient; }
inline float* Material::Specular() { return mSpecular; }
inline float Material::Shininess() { return mShininess; }
inline float Material::Alpha() { return mAlpha; }

inline std::vector<Texture*>& Material::GetDiffuseTexList() { return mDiffuseTextures; }
inline std::vector<Texture*>& Material::GetSpecularTexList() { return mSpecularTextures; }
inline std::vector<Texture*>& Material::GetAlphaTexList() { return mAlphaTextures; }
inline std::vector<Texture*>& Material::GetBumpTexList() { return mBumpTextures; }

}

#endif //MATERIAL_H