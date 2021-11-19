#ifndef CONFIG_H
#define CONFIG_H

/*

Global Config Options

*/

#ifdef __cplusplus
    #include <string>
    #include <assert.h>

    #if DEBUG
        #define ASSERT(x) assert(x)
    #else
        #define ASSERT(x)
    #endif

#endif

#if defined( __APPLE__ ) && defined( __MACH__ )

#include <SDL2/SDL_platform.h>
#define PLATFORM_MAC 1

#ifdef __cplusplus
const std::string LINE_END = "\n";
#endif

#else

#include <SDL_platform.h>

#endif

#ifdef __WIN32__

#define PLATFORM_WINDOWS 1

#define DISABLE_D3D 1

#ifdef __cplusplus
const std::string LINE_END = "\r\n";
#endif

    #ifdef _MSC_VER
		#ifndef __STATIC_BUILD__
			#ifdef nyx_EXPORTS
				#define  NYX_EXPORT __declspec(dllexport)
			#else
				#define  NYX_EXPORT __declspec(dllimport)
			#endif
		#else
			#define NYX_EXPORT
		#endif
	#else
		#define NYX_EXPORT
	#endif
#else
	#define NYX_EXPORT
#endif

//custom type definitions
typedef unsigned int uint; 
typedef unsigned char uint8;
typedef unsigned char ubyte;

#endif // CONFIG_H
