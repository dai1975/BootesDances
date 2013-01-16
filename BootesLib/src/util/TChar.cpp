#include "../include.h"
#include <bootes/lib/util/TChar.h>
#include <locale.h>
#include <errno.h>

namespace bootes { namespace lib { namespace util {

wchar_t* TChar::W2W(const wchar_t* src)
{
   size_t len  = _tcslen(src) + 1;
   wchar_t* r = new wchar_t[len];
   memcpy(r, src, sizeof(wchar_t)*len);
   return r;
}

char* TChar::C2C(const char* src)
{
   size_t len  = strlen(src) + 1;
   char* r = new char[len];
   memcpy(r, src, sizeof(char)*len);
   return r;
}

wchar_t* TChar::C2W(const char* src)
{
   size_t wsize;
   if (mbstowcs_s(&wsize, NULL, 0, src, 0) != 0) {
      return NULL;
   }
   wchar_t* ret = new wchar_t[wsize];
   if (mbstowcs_s(&wsize, ret, wsize, src, wsize) != 0) {
      delete[] ret;
      return NULL;
   }
   return ret;
}

char* TChar::W2C(const wchar_t* src)
{
   size_t msize;
   int r;
   if ((r = wcstombs_s(&msize, NULL, 0, src, 0)) != 0) {
      return NULL;
   }
   char* ret = new char[msize];
   if ((r = wcstombs_s(&msize, ret, msize, src, msize)) != 0) {
      delete[] ret;
      return NULL;
   }
   return ret;
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
