/*
 
 OpenGL Vertex Buffer
 
 */

#include "glew.h"

#if PLATFORM_MAC
#include <OpenCL/OpenCL.h>
#else
#define CL_TARGET_OPENCL_VERSION 200
#include <CL\opencl.h>
#endif

#include "gl_vertex_buffer.h"
#include "gl_utils.h"

namespace NYX {
    
    GLVertexBuffer::~GLVertexBuffer( void )
    {
        if ( mSharedCLBuffer )
            clReleaseMemObject(mSharedCLBuffer);
        
        glDeleteBuffers(1, &mGLid);
    }
    
    void GLVertexBuffer::Create( size_t vertex_size, uint number_of_vertices, void* vertex_buffer )
    {
        glGenBuffers(1, &mGLid);
        glBindBuffer(GL_ARRAY_BUFFER, mGLid);
        int mem_size = vertex_size*number_of_vertices;
        
        if ( !vertex_buffer )
        {
            // create an empty buffer
            glBufferData(GL_ARRAY_BUFFER, mem_size, nullptr, GL_DYNAMIC_DRAW);
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, mem_size, vertex_buffer, GL_DYNAMIC_DRAW);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        CheckGLError();
    }
    
    int GLVertexBuffer::CreateCLBufferFromThis( cl_context ctx )
    {
        int error_code = 0;
        mSharedCLBuffer = clCreateFromGLBuffer(ctx, CL_MEM_READ_WRITE, mGLid, &error_code);
        return error_code;
    }
    
    void GLVertexBuffer::Bind( void )
    {
        glBindBuffer(GL_ARRAY_BUFFER, mGLid);
    }
    
    void GLVertexBuffer::Unbind( void )
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    float* GLVertexBuffer::Lock()
    {
        float* bufferdata;
        
        glBindBuffer(GL_ARRAY_BUFFER, mGLid);
        bufferdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        
        CheckGLError();
        
        return bufferdata;
    }
    
    void GLVertexBuffer::Unlock()
    {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        CheckGLError();
    }
    
    int GLVertexBuffer::LockCL( cl_command_queue queue )
    {
        cl_int error = clEnqueueAcquireGLObjects(queue, 1, &mSharedCLBuffer, 0, NULL, NULL);
        return error;
    }
    
    int GLVertexBuffer::UnlockCL( cl_command_queue queue )
    {
        cl_int error = clEnqueueReleaseGLObjects(queue, 1, &mSharedCLBuffer, 0, NULL, NULL);
        return error;
    }
    
    int GLVertexBuffer::UpdateCLBuffer( cl_command_queue queue, size_t size, void* src )
    {
        cl_int error = clEnqueueWriteBuffer(queue, mSharedCLBuffer, CL_TRUE, 0, size, src, 0, NULL, NULL);
        return error;
    }
    
    void GLVertexBuffer::Draw( IRenderer::E_POLYGON_TYPE poly_type, uint polygon_count, uint start_index, bool has_indices )
    {
        if ( has_indices )
        {
            switch (poly_type)
            {
                case IRenderer::E_POINT:
                    glDrawElementsBaseVertex(GL_POINTS, polygon_count, GL_UNSIGNED_INT, 0, start_index);
                    break;
                case IRenderer::E_LINE:
                    glDrawElementsBaseVertex(GL_LINES, polygon_count*2, GL_UNSIGNED_INT, 0, start_index);
                    break;
                case IRenderer::E_TRIANGLE:
                    glDrawElementsBaseVertex(GL_TRIANGLES, polygon_count*3, GL_UNSIGNED_INT, 0, start_index);
                    break;
                case IRenderer::E_QUAD:
                    glDrawElementsBaseVertex(GL_QUADS, polygon_count*4, GL_UNSIGNED_INT, 0, start_index);
                    break;
                case IRenderer::E_TRIANGLE_STRIP:
                    glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, polygon_count, GL_UNSIGNED_INT, 0, start_index);
                    break;
                /*case IRenderer::E_POLYGON:
                    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->PolyCount()*mesh->CustomPolySize(), GL_UNSIGNED_INT, 0, mesh->StartIndex());*/
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch (poly_type)
            {
                case IRenderer::E_POINT:
                    glDrawArrays(GL_POINTS, 0, polygon_count);
                    break;
                case IRenderer::E_TRIANGLE:
                    glDrawArrays(GL_TRIANGLES, 0, polygon_count); //this is the number of vertices
                    break;
                case IRenderer::E_TRIANGLE_STRIP:
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, polygon_count);
                    break;
                default:
                    break;
            }
        }
        
        CheckGLError();
        
    }
    
}