/*
 
 Base Widget Class (abstract)
 
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <memory>
#include "ui_events.h"

namespace NYX {
    
    // Required Classes
    class Effect;
    class VertexBuffer;
    class VertexArrayObject;
    class Texture;
    class IRenderer;
    
    // UI Vertex Format
    struct NYX_EXPORT UIVertex
    {
        UIVertex() {
            memset(position, 0, 2*sizeof(float));
            memset(uv, 0, 2*sizeof(float));
        }
        
        float position[2];
        float uv[2];
    };
    
    class NYX_EXPORT Widget
    {
    public:
        
        virtual ~Widget( void );
        
        void SetBounds( uint top, uint left, uint width, uint height );
        void SetPosition( uint top, uint left );
        void SetDimensions( uint width, uint height );
        void SetAppearance( std::string appearance_name );
        void SetSelectedAppearance( std::string appearance_name );
        void SetClickedAppearance( std::string appearance_name );
        
        uint GetTop( void )                                                 { return top; }
        uint GetLeft( void )                                                { return left; }
        uint GetWidth( void )                                               { return width; }
        uint GetHeight( void )                                              { return height; }
        
        bool IsUnderMouse( uint x, uint y );
        
        virtual void Draw( void ) = 0;
        virtual bool HandleUIEvent( UIEvent& event ) = 0;
        
    protected:
        
        Widget( IRenderer* renderer );
        
        uint top = 0;
        uint left = 0;
        uint width = 0;
        uint height = 0;
        
        IRenderer    *mRenderer;
        Effect       *mShaderProgram;
        VertexBuffer *mVertexBuffer = nullptr;
        VertexArrayObject *mVertexArray = nullptr;
        Texture      *mAppearance = nullptr;
        Texture      *mSelectedAppearance = nullptr;
        Texture      *mClickedAppearance = nullptr;
    };
    
    typedef std::shared_ptr<Widget> WidgetPtr;
}

#endif /* WIDGET_H */
