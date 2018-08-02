/*
 
 Base Vertex Buffer Class
 
 */


#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "irenderer.h"

namespace NYX {
    
    // Required Classes and Types
    
    // forward declare CL types for shared buffers with OpenCL
    typedef struct _cl_context *        cl_context;
    typedef struct _cl_mem *            cl_mem;
    typedef struct _cl_command_queue *  cl_command_queue;
    
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer( void ) {}
        
        virtual void Create( size_t vertex_size, uint number_of_vertices, void* vertex_buffer = nullptr ) = 0;
        virtual int CreateCLBufferFromThis( cl_context ctx ) = 0;
        virtual void Bind( void ) = 0;
        virtual void Unbind( void ) = 0;
        virtual float* Lock( void ) = 0;
        virtual void Unlock( void ) = 0;
        virtual int LockCL( cl_command_queue queue ) = 0;
        virtual int UnlockCL( cl_command_queue queue ) = 0;
        virtual int UpdateCLBuffer( cl_command_queue queue, size_t size, void* src ) = 0;
        virtual void Draw( IRenderer::E_POLYGON_TYPE poly_type, uint polygon_count, uint start_index = 0, bool has_indices = true ) = 0;
        
        cl_mem* GetSharedCLBuffer( void )            { return &mSharedCLBuffer; }
        
    protected:
        VertexBuffer( void ) = default;
        
        // OpenCL buffer
        cl_mem mSharedCLBuffer = nullptr;
    };
    
}


#endif // VERTEX_BUFFER_H
