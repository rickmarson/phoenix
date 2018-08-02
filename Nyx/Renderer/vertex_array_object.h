/*
 
 Base Vertex Array Object Class
 
 */


#ifndef VERTEX_ARRAY_OBJECT_H
#define VERTEX_ARRAY_OBJECT_H

namespace NYX {
    
    // Required Classes
    class Effect;
    
    class VertexArrayObject
    {
    public:
        virtual ~VertexArrayObject( void ) {}
        
        virtual void Create( void ) = 0;
        virtual void Bind( void ) = 0;
        virtual void Unbind( void ) = 0;
        virtual void EnableVertexAttribute( Effect* effect, std::string attrib_name, size_t attrib_size, size_t vertex_size, size_t offset ) = 0;
        virtual void DisableAllAttributes( void ) = 0;
        
    protected:
        VertexArrayObject( void ) = default;
    };
    
}

#endif //VERTEX_ARRAY_OBJECT_H

