/*
 Resource types and relative handles
 */

#include "resource.h"
#include "stb/stb_image.h"
#include "Renderer/texture.h"
#include "Utils/log_manager.h"
#include "PhysFS/physfs.h"
#include "model.h"

//---------------------------------------------------------------------------
// Resources and Handles by type
//---------------------------------------------------------------------------

//*** Graphics / Textures
namespace NYX {

ubyte* ReadPixels(int &width, int &height, ubyte* buffer, size_t size)
{
    int bpp = 0;
    return stbi_load_from_memory(buffer, size, &width, &height, &bpp, STBI_rgb_alpha );
}

eResourceType  ImageResource::GetType()
{
	if (bIsCubeMap)
		return RES_TEXTURE_CUBE_MAP;
	else
		return RES_TEXTURE;
}

Texture* ImageResource::GetTexture()
{
    return mTexture;
}

bool ImageResource::Load(string resName)
{
	if (bIsCubeMap)
		return LoadCubeMap(resName);
	else
		return LoadTexture2D(resName);
}

bool ImageResource::LoadTexture2D(string resName)
{
	int exist = PHYSFS_exists(resName.c_str());

	if (exist == 0)
	{
		string msg = string("Resource Cache Error: Failed to load resource: ") + resName + string(" . File is not present in the search paths.");

		LogManager::GetInstance()->LogMessage(msg.c_str());
		return false;
	}
	else
	{
		PHYSFS_file* f = PHYSFS_openRead(resName.c_str());

		PHYSFS_sint32 f_size = (PHYSFS_sint32)PHYSFS_fileLength(f);

		if (f_size == -1)
		{
			string msg = string("Resource Cache Error: Size could not be determined for resource: ") + resName;

			LogManager::GetInstance()->LogMessage(msg.c_str());

			PHYSFS_close(f);

			return false;

		}
		else
		{
			ubyte *buffer = new ubyte[f_size];

			PHYSFS_sint32 length_read = (PHYSFS_sint32)PHYSFS_read(f, buffer, 1, f_size);

			if (length_read < f_size)
			{
				string msg = string("Resource Cache Error: File was not entirely loaded. ") + resName;

				LogManager::GetInstance()->LogMessage(msg.c_str());

				PHYSFS_close(f);

				delete buffer;

				return false;
			}
			else
			{
				PHYSFS_close(f);

				//Load Image and related OpenGL Texture.

				int w(0), h(0);
				//pointer to the image, once loaded
				ubyte* pixels = ReadPixels(w, h, buffer, f_size);
                mTexture = mRenderer->CreateTexture();
                
                if (mTexture == nullptr)
                {
                    string msg = string("Resource Cache Error: Could not load OpenGL Texture: ") + resName;
                    
                    LogManager::GetInstance()->LogMessage(msg.c_str());
                    
                    return false;
                    
                }
                
                mTexture->Create(w, h, Texture::TextureFormat::RGBA8, pixels);
				//delete pixels and unload freeimage img buffer.
				//pixels are linked to img so cannot unload it inside ReadPixels
                stbi_image_free(pixels);
				//the memory region pointed to by pixels was freed along with img.

                //note: buffer is deleted by renderer.
                string msg = "Texture Loaded: " + resName;
                LogManager::GetInstance()->LogMessage(msg.c_str());

                return true;
			}

		}

	}
}

bool ImageResource::LoadCubeMap(string resName)
{
	//generate the 6 texture names from resName.
	/*note 1: a specific convention is used in naming the textures:
		<base_name>_<axis>.extension
	 e.g. sky_negx.jpg, sky_posx.jpg, ...
	 note 2: since the file type cannot be known in advance, resName will be in the form:
		<base_name>.extension
	 e.g. sky.jpg
	 This avoids messing up the resource cache, but care must be taken in following the naming 
	 conventions.
	*/
	size_t dot_pos = resName.find_last_of(".");
    string baseName = resName.substr(0, dot_pos);
	string ext = resName.substr(dot_pos, resName.length()-1);
	static const char* axis[] = { "_negx", "_posx", "_negy", "_posy", "_negz", "_posz" };
	string textureNames[6]; 

	for (int i = 0; i < 6; i++)
		textureNames[i] = baseName + string(axis[i]) + ext;

    mTexture = mRenderer->CreateTexture();
    
    if (mTexture == 0)
    {
        string msg = string("Resource Cache Error: Could not create Cube Map: ") + resName;
        LogManager::GetInstance()->LogMessage(msg.c_str());
        return false;
    }
    
	mTexture->BeginCubeMap();

	for (int i = 0; i < 6; i++)
	{
		int exist = PHYSFS_exists(textureNames[i].c_str());

		if (exist == 0)
		{
			string msg = string("Resource Cache Error: Failed to load resource: ") + textureNames[i] + string(" . File is not present in the search paths.");
			LogManager::GetInstance()->LogMessage(msg.c_str());
			return false;
		}
		else
		{
			PHYSFS_file* f = PHYSFS_openRead(textureNames[i].c_str());
			PHYSFS_sint32 f_size = (PHYSFS_sint32)PHYSFS_fileLength(f);

			if (f_size == -1)
			{
				string msg = string("Resource Cache Error: Size could not be determined for resource: ") + textureNames[i];
				LogManager::GetInstance()->LogMessage(msg.c_str());
				PHYSFS_close(f);
				return false;
	
			}
			else
			{
				ubyte *buffer = new ubyte[f_size];

				PHYSFS_sint32 length_read = (PHYSFS_sint32)PHYSFS_read(f, buffer, 1, f_size);

				if (length_read < f_size)
				{
					string msg = string("Resource Cache Error: File was not entirely loaded. ") + textureNames[i];
					LogManager::GetInstance()->LogMessage(msg.c_str());
					PHYSFS_close(f);
					delete buffer;
					return false;
				}
				else
				{
					PHYSFS_close(f);
					//Load Image and related OpenGL Texture.
					int w(0), h(0);
					//pointer to the image, once loaded
					ubyte* pixels = ReadPixels(w, h, buffer, f_size);
					mTexture->AddCubeMapFace(i, w, h, pixels);
					//delete pixels and unload freeimage img buffer.
					//pixels are linked to img so cannot unload it inside ReadPixels
                    stbi_image_free(pixels);
					//the memory region pointed to by pixels was freed along with img.
				}
			}

		}
	}

	mTexture->EndCubeMap();

	string msg = "Cube Map Loaded: " + resName;
	LogManager::GetInstance()->LogMessage(msg.c_str());

	return true;
}

bool ImageResource::Unload()
{
	delete mTexture;

	mTexture = nullptr;

	return true;
}

//*** Graphics / Obj models

eResourceType ObjResource::GetType()
{
	return RES_MODEL;
}

ModelPtr ObjResource::GetModel()
{
	return mModel;
}

bool ObjResource::Load(string resName)
{
	PHYSFS_sint32 exist = (PHYSFS_sint32)PHYSFS_exists(resName.c_str());

	if (exist == 0)
	{
		string msg = string("Resource Cache Error: Failed to load resource: ") + resName + string(" . File is not present in the search paths.");

		LogManager::GetInstance()->LogMessage(msg.c_str());
		return false;
	}
	else
	{
		PHYSFS_file* f = PHYSFS_openRead(resName.c_str());

		PHYSFS_sint32 f_size = (PHYSFS_sint32)PHYSFS_fileLength(f);

		if (f_size == -1)
		{
			string msg = string("Resource Cache Error: Size could not be determined for resource: ") + resName;

			LogManager::GetInstance()->LogMessage(msg.c_str());

			PHYSFS_close(f);

			return false;

		}
		else
		{
			char *buffer = new char[f_size+1];

			PHYSFS_sint32 length_read = (PHYSFS_sint32)PHYSFS_read(f, buffer, 1, f_size);

			if (length_read < f_size)
			{
				string msg = string("Resource Cache Error: File was not entirely loaded. ") + resName;

				LogManager::GetInstance()->LogMessage(msg.c_str());

				PHYSFS_close(f);

				delete buffer;

				return false;
			}
			else
			{
				PHYSFS_close(f);

				//add string termination
				buffer[f_size] = '\0';
				//Load Wavefront OBJ model
                mModel = ModelPtr( new Model() );

				if (!mModel->importFromMemory(buffer))
				{
					string msg = string("Resource Cache Error: Could not load OBJ Model: ") + resName;

					LogManager::GetInstance()->LogMessage(msg.c_str());

					delete buffer;

					return false;

				}
				else
				{
					delete buffer;

					string msg = "OBJ Model Loaded: " + resName;
					LogManager::GetInstance()->LogMessage(msg.c_str());

					return true;

				}
			}

		}

	}
}

bool ObjResource::Unload()
{
	//nothing to be done, ModelOBJ cleans after itself
	return true;
}

//***** Standalone Materials

eResourceType MaterialResource::GetType()
{
	return RES_MATERIAL;
}

MaterialPtr MaterialResource::GetMaterial()
{
	return mMaterial;
}

bool MaterialResource::Load(string resName)
{
	PHYSFS_sint32 exist = (PHYSFS_sint32)PHYSFS_exists(resName.c_str());

	if (exist == 0)
	{
		string msg = string("Resource Cache Error: Failed to load resource: ") + resName + string(" . File is not present in the search paths.");

		LogManager::GetInstance()->LogMessage(msg.c_str());
		return false;
	}
	else
	{
		PHYSFS_file* f = PHYSFS_openRead(resName.c_str());

		PHYSFS_sint32 f_size = (PHYSFS_sint32)PHYSFS_fileLength(f);

		if (f_size == -1)
		{
			string msg = string("Resource Cache Error: Size could not be determined for resource: ") + resName;

			LogManager::GetInstance()->LogMessage(msg.c_str());

			PHYSFS_close(f);

			return false;

		}
		else
		{
			char *buffer = new char[f_size+1];

			PHYSFS_sint32 length_read = (PHYSFS_sint32)PHYSFS_read(f, buffer, 1, f_size);

			if (length_read < f_size)
			{
				string msg = string("Resource Cache Error: File was not entirely loaded. ") + resName;

				LogManager::GetInstance()->LogMessage(msg.c_str());

				PHYSFS_close(f);

				delete buffer;

				return false;
			}
			else
			{
				PHYSFS_close(f);

				//add string termination
				buffer[f_size] = '\0';
				//Load .mtl material with no .obj attached. 

                mMaterial = MaterialPtr( new Material() );

				if (!mMaterial->ImportSingleMaterialFromMemory(buffer))
				{
					string msg = string("Resource Cache Error: Could not load OBJ Model: ") + resName;

					LogManager::GetInstance()->LogMessage(msg.c_str());
					//buffer is deleted inside import function

					return false;

				}
				else
				{
					//buffer is deleted inside import function

					string msg = "Material Loaded: " + resName;
					LogManager::GetInstance()->LogMessage(msg.c_str());

					return true;

				}
			}

		}

	}
}

bool MaterialResource::Unload()
{
	//nothing to be done, Material cleans after itself
	return true;
}

// ********** Fonts
    
FontResource::~FontResource( void )
{
    if ( font )
    {
        TTF_CloseFont( font );
        font = nullptr;
    }
}
    
eResourceType FontResource::GetType( void )
{
    return RES_FONT;
}

TTF_Font* FontResource::GetFont( void )
{
    return font;
}

bool FontResource::Load( string font_name )
{
    return Load( font_name, size );
}
    
bool FontResource::Load( string font_name, int size )
{
    PHYSFS_sint32 exist = (PHYSFS_sint32)PHYSFS_exists(font_name.c_str());
    
    if (exist == 0)
    {
        string msg = string("Resource Cache Error: Failed to load resource: ") + font_name + string(" . File is not present in the search paths.");
        
        LogManager::GetInstance()->LogMessage(msg.c_str());
        return false;
    }
    else
    {
        const char* font_directory = PHYSFS_getRealDir(font_name.c_str());
        string full_font_name = string(font_directory) + PHYSFS_getDirSeparator() + font_name;
        font = TTF_OpenFont( full_font_name.c_str(), size );
        
        if ( font == nullptr )
        {
            LogManager::GetInstance()->LogMessage( string("TTF failed to open Font.") + font_name );
            return false;
        }
        
        this->size = size;
    }
    
    return true;
}

bool FontResource::Unload( void )
{
    if ( font )
    {
        TTF_CloseFont(font);
        font = nullptr;
        return true;
    }
    
    return false;
}
    
}