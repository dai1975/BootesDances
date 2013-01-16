#ifndef BOOTES_LIB_D3DX9_RESOURCEVIEW_H
#define BOOTES_LIB_D3DX9_RESOURCEVIEW_H

#include "macros.h"
#include "Font.h"
#include "../framework/View.h"
#include <vector>

namespace bootes { namespace lib { namespace d3dx9 {

class ResourceView: public ::bootes::lib::framework::View
{
public:
   ResourceView();
   virtual ~ResourceView();

   bool getFontTexture(unsigned int ch, Font* out);

public:
   virtual void onUpdate(double currentTime, int elapsedTime);
   virtual void onRender(double currentTime, int elapsedTime);
   virtual bool onInput(const ::bootes::lib::framework::InputEvent*);
   virtual void onLostDevice();
   virtual void onResetDevice();

private:
   bool loadFontTexture(unsigned int ch);
   std::vector< Font > m_vFont;
   std::vector< bool > m_vUse;
};

} } }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
