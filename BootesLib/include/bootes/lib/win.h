#ifndef BOOTES_LIB_WIN_H
#define BOOTES_LIB_WIN_H

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

//#ifndef _WIN32_IE
//#define _WIN32_IE 0x0700
//#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>

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
