#ifndef MYRENDERER_CHARCONVERTER_H
#define MYRENDERER_CHARCONVERTER_H

#include <windows.h>
#include <tchar.h>
#include <wchar.h>

namespace DirectShowTexture {

class CharConverter
{
public:
   static wchar_t* C2W(const char* src);
   static char*    W2C(const WCHAR* src);
   static char*    C2C(const char* src);
   static wchar_t* W2W(const wchar_t* src);

#if defined(_UNICODE)
   inline static wchar_t* T2W(const TCHAR* src) { return W2W(src); }
   inline static TCHAR*   W2T(const WCHAR* src) { return W2W(src); }
   inline static char*    T2C(const TCHAR* src) { return W2C(src); }
   inline static TCHAR*   C2T(const  char* src) { return C2W(src); }
#else
   inline static wchar_t* T2W(const TCHAR* src) { return C2W(src); }
   inline static TCHAR*   W2T(const WCHAR* src) { return W2C(src); }
   inline static char*    T2C(const TCHAR* src) { return C2C(src); }
   inline static TCHAR*   C2T(const  char* src) { return C2C(src); }
#endif
};
}

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
