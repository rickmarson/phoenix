/*

Open GL Utility Functions

*/
#include "gl_utils.h"
#include "glew.h"
#include "Utils/log_manager.h"

namespace NYX
{
    std::string DefaultErrorString( GLenum error_code )
    {
        switch ( error_code )
        {
            case GL_NO_ERROR:
                return "OpenGL Error: No error. Shouldn't be here.";
            case GL_INVALID_ENUM:
                return "OpenGL Error: Invalid Enum.";
            case GL_INVALID_VALUE:
                return "OpenGL Error: Invalid Value.";
            case GL_INVALID_OPERATION:
                return "OpenGL Error: Invalid Operation.";
            case GL_STACK_OVERFLOW:
                return "OpenGL Error: Stack Overflow.";
            case GL_STACK_UNDERFLOW:
                return "OpenGL Error: Stack Underflow.";
            case GL_OUT_OF_MEMORY:
                return "OpenGL Error: Out of memory.";
            default:
                return "OpenGL Error: Unknown error code.";
        }
    }
    
	void CheckGLError(void)
	{
#if DEBUG

		GLenum error_code = GL_NO_ERROR;

		do
		{
			error_code = glGetError();

			if (error_code != GL_NO_ERROR)
			{
				std::string error_string = (char*)glewGetErrorString(error_code);
                if ( error_string == "Unknown error" )
                {
                    error_string = DefaultErrorString( error_code );
                }
				LogManager::GetInstance()->LogMessage(error_string);
			}

		} while (error_code != GL_NO_ERROR);

#endif
	}
    
	void DumpProgramInfoLog( uint progId )
	{
		int length, infoLogLength = 0;
		glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &length);
		//visual studio does not support dynamic array allocation. in gcc this would have been GLchar infoLog[length].
		GLchar *infoLog = new GLchar[length];
		glGetProgramInfoLog(progId, length, &infoLogLength, infoLog);
		std::string message;
		message.assign(infoLog, infoLogLength);
		LogManager::GetInstance()->LogMessage(message);
		delete infoLog;
	}
}

