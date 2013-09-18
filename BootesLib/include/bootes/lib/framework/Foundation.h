#ifndef BOOTES_LIB_FRAMEWORK_FOUNDATION_H
#define BOOTES_LIB_FRAMEWORK_FOUNDATION_H

#include "macros.h"
#include <string>
#include "GameTime.h"
#include "Event.h"
#include "Game.h"
#include "View.h"
#include "Wiimote.h"
#include "../d3dx9.h"
#include "../d3dx9/ResourceView.h"

namespace bootes { namespace lib { namespace framework {

class Foundation
{
public:
   Foundation();
   virtual ~Foundation();

   virtual bool initialize(HINSTANCE, int nCmdShow, Game*);
   virtual void finalize();
   virtual int mainLoop();

   HWND getHwnd();
   IDirect3DDevice9* getD3D9Device();
   Game* getGame();
   ::bootes::lib::d3dx9::ResourceView* getDxResource();
   EventManager* getEventManager();
   Wiimote* getWiimote();
   const GameTime* getGameTime() const;

   virtual void queue(const Event* e);
   std::string toString(void* p) const;

   virtual bool notifySensorEvent(const GameTime* gt, const InputEvent* ev);
   virtual bool notifyInputEvent(const GameTime* gt, const InputEvent* ev);

protected:
   virtual Wiimote* createWiimote() const;
private:
   void* _data;
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
