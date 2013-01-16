#ifndef DIRECTSHOWTEXTURERENDERER_LINK_H
#define DIRECTSHOWTEXTURERENDERER_LINK_H

// DirectX
//#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

// DirectShow
//#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")

//WindowsSDK Samples/multimedia/directshow/baseclasses
//#include <streams.h>
#if defined(NDEBUG)
#pragma comment(lib, "strmbase.lib")
#else
#pragma comment(lib, "strmbasd.lib")
#endif

#include "DirectShowTexturePlayer.h"
#pragma comment(lib, "DirectShowTexture.lib")

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 3
 * End:
 */
