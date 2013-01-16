#ifndef MYRENDERER_MEDIASAMPLE_H
#define MYRENDERER_MEDIASAMPLE_H

#include "../include/DirectShowTextureRenderer.h"

namespace DirectShowTexture {

class TexMediaSample: public CMediaSample
{
public:
   TexMediaSample(
        __in_opt LPCTSTR pName,
        __in_opt CBaseAllocator *pAllocator,
        __inout_opt HRESULT *phr,
		  IDirect3DTexture9* pTex);
//        LONG length = 0);
#ifdef UNICODE
	TexMediaSample(
        __in_opt LPCSTR pName,
        __in_opt CBaseAllocator *pAllocator,
        __inout_opt HRESULT *phr,
		  IDirect3DTexture9* pTex);
//        LONG length = 0);
#endif

	virtual ~TexMediaSample();

	bool IsLocked() const;
	HRESULT LockTexture(RectInfo*);
	HRESULT UnlockTexture();
	HRESULT UnlockAndReplaceTexture(IDirect3DTexture9** ppTex);
//	HRESULT UnlockAndReplaceTexture(IDirect3DSurface9** ppTex);

protected:
	IDirect3DTexture9* m_pTex;
//	IDirect3DSurface9* m_pSurface;
	D3DLOCKED_RECT m_lockedRect;
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
