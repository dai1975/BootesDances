#ifndef MYRENDERER_TEXRENDERER_H
#define MYRENDERER_TEXRENDERER_H

#include "../include/DirectShowTextureRenderer.h"
#include "TexInputPin.h"

namespace DirectShowTexture {

class TexRenderer : public DirectShowTexture::Renderer
{
public:
   explicit TexRenderer(IDirect3DDevice9*, D3DPOOL pool, HRESULT *phr);
	virtual ~TexRenderer();

   virtual void GetInitialVideoInfo(VideoInfo*);

   virtual HRESULT CreateTexture(IDirect3DTexture9**, RectInfo*);
   virtual HRESULT TakeTexture(IDirect3DTexture9**, RectInfo*, bool* update);
   virtual HRESULT ReleaseTexture();
   virtual HRESULT SwapTexture(IDirect3DTexture9**, RectInfo*, bool* update);

	inline bool IsTexAllocated() const { return m_pTexInputPin->isTexAllocated(); }
   inline TexInputPin* GetTexInputPin() { return m_pTexInputPin; }

protected:
	//CBaseRendererのオーバーライド
	virtual HRESULT DoRenderSample(IMediaSample *pMediaSample);
	virtual void OnReceiveFirstSample(IMediaSample *pMediaSample);
	virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT SetMediaType(const CMediaType *pMediaType);
	virtual HRESULT PrepareReceive(IMediaSample *pMediaSample);

private:
   HRESULT DoRenderSample_Tex(IMediaSample *pMediaSample);
   HRESULT TakeTexture_Tex(IDirect3DTexture9**, RectInfo*, bool* update);
   HRESULT ReleaseTexture_Tex();
   HRESULT SwapTexture_Tex(IDirect3DTexture9**, RectInfo*, bool* update);

	HRESULT DoRenderSample_Default(IMediaSample *pMediaSample);
   HRESULT TakeTexture_Default(IDirect3DTexture9**, RectInfo*, bool* update);
   HRESULT ReleaseTexture_Default();
   HRESULT SwapTexture_Default(IDirect3DTexture9**, RectInfo*, bool* update);

	IDirect3DDevice9* m_pDev;
   TexInputPin* m_pTexInputPin; //m_pInputPin の名は親が使っている

   HANDLE m_mutex;
   VideoInfo m_initialInfo;
	IMediaSample* m_pLatestSample;
   int m_iTake;
   bool m_update;
   struct Tex {
      RectInfo rectInfo;
      //REFERENCE_TIME time;
      IDirect3DTexture9* pTex;
   } m_tex[2];
//	IDirect3DSurface9* m_pSurface;
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
