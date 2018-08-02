/*
 Resource cache based on PhysFS
 */

#include "resource_cache.h"
#include "Utils/file_manager.h"
#include "Utils/log_manager.h"
#include "PhysFS/physfs.h"

namespace NYX {

//---------------------------------------------------------------------------
// Resource Cache
//---------------------------------------------------------------------------

#if PLATFORM_MAC
	const std::string DIR_SEPARATOR = "/";
#elif PLATFORM_WINDOWS
	const std::string DIR_SEPARATOR = "\\";
#endif

ResourceCache::ResourceCache(IRenderer *curRenderer)
{
    mRunDirectory = FileManager::GetInstance()->GetWorkingDirectory();
    
	mRenderer = curRenderer;

    LogManager::GetInstance()->LogMessage("Resource Cache Initialized.");
}

ResourceCache::~ResourceCache()
{
    LogManager::GetInstance()->LogMessage("Resource Cache Destroyed.");
}

bool ResourceCache::RegisterSearchPath(string mountPoint, string searchPath)
{
	string fullPath;
	
#if PLATFORM_WINDOWS
	size_t pos = searchPath.find('/');
	searchPath = searchPath.replace(pos, 1, DIR_SEPARATOR);
#endif

	if ( (searchPath[1] != ':') || (searchPath[0] != '/'))
		fullPath = mRunDirectory + DIR_SEPARATOR + searchPath;
	else
		fullPath = searchPath;

    if ( !FileManager::GetInstance()->RegisterSearchPath(mountPoint, fullPath) )
	{
		string msg = string("Cache Error: Failed to mount search path: ") + searchPath;

		LogManager::GetInstance()->LogMessage(msg.c_str());

		return false;
	}
	else
	{
		mSearchPaths.push_back(searchPath);
		return true;
	}
}

ResourcePtr ResourceCache::RequestResource(string resName, eResourceType resType)
{
    ResourcePtr tempRes;
    
    // check to see if the requested reource has already been loaded
    // if it is return its handle without loading.
    ResMap::iterator resIter = mResources.find(resName);
    if ( resIter != mResources.end())
    {
    	tempRes = (*resIter).second;
    	return tempRes;
    }

    // if the requested resource is not already stored, load it, store it and return the handle.
    switch (resType)
    {
        case RES_MODEL:
        {
        	tempRes = ResourcePtr( new ObjResource() );
        	ObjResource* objRes = dynamic_cast<ObjResource*>(tempRes.get());
        	objRes->Load(resName);
            break;
        }
        case RES_TEXTURE:
        {
        	tempRes = ResourcePtr( new ImageResource(mRenderer, false) );
        	ImageResource* imageRes = dynamic_cast<ImageResource*>(tempRes.get());
        	imageRes->Load(resName);
            break;
        }
		case RES_TEXTURE_CUBE_MAP:
        {
        	tempRes = ResourcePtr( new ImageResource(mRenderer, true) );
        	ImageResource* imageRes = dynamic_cast<ImageResource*>(tempRes.get());
        	imageRes->Load(resName);
            break;
        }
		case RES_MATERIAL:
		{
			tempRes = ResourcePtr( new MaterialResource() );
        	MaterialResource* matRes = dynamic_cast<MaterialResource*>(tempRes.get());
        	matRes->Load(resName);
            break;
		}
        default:
            break;
    }
    
    mResources[resName] = tempRes;

    return tempRes;
}

ResourcePtr ResourceCache::RequestFontResource( string fontName, int size )
{
    ResourcePtr tempFont;
    
    // check to see if the requested reource has already been loaded
    // if it is return its handle without loading.
    ResMap::iterator resIter = mResources.find(fontName);
    if ( resIter != mResources.end() )
    {
        tempFont = (*resIter).second;
        
        if ( ((FontResource*)tempFont.get())->GetFont() &&  ((FontResource*)tempFont.get())->GetSize() == size )
            return tempFont;
    }
    
    tempFont = ResourcePtr( new FontResource() );
    FontResource* font = dynamic_cast<FontResource*>(tempFont.get());
    font->Load(fontName, size);
    
    mResources[fontName] = tempFont;
    
    return tempFont;
    
}

char* ResourceCache::RequestSourceCode(string srcName)
{
	PHYSFS_sint32 exist = (PHYSFS_sint32)PHYSFS_exists(srcName.c_str());

	char *buffer = NULL;

	if (exist == 0)
	{
		string msg = string("Resource Cache Error: Failed to load resource: ") + srcName + string(" . File is not present in the search paths.");

		LogManager::GetInstance()->LogMessage(msg.c_str());
		return NULL;
	}
	else
	{
		PHYSFS_file* f = PHYSFS_openRead(srcName.c_str());

		PHYSFS_sint32 f_size = (PHYSFS_sint32)PHYSFS_fileLength(f);

		if (f_size == -1)
		{
			string msg = string("Resource Cache Error: Size could not be determined for resource: ") + srcName;

			LogManager::GetInstance()->LogMessage(msg.c_str());

			PHYSFS_close(f);

			return NULL;

		}
		else
		{
			buffer = new char[f_size+1]; //extra space to append null-termination char

			PHYSFS_sint32 length_read = (PHYSFS_sint32)PHYSFS_read(f, buffer, 1, f_size);

			if (length_read < f_size)
			{
				string msg = string("Resource Cache Error: File was not entirely loaded. ") + srcName;

				LogManager::GetInstance()->LogMessage(msg.c_str());

				PHYSFS_close(f);

				return NULL;
			}
			else
			{
				PHYSFS_close(f);
				
				//null-terminate buffer
				buffer[length_read] = '\0';
			
				return buffer;
			}

		}

	}
}

ubyte* ResourceCache::RequestBinary(string resName, int &size)
{
	PHYSFS_sint32 exist = (PHYSFS_sint32)PHYSFS_exists(resName.c_str());

	ubyte *buffer = NULL;

	if (exist == 0)
	{
		string msg = string("Resource Cache Error: Failed to load resource: ") + resName + string(" . File is not present in the search paths.");

		LogManager::GetInstance()->LogMessage(msg.c_str());
		return NULL;
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

			return NULL;

		}
		else
		{
			buffer = new ubyte[f_size]; 

			PHYSFS_sint32 length_read = (PHYSFS_sint32)PHYSFS_read(f, buffer, 1, f_size);

			if (length_read < f_size)
			{
				string msg = string("Resource Cache Error: File was not entirely loaded. ") + resName;

				LogManager::GetInstance()->LogMessage(msg.c_str());

				PHYSFS_close(f);

				return NULL;
			}
			else
			{
				PHYSFS_close(f);

				size = length_read;
							
				return buffer;
			}

		}

	}
}

bool ResourceCache::UnloadResource(string resName)
{
	ResMap::iterator resIter = mResources.find(resName);

	if (resIter == mResources.end())
	{
		string msg = "Failed to unload resource: " + resName + " . File not found.";
		LogManager::GetInstance()->LogMessage(msg.c_str());

		return false;
	}
	else
	{
		(*resIter).second->Unload();
		mResources.erase(resIter);
		return true;
	}
}

void ResourceCache::UnloadAllResources()
{
	for (ResMap::iterator it = mResources.begin(); it != mResources.end(); ++it)
		(*it).second->Unload();

	mResources.clear();
}

}