#ifndef BOOTESDANCES_VIEW_EDITOR_WIIMOTEWINDOW_H
#define BOOTESDANCES_VIEW_EDITOR_WIIMOTEWINDOW_H

#include "../../include.h"
#include <map>
#include <string>
#include <list>
#include <bootes/cegui/Direct3D9GeometryBuffer.h>
#include <bootes/dances/BootesWiimote.h>

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

class WiimoteRenderEffect: public CEGUI::RenderEffect
{
   bool _wiimote;
   bool _bCalib;
   float _yaw,_pitch,_roll;
public:
   WiimoteRenderEffect();
   void onWiimote(const BootesWiimote*);

   virtual int  getPassCount() const;
   virtual void performPreRenderFunctions(const int pass);
   virtual void performPostRenderFunctions();
   virtual bool realiseGeometry(CEGUI::RenderingWindow& window, CEGUI::GeometryBuffer& geo);
   virtual bool update(const float elapsed, CEGUI::RenderingWindow& window);
};

class WiimoteWindow: public CEGUI::Window
{
public:
   static const CEGUI::String WidgetTypeName;

   WiimoteWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~WiimoteWindow();
   bool init();

public:
   void onRender(double currentTime, int elapsedTime);
   virtual void drawSelf(const CEGUI::RenderingContext& ctx);

private:
   ::bootes::cegui::Direct3D9GeometryBuffer* _mygeo;
   WiimoteRenderEffect* _pRenderEffect;
   CEGUI::Window*       _pStaticImage;
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
