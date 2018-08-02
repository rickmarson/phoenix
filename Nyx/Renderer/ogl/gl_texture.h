/*
 
OpenGL Texture Class
 
 */

#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "texture.h"


namespace NYX
{
    // Required Classes
    class GLRenderer;
    class GLRenderBuffer;
    
    class NYX_EXPORT GLTexture : public Texture
    {
        friend class GLRenderer;
        friend class GLRenderBuffer;

    public:
        
        virtual ~GLTexture( void );
        
        virtual void Create( uint width, uint height, TextureFormat format_in, const ubyte* pixels, bool mipmapped = false, uint msaa_samples = 0 ) override ;
        virtual void Update( const ubyte* pixels ) override;
        virtual void BeginCubeMap( void ) override;
        virtual void AddCubeMapFace(uint face, uint width, uint height, ubyte* pixels) override;
        virtual void EndCubeMap( void ) override;
        virtual void Bind( void ) override;
        virtual void Unbind( void ) override;
        
    protected:
        
        GLTexture( void ) = default;
        
    protected:
        
        uint mGLTextureID = 0;
    };
}


#endif // GL_TEXTURE_H
