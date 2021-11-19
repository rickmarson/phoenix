/*
 
 OpenGL Texture Class
 
 */

#include "gl_render_buffer.h"
#include "gl_texture.h"
#include "gl_renderer.h"
#include "gl_utils.h"
#include "glew.h"

namespace NYX {

    GLRenderBuffer::~GLRenderBuffer( void )
    {
        if ( mGLColorBufferID > 0 )
            glDeleteRenderbuffers(1, &mGLColorBufferID);
        
        if ( mGLDepthBufferID > 0 )
            glDeleteRenderbuffers(1, &mGLDepthBufferID );
        
        glDeleteFramebuffers(1, &mGLFrameBufferID);
        
        if ( temp_buffer )
            delete [] temp_buffer;
    }
    
    void GLRenderBuffer::Create( StorageType storage, uint width, uint height, bool has_stencil, uint msaa )
    {
        // make sure we are not calling this on a buffer already initialised
        ASSERT( mGLFrameBufferID == 0 && mGLColorBufferID == 0 && mGLDepthBufferID == 0 );
        
        // copy buffer config
        mRenderBufferInfo.width = width;
        mRenderBufferInfo.height = height;
        mRenderBufferInfo.has_stencil = has_stencil;
        mRenderBufferInfo.storage_type = storage;
        
        // save current binding
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, (GLint*)&mOldFrameBuffer );
        
        // validate msaa value
        if ( msaa > 0 )
        {
            GLint max_msaa_samples = 0;
            glGetIntegerv( GL_MAX_SAMPLES, &max_msaa_samples );
            
            if ( msaa > (GLuint)max_msaa_samples )
                msaa = (GLuint)max_msaa_samples;
        }
        
        mRenderBufferInfo.msaa = msaa;
        
        // gen framebuffer
        glGenFramebuffers( 1, &mGLFrameBufferID );
        glBindFramebuffer( GL_FRAMEBUFFER, mGLFrameBufferID );

        CheckGLError();
        
        // create framebuffer storage
        if ( storage == StorageType::MULTISAMPLE_STORAGE && msaa > 0 )
            CreateMultisampleBuffer();
        else if ( storage == StorageType::TEXTURE_STORAGE )
            CreateTextureBuffer();
        else
            CreateBuffer();
        
        ASSERT( glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE );
        
        glBindFramebuffer( GL_FRAMEBUFFER, mOldFrameBuffer );
        
