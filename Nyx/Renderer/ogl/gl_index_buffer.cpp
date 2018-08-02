/*
 
 OpenGL Index Buffer
 
 */

#include "gl_index_buffer.h"
#include "Glew/glew.h"
#include "gl_utils.h"

namespace NYX {
 
    GLIndexBuffer::~GLIndexBuffer( void )
    {
        glDeleteBuffers(1, &mGLid);
    }
    
    void GLIndexBuffer::Create( size_t index_size, uint number_of_indices, void* index_buffer )
    {
        glGenBuffers(1, &mGLid);
        
        int mem_size = index_size*number_of_indices;
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGLid);
        
        if ( !index_buffer )
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size, nullptr, GL_STATIC_DRAW);
        }
        else
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size, index_buffer, GL_STATIC_DRAW);
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        CheckGLError();
    }
    
    void GLIndexBuffer::Bind( void )
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGLid);
    }
    
    void GLIndexBuffer::Unbind( void )
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    float* GLIndexBuffer::Lock()
    {
        float* bufferdata;
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGLid);
        bufferdata = (float*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
        
        CheckGLError();
        
        return bufferdata;
    }
    
    void GLIndexBuffer::Unlock()
    {
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        CheckGLError();
    }
    
    
}