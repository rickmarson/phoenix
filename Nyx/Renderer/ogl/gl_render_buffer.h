/*
 
 OpenGL Render Buffer Class
 
 */

#ifndef GL_RENDER_BUFFER_H
#define GL_RENDER_BUFFER_H

#include "render_buffer.h"


namespace NYX
{
    // Required Classes
    class GLRenderer;
    
    class NYX_EXPORT GLRenderBuffer : public RenderBuffer
    {
        friend class GLRenderer;
        
    public:
        
        virtual ~GLRenderBuffer( void );
        
        virtual void Create( StorageType storage, uint width, uint height, bool has_stencil = true, uint msaa = 0 ) override ;
        virtual void Bind( void ) override;
        virtual void Unbind( void ) override;
        virtual void Blit( void ) override;
        virtual const Texture* CopyToTexture( void ) override;
        virtual const ubyte* ReadPixels( void ) const override;
        virtual void ReleasePixels( void ) const override;
        
    protected:
        
        GLRenderBuffer( void ) = default;
        void CreateBuffer( void );
        void CreateMultisampleBuffer( void );
        void CreateTextureBuffer( void );
        
    protected:
       
        uint mGLFrameBufferID = 0;
        
        uint mGLColorBufferID = 0;
        uint mGLDepthBufferID = 0;
        
        uint mOldFrameBuffer = 0;
        int mOldViewport[4];
        mutable ubyte *temp_buffer = nullptr;
    };
}


#endif // GL_RENDER_BUFFER_H
