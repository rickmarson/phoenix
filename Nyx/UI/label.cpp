/*
 
 A Simple Text Label
 
 */

#include "label.h"
#include "texture.h"
#include "vertex_buffer.h"
#include "vertex_array_object.h"
#include "Cache/resource_cache.h"

namespace NYX {
    
    WidgetPtr Label::Create( IRenderer* renderer )
    {
        WidgetPtr new_label = WidgetPtr( new Label( renderer ) );
        return new_label;
    }
    
    Label::Label( IRenderer* renderer ) :
        Widget( renderer )
    {
        mTextColor[0] = 240;
        mTextColor[1] = 240;
        mTextColor[2] = 240;
        mTextSize = 12;
    }
    
    Label::~Label( void )
    {
        delete mTextBuffer;
    }
    
    void Label::SetTextColor( ubyte r, ubyte g, ubyte b )
    {
        mTextColor[0] = r;
        mTextColor[1] = g;
        mTextColor[2] = b;
    }
    
    void Label::SetText( std::string text, std::string font_name, int text_size )
    {
        ResourcePtr fontRes = ResourceCache::GetInstance()->RequestFontResource( font_name, text_size );
        
        TTF_Font* font = ((FontResource*)fontRes.get())->GetFont();
        
        mText = text;
        mTextSize = text_size;
        
        SDL_Color font_color = {mTextColor[0], mTextColor[1], mTextColor[2], 255};
        SDL_Surface* text_image;
        
        text_image = TTF_RenderText_Blended( font, text.c_str(), font_color );
        
        width = text_image->w;
        height = text_image->h;
        
        // convert to a RGBA texture format
        SDL_Surface *rgba_text_buffer = SDL_CreateRGBSurface( 0, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff );
        
        SDL_BlitSurface( text_image, 0, rgba_text_buffer, 0 );
        
        mTextBuffer = mRenderer->CreateTexture();
        mTextBuffer->Create( text_image->w, text_image->h, Texture::TextureFormat::RGBA8, (ubyte*)rgba_text_buffer->pixels );
        
		SDL_FreeSurface( rgba_text_buffer );
		SDL_FreeSurface( text_image );
    }
    
    void Label::Draw( void )
    {
        if ( !mCanDraw )
        {
            InitBuffer();
        }
        
        mVertexArray->Bind();
        mVertexBuffer->Bind();
        
        mRenderer->UseShader(mShaderProgram);
        
        list<ShaderUniform>& uniforms = mRenderer->GetShaderUniforms();
        
        // firts pass : appearance (if any)
        if ( mAppearance )
        {
            mRenderer->BindTexture(mAppearance);
            
            // load uniforms
            int tex_stack_id = mRenderer->GetActiveTexStackDepth() - 1;
            (uniforms.front()).SetValue( tex_stack_id );
        
            mRenderer->LoadShaderUniforms();
        
            mVertexBuffer->Draw( IRenderer::E_TRIANGLE_STRIP, 4, 0, false );
        
            mRenderer->UnbindAllTextures();
        }
        
        // second pass: actual text
        if ( mTextBuffer )
        {
            mRenderer->BindTexture(mTextBuffer);
            
            // reload uniforms
            int tex_stack_id = mRenderer->GetActiveTexStackDepth() - 1;
            (uniforms.front()).SetValue( tex_stack_id );
            
            mRenderer->LoadShaderUniforms();
            
            mVertexBuffer->Draw( IRenderer::E_TRIANGLE_STRIP, 4, 0, false );
            
            mRenderer->UnbindAllTextures();
        }
        
        mRenderer->UseShader(nullptr);
        mVertexBuffer->Unbind();
        mVertexArray->Unbind();
    }
    
    void Label::InitBuffer( void )
    {
        // set up vertices
        int viewport_x, viewport_y, viewport_w, viewport_h;
        mRenderer->GetViewport(viewport_x, viewport_y, viewport_w, viewport_h);
        
        UIVertex quad[4];
        
        // normalise points
        // note: the image produced by SDL_ttf / SDL_Surface starts from the upper-left corner as most
        // image format do, but OpenGL/D3D start form the lower-left corner, so the UV coordinates need to
        // be flipped along V.
        quad[0].position[0] = (2.0*left - viewport_w) / (float)viewport_w;
        quad[0].position[1] = (2.0*(top - height) - viewport_h) / (float)viewport_h;
        quad[0].uv[0] = 0.0;
        quad[0].uv[1] = 1.0;
        
        quad[1].position[0] = (2.0*(left + width) - viewport_w) / (float)viewport_w;
        quad[1].position[1] = (2.0*(top - height) - viewport_h) / (float)viewport_h;
        quad[1].uv[0] = 1.0;
        quad[1].uv[1] = 1.0;
        
        quad[2].position[0] = (2.0*left - viewport_w) / (float)viewport_w;
        quad[2].position[1] = (2.0*top - viewport_h) / (float)viewport_h;
        quad[2].uv[0] = 0.0;
        quad[2].uv[1] = 0.0;
        
        quad[3].position[0] = (2.0*(left + width) - viewport_w) / (float)viewport_w;
        quad[3].position[1] = (2.0*top - viewport_h) / (float)viewport_h;
        quad[3].uv[0] = 1.0;
        quad[3].uv[1] = 0.0;
        
        mShaderProgram = mRenderer->CreateShaderProgram( "UIShader" ); // this actually creates the shader only once for all widgets.
        
        mVertexBuffer = mRenderer->CreateVertexBuffer();
        mVertexBuffer->Create( sizeof(UIVertex), 4, quad );
        
        mVertexArray = mRenderer->CreateVertexArrayObject();
        mVertexArray->Create();
        
        mVertexBuffer->Bind();
        mVertexArray->Bind();
        
        mVertexArray->EnableVertexAttribute(mShaderProgram, "vVertex", 2, sizeof(UIVertex), offsetof(struct UIVertex, position));
        mVertexArray->EnableVertexAttribute(mShaderProgram, "vTexCoord", 2, sizeof(UIVertex), offsetof(struct UIVertex, uv));
        
        mVertexArray->Unbind();
        mVertexBuffer->Unbind();
        
        mCanDraw = true;
    }
    
    bool Label::HandleUIEvent( UIEvent& event )
    {
        // Nothing to do for labels
        return false;
    }
}
