/*
 
 The UI Manager.
 Implements a very simple UI layer.
 
 */


#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "widget.h"
#include <list>

namespace NYX {
    
    // Required Classes
    class IRenderer;
    class RenderBuffer;
    class Effect;
    struct TTF_Font;
    
    class NYX_EXPORT UIManager
    {
    public:
        
        UIManager() = default;
        virtual ~UIManager();
        
        void Initialise( IRenderer* renderer );
        void AddWidget( WidgetPtr widget );
        void UpdateUI( void );
        void RequestRedraw( void )                      { mDirty = true; }
        void Render( void );
        bool HandleUIEvent( UIEvent& event );
        
        std::string GetDefaultFont( void )              { return default_font; }
        
    protected:
        
        std::list< WidgetPtr > mWidgets;
        
        IRenderer* mRenderer = nullptr;
        RenderBuffer* mUIbuffer = nullptr;
        Effect* mShaderProgram = nullptr;
        VertexBuffer *mVertexBuffer = nullptr;
        VertexArrayObject *mVertexArray = nullptr;
        std::string default_font;
        bool mDirty = true;
    };
    
}

#endif /* UI_MANAGER_H */
