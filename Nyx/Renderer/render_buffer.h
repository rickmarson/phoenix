/*
 
 Base Render Buffer Class
 
*/

#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "texture.h"

namespace NYX
{
    // it would be more correct to call this FrameBuffer
    class NYX_EXPORT RenderBuffer
    {
    public:
        
        enum class StorageType
        {
            BUFFER_STORAGE,
            MULTISAMPLE_STORAGE,
            TEXTURE_STORAGE
        };
        
        struct RenderBufferInfo
        {
            uint width = 0;
            uint height = 0;
            uint msaa = 0;
            bool has_stencil = false;
            bool is_multisampled = false;
            StorageType storage_type;
        };
        
        virtual ~RenderBuffer( void )
        {
            delete mColorTexture;
            delete mDepthStencilTexture;
        }
        
        virtual void Create( StorageType storage, uint width, uint height, bool has_stencil = true, uint msaa = 0 ) = 0;
        virtual void Bind( void ) = 0;
        virtual void Unbind( void ) = 0;
        virtual void Blit( void ) = 0;
        virtual const Texture* CopyToTexture( void ) = 0;
        virtual const ubyte* ReadPixels( void ) const = 0;
        virtual void ReleasePixels( void ) const = 0;
        
        const RenderBufferInfo& RenderBufferInfo( void ) const              { return mRenderBufferInfo; }
        Texture* GetColor( void ) const                                     { return mColorTexture; }
        Texture* GetDepthStencil( void ) const                              { return mDepthStencilTexture; }
        
    protected:
        
        RenderBuffer( void ) = default;
        
        struct RenderBufferInfo mRenderBufferInfo;
        Texture* mColorTexture = nullptr;
        Texture* mDepthStencilTexture = nullptr;
    };
    
}

#endif // RENDER_BUFFER_H
