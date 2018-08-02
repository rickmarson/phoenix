/*
 
OpenGL Index Buffer
 
 */

#ifndef GL_INDEX_BUFFER_H
#define GL_INDEX_BUFFER_H

#include "index_buffer.h"

namespace NYX {
    
    // Required Classes
    class GLRenderer;
    
    class GLIndexBuffer : public IndexBuffer
    {
        friend class GLRenderer;
        
    public:
        virtual ~GLIndexBuffer( void );
        
        virtual void Create( size_t index_size, uint number_of_indices, void* index_buffer = nullptr );
        virtual void Bind( void );
        virtual void Unbind( void );
        virtual float* Lock();
        virtual void Unlock();
        
    protected:
        GLIndexBuffer( void ) = default;
        
        uint mGLid = 0;
    };
    
}


#endif // GL_INDEX_BUFFER_H