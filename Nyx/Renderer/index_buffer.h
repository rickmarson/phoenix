/*
 
 Base Index Buffer Class
 
 */


#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

namespace NYX {
    
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer( void ) {}
        
        virtual void Create( size_t index_size, uint number_of_indices, void* index_buffer = nullptr ) = 0;
        virtual void Bind( void ) = 0;
        virtual void Unbind( void ) = 0;
        virtual float* Lock() = 0;
        virtual void Unlock() = 0;
        
    protected:
        IndexBuffer( void ) = default;
    };
    
}

#endif // INDEX_BUFFER_H
