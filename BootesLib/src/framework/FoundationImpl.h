#ifndef BOOTES_LIB_SRC_FRAMEWROK_FOUNDATIONIMPL_H
#define BOOTES_LIB_SRC_FRAMEWROK_FOUNDATIONIMPL_H

#include "../include.h"
#include <bootes/lib/framework/Foundation.h>
#include <bootes/lib/util/Timer.h>

namespace bootes { namespace lib { namespace framework {

class FoundationImpl
{
public:
   explicit FoundationImpl(Foundation*);
   ~FoundationImpl();
   std::string toString(void* p) const;

public:
   bool initialize_(HINSTANCE hInstance, int nCmdShow, Game*, Wiimote*);
   void finalize_();
   int mainLoop_();

   HWND getHwnd();
   IDirect3DDevice9* getD3D9Device();
   Game* getGame();
   ::bootes::lib::d3dx9::ResourceView* getDxResource();
   EventManager* getEventManager();
   Wiimote* getWiimote();
   const GameTime* getGameTime() const;

   void queue_(const Event* e);
   bool notifySensorEvent_(const GameTime* gt, const InputEvent* ev);
   bool notifyInputEvent_(const GameTime* gt, const InputEvent* ev);

private:
   bool initWindow(HINSTANCE hInstance, int nCmdShow);
   bool initDirectX();
   void finWindow();
   void finDirectX();
   friend LRESULT CALLBACK gWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

   void restore();
   void update(const GameTime* gt);
   void render(const GameTime* gt);

private:
   Foundation* _parent;
   HINSTANCE _hInstance;
   HWND _hWnd;
   IDirect3D9* _pD3D;
   IDirect3DDevice9* _pD3DDev;
   EventManager* _pEventManager;
   Wiimote* _pWiimote;
   GameTime _gt;

   ::bootes::lib::util::Timer _timer;
   Game* _pGame;
   ::bootes::lib::d3dx9::ResourceView* _pDxResource;
   bool _bLost;
   D3DPRESENT_PARAMETERS _d3dparam;
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
