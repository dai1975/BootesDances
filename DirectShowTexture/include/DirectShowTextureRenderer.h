#ifndef DIRECTSHOWTEXTURE_RENDERER_H
#define DIRECTSHOWTEXTURE_RENDERER_H

#include "DirectShowTextureData.h"
#include <tchar.h>
#include <dshow.h>

namespace DirectShowTexture {


class Renderer: public CBaseVideoRenderer
{
protected:
	Renderer(HRESULT*);
public:
   typedef DirectShowTexture::RectInfo RectInfo;

   //static HRESULT CreateInstance(IDirect3DDevice9*, Renderer**);
	virtual ~Renderer();
		
   virtual HRESULT CreateTexture(IDirect3DTexture9**, RectInfo*) = 0;
   virtual HRESULT SwapTexture(IDirect3DTexture9**, RectInfo*, bool* update) = 0;
   virtual HRESULT TakeTexture(IDirect3DTexture9**, RectInfo*, bool* update) = 0;
   virtual HRESULT ReleaseTexture() = 0;
   virtual void GetInitialVideoInfo(VideoInfo*) = 0;
};		

}

typedef DirectShowTexture::Renderer    TexRenderer;

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 3
 * End:
 */
