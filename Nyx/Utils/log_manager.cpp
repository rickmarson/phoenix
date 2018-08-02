/*

 Log Manager

*/

#include "log_manager.h"

#include "Utils/file_manager.h"
#include <ctime>

namespace NYX {

LogManager::LogManager( const string& logFileName )
{
    mLogFileName = logFileName;
    
    string msg = "Logger Started. Logging in ";
    msg += logFileName;
    
    FileManager::GetInstance()->Write( msg, logFileName );
}

void LogManager::LogMessage(const char* message)
{
    string formatted_message;
    
    time_t rawtime;

    time( &rawtime );

    string timetag( ctime( &rawtime ) );
    
    size_t pos = timetag.find("\n");
    timetag.replace(pos, 1, "");
    
    formatted_message = "[";
    formatted_message += timetag + "]\t";
    formatted_message += message;
    formatted_message += LINE_END;
 
    FileManager::GetInstance()->WriteAppend(formatted_message, mLogFileName);
}

void LogManager::LogMessage(const string& message)
{
    string formatted_message;

	time_t rawtime;

	time( &rawtime );

	string timetag( ctime( &rawtime ) );

	size_t pos = timetag.find("\n");
	timetag.replace(pos, 1, "");

    formatted_message = "[";
    formatted_message += timetag + "]\t";
    formatted_message += message;
    formatted_message += LINE_END;
    
    FileManager::GetInstance()->WriteAppend(formatted_message, mLogFileName);
}

}