/*
 
 Shader Utilities, GLSL
 
 */

#include "glsl_shader_utils.h"
#include "Utils/file_manager.h"

#include "Glew/glew.h"
#include <map>

using namespace std;

#if DEBUG
#define DUMP_SHADER_SOURCE 1
#endif

const string GLSL_VERSION("#version 330");

const map<string, string> KEYWORDS = {  {"VertexOut", "gl_Position"},
                                        {"ArrayIn", "gl_in"} };
const string UNIFORM_PREFIX = "uniform";
const string INPUT_PREFIX = "in";
const string OUTPUT_PREFIX = "out";

namespace NYX {

    bool GLSLShaderHelper::LoadEffectFromFile(std::string shader_file)
    {
        bool ok = false;
        
        ok = ParseShaderFile(shader_file);
       
        ok |= ProcessVertexShaderSource();
        ok |= ProcessFragmentShaderSource();
        ok |= ProcessGeometryShaderSource();
        
        return ok;
    }
    
    bool GLSLShaderHelper::ProcessVertexShaderSource( void )
    {
        string temp_string = mVertexShaderSource;
        
        mVertexShaderSourceGLSL.clear();
        
        mVertexShaderSourceGLSL = GLSL_VERSION + LINE_END + LINE_END;
        
        // print inputs
        for ( auto input = mAttributeStrings.begin(); input != mAttributeStrings.end(); input++ )
        {
            mVertexShaderSourceGLSL += INPUT_PREFIX + " " + (*input) + LINE_END;
        }
        
        // print uniforms
        for ( auto uniform = mUniformStrings.begin(); uniform != mUniformStrings.end(); uniform++ )
        {
            mVertexShaderSourceGLSL += UNIFORM_PREFIX + " " + (*uniform) + LINE_END;
        }
        
        // print outputs
        for ( auto output = mVaryingStrings.begin(); output != mVaryingStrings.end(); output++ )
        {
            mVertexShaderSourceGLSL += OUTPUT_PREFIX + " " + (*output) + LINE_END;
        }
        
        // start program
        mVertexShaderSourceGLSL += "void main(void)" + LINE_END + "{" + LINE_END;
        
        // convert and add source
        for ( auto keyword = KEYWORDS.begin(); keyword != KEYWORDS.end(); keyword++ )
        {
            while ( temp_string.find((*keyword).first) != string::npos )
            {
                size_t pos = temp_string.find((*keyword).first);
                temp_string.replace( pos, (*keyword).first.length(), (*keyword).second );
            }
        }
        
        mVertexShaderSourceGLSL += temp_string;
        
        // end program
        mVertexShaderSourceGLSL += LINE_END + "}" + LINE_END;
        
#if DUMP_SHADER_SOURCE
        FileManager::GetInstance()->DumpDebugFile(mShaderName + "VS.txt", mVertexShaderSourceGLSL);
#endif
        
        return true;
    }
    
    bool GLSLShaderHelper::ProcessFragmentShaderSource( void )
    {
        string temp_string = mFragmentShaderSource;
        
        mFragmentShaderSourceGLSL.clear();
        
		mFragmentShaderSourceGLSL = GLSL_VERSION + LINE_END + LINE_END;
        
        // print inputs
        for ( auto input = mVaryingStrings.begin(); input != mVaryingStrings.end(); input++ )
        {
            mFragmentShaderSourceGLSL += INPUT_PREFIX + " " + (*input) + LINE_END;
        }
        
        // print uniforms
        for ( auto uniform = mUniformStrings.begin(); uniform != mUniformStrings.end(); uniform++ )
        {
            mFragmentShaderSourceGLSL += UNIFORM_PREFIX + " " + (*uniform) + LINE_END;
        }
        
        // print output (frag color)
        mFragmentShaderSourceGLSL += "out vec4 ColorOut;" + LINE_END + LINE_END;
        
        // start program
        mFragmentShaderSourceGLSL += "void main(void)" + LINE_END + "{" + LINE_END;
        
        // convert and print source
        for ( auto keyword = KEYWORDS.begin(); keyword != KEYWORDS.end(); keyword++ )
        {
            while ( temp_string.find((*keyword).first) != string::npos )
            {
                size_t pos = temp_string.find((*keyword).first);
                temp_string.replace( pos, (*keyword).first.length(), (*keyword).second );
            }
        }
        
        mFragmentShaderSourceGLSL += temp_string;
        
        // end program
        mFragmentShaderSourceGLSL += LINE_END + "}" + LINE_END;
        
#if DUMP_SHADER_SOURCE
        FileManager::GetInstance()->DumpDebugFile(mShaderName + "FS.txt", mFragmentShaderSourceGLSL);
#endif
        
        return true;
    }
    
    bool GLSLShaderHelper::ProcessGeometryShaderSource( void )
    {
        if ( mGeometryShaderSource.empty() )
            return true;
        
        string temp_string = mGeometryShaderSource;
        
        mGeometryShaderSourceGLSL.clear();
        
        mGeometryShaderSourceGLSL = GLSL_VERSION + LINE_END + LINE_END;
        
        mGeometryShaderSourceGLSL += "layout(points) in;" + LINE_END;
        
        mGeometryShaderSourceGLSL += "layout(triangle_strip, max_vertices = " + std::to_string(mGeometryShaderMaxVertexOut) + ") out;" + LINE_END;
        
        //print inputs
        for ( auto input = mVaryingStrings.begin(); input != mVaryingStrings.end(); input++ )
        {
            string temp_input = (*input);
            size_t end_pos = temp_input.find(";");
            temp_input = temp_input.substr(0, end_pos);
            mGeometryShaderSourceGLSL += INPUT_PREFIX + " " + temp_input + "[];" + LINE_END;
        }
        
        // print uniforms
        for ( auto uniform = mUniformStrings.begin(); uniform != mUniformStrings.end(); uniform++ )
        {
            mGeometryShaderSourceGLSL += UNIFORM_PREFIX + " " + (*uniform) + LINE_END;
        }
        
        // start program
        mGeometryShaderSourceGLSL += "void main(void)" + LINE_END + "{" + LINE_END;
        
        // convert and add source
        for ( auto keyword = KEYWORDS.begin(); keyword != KEYWORDS.end(); keyword++ )
        {
            while ( temp_string.find((*keyword).first) != string::npos )
            {
                size_t pos = temp_string.find((*keyword).first);
                temp_string.replace( pos, (*keyword).first.length(), (*keyword).second );
            }
        }
        
        mGeometryShaderSourceGLSL += temp_string;
        
        // end program
        mGeometryShaderSourceGLSL += LINE_END + "}" + LINE_END;
        
#if DUMP_SHADER_SOURCE
        FileManager::GetInstance()->DumpDebugFile(mShaderName + "GS.txt", mGeometryShaderSourceGLSL);
#endif        
        return true;
    }

}
