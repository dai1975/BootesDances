#include "FoundationImpl.h"
#include <mmsystem.h>
#include <bootes/lib/framework/View.h>
#include <bootes/lib/framework/Game.h>
#include "EventManagerImpl.h"

namespace bootes { namespace lib { namespace framework {

FoundationImpl::FoundationImpl(Foundation* parent)
{
   _parent = parent;
   _hWnd = NULL;
   _pD3D = NULL;
   _pD3DDev = NULL;
   _pGame = NULL;
   _pDxResource = NULL;
   _bLost = false;
   _pEventManager = NULL;
   _pWiimote = NULL;
}

FoundationImpl::~FoundationImpl()
{
   finalize_();
}

std::string FoundationImpl::toString(void* p) const
{
   static char tbl[] = "0123456789ABCDEF";
   char buf[128];
   uintptr_t v = reinterpret_cast< uintptr_t >(p);
   int i=0;
   while (i<127) {
      unsigned char x = v & 0x0F;
      buf[i++] = tbl[x];
      v >>= 4;
      if (v == 0) { break; }
   }
   buf[i] = '\0';
   return std::string(buf);
}

bool FoundationImpl::initialize_(HINSTANCE hInstance, int nCmdShow, Game* pGame, Wiimote* pWiimote)
{
   _pEventManager = new EventManagerImpl();

   _pWiimote = pWiimote;

   timeBeginPeriod(1);
   if (! initWindow(hInstance, nCmdShow)) { return false; }
   if (! initDirectX()) { return false; }

   _pDxResource = new ::bootes::lib::d3dx9::ResourceView();

   _pGame = pGame;
   _pGame->addView(_pDxResource);

   return true;
}

void FoundationImpl::finalize_()
{
   if (_pWiimote) {
      delete _pWiimote;
      _pWiimote = NULL;
   }

   // _pDxResouce is deleted by Game
   if (_pGame) {
      delete _pGame;
      _pGame = NULL;
   }

   timeEndPeriod(1);
   if (_pEventManager) {
      delete _pEventManager;
      _pEventManager = NULL;
   }

   finDirectX();
   finWindow();
}

HWND FoundationImpl::getHwnd()
{
   return _hWnd;
}

IDirect3DDevice9* FoundationImpl::getD3D9Device()
{
   return _pD3DDev;
}

Game* FoundationImpl::getGame()
{
   return _pGame;
}

::bootes::lib::d3dx9::ResourceView* FoundationImpl::getDxResource()
{
   return _pDxResource;
}

EventManager* FoundationImpl::getEventManager()
{
   return _pEventManager;
}

Wiimote* FoundationImpl::getWiimote()
{
   return _pWiimote;
}

const GameTime* FoundationImpl::getGameTime() const
{
   return &_gt;
}

void FoundationImpl::queue_(const Event* e)
{
   _pEventManager->queue(e);
}

bool FoundationImpl::notifySensorEvent_(const GameTime* gt, const InputEvent* ev)
{
   Game::t_views::const_reverse_iterator i;
   const Game::t_views& views = _pGame->getViews();
   for (i = views.rbegin(); i != views.rend(); ++i) {
      if ((*i)->onSensorInput(gt, ev)) {
         return true;
      }
   }
   return false;
}

bool FoundationImpl::notifyInputEvent_(const GameTime* gt, const InputEvent* ev)
{
   Game::t_views::const_reverse_iterator i;
   const Game::t_views& views = _pGame->getViews();
   for (i = views.rbegin(); i != views.rend(); ++i) {
      if ((*i)->onInput(gt, ev)) {
         return true;
      }
   }
   return false;
}

LRESULT CALLBACK gWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   LONG_PTR v = GetWindowLongPtr(hWnd, GWLP_USERDATA);
   FoundationImpl* p = reinterpret_cast< FoundationImpl* >(v);
   if (p != NULL) {
      return p->wndProc(hWnd, message, wParam, lParam);
   } else {
      return ::DefWindowProc(hWnd, message, wParam, lParam);
   }
}

bool FoundationImpl::initWindow(HINSTANCE hInstance, int nCmdShow)
{
   TCHAR* szWindowClass = _T("Bootes");
   TCHAR* szTitle = _T("Bootes");
   {
      WNDCLASSEX wcex;
      wcex.cbSize        = sizeof(WNDCLASSEX);
      wcex.style         = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc   = gWndProc;
      wcex.cbClsExtra    = 0;
      wcex.cbWndExtra    = sizeof(LONG_PTR);
      wcex.hInstance     = hInstance;
      wcex.hIcon         = NULL; //LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DANCEFELIX));
      wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
      wcex.lpszMenuName  = NULL; //MAKEINTRESOURCE(IDC_DANCEFELIX);
      wcex.lpszClassName = szWindowClass;
      wcex.hIconSm       = NULL; //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
      RegisterClassEx(&wcex);
   }
   
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (! hWnd) {
      return false;
   }
   
   {
      LONG_PTR p = reinterpret_cast< LONG_PTR >((void*)this);
      SetWindowLongPtr(hWnd, GWLP_USERDATA, p);
   }
   
   _hInstance = hInstance;
   _hWnd = hWnd;
   
   ShowWindow(_hWnd, nCmdShow);
   UpdateWindow(_hWnd);

   return true;
}
void FoundationImpl::finWindow()
{
   if (_hWnd) {
      DestroyWindow(_hWnd);
   }
   _hWnd = NULL;
}

bool FoundationImpl::initDirectX()
{
   HRESULT hr;
   assert(_pD3D == NULL);
   assert(_pD3DDev == NULL);

   if (! IsWindow(_hWnd)) { return false; }

   { //D3D9
      IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
      if (pD3D == NULL) {
         return false;
      }

      {
         D3DDISPLAYMODE dm;
         hr = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
         if (hr != S_OK) { return false; }

         ZeroMemory(&_d3dparam, sizeof(_d3dparam));
         //param.BackBufferWidth = 640;
         //param.BackBufferHeight = 400;
         _d3dparam.BackBufferFormat = D3DFMT_UNKNOWN;
         //m_D3DParam.BackBufferFormat = dm.Format;
         //param.BackBufferCount = 1;
         //param.MultiSampleType = D3DMULTISAMPLE_NONE;
         //param.MultiSampleQuality = 0;
         //param.SwapEffect = D3DSWAPEFFECT_DISCARD;
         _d3dparam.SwapEffect = D3DSWAPEFFECT_COPY;
         _d3dparam.hDeviceWindow = _hWnd;
         _d3dparam.Windowed = TRUE;
         _d3dparam.EnableAutoDepthStencil = TRUE; //z-buffer
         _d3dparam.AutoDepthStencilFormat = D3DFMT_D16;
         //param.Flags = 0;
         //param.FullScreen_RefreshRateInHz = 0;
         //param.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

         IDirect3DDevice9* pDev;
         DWORD behavior = 
            //D3DCREATE_SOFTWARE_VERTEXPROCESSING |
            D3DCREATE_HARDWARE_VERTEXPROCESSING |
            D3DCREATE_MULTITHREADED;
         hr = pD3D->CreateDevice(0, D3DDEVTYPE_HAL, _hWnd,
                                 behavior, &_d3dparam, &pDev);
         if (hr != D3D_OK) {
            switch (hr) {
            case D3DERR_DEVICELOST:
               return false;
            case D3DERR_INVALIDCALL:
               return false;
            case D3DERR_NOTAVAILABLE:
               return false;
            case D3DERR_OUTOFVIDEOMEMORY:
               return false;
            }
            return false;
         }

         _pD3D = pD3D;
         _pD3DDev = pDev;
      }
   }
   return true;
}
void FoundationImpl::finDirectX()
{
   if (_pD3DDev) {
      _pD3DDev->Release();
      _pD3DDev = NULL;
   }
   if (_pD3D) {
      _pD3D->Release();
      _pD3D = NULL;
   }
}


LRESULT FoundationImpl::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int wmId, wmEvent;
   PAINTSTRUCT ps;
   HDC hdc;
   LRESULT result = 0;
   
