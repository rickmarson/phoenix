/*
 
 Base Texture Class
 
*/

#ifndef TEXTURE_H
#define TEXTURE_H

namespace NYX
{
    class NYX_EXPORT Texture
    {
    public:
        
        enum class TextureFormat
        {
            RGBA8,
            DEPTH,
            DEPTH_STENCIL,
            DXT1,
            DXT3,
            DXT5
        };
        
        struct TextureInfo
        {
            uint width = 0;
            uint height = 0;
            uint msaa_samples = 0;
            TextureFormat format;
            bool is_mipmapped = false;
            bool IsCubeMap = false;
        };
        
        virtual ~Texture( void ) {}
        
        virtual void Create( uint width, uint height, TextureFormat format_in, const ubyte* pixels, bool mipmapped = false, uint msaa_samples = 0 ) = 0;
        virtual void Update( const ubyte* pixels ) = 0;
        virtual void BeginCubeMap( void ) = 0;
        virtual void AddCubeMapFace(uint face, uint w, uint h, ubyte* pixels) = 0;
        virtual void EndCubeMap( void ) = 0;
        virtual void Bind( void ) = 0;
        virtual void Unbind( void ) = 0;
        
        const TextureInfo& TextureInfo( void ) const                    { return mTextureInfo; }
        bool IsCompressed( void );
        
    protected:
        
        Texture( void ) = default;
        
        struct TextureInfo mTextureInfo;
       
    };
    
    
    inline bool Texture::IsCompressed( void )
    {
        switch (mTextureInfo.format)
        {
            case TextureFormat::DXT1:
            case TextureFormat::DXT3:
            case TextureFormat::DXT5:
                return true;
            default:
                return false;
        }
    }
    
}

#endif
