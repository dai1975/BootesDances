#include "../include.h"
#include <bootes/dances/EvMouse.h>
#include <bootes/dances/EvMovie.h>
#include <bootes/dances/EvEditor.h>
#include <bootes/dances/EvStage.h>
#include "BootesGame.h"
#include "BootesFoundation.h"
#include <sstream>
#include <Shlobj.h>
#include <locale.h>

::bootes::lib::framework::Foundation* g_pFnd = NULL;
IBootesGame* g_pGame = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
   int ret = 0;

   setlocale(LC_ALL, ".ACP");

   std::basic_string< TCHAR > datadir;
   {
      TCHAR basedir[MAX_PATH];
      //int cs = CSIDL_APPDATA; //appdata
      int cs = CSIDL_PERSONAL; //mydocuments
      cs |= CSIDL_FLAG_CREATE;
      HRESULT hr = SHGetFolderPath(NULL, cs, NULL, SHGFP_TYPE_CURRENT, basedir);
      if (hr != S_OK) {
         return -1;
      }
      std::basic_ostringstream< TCHAR > o;
      o << basedir << _T("\\BootesDances");
      datadir = o.str();
      if (CreateDirectory(datadir.c_str(), NULL) == 0) {
         DWORD e = GetLastError();
         if (e != ERROR_ALREADY_EXISTS) {
            return -1;
         }
      }
   }

   class Initializer: public bootes::lib::Initializer {
   public:
      inline virtual ::bootes::lib::framework::Foundation* createFoundation() { return new BootesFoundation(); }
   } initializer;

   if (! ::bootes::lib::Initialize(&initializer)) {
      return -1;
   }
   ::bootes::lib::framework::Foundation* pFnd = bootes::lib::GetFoundation();

   BootesGame* pGame = new BootesGame(datadir.c_str(), true);
   if (! pFnd->initialize(hInstance, nCmdShow, pGame)) {
      goto fin;
   }

   ::bootes::lib::framework::EventManager* em = pFnd->getEventManager();
   Register_EvMovie(em);
   Register_EvMouse(em);
   Register_EvEditor(em);
   Register_EvStage(em);

   g_pFnd  = pFnd;
   g_pGame = pGame;
   ret = pFnd->mainLoop();

fin:
   pFnd->finalize();
   g_pFnd = NULL;
   delete pFnd;
   return ret;
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