        mOldFrameBuffer = 0;
    }
    
    void GLRenderBuffer::Bind( void )
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&mOldFrameBuffer);
        glGetIntegerv(GL_VIEWPORT, (GLint*)mOldViewport);
        
        if ( mRenderBufferInfo.msaa )
        {
            glEnable( GL_MULTISAMPLE );
        }
        
        glBindFramebuffer( GL_FRAMEBUFFER, mGLFrameBufferID );
        glViewport(0, 0, mRenderBufferInfo.width, mRenderBufferInfo.height);
        
        // prepare buffer for rendering (avoids nasty artifacts)
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        CheckGLError();
    }
    
    void GLRenderBuffer::Unbind( void )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, mOldFrameBuffer );
        glViewport(mOldViewport[0], mOldViewport[1], mOldViewport[2], mOldViewport[3]);
        
        if ( mRenderBufferInfo.msaa )
        {
            glDisable( GL_MULTISAMPLE );
        }

        CheckGLError();
        
        // potentially generate mip maps on the color attachment
        
        mOldFrameBuffer = 0;
        memset(mOldViewport, 0, sizeof(mOldViewport));
    }
    
    const ubyte* GLRenderBuffer::ReadPixels( void ) const
    {
        temp_buffer = new ubyte[ mRenderBufferInfo.width * mRenderBufferInfo.height * 4 ];
        
        if ( !temp_buffer )
            return nullptr;
        
        GLint old_framebuffer;
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &old_framebuffer );
        
        glBindFramebuffer( GL_FRAMEBUFFER, mGLFrameBufferID );
        
        glReadPixels( 0, 0, mRenderBufferInfo.width, mRenderBufferInfo.height, GL_RGBA, GL_UNSIGNED_BYTE, temp_buffer );
        
        CheckGLError();
        
        glBindFramebuffer( GL_FRAMEBUFFER, old_framebuffer );
        
        return temp_buffer;
    }
    
    void GLRenderBuffer::ReleasePixels( void ) const
    {
        delete [] temp_buffer;
        temp_buffer = nullptr;
    }
    
    void GLRenderBuffer::Blit( void )
    {
        // copies the contents of this buffer to the window back buffer
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); // the window draw buffer
        glBindFramebuffer( GL_READ_FRAMEBUFFER, mGLFrameBufferID ); // read from this
        glDrawBuffer( GL_BACK );
        // assumes the dimensions of this buffer match the window buffer.
        glBlitFramebuffer( 0, 0, mRenderBufferInfo.width, mRenderBufferInfo.height, 0, 0, mRenderBufferInfo.width, mRenderBufferInfo.height, GL_COLOR_BUFFER_BIT, GL_NEAREST );
        
        // reset
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        CheckGLError();
        
    }
    
    const Texture* GLRenderBuffer::CopyToTexture( void )
    {
        // make sure this is not a buffer that is already rendering to texture
        ASSERT( mRenderBufferInfo.storage_type != StorageType::TEXTURE_STORAGE );
        
        const ubyte* pixels = ReadPixels();
        
        // if this is the first call, create a new texture, if not update the existing one
        if ( mColorTexture == nullptr )
        {
            mColorTexture->Create(mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::RGBA8, pixels, true );
        }
        else
        {
             mColorTexture->Update( pixels );
        }
        
        return mColorTexture;
    }
    
    void GLRenderBuffer::CreateBuffer( void )
    {
        // color attachment
        
        glGenRenderbuffers( 1, &mGLColorBufferID );
        glBindRenderbuffer( GL_RENDERBUFFER, mGLColorBufferID );
        
        glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, mRenderBufferInfo.width, mRenderBufferInfo.height );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mGLColorBufferID );
        
        CheckGLError();
        
        // depth & stencil attachments
        glGenRenderbuffers( 1, &mGLDepthBufferID );
        glBindRenderbuffer( GL_RENDERBUFFER, mGLDepthBufferID );
        
        if ( mRenderBufferInfo.has_stencil )
        {
            glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mRenderBufferInfo.width, mRenderBufferInfo.height );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mGLDepthBufferID );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mGLDepthBufferID );
        }
        else
        {
            glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mRenderBufferInfo.width, mRenderBufferInfo.height );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mGLDepthBufferID );
        }
        
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );
        
        CheckGLError();
    }
    
    void GLRenderBuffer::CreateMultisampleBuffer( void )
    {
        // color attachment
        
        glGenRenderbuffers( 1, &mGLColorBufferID );
        glBindRenderbuffer( GL_RENDERBUFFER, mGLColorBufferID );
        
        glRenderbufferStorageMultisample( GL_RENDERBUFFER, mRenderBufferInfo.msaa, GL_RGBA8, mRenderBufferInfo.width, mRenderBufferInfo.height );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mGLColorBufferID );
        
        CheckGLError();
        
        // depth & stencil attachments
        glGenRenderbuffers( 1, &mGLDepthBufferID );
        glBindRenderbuffer( GL_RENDERBUFFER, mGLDepthBufferID );
        
        if ( mRenderBufferInfo.has_stencil )
        {
            glRenderbufferStorageMultisample( GL_RENDERBUFFER, mRenderBufferInfo.msaa, GL_DEPTH24_STENCIL8, mRenderBufferInfo.width, mRenderBufferInfo.height );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mGLDepthBufferID );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mGLDepthBufferID );
        }
        else
        {
            glRenderbufferStorageMultisample( GL_RENDERBUFFER, mRenderBufferInfo.msaa, GL_DEPTH_COMPONENT24, mRenderBufferInfo.width, mRenderBufferInfo.height );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mGLDepthBufferID );
        }
        
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );
        
        CheckGLError();

    }
    
    void GLRenderBuffer::CreateTextureBuffer( void )
    {
        ASSERT( mColorTexture == nullptr && mDepthStencilTexture == nullptr );
        
        // color attachment
        mColorTexture = new GLTexture();
    
        if ( !mColorTexture )
            return;
        
        if ( mRenderBufferInfo.msaa > 0 )
        {
            mColorTexture->Create(mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::RGBA8, nullptr, false, mRenderBufferInfo.msaa );
            mColorTexture->Bind();
            
            // optional additional tex parameters
            
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ((GLTexture*)mColorTexture)->mGLTextureID, 0);
            
            CheckGLError();
            
            mColorTexture->Unbind();
        }
        else
        {
            mColorTexture->Create(mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::RGBA8, nullptr);
            mColorTexture->Bind();
        
            // optional additional tex parameters
        
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ((GLTexture*)mColorTexture)->mGLTextureID, 0);
        
            CheckGLError();
        
            mColorTexture->Unbind();
        }
        
        // depth and stencil attachments
        mDepthStencilTexture = new GLTexture();
        
        if ( !mDepthStencilTexture )
            return;
        
        if ( mRenderBufferInfo.msaa > 0 )
        {
            if ( mRenderBufferInfo.has_stencil )
            {
                mDepthStencilTexture->Create( mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::DEPTH_STENCIL, nullptr, false, mRenderBufferInfo.msaa );
            }
            else
            {
                mDepthStencilTexture->Create( mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::DEPTH, nullptr, false, mRenderBufferInfo.msaa );
            }
            
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, ((GLTexture*)mDepthStencilTexture)->mGLTextureID, 0);
            
            CheckGLError();
            
            if ( mRenderBufferInfo.has_stencil )
            {
                glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, ((GLTexture*)mDepthStencilTexture)->mGLTextureID, 0);
                
                CheckGLError();
            }
        }
        else
        {
            if ( mRenderBufferInfo.has_stencil )
            {
                mDepthStencilTexture->Create( mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::DEPTH_STENCIL, nullptr);
            }
            else
            {
                mDepthStencilTexture->Create( mRenderBufferInfo.width, mRenderBufferInfo.height, Texture::TextureFormat::DEPTH, nullptr);
            }
        
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ((GLTexture*)mDepthStencilTexture)->mGLTextureID, 0);
        
            CheckGLError();
        
            if ( mRenderBufferInfo.has_stencil )
            {
                glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ((GLTexture*)mDepthStencilTexture)->mGLTextureID, 0);
            
                CheckGLError();
            }
        }

    }
}
