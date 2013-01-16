#include "MoveRendererResource.h"

MoveRendererResource::MoveRendererResource()
{
   for (int i=0; i<N_TEX; ++i) {
      _tex[i] = NULL;
   }
}
MoveRendererResource::~MoveRendererResource()
{
   onLostDevice();
   for (int i=0; i<N_TEX; ++i) {
      if (_tex[i]) {
         _tex[i]->Release();
         _tex[i] = NULL;
      }
   }
}

void MoveRendererResource::onLostDevice()
{
   for (int i=0; i<N_TEX; ++i) {
      if (_tex[i]) {
         _tex[i]->Release();
         _tex[i] = NULL;
      }
   }
}

void MoveRendererResource::onResetDevice()
{
   for (int i=0; i<N_TEX; ++i) {
      if (_tex[i]) {
         _tex[i]->Release();
         _tex[i] = NULL;
      }
   }
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   HRESULT hr;
   hr = D3DXCreateTextureFromFile(pDev,
      _T("datafiles\\imagesets\\ribbon2.png"), &_tex[TEX_RIBBON]);
   hr = D3DXCreateTextureFromFile(pDev,
      _T("datafiles\\imagesets\\dot.png"), &_tex[TEX_DOT]);
   hr = D3DXCreateTextureFromFile(pDev,
      _T("datafiles\\imagesets\\star2.png"), &_tex[TEX_MARKER]);
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