   switch (message) {
   case WM_COMMAND:
      wmId    = LOWORD(wParam);
      wmEvent = HIWORD(wParam);
      /*
      switch (wmId) {
      case IDM_ABOUT:
         DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
         break;
      case IDM_EXIT:
         DestroyWindow(hWnd);
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
      }
      */
      break;
   case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
      
   case WM_KEYDOWN:
   case WM_KEYUP:
   case WM_CHAR:
   case WM_MOUSEMOVE:
   case WM_MOUSELEAVE:
   case WM_NCMOUSEMOVE:
   case 0x020A: //WHEEL
   case WM_LBUTTONDOWN:
   case WM_LBUTTONUP:
   case WM_RBUTTONDOWN:
   case WM_RBUTTONUP:
   case WM_MBUTTONDOWN:
   case WM_MBUTTONUP:
      {
         if (_pGame) {
            WndmsgEvent ev;
            ev._hWnd = hWnd;
            ev._message = message;
            ev._wParam = wParam;
            ev._lParam = lParam;
            result = (_parent->notifyInputEvent(&_gt, &ev)? 0: 1);
         }
         break;
      }
      
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return result;
}

/*
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
*/

int FoundationImpl::mainLoop_()
{
   _timer.reset();
   _timer.start();

   MSG  msg;
/*
   while (true) {
      if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
         if (msg.message == WM_QUIT) { break; }

      } else {
         double totalTime;
         int    elapsedTime, tmp;
         _timer.get(&totalTime, &elapsedTime);

         restore();

         _timer.get(&totalTime, &tmp);
         elapsedTime += tmp;
         update(totalTime, elapsedTime);

         _timer.get(&totalTime, &tmp);
         elapsedTime += tmp;
         render(totalTime, elapsedTime);
      }
   }
*/
   struct {
      double event;
      double wiimote, game, render;
   } t0,t1;
   _timer.get(&_gt.total, NULL); //msec
   t0.event = t0.wiimote = t0.game = t0.render = _gt.total;
   t1.event = t1.wiimote = t1.game = t1.render = _gt.total;
   double t1_min = _gt.total;

   double frame_msec = 1000.0 / 60;
   bool quit = false;

   std::list< WiimoteEvent > lWiimoteEvent;
   WiimoteEvent wev;
   bool wev_given;
   double rec[2][6]; //performance record
   int ri = 1;
   while (!quit) {
      ri ^= 1;
      for (int i=0; i<6; ++i) { rec[ri][i] = 0; }
      _timer.get(&rec[ri][0], NULL); //msec

      wev_given = false;
      _timer.get(&_gt.total, NULL); //msec
      while (true) {
         _gt.elapsed = _gt.total - t0.event; //used in wndproc
         int cnt = 0;
         while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            if (msg.message == WM_QUIT) { quit = true; }
            if (10 < ++cnt) { break; }
         }
         _timer.get(&rec[ri][1], NULL); //msec
         _pEventManager->clock(&_gt, t1_min - _gt.total);
         _timer.get(&rec[ri][2], NULL); //msec

         if (_pWiimote && _pWiimote->isConnected()) {
            lWiimoteEvent.clear();
            _pWiimote->consumeEvents(&lWiimoteEvent);
            if (! lWiimoteEvent.empty()) {
               std::list< WiimoteEvent >::iterator i;
               for (i = lWiimoteEvent.begin(); i != lWiimoteEvent.end(); ++i) {
                  _parent->notifySensorEvent(&_gt, &(*i));
               }
               wev = *(--i);
               wev_given = true;
            }
         }

         t0.event = _gt.total;
         _timer.get(&_gt.total, NULL); //msec
         if (t1_min <= _gt.total) { break; }
      }

      if (_pWiimote && _pWiimote->isConnected() && t1.wiimote <= _gt.total) {
         if (wev_given) {
            _gt.elapsed = static_cast< int >(_gt.total - t0.wiimote);
            _parent->notifyInputEvent(&_gt, &wev);
         }
         _timer.get(&_gt.total, NULL); //msec
         t0.wiimote = _gt.total;
         t1.wiimote = t0.wiimote + 3;
         if (t1_min < t1.wiimote) { t1_min = t1.wiimote; }
         _timer.get(&rec[ri][3], NULL); //msec
      }

      if (t1.game <= _gt.total) {
         _gt.elapsed = static_cast< int >(_gt.total - t0.game);
         update(&_gt);
         _timer.get(&_gt.total, NULL); //msec
         t0.game = _gt.total;
         //t1.game = t + (frame_msec / 2);
         t1.game = t0.game + 3;
         if (t1_min < t1.game) { t1_min = t1.game; }
         _timer.get(&rec[ri][4], NULL); //msec
      }

      if (t1.render <= _gt.total) {
         restore();
         _timer.get(&_gt.total, NULL); //msec
         _gt.elapsed = static_cast< int >(_gt.total - t0.render);
         render(&_gt);
         _timer.get(&_gt.total, NULL); //msec
         t0.render = _gt.total;
         t1.render = t0.render + frame_msec;
         if (t1_min < t1.render) { t1_min = t1.render; }
         _timer.get(&rec[ri][5], NULL); //msec
      }
   }
   
