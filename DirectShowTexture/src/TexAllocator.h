#ifndef MYRENDERER_ALLOCATOR_H
#define MYRENDERER_ALLOCATOR_H

#include "../include/DirectShowTextureRenderer.h"
#include "TexMediaSample.h"

namespace DirectShowTexture {

class TexAllocator: public CBaseAllocator //CMemAllocator
{
public:
	TexAllocator(D3DPOOL, TCHAR *, LPUNKNOWN, HRESULT *);
#if defined(UNICODE)
   TexAllocator(D3DPOOL, char *, LPUNKNOWN, HRESULT *);
#endif
	virtual ~TexAllocator();

	virtual STDMETHODIMP SetProperties(ALLOCATOR_PROPERTIES* pRequest, ALLOCATOR_PROPERTIES* pActual);
	virtual STDMETHODIMP GetBuffer(__deref_out IMediaSample **ppBuffer,
                                  __in_opt REFERENCE_TIME *pStartTime,
                                  __in_opt REFERENCE_TIME *pEndTime,
                                  DWORD dwFlags
                                  );
	virtual STDMETHODIMP ReleaseBuffer(IMediaSample *pBuffer);

	HRESULT CheckMediaType(IDirect3DDevice9* pDev, const CMediaType* p) const;
	HRESULT SetMediaType(IDirect3DDevice9* pDev, const CMediaType* p, IDirect3DTexture9** ppTex);

	inline void GetRectInfo(RectInfo* p) const { *p = m_rectInfo; }
	inline void GetVideoInfo(VideoInfo* p) const { *p = m_videoInfo; }
	HRESULT CreateTexture(IDirect3DTexture9**);
//	HRESULT CreateSurface(IDirect3DSurface9**);

protected:
	void ReallyFree(void);
	virtual void Free(void);
	virtual HRESULT Alloc(void);

private:
	IDirect3DDevice9* m_pDev;
   D3DPOOL m_pool;
   RectInfo m_rectInfo;
   VideoInfo m_videoInfo;
	AM_MEDIA_TYPE* m_pMediaType;
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
