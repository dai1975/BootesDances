#ifndef DIRECTSHOWTEXTURE_DATA_H
#define DIRECTSHOWTEXTURE_DATA_H

#include <windows.h>
#include <d3dx9.h>
//WindowsSDK Samples/multimedia/directshow/baseclasses
#include <streams.h>

namespace DirectShowTexture {

struct VideoInfo
{
   LONGLONG max_time;
   LONGLONG tpf_100ns;
   LONG width, height;
};
struct FrameFormat {
	const GUID* media;
	D3DFORMAT d3d;
	int pixelsize;
};
struct RectInfo
{
	BITMAPINFOHEADER bmi;
	UINT texWidth;
	UINT texHeight;
	INT texPitch;
	const FrameFormat* pFormat;
};

}

#endif
/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 3
 * End:
 */
