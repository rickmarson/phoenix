/*

 Log Manager

*/

#include "file_manager.h"

#if PLATFORM_WINDOWS
#include <window.h>
#endif


#include "physfs.h"

namespace NYX {

FileManager::FileManager(const char *argv0)
{
#if PLATFORM_WINDOWS
	// This is a workaround for Visual Studio passing a path relative to the solution in argv0
	TCHAR buffer[256];
	DWORD const buffer_chars = sizeof( buffer ) / sizeof( buffer[0] );

	GetModuleFileName( NULL, buffer, buffer_chars );
	
	PHYSFS_init( buffer );

#else

	PHYSFS_init( argv0 );

#endif
    
    mBaseWriteDir = PHYSFS_getBaseDir();
    mWorkingDir = mBaseWriteDir;
    
#if PLATFORM_MAC
    // PHYSFS_getBaseDir on Mac returns the location of the app bundle, but the resources are actually inside it
    mAppName = argv0;
    size_t pos = mAppName.find(".app", 0);
    mAppName = mAppName.substr(0, pos+4);
    pos = mAppName.find_last_of("/");
    mAppName = mAppName.substr(pos+1, string::npos);
    mWorkingDir += mAppName + "/Contents";
#endif
    
    PHYSFS_setWriteDir(mBaseWriteDir.c_str());
}

FileManager::~FileManager()
{
    PHYSFS_deinit();
}

bool FileManager::RegisterSearchPath( const string& mount_point, const string& search_path )
{
    return PHYSFS_mount(search_path.c_str(), mount_point.c_str(), 1);
}

bool FileManager::Write( const string& contents, const string& file_name, string rel_path )
{
    string full_dir;
    if ( !rel_path.empty() )
    {
        full_dir += rel_path + "/";
        if ( PHYSFS_exists(full_dir.c_str()) == 0 )
        {
            if ( PHYSFS_mkdir(full_dir.c_str()) == 0 )
                return false;
        }
    }
    
    string full_path = full_dir + file_name;
    
    PHYSFS_File* file = PHYSFS_openWrite(full_path.c_str());
    
    if ( file )
    {
        PHYSFS_write(file, contents.c_str(), contents.size(), 1);
        
        PHYSFS_close(file);
        
        return true;
    }
    else
    {
        return false;
    }
    
}
    
bool FileManager::WriteAppend( const string& contents, const string& file_name, string rel_path )
{
    string full_dir;
    if ( !rel_path.empty() )
    {
        full_dir += rel_path + "/";
        if ( PHYSFS_exists(full_dir.c_str()) == 0 )
        {
            return false;
        }
    }
    
    string full_path = full_dir + file_name;
    
    PHYSFS_File* file = PHYSFS_openAppend(full_path.c_str());
    
    if ( file )
    {
        PHYSFS_write(file, contents.c_str(), contents.size(), 1);
        
        PHYSFS_close(file);
        
        return true;
    }
    else
        return false;
    
}

bool FileManager::DumpDebugFile( const string& file_name , const string& debug_info )
{
    string full_dir = "dumps/";
    if ( PHYSFS_exists(full_dir.c_str()) == 0 )
    {
        PHYSFS_mkdir(full_dir.c_str());
    }
    
    string full_path = full_dir + file_name;
    
    PHYSFS_File* file = PHYSFS_openWrite(full_path.c_str());
    
    if ( file )
    {
        PHYSFS_write(file, debug_info.c_str(), debug_info.size(), 1);
    
        PHYSFS_close(file);
        
        return true;
    }
    else
    {
        return false;
    }
}

}