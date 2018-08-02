/*
 Resource types and relative handles
 */

#ifndef RESHANDLES_H
#define RESHANDLES_H

#include "iresources.h"
#include "irenderer.h"

#if PLATFORM_MAC
#include <SDL2_ttf/SDL_ttf.h>
#else
#include "SDL/SDL_ttf/SDL_ttf.h"
#endif

// ****** Textures

namespace NYX {

class Texture;
    
class NYX_EXPORT ImageResource : public IResource
{
public:

    ImageResource(IRenderer *rend, bool cubeMap) {
		mRenderer = rend;
		bIsCubeMap = cubeMap;
	}
	virtual ~ImageResource() {}

    eResourceType GetType();
	Texture* GetTexture();
	bool Load(string resName);
	
	bool Unload();

private:

	bool bIsCubeMap;
    Texture* mTexture = nullptr;
	IRenderer *mRenderer;
	bool LoadCubeMap(string resName);
	bool LoadTexture2D(string resName);
};

// ****** 3D models

class NYX_EXPORT ObjResource : public IResource
{
public:

    ObjResource() {}
	virtual ~ObjResource() {}

    eResourceType GetType();
	ModelPtr GetModel();
	bool Load(string resName);
	bool Unload();

private:

    ModelPtr mModel;
};

//********* standalone materials

class NYX_EXPORT MaterialResource : public IResource
{
public:

    MaterialResource() {}
	virtual ~MaterialResource() {}

    eResourceType GetType();
	MaterialPtr GetMaterial();
	bool Load(string resName);
	bool Unload();

private:

    MaterialPtr mMaterial;
};

//************ Fonts
    
class NYX_EXPORT FontResource : public IResource
{
public:
    
    FontResource( void ) = default;
    ~FontResource( void );
    
    eResourceType GetType( void );
    TTF_Font* GetFont( void );
    int GetSize( void )                         { return size; }
    bool Load( string font_name );
    bool Load( string font_name, int size );
    bool Unload( void );
    
private:
    
    TTF_Font *font = nullptr;
    int size;
};
    
}

#endif // RESHANDLES_H
