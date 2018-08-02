
/*
 Resource cache based on PhysFS
 */

#ifndef RESCACHE_H
#define RESCACHE_H

#include "singleton.h"
#include "resource.h"
#include <map>

namespace NYX {

typedef std::shared_ptr<IResource> ResourcePtr;

class NYX_EXPORT ResourceCache : public SingletonClass<ResourceCache>
{
public:
    
    ResourceCache(IRenderer *curRenderer = nullptr);
    ~ResourceCache();

    void SetRenderer( IRenderer* curRenderer )                                  { mRenderer = curRenderer; }
    // resName: name of the resource file without path.
    bool RegisterSearchPath(string mountPoint, string searchPath);
    ResourcePtr RequestResource(string resName, eResourceType resType);
    ResourcePtr RequestFontResource( string fontName, int size );
    char* RequestSourceCode(string srcName); //used for both scripts and shaders
	ubyte* RequestBinary(string resName, int &size);
    bool UnloadResource(string resName);
    void UnloadAllResources();

private:

    // private types
    typedef map<string, ResourcePtr> ResMap;
    
    // members

    string mAppName;
    string mRunDirectory; // full path to the directory from which the executable is being run.
    list<string> mSearchPaths; // search directories and / or archives with relative paths

    ResMap mResources;
	
	IRenderer *mRenderer;
};

}

#endif // RESCACHE_H
