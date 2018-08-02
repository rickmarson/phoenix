/*

Resource Cache Iterfaces
 
*/

#ifndef I_RES_CACHE_NTERFACES_H
#define I_RES_CACHE_NTERFACES_H

// interface list
namespace NYX {

class IResHandle;
class IResource;

// resource types
enum eResourceType
{
    RES_SOUND = 2001,
    RES_TEXTURE,
    RES_TEXTURE_CUBE_MAP,
    RES_MODEL,
    RES_MATERIAL,
    RES_FONT
};

// Resources

class NYX_EXPORT IResource
{
public:
	virtual ~IResource() {}
	virtual eResourceType GetType() = 0;
	virtual bool Load(std::string resName) = 0;
	virtual bool Unload() = 0;
};


};

#endif // I_RES_CACHE_NTERFACES_H
