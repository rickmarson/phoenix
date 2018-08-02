/*
 
A Simple Text Label
 
 */

#ifndef LABEL_H
#define LABEL_H

#include "widget.h"

namespace NYX {
    
    // Required Classes
    
    class NYX_EXPORT Label : public Widget
    {
    public:
        
        virtual ~Label( void );
        
        static WidgetPtr Create( IRenderer* renderer );
        
        void SetText( std::string text, std::string font, int text_size = 12 );
        void SetTextColor( ubyte r, ubyte g, ubyte b );
        
        virtual void Draw( void ) override;
        virtual bool HandleUIEvent( UIEvent& event ) override;
        
    protected:
        
        Label( IRenderer* renderer );
        void InitBuffer( void );
        
        std::string mText;
        ubyte mTextColor[3];
        int   mTextSize = 12;
        Texture*    mTextBuffer = nullptr;
        bool mCanDraw = false;
    };
    
}

#endif /* LABEL_H */
