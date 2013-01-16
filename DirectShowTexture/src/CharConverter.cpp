#include "CharConverter.h"

#include <locale.h>

namespace DirectShowTexture {

wchar_t* CharConverter::W2W(const wchar_t* src)
{
   size_t len  = _tcslen(src) + 1;
   size_t size = sizeof(wchar_t) * len;
   wchar_t* r = (wchar_t*)malloc(size);
   memcpy(r, src, size);
   return r;
}

char* CharConverter::C2C(const char* src)
{
   size_t len  = strlen(src) + 1;
   size_t size = sizeof(char) * len;
   char* r = (char*)malloc(size);
   memcpy(r, src, size);
   return r;
}

wchar_t* CharConverter::C2W(const char* src)
{
   size_t wsize;
   if (mbstowcs_s(&wsize, NULL, 0, src, 0) != 0) {
      return NULL;
   }
   wchar_t* ret = (wchar_t*)malloc(sizeof(wchar_t)*wsize);
   if (mbstowcs_s(&wsize, ret, wsize, src, wsize) != 0) {
      free(ret);
      return NULL;
   }
   return ret;
}

char* CharConverter::W2C(const wchar_t* src)
{
   size_t msize;
   if (wcstombs_s(&msize, NULL, 0, src, 0) != 0) {
      return NULL;
   }
   char* ret = (char*)malloc(sizeof(char)*msize);
   if (wcstombs_s(&msize, ret, msize, src, msize) != 0) {
      free(ret);
      return NULL;
   }
   return ret;
}

}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
