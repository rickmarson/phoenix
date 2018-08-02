/*
 
Shader Utilities, Generic code

*/

#include "shader_utils.h"
#include "Cache/resource_cache.h"
#include "Utils/log_manager.h"
#include <streambuf>
#include <set>

using namespace std;

namespace NYX {
    
    const set<string> types = { "float", "int", "vec2", "vec3", "vec4", "mat3", "mat4", "sampler2D" };
    const size_t MAX_BUFFER = 300;
    
    bool ShaderHelper::ParseShaderFile(std::string shader_file)
    {
        char *src = ResourceCache::GetInstance()->RequestSourceCode(shader_file);
        
        if ( !src )
            return false;
        
        string src_string(src);
        char buffer[MAX_BUFFER];
        string temp_string;
        stringstream stream(src_string, ios::in);
        
        while ( stream.good() )
        {
            stream.getline(buffer, MAX_BUFFER, ' ');
            temp_string = buffer;
            
            if ( temp_string == "begin" )
                ParseBlock(stream);
            else
            {
                string message = "Malformed Shader File: ";
                message += shader_file;
                LogManager::GetInstance()->LogMessage(message);
                return false;
            }
        }
        
        return true;
    }
    
    bool ShaderHelper::ParseBlock( stringstream& stream )
    {
        char buffer[MAX_BUFFER];
        string temp_string;
        
        stream.getline(buffer, MAX_BUFFER, LINE_END[0]);
        temp_string = buffer;
            
        if ( temp_string == "vertex" )
        {
            ParseAttributes(stream);
        }
        else if ( temp_string == "uniforms" )
        {
            ParseUniforms(stream);
        }
        else if ( temp_string == "vertex_outputs" )
        {
            ParseVaryings(stream);
        }
        else if ( temp_string == "vertex_shader" )
        {
            ParseSource(stream, SourceType::SRC_VS);
        }
        else if ( temp_string == "fragment_shader" )
        {
            ParseSource(stream, SourceType::SRC_FS);
        }
        else if ( temp_string.find("geometry_shader") != string::npos )
        {
            // read in max_vertex_out
			size_t pos = temp_string.find( "max_vertex_out=" );
			if ( pos != string::npos )
			{
				string max_vertex_out_str = temp_string.substr(pos + 15);
				mGeometryShaderMaxVertexOut = std::stoi(max_vertex_out_str);
			}
            ParseSource(stream, SourceType::SRC_GS);
        }
            
        return true;
    }
    
    bool ShaderHelper::ParseAttributes( stringstream& stream )
    {
        string temp_line;
        char buffer[MAX_BUFFER];
        
        while ( stream.good() )
        {
            TrimWhitespaces(stream);
            
            stream.getline(buffer, MAX_BUFFER, LINE_END[0]);
            temp_line = buffer;
            
            if ( temp_line == "end" )
            {
                EndBlock( stream );
                break;
            }
            else if ( temp_line.substr(0, 2) == "//" ) //ignore comment line
            {
                continue;
            }
            else if ( temp_line.length() > 1 ) // ignore empty lines
            {
                RemoveComment(temp_line);
                
                mAttributeStrings.push_back(temp_line);
            }
        }
        
        return true;
    }
    
    bool ShaderHelper::ParseUniforms( stringstream& stream )
    {
        string temp_line;
        char buffer[MAX_BUFFER];
        
        while ( stream.good() )
        {
            TrimWhitespaces(stream);
            
            stream.getline(buffer, MAX_BUFFER, LINE_END[0]);
            temp_line = buffer;
            
            if ( temp_line == "end" )
            {
                EndBlock(stream);
                break;
            }
            else if ( temp_line.substr(0, 2) == "//" ) //ignore comment line
            {
                continue;
            }
            else if ( temp_line.length() > 1 )
            {
                RemoveComment(temp_line);
            
                mUniformStrings.push_back(temp_line);
            
                for ( auto type = types.begin(); type != types.end(); type++ )
                {
                    size_t start_pos = temp_line.find( *type);
                
                    if ( start_pos != string::npos )
                    {
                        // take 1 white space into account
                        string uniform_name = temp_line.substr( start_pos + (*type).length() + 1, string::npos );
                    
                        CleanupString(uniform_name);
                        
                        ShaderUniform uniform(uniform_name);
                        mShaderUniformNames.push_back( std::move(uniform) );
                    
                        break;
                    }
                }
            }
        }

        return true;
    }
    
    bool ShaderHelper::ParseVaryings( stringstream& stream )
    {
        static const size_t max_buffer = 300;
        
        string temp_line;
        char buffer[max_buffer];
        
        TrimWhitespaces(stream);
        
        while ( stream.good() )
        {
            TrimWhitespaces(stream);
            
            stream.getline(buffer, max_buffer, LINE_END[0]);
            temp_line = buffer;
            
            if ( temp_line == "end" )
            {
                EndBlock(stream);
                break;
            }
            else if ( temp_line.substr(0, 2) == "//" ) //ignore comment line
            {
                continue;
            }
            else if ( temp_line.length() > 1 )
            {
                RemoveComment(temp_line);
                
                mVaryingStrings.push_back(temp_line);
            }
        }

        return true;
    }
    
    bool ShaderHelper::ParseSource( stringstream& stream, SourceType src_type )
    {
        static const size_t max_buffer = 300;
        
        string temp_line;
        string source;
        char buffer[max_buffer];
        
        while ( stream.good() )
        {
            stream.getline(buffer, MAX_BUFFER, LINE_END[0]);
			// don't want to trim all the whitespaces at the beginning of a line to preserve indentation, 
			// but on Windows we need to get rid of the '\n' that follows '\r' (it's usually taken care of in
			// TrimWhitespaces)
#if PLATFORM_WINDOWS
			if (stream.peek() == LINE_END[1])
				stream.ignore(1);
#endif

            temp_line = buffer;
            
            if ( temp_line == "end" )
            {
                EndBlock(stream);
                break;
            }
            else if ( temp_line.substr(0, 2) == "//" ) //ignore comment line
            {
                continue;
            }
            else
            {
                RemoveComment(temp_line);
                source += temp_line + LINE_END;
            }
        }

        switch ( src_type )
        {
            case SourceType::SRC_VS:
                mVertexShaderSource = source;
                break;
            case SourceType::SRC_FS:
                mFragmentShaderSource = source;
                break;
            case SourceType::SRC_GS:
                mGeometryShaderSource= source;
                break;
        }
        
        return true;
    }
    
    
    void ShaderHelper::TrimWhitespaces( stringstream& stream )
    {
        while ( stream.peek() == ' ' || stream.peek() == '\t' || stream.peek() == '\n' || stream.peek() == '\r' )
        {
            stream.ignore(1);
        }
    }
    
    void ShaderHelper::CleanupString(string& str)
    {
        // delete the ending ';'
        if ( str.find(';') )
        {
            str.replace( str.find(';'), 1, "" );
        }
        
        // trim trailing whitespaces
        while ( str.find( ' ' ) != string::npos )
        {
            str.replace( str.find(' '), 1, "" );
        }
    }
    
    void ShaderHelper::RemoveComment( string& str )
    {
        size_t pos = str.find("//");
        
        if ( pos != string::npos )
        {
            str = str.substr(0, pos);
        }
    }
    
    void ShaderHelper::EndBlock( stringstream& stream )
    {
		while (stream.peek() == '\n' || stream.peek() == '\r')
        {
            stream.ignore(1);
        }
    }
}
