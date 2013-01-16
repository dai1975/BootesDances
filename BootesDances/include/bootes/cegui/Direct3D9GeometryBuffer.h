#ifndef BOOTESDANCES_VIEW_CEGUID3DX9GEOMETRYBUFFER_H
#define BOOTESDANCES_VIEW_CEGUID3DX9GEOMETRYBUFFER_H

/*
 * override CEGUI::Direct3D9GeometryBuffer#draw()
 */

#include <string>
#include <bootes/lib/d3dx9.h>
#include <CEGUI.h>
#include <RendererModules/Direct3D9/CEGUIDirect3D9GeometryBuffer.h>
#include <RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h>

namespace bootes { namespace cegui {

class Direct3D9GeometryBuffer
   : public ::CEGUI::Direct3D9GeometryBuffer
{
public:
   Direct3D9GeometryBuffer(::CEGUI::Direct3D9Renderer& owner, LPDIRECT3DDEVICE9 device);
   void appendBoard(const CEGUI::Rect& pos, const CEGUI::Rect& tpos, const CEGUI::colour& color, CEGUI::Texture* tex);

   virtual void reset();
   virtual void draw() const;

private:
  struct TextData {
     std::string str;
     int length;
     CEGUI::Rect rect;

     inline TextData(const char* s, int l, CEGUI::Rect r) {
        str = s;
        length = l;
        rect = r;
     }
     inline TextData& operator=(const TextData& r) {
        str = r.str;
        length = r.length;
        rect = r.rect;
        return *this;
     }
  };
};

} }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */

