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
#include <tchar.h>
#include <string.h>
#include <shellapi.h>

::bootes::lib::framework::Foundation* g_pFnd = NULL;
IBootesGame* g_pGame = NULL;

namespace {

bool parseArgs(BootesGameOption* opt)
{
   int argc    = __argc;
   TCHAR** argv = __wargv;
   while (--argc) {
      ++argv;
      if (**argv == _T('-')) {
         if (_tcscmp(_T("-d"), *argv) == 0) {
            if (--argc == 0) return false;
            ++argv;
            opt->datadir = *argv;
         } else if (_tcscmp(_T("-e"), *argv) == 0) {
            opt->editable = true;
         }
      }
   }
   return true;
}

}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
   int ret = 0;

   setlocale(LC_ALL, ".ACP");

   BootesGameOption opt;
   if (! parseArgs(&opt)) {
      return -1;
   }

   if (opt.datadir.empty()) {
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
      opt.datadir = o.str();
      if (CreateDirectory(opt.datadir.c_str(), NULL) == 0) {
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

   BootesGame* pGame = new BootesGame(opt);
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
