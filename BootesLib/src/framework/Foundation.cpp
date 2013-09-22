#include "../include.h"
#include <bootes/lib/framework/Foundation.h>
#include "FoundationImpl.h"

namespace bootes { namespace lib { namespace framework {

Foundation::Foundation()
{
   _data = new FoundationImpl(this);
}

Foundation::~Foundation()
{
   delete static_cast< FoundationImpl* >(_data);
}

Wiimote* Foundation::createWiimote() const
{
   return new Wiimote();
}

bool Foundation::initialize(HINSTANCE h, int nCmdShow, Game* g)
{
   Wiimote* pWiimote = createWiimote();
   return static_cast< FoundationImpl* >(_data)->initialize_(h, nCmdShow, g, pWiimote);
}

void Foundation::finalize()
{
   return static_cast< FoundationImpl* >(_data)->finalize_();
}

int Foundation::mainLoop()
{
   return static_cast< FoundationImpl* >(_data)->mainLoop_();
}

HWND Foundation::getHwnd()
{
   return static_cast< FoundationImpl* >(_data)->getHwnd();
}

IDirect3DDevice9* Foundation::getD3D9Device()
{
   return static_cast< FoundationImpl* >(_data)->getD3D9Device();
}

Game* Foundation::getGame()
{
   return static_cast< FoundationImpl* >(_data)->getGame();
}

::bootes::lib::d3dx9::ResourceView* Foundation::getDxResource()
{
   return static_cast< FoundationImpl* >(_data)->getDxResource();
}

EventManager* Foundation::getEventManager()
{
   return static_cast< FoundationImpl* >(_data)->getEventManager();
}

Wiimote* Foundation::getWiimote()
{
   return static_cast< FoundationImpl* >(_data)->getWiimote();
}

const GameTime* Foundation::getGameTime() const
{
   return static_cast< FoundationImpl* >(_data)->getGameTime();
}

void Foundation::queue(const Event* e)
{
   static_cast< FoundationImpl* >(_data)->queue_(e);
}

std::string Foundation::toString(void* p) const
{
   return static_cast< FoundationImpl* >(_data)->toString(p);
}

bool Foundation::notifyInputEvent(const GameTime* gt, const InputEvent* ev)
{
   return static_cast< FoundationImpl* >(_data)->notifyInputEvent_(gt, ev);
}

bool Foundation::notifySensorEvent(const GameTime* gt, const InputEvent* ev)
{
   return static_cast< FoundationImpl* >(_data)->notifySensorEvent_(gt, ev);
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