   _timer.pause();
   _timer.reset();
   return 0;
}

void FoundationImpl::restore()
{
   if (! _bLost) { return; }

   HRESULT hr;
   Game::t_views::const_iterator i;
   const Game::t_views& views = _pGame->getViews();

   Sleep( 50 );
   {
      if( FAILED(hr = _pD3DDev->TestCooperativeLevel()) ) {
         if( D3DERR_DEVICELOST == hr ) {
            return;
         }
      }
      
      for (i = views.begin(); i != views.end(); ++i) {
         (*i)->onLostDevice();
      }
      hr = _pD3DDev->Reset(&_d3dparam);
      if( FAILED( hr ) ) {
         if (hr == D3DERR_DEVICELOST) {
            return;
         } else {
            return;
         }
      }
      for (i = views.begin(); i != views.end(); ++i) {
         (*i)->onResetDevice();
      }
      _bLost = false;
   }
}

void FoundationImpl::update(const GameTime* gt)
{
   Game::t_views::const_iterator i;
   const Game::t_views& views = _pGame->getViews();
   ::bootes::lib::util::Timer timer;
   int dt[10];
   int ti = 0;

   timer.start();
   {
      _pGame->onUpdate(gt);
      for (i = views.begin(); i != views.end(); ++i) {
         (*i)->onUpdate(gt);
      }
   }
   timer.get(NULL, &dt[ti++]);
}

void FoundationImpl::render(const GameTime* gt)
{
   HRESULT hr;
   Game::t_views::const_iterator i;
   const Game::t_views& views = _pGame->getViews();

   ::bootes::lib::util::Timer timer;
   int dt[10];
   int ti = 0;

   timer.start();
   {
      _pD3DDev->BeginScene();
      _pD3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
      
      for (i = views.begin(); i != views.end(); ++i) {
         (*i)->onRender(gt);
      }
      
      _pD3DDev->EndScene();
      hr = _pD3DDev->Present(NULL, NULL, NULL, NULL);
      if( FAILED( hr ) ) {
         if (D3DERR_DEVICELOST == hr) {
            _bLost = true;
         } else if(D3DERR_DRIVERINTERNALERROR == hr) {
            _bLost = true;
         }
      }
   }
   timer.get(NULL, &dt[ti++]);
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
