#include "WiimoteWindow.h"
#include <mmsystem.h>

namespace bootes { namespace dances { namespace editor {

WiimoteRenderEffect::WiimoteRenderEffect()
{
   _wiimote = false;
}

void WiimoteRenderEffect::onWiimote(const BootesWiimote* pWiimote)
{
   if (pWiimote == NULL) {
      _wiimote = false;
      return;
   }

   _yaw   = pWiimote->orien().yaw   * 3.141592f / 180.0f;
   _pitch = pWiimote->orien().pitch * 3.141592f / 180.0f;
   _roll  = pWiimote->orien().roll  * 3.141592f / 180.0f;
   _bCalib = pWiimote->isCalib();
   _wiimote = true;
}

int  WiimoteRenderEffect::getPassCount() const
{
   return 1;
}

void WiimoteRenderEffect::performPreRenderFunctions(const int pass)
{
   ;
}

bool WiimoteRenderEffect::realiseGeometry(CEGUI::RenderingWindow& window, CEGUI::GeometryBuffer& geo)
{
   return false;
}

bool WiimoteRenderEffect::update(const float elapsed, CEGUI::RenderingWindow& window)
{
   return true;
}

void WiimoteRenderEffect::performPostRenderFunctions()
{
   HRESULT hr;
   CEGUI::Renderer* p = CEGUI::System::getSingleton().getRenderer();
   CEGUI::Direct3D9Renderer* p9 = static_cast< CEGUI::Direct3D9Renderer* >(p);
   IDirect3DDevice9* pDev = p9->getDevice();

   if (! _wiimote) {
      pDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
      return;
   }

   D3DCOLOR color = (_bCalib)? D3DCOLOR_XRGB(255,0,0): D3DCOLOR_XRGB(64,64,64);
   pDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);

   const D3DXVECTOR3 BOXSIZE(20,10,100);
   ID3DXMesh* pBox = NULL;
   if (FAILED(D3DXCreateBox(pDev, BOXSIZE.x, BOXSIZE.y, BOXSIZE.z, &pBox, NULL))) { goto fin; }

   {      
      float t = timeGetTime();
      D3DXMATRIX matWorld, matTrans, matRot, matScale;
      D3DXMatrixScaling(&matScale, 1.0f, 1.0f, 1.0f);
      D3DXMatrixRotationYawPitchRoll(&matRot, _yaw, -_pitch, -_roll);
      D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 0.0f);
      matWorld = matScale * matRot * matTrans;
      pDev->SetTransform(D3DTS_WORLD, &matWorld);
   }
   {
      D3DXMATRIX matView;
      D3DXVECTOR3 vecFrom(50.0f, 30.0f, -100.0f);
      D3DXVECTOR3 vecAt  (0.0f, 0.0f, 0.0f);
      D3DXVECTOR3 vecUp  (0.0f, 1.0f, 0.0f);
      D3DXMatrixLookAtLH(&matView, &vecFrom, &vecAt, &vecUp);
      pDev->SetTransform(D3DTS_VIEW, &matView);
   }
   {
      D3DXMATRIX matProj;
      float fAngle  = 60 * 3.141592f / 180.0f;
      float fAspect = 1.0f;
      float fNear   = 0.1f;
      float fFar    = 200.0f;
      D3DXMatrixPerspectiveFovLH(&matProj, fAngle, fAspect, fNear, fFar);
      pDev->SetTransform(D3DTS_PROJECTION, &matProj);
   }

