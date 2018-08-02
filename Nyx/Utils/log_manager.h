/*

Log Manager

*/


#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "singleton.h"

using namespace std;

namespace NYX {

/*
  The Log Manager is responsible for saving messages to an ASCII file.
  It is implemented as a singleton and is easily accessible from anywhere in the application.
  By default, the log file is created in the executable directory (win and linux) or in the app
  bundle directory (mac), and is overwritten every game session. The log file path can optionally be
  changed by passing a different argument to setLogFile().

  Messages are always logged with a leading time stamp, in the format: DAY/MONTH/YEAR HOUR/MIN/SEC.FRACTION

  If debug mode is off, only messages regarding the application status at startup and shutdown
  are logged. If debug mode is on, other optional messages may be logged.

  Note: the MainWindow will load the following options from from the config.xml file, so that they can be
  changed at runtime without the need to recompile:
        - log file name (with an optional  relative path)
        - debug mode on/off
  These options must be manually edited in the XML file as they are not exposed in the GUI.
*/
class NYX_EXPORT LogManager : public SingletonClass<LogManager>
{
public:

    LogManager(const std::string& logFileName = "log.txt");
    ~LogManager() = default;

    void LogMessage(const char* message);
    void LogMessage(const string& message);
    
private:
    
    std::string mLogFileName;
    
};

}

#endif // LOGMANAGER_H
