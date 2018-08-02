/*
 
 Base Widget Class (abstract)
 
 */

#include "widget.h"
#include "texture.h"
#include "vertex_buffer.h"
#include "Cache/resource_cache.h"

namespace NYX {
    
    Widget::Widget( IRenderer* renderer ) :
        mRenderer(renderer)
    {
        
    }
    
    Widget::~Widget( void )
    {
        mAppearance = nullptr;
        
        // this needs to be deleted via the resource cache
        /*if ( appearance )
            delete appearance;
        */
        
        if ( mVertexBuffer )
            delete mVertexBuffer;
    }
    
    void Widget::SetBounds( uint top, uint left, uint width, uint height )
    {
        this->top = top;
        this->left = left;
        this->width = width;
        this->height = height;
    }
    
    bool Widget::IsUnderMouse( uint x, uint y )
    {
        if ( (x >= left) && (x <= left+width)  &&
            ( y >= top-height) && ( y <= top ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void Widget::SetPosition( uint top, uint left )
    {
        this->top = top;
        this->left = left;
    }
    
    void Widget::SetDimensions( uint width, uint height )
    {
        this->width = width;
        this->height = height;
    }
    
    void Widget::SetAppearance( std::string appearance_name )
    {
        ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(appearance_name, RES_TEXTURE);
        ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
        mAppearance = himg->GetTexture();
    }
    
    void Widget::SetSelectedAppearance( std::string appearance_name )
    {
        ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(appearance_name, RES_TEXTURE);
        ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
        mSelectedAppearance = himg->GetTexture();
    }
    
    void Widget::SetClickedAppearance( std::string appearance_name )
    {
        ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(appearance_name, RES_TEXTURE);
        ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
        mClickedAppearance = himg->GetTexture();
    }

}
