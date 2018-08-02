/*
 
 OpenGL Vertex Buffer
 
 */


#ifndef GL_VERTEX_BUFFER_H
#define GL_VERTEX_BUFFER_H

#include "vertex_buffer.h"

namespace NYX {
    
    // Required Classes
    class GLRenderer;
    class ParticleFX;
    
    class GLVertexBuffer : public VertexBuffer
    {
        friend class GLRenderer;
        friend class ParticleFX;
        
    public:
        virtual ~GLVertexBuffer( void );
        
        virtual void Create( size_t vertex_size, uint number_of_vertices, void* vertex_buffer = nullptr ) override;
        virtual int CreateCLBufferFromThis( cl_context ctx ) override;
        virtual void Bind( void ) override;
        virtual void Unbind( void ) override;
        virtual float* Lock( void ) override;
        virtual void Unlock( void ) override;
        virtual int LockCL( cl_command_queue queue ) override;
        virtual int UnlockCL( cl_command_queue queue ) override;
        virtual int UpdateCLBuffer( cl_command_queue queue, size_t size, void* src ) override;
        virtual void Draw( IRenderer::E_POLYGON_TYPE poly_type, uint polygon_count, uint start_index = 0, bool has_indices = true ) override;
        
    protected:
        GLVertexBuffer( void ) = default;
        
    protected:
        
        uint mGLid = 0;
        
    };
    
}

#endif // GL_VERTEX_BUFFER_H
