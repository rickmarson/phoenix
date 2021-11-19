/*
 
 The UI Manager.
 
 */

#include "ui_manager.h"
#include "effect.h"
#include "render_buffer.h"
#include "vertex_buffer.h"
#include "vertex_array_object.h"
#include "texture.h"

#if PLATFORM_MAC
#include <SDL2_ttf/SDL_ttf.h>
#else
#include "SDL_ttf.h"
#endif

namespace NYX {
    
    UIManager::~UIManager()
    {
        if ( mUIbuffer )
            delete mUIbuffer;
        
        // should have the renderer unload effects
        
        mRenderer = nullptr;
    }
    
    void UIManager::Initialise( IRenderer* renderer )
    {
        default_font = "Capture_it.ttf"; //"OpenSans-Regular.ttf";

        ASSERT( renderer );
        mRenderer = renderer;
        
        mShaderProgram = mRenderer->CreateShaderProgram( "UIShader" );
        
        int viewport_x, viewport_y, viewport_w, viewport_h;
        mRenderer->GetViewport(viewport_x, viewport_y, viewport_w, viewport_h);
        
        mUIbuffer = mRenderer->CreateRenderBuffer();
        mUIbuffer->Create( RenderBuffer::StorageType::TEXTURE_STORAGE, viewport_w, viewport_h );
        
        // create a quad to render the buffer color attachment texture with
        UIVertex quad[4];
        
        // normalise points
        // lower-left
        quad[0].position[0] = -1.0;
        quad[0].position[1] = -1.0;
        quad[0].uv[0] = 0.0;
        quad[0].uv[1] = 0.0;
        
        //lower-right
        quad[1].position[0] = 1.0;
        quad[1].position[1] = -1.0;
        quad[1].uv[0] = 1.0;
        quad[1].uv[1] = 0.0;
        
        // upper-left
        quad[2].position[0] = -1.0;
        quad[2].position[1] = 1.0;
        quad[2].uv[0] = 0.0;
        quad[2].uv[1] = 1.0;
        
        // upper-right
        quad[3].position[0] = 1.0;
        quad[3].position[1] = 1.0;
        quad[3].uv[0] = 1.0;
        quad[3].uv[1] = 1.0;
        
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

    }
    
    void UIManager::AddWidget( WidgetPtr widget )
    {
        mWidgets.push_back(widget);
    }
    
    void UIManager::UpdateUI( void )
    {
        if ( mDirty )
        {
            mRenderer->DisableDepthTest();
            mUIbuffer->Bind();
        
            for ( auto widget = mWidgets.begin(); widget != mWidgets.end(); widget++ )
            {
                (*widget)->Draw();
            }
        
            mUIbuffer->Unbind();
            mDirty = false;
        }
    }
    
    void UIManager::Render( void )
    {
        mRenderer->DisableDepthTest();
        mRenderer->EnableAlphaBlending();
        
        mVertexArray->Bind();
        mVertexBuffer->Bind();
        
        mRenderer->UseShader(mShaderProgram);
        
        list<ShaderUniform>& uniforms = mRenderer->GetShaderUniforms();
        
        mRenderer->BindTexture( mUIbuffer->GetColor() );
        
        // load uniforms
        int tex_stack_id = mRenderer->GetActiveTexStackDepth() - 1;
        (uniforms.front()).SetValue( tex_stack_id );
            
        mRenderer->LoadShaderUniforms();
            
        mVertexBuffer->Draw( IRenderer::E_TRIANGLE_STRIP, 4, 0, false );
            
        mRenderer->UnbindAllTextures();
    
        mRenderer->UseShader(nullptr);
        mVertexBuffer->Unbind();
        mVertexArray->Unbind();

        mRenderer->DisableAlphaBlending();
    }
    
    bool UIManager::HandleUIEvent( UIEvent& event )
    {
        switch ( event.id )
        {
            case UI_EVENT_ID::UI_MOUSE_MOVED:
            case UI_EVENT_ID::UI_MOUSE_BUTTON_CLICKED:
            {
                uint x = (event.id == UI_EVENT_ID::UI_MOUSE_MOVED) ? ((UIMouseMovedEvent&)event).x : ((UIMouseButtonEvent&)event).x;
                uint y = (event.id == UI_EVENT_ID::UI_MOUSE_MOVED) ? ((UIMouseMovedEvent&)event).y : ((UIMouseButtonEvent&)event).y;
                
                for ( auto widget = mWidgets.begin(); widget != mWidgets.end(); widget++ )
                {
                    if ( (*widget)->IsUnderMouse(x, y) )
                    {
                        return (*widget)->HandleUIEvent(event);
                    }
                }
                return false;
            }
                break;
            default:
                return false;
                break;
        }
        
        
    }

}
