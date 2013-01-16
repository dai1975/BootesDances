#include "texMediaSample.h"

#define RENDER_SURFACE 1

namespace DirectShowTexture {

//--------------------------------------------------------------------
TexMediaSample::TexMediaSample(
  __in_opt LPCTSTR pName,
  __in_opt CBaseAllocator *pAllocator,
  __inout_opt HRESULT *phr,
  IDirect3DTexture9* pTex)
//  LONG length)
: CMediaSample(pName, pAllocator, phr, NULL, 0)
{
	if (pTex == NULL) { *phr = E_FAIL; return; }
	if (FAILED(*phr)) { return; }
	m_pTex = pTex;
	m_pTex->AddRef();
}

#ifdef UNICODE
TexMediaSample::TexMediaSample(
        __in_opt LPCSTR pName,
        __in_opt CBaseAllocator *pAllocator,
        __inout_opt HRESULT *phr,
		  IDirect3DTexture9* pTex)
//        LONG length)
: CMediaSample(pName, pAllocator, phr, NULL, 0)
{
	if (pTex == NULL) { *phr = E_FAIL; return; }
	if (FAILED(*phr)) { return; }
	m_pTex = pTex;
	m_pTex->AddRef();
}
#endif

TexMediaSample::~TexMediaSample()
{
	m_pTex->Release();
}

bool TexMediaSample::IsLocked() const
{
	return (m_pBuffer != NULL);
}

HRESULT TexMediaSample::LockTexture(RectInfo* pInfo)
{
   HRESULT hr;
   hr = m_pTex->LockRect(0, &m_lockedRect, NULL, D3DLOCK_NOSYSLOCK);
	if (FAILED(hr)) { return hr; }
	ASSERT(pInfo->texPitch == m_lockedRect.Pitch);
	SetPointer((BYTE*)m_lockedRect.pBits, pInfo->texHeight * m_lockedRect.Pitch);
	return hr;
}

HRESULT TexMediaSample::UnlockTexture()
{
	if (m_pBuffer == NULL) { return S_OK; }
	HRESULT hr = m_pTex->UnlockRect(0);
	if (FAILED(hr)) { return hr; }
	m_pBuffer = NULL;
	return S_OK;
}

HRESULT TexMediaSample::UnlockAndReplaceTexture(IDirect3DTexture9** pp)
{
	if (m_pBuffer == NULL) { return S_OK; }

	HRESULT hr = m_pTex->UnlockRect(0);
	if (FAILED(hr)) { return hr; }
	m_pBuffer = NULL;

	if (*pp == NULL) {
		return E_FAIL;
	}

	//参照カウントはいじらない
	IDirect3DTexture9* tmp = m_pTex;
	m_pTex = *pp;
	*pp = tmp;
	return S_OK;
}

}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 3
 * End:
 */
