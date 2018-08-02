/*

Log Manager

*/


#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "singleton.h"

using namespace std;

namespace NYX {


class NYX_EXPORT FileManager : public SingletonClass<FileManager>
{
public:

    FileManager(const char* argv0);
    ~FileManager();

    bool RegisterSearchPath( const string& mount_point, const string& search_path );
    const string& GetWorkingDirectory( void )  const                              { return mWorkingDir; }
    const string& GetBaseWriteDirectory( void ) const                             { return mBaseWriteDir; }
   
    bool DumpDebugFile( const string& file_name , const string& debug_info );
    bool Write( const string& contents, const string& file_name, string rel_path = "" );
    bool WriteAppend( const string& contents, const string& file_name, string rel_path = "" );
    
private:
    
    string mAppName;
    string mWorkingDir;
    string mBaseWriteDir;

};

}

#endif // FILE_MANAGER_H
