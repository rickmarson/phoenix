/*
 
 OpenGL Vertex Array Object
 
 */

#include "gl_vertex_array_object.h"
#include "Renderer/ogl/glsl_effect.h"
#include "glew.h"
#include "gl_utils.h"

namespace NYX {
 
    GLVertexArrayObject::~GLVertexArrayObject( void )
    {
        glDeleteVertexArrays(1, &mGLid);
    }
    
    void GLVertexArrayObject::Create( void )
    {
        glGenVertexArrays(1, &mGLid);
    }
    
    void GLVertexArrayObject::Bind( void )
    {
        glBindVertexArray(mGLid);
    }
    
    void GLVertexArrayObject::Unbind( void )
    {
        glBindVertexArray(0);
    }
    
    void GLVertexArrayObject::EnableVertexAttribute( Effect* effect, std::string attrib_name,  size_t attrib_size, size_t vertex_size, size_t offset )
    {
        GLint attrib_loc = ((GLSLEffect*)effect)->GetAttributeLocation(attrib_name);
        
        glEnableVertexAttribArray(attrib_loc);
        glVertexAttribPointer(attrib_loc, attrib_size, GL_FLOAT, GL_FALSE, vertex_size, (void*)offset);
        
        CheckGLError();
    }
    
    void GLVertexArrayObject::DisableAllAttributes( void )
    {
        GLint max_attribs;
        glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &max_attribs );
        
        for ( GLint i = 0; i < max_attribs; i++ )
            glDisableVertexAttribArray( i );
    }
    
}