   pDev->SetRenderState(D3DRS_ZENABLE, true);
   pDev->SetRenderState(D3DRS_LIGHTING, false);
   pDev->SetTexture(0, NULL);
   hr = pBox->DrawSubset(0);
   {
      DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
      struct Vertex { float x,y,z; DWORD diffuse; };
      int x = BOXSIZE.x/2;
      int y = BOXSIZE.y/2;
      int z = BOXSIZE.z/2;
      Vertex vtx[] = {
         { 0,0,0,D3DCOLOR_XRGB(255,0,0) }, { 1000,0,0,D3DCOLOR_XRGB(255,0,0) },
         { 0,0,0,D3DCOLOR_XRGB(0,255,0) }, { 0,1000,0,D3DCOLOR_XRGB(0,255,0) },
         { 0,0,0,D3DCOLOR_XRGB(0,0,255) }, { 0,0,1000,D3DCOLOR_XRGB(0,0,255) },

         { -x,-y,-z,D3DCOLOR_XRGB(0,0,0) },  { +x,-y,-z,D3DCOLOR_XRGB(0,0,0) },
         { -x,-y,+z,D3DCOLOR_XRGB(0,0,0) },  { +x,-y,+z,D3DCOLOR_XRGB(0,0,0) },
         { -x,+y,-z,D3DCOLOR_XRGB(0,0,0) },  { +x,+y,-z,D3DCOLOR_XRGB(0,0,0) },
         { -x,+y,+z,D3DCOLOR_XRGB(0,0,0) },  { +x,+y,+z,D3DCOLOR_XRGB(0,0,0) },

         { -x,-y,-z,D3DCOLOR_XRGB(0,0,0) },  { -x,+y,-z,D3DCOLOR_XRGB(0,0,0) },
         { -x,-y,+z,D3DCOLOR_XRGB(0,0,0) },  { -x,+y,+z,D3DCOLOR_XRGB(0,0,0) },
         { +x,-y,-z,D3DCOLOR_XRGB(0,0,0) },  { +x,+y,-z,D3DCOLOR_XRGB(0,0,0) },
         { +x,-y,+z,D3DCOLOR_XRGB(0,0,0) },  { +x,+y,+z,D3DCOLOR_XRGB(0,0,0) },

         { -x,-y,-z,D3DCOLOR_XRGB(0,0,0) },  { -x,-y,+z,D3DCOLOR_XRGB(0,0,0) },
         { -x,+y,-z,D3DCOLOR_XRGB(0,0,0) },  { -x,+y,+z,D3DCOLOR_XRGB(0,0,0) },
         { +x,-y,-z,D3DCOLOR_XRGB(0,0,0) },  { +x,-y,+z,D3DCOLOR_XRGB(0,0,0) },
         { +x,+y,-z,D3DCOLOR_XRGB(0,0,0) },  { +x,+y,+z,D3DCOLOR_XRGB(0,0,0) },
      };
      pDev->SetFVF(fvf);
      pDev->DrawPrimitiveUP(D3DPT_LINELIST, 15, vtx, sizeof(Vertex));
   }

fin:
   if (pBox) { pBox->Release(); }
   p9->beginRendering();
}

//--------------------------------------------------------------------------
const CEGUI::String WiimoteWindow::WidgetTypeName = "BootesDancesWiimoteWindow";

WiimoteWindow::WiimoteWindow(const CEGUI::String& type, const CEGUI::String& name)
  : CEGUI::Window(type, name)
{
   _mygeo = NULL;
   _pRenderEffect = NULL;
}

WiimoteWindow::~WiimoteWindow()
{
   //CEGUI::Renderer* pRenderer = CEGUI::System::getSingleton().getRenderer();
   if (_mygeo) {
      delete _mygeo;
      _mygeo = NULL;
      d_geometry = NULL;
   }
   if (_pRenderEffect) {
      delete _pRenderEffect;
   }
}

bool WiimoteWindow::init()
{
/*
   _pScreen = wm.createWindow("TaharezLook/StaticImage", "Screen");
   _pScreen->setSize(CEGUI::UVector2(x,y));
   _pScreen->setPosition(CEGUI::UVector2(x,y));
   _pScreen->setUsingAutoRenderingSurface(false);
*/
   CEGUI::Renderer* p = CEGUI::System::getSingleton().getRenderer();
   p->destroyGeometryBuffer(*d_geometry);

   CEGUI::Direct3D9Renderer* p9 = static_cast< CEGUI::Direct3D9Renderer* >(p);
   _pRenderEffect = new WiimoteRenderEffect();
   _mygeo = new ::bootes::cegui::Direct3D9GeometryBuffer(*p9, p9->getDevice());
   _mygeo->setRenderEffect(_pRenderEffect);
   d_geometry = _mygeo;

   setUsingAutoRenderingSurface(true);

   return true;
}

void WiimoteWindow::onRender(double currentTime, int elapsedTime)
{
   BootesWiimote* pWiimote = static_cast< BootesWiimote* >(g_pFnd->getWiimote());
   if (pWiimote == NULL || !pWiimote->isConnected() || !pWiimote->isMotionPlusEnabled()) {
      _pRenderEffect->onWiimote(NULL);
   } else {
      _pRenderEffect->onWiimote(pWiimote);
   }
/*
   CEGUI::RenderingSurface* p = getRenderingSurface();
   if (p) {
      p->invalidate();
   }
*/
   //invalidate();
   if (d_surface) {
      invalidateRenderingSurface();
   }
}

void WiimoteWindow::drawSelf(const CEGUI::RenderingContext& ctx)
{
   CEGUI::Window::drawSelf(ctx);

   if (_mygeo) {
      _mygeo->reset();
      CEGUI::Renderer* pRenderer = CEGUI::System::getSingleton().getRenderer();
   }
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
