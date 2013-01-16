#ifndef MYRENDERER_TEXINPUTPIN_H
#define MYRENDERER_TEXINPUTPIN_H

#include "../include/DirectShowTextureRenderer.h"
#include "TexAllocator.h"

namespace DirectShowTexture {

class TexInputPin : public CRendererInputPin
{
public:
	TexInputPin(D3DPOOL, CBaseRenderer *pRenderer, CCritSec *pInterfaceLock, HRESULT *phr, LPCWSTR name);
	virtual ~TexInputPin();
	bool isTexAllocated() const;

	STDMETHODIMP GetAllocatorRequirements(__out ALLOCATOR_PROPERTIES*pProps);
	STDMETHODIMP GetAllocator(__deref_out IMemAllocator **ppAllocator);
	STDMETHODIMP NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly);

	inline TexAllocator* getTexAllocator() { return m_pTexAllocator; }
	inline const TexAllocator* getTexAllocator() const { return m_pTexAllocator; }

   virtual HRESULT GetMediaType(int iPosition, __inout CMediaType *pMediaType);


private:
	CCritSec *m_pInterfaceLock;
	bool m_bActive;
   TexAllocator* m_pTexAllocator; //m_pAllocator という名は親クラスで使っている
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
