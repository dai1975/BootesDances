#include "TexInputPin.h"

namespace DirectShowTexture {

//--------------------------------------------------------------------
TexInputPin::TexInputPin(D3DPOOL pool,
                         CBaseRenderer *pRenderer,
                         CCritSec *pInterfaceLock,
                         HRESULT *phr,
                         LPCWSTR name)
: CRendererInputPin(pRenderer, phr, name)
{
	m_pAllocator = NULL;
	m_pTexAllocator = NULL;

	if (*phr != S_OK) { return; }

	m_pInterfaceLock = pInterfaceLock;
	m_bActive = false;

	m_pTexAllocator = new TexAllocator(pool, NAME("TexAllocator"), NULL, phr);
	if (*phr != S_OK) {
		m_pTexAllocator = NULL;
		return;
	}
	m_pTexAllocator->AddRef();

	// m_pAllocator は、「出力ピンが決定した、実際に使われるアロケータ」を格納する
	// ここでセットしても最初に BreakConnect が呼ばれ、解放されてしまう。
	// 明示的に GetAllocator でセットする。
	m_pAllocator = NULL;
}

TexInputPin::~TexInputPin()
{
	if (m_pTexAllocator) {
		m_pTexAllocator->Release();
		m_pTexAllocator = NULL;
	}
}


HRESULT TexInputPin::GetMediaType(int iPosition, __inout CMediaType *pMediaType)
{
   return CRendererInputPin::GetMediaType(iPosition, pMediaType);
}


STDMETHODIMP TexInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES* pProps)
{
	pProps->cbAlign = 1;
	pProps->cbPrefix = 0;
	return S_OK;
}

STDMETHODIMP TexInputPin::GetAllocator(__deref_out IMemAllocator **ppAllocator)
{
#if 0 //デフォルトアロケータを使う場合
	return CBaseInputPin::GetAllocator(ppAllocator);
#else
    CheckPointer(ppAllocator,E_POINTER);
    ValidateReadWritePtr(ppAllocator,sizeof(IMemAllocator *));
    CAutoLock cObjectLock(m_pLock);

    // TexPlayer#Connect() の RenderStream からここに来た時に、SEGV することがある
    if (m_pAllocator == NULL) {
		 // m_pAllocator は、「出力ピンが決定した、実際に使われるアロケータ」を格納する
		 // NotifyAllocator で自身の推薦と異なるアロケータが来たら、置き換えられる
		 // 置き換える時、元々の m_pAllocator は Release される
		 // 置き換わらない場合はそのままで、最終的にデストラクタで Release される
		 // m_pAllocator の扱いはこれに従い、1カウント付けといて参照解放は任せる。
		 // 本当に解放するのはこのクラスの仕事にして、m_pTexAllocator で参照しとく。
		 m_pAllocator = m_pTexAllocator;
    }
    ASSERT(m_pAllocator != NULL);
    *ppAllocator = m_pAllocator;
    m_pAllocator->AddRef();
    return NOERROR;
#endif
}

bool TexInputPin::isTexAllocated() const
{
	return m_bActive;
}

STDMETHODIMP TexInputPin::NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly)
{
	CAutoLock cInterfaceLock(m_pInterfaceLock);

	HRESULT hr = CBaseInputPin::NotifyAllocator(pAllocator, bReadOnly);
	if(FAILED(hr)){
		return hr;
	}

	m_bActive = (pAllocator == m_pTexAllocator);

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
