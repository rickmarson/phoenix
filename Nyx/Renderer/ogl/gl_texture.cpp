/*
 
 OpenGL Texture Class
 
 */

#include "gl_texture.h"
#include "gl_renderer.h"
#include "gl_utils.h"
#include "glew.h"

namespace NYX {
    
    const GLenum cubemap[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

    GLTexture::~GLTexture( void )
    {
        glDeleteTextures(1, &mGLTextureID);
    }
    
    void GLTexture::Create( uint width, uint height, TextureFormat format_in, const ubyte* pixels, bool mipmapped, uint msaa_samples )
    {
        // save info
        mTextureInfo.width = width;
        mTextureInfo.height = height;
        mTextureInfo.format = format_in;
        mTextureInfo.IsCubeMap = false;
        
        // sanity check
        if ( msaa_samples > 0 )
        {
            GLint max_msaa_samples = 0;
            glGetIntegerv( GL_MAX_SAMPLES, &max_msaa_samples );
            
            if ( msaa_samples > (GLuint)max_msaa_samples )
                msaa_samples = (GLuint)max_msaa_samples;
        }
        
        mTextureInfo.msaa_samples = msaa_samples;
        
        // disable mipmaps if multisample is on
        if ( msaa_samples > 0 && mipmapped )
            mipmapped = false;
        
        mTextureInfo.is_mipmapped = mipmapped;
        
        // create texture and general settings

        glGenTextures(1, &mGLTextureID);
        
        if ( msaa_samples > 0 )
            glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, mGLTextureID );
        else
            glBindTexture( GL_TEXTURE_2D, mGLTextureID );
        

        if ( format_in == TextureFormat::RGBA8 )
        {
            //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
            
            if ( mTextureInfo.is_mipmapped )
            {
                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            }
            else
            {
                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            }
            
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            
            if ( pixels && msaa_samples == 0 )
            {
                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
            }
            else if ( msaa_samples > 0 )
            {
                glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, msaa_samples, GL_RGBA8, width, height, false );
            }
            else
            {
                // black texture
                size_t mem_size = width*height*4;
                ubyte* blank = new ubyte[mem_size];
                memset(blank, 0, mem_size);
                
                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, blank );
                
                delete [] blank;
                
            }
        
            CheckGLError();
        }
        else if ( format_in == TextureFormat::DEPTH )
        {
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );

            // look at the SDL initialisation to get the depth component size.
            // TODO: should probably move this to a gloab config somewhere
            
            if ( msaa_samples == 0 )
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            }
            else
            {
                glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, msaa_samples, GL_DEPTH_COMPONENT, width, height, false );
            }
            
            CheckGLError();
        }
        else if ( format_in == TextureFormat::DEPTH_STENCIL )
        {
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
            
            // look at the SDL initialisation to get the depth & stencil components size.
            // TODO: should probably move this to a gloab config somewhere
            
            if ( msaa_samples == 0)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
            }
            else
            {
                glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, msaa_samples, GL_DEPTH_STENCIL, width, height, false );
            }
            
            CheckGLError();
        }
        else if ( IsCompressed() )
        {
            // compressed texture code
        }
            
        if ( mipmapped && !IsCompressed() )
        {
            glGenerateMipmap( GL_TEXTURE_2D );
            
            CheckGLError();
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void GLTexture::Update( const ubyte* pixels )
    {
        if ( mTextureInfo.format != TextureFormat::RGBA8 )
            return;
        
        glBindTexture( GL_TEXTURE_2D, mGLTextureID );
        
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mTextureInfo.width, mTextureInfo.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
        
        if ( mTextureInfo.is_mipmapped )
            glGenerateMipmap( GL_TEXTURE_2D );
        
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
    
    void GLTexture::BeginCubeMap( void )
    {
        mTextureInfo.IsCubeMap = true;
        mTextureInfo.msaa_samples = 0;
        mTextureInfo.is_mipmapped = false;
        
        glGenTextures(1, &mGLTextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mGLTextureID);
        
        // Set up texture maps
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        CheckGLError();
    }
    
    void GLTexture::AddCubeMapFace(uint face, uint width, uint height, ubyte* pixels)
    {
        // save info
        mTextureInfo.width = width;
        mTextureInfo.height = height;
        mTextureInfo.format = TextureFormat::RGBA8;
   
        glTexImage2D(cubemap[face], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        CheckGLError();
    }
    
    void GLTexture::EndCubeMap( void )
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        
        CheckGLError();
    }
    
    void GLTexture::Bind( void )
    {
        if ( mTextureInfo.msaa_samples > 0 )
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mGLTextureID);
        else if ( mTextureInfo.IsCubeMap )
            glBindTexture(GL_TEXTURE_CUBE_MAP, mGLTextureID);
        else
            glBindTexture(GL_TEXTURE_2D, mGLTextureID);
        
    }
    
    void GLTexture::Unbind( void )
    {
        if ( mTextureInfo.msaa_samples > 0 )
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        else if ( mTextureInfo.IsCubeMap )
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        else
            glBindTexture(GL_TEXTURE_2D, 0);
    }
    
}
