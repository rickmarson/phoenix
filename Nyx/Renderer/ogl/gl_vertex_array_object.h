/*
 
 OpenGL Vertex Array Object
 
 */


#ifndef GL_VERTEX_ARRAY_OBJECT_H
#define GL_VERTEX_ARRAY_OBJECT_H

#include "vertex_array_object.h"

namespace NYX {
    
    // Required Classes
    class GLRenderer;
    
    class GLVertexArrayObject : public VertexArrayObject
    {
        friend class GLRenderer;
        
    public:
        virtual ~GLVertexArrayObject( void );
        
        virtual void Create( void ) override;
        virtual void Bind( void ) override;
        virtual void Unbind( void ) override;
        virtual void EnableVertexAttribute( Effect* effect, std::string attrib_name,  size_t attrib_size, size_t vertex_size, size_t offset ) override;
        virtual void DisableAllAttributes( void ) override;
        
    protected:
        GLVertexArrayObject( void ) = default;
        
        uint mGLid = 0;
    };
    
}


#endif // GL_VERTEX_ARRAY_OBJECT_H
