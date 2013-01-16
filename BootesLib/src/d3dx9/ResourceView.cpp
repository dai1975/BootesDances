#include "../include.h"
#include <bootes/lib/d3dx9/ResourceView.h>
#include <bootes/lib/booteslib.h>
#include <bootes/lib/d3dx9/Font.h>

namespace bootes { namespace lib { namespace d3dx9 {

ResourceView::ResourceView()
{
   m_vFont.resize(0x80 - 0x20);
   m_vUse.resize(m_vFont.size());
   for (size_t i=0; i<m_vFont.size(); ++i) {
      m_vFont[i].pTex = NULL;
      m_vUse[i] = false;
   }
}

ResourceView::~ResourceView()
{
   for (size_t i=0; i<m_vFont.size(); ++i) {
      if (m_vFont[i].pTex) {
         m_vFont[i].pTex->Release();
         m_vFont[i].pTex = NULL;
      }
   }
}

bool ResourceView::loadFontTexture(unsigned int ch)
{
   int i = ch - 0x20;
   if (m_vFont[i].pTex) { return true; }

   IDirect3DDevice9* pDev = ::bootes::lib::GetFoundation()->getD3D9Device();
   return CreateFontTexture(pDev, D3DUSAGE_DYNAMIC, D3DPOOL_DEFAULT, ch, &m_vFont[i]);
}

bool ResourceView::getFontTexture(unsigned int ch, Font* out)
{
   int i = ch - 0x20;
   m_vUse[i] = true;
   if (m_vFont[i].pTex == NULL) {
      loadFontTexture(ch);
   }
   memcpy(out, &m_vFont[i], sizeof(Font));
   if (m_vFont[i].pTex) {
      m_vFont[i].pTex->AddRef();
      return true;
   } else {
      return false;
   }
}

void ResourceView::onLostDevice()
{
   for (size_t i=0; i<m_vFont.size(); ++i) {
      if (m_vFont[i].pTex) {
         m_vFont[i].pTex->Release();
         m_vFont[i].pTex = NULL;
      }
   }
}

void ResourceView::onResetDevice()
{
   for (size_t i=0; i<m_vUse.size(); ++i) {
      if (m_vUse[i]) {
         loadFontTexture(i + 0x20);
      }
   }
}

void ResourceView::onUpdate(double currentTime, int elapsedTime)
{
}

void ResourceView::onRender(double currentTime, int elapsedTime)
{
}

bool ResourceView::onInput(const ::bootes::lib::framework::InputEvent*)
{
   return false;
}

} } }


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
