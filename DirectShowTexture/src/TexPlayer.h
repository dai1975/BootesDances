#ifndef MYRENDERER_TEXPLAYER_H
#define MYRENDERER_TEXPLAYER_H

#include "../include/DirectShowTexturePlayer.h"
#include "TexRenderer.h"

namespace DirectShowTexture {

class TexPlayer: public DirectShowTexture::Player
{
public:
   explicit TexPlayer(IDirect3DDevice9*, D3DPOOL pool);
   virtual ~TexPlayer();

   virtual STATE getState();
   virtual LONGLONG getTime();
   virtual bool load(const TCHAR* filename, VideoInfo*);
   virtual bool unload();
   virtual bool play();
   virtual bool pause();
   virtual bool seek_set(__int64 offset);
   virtual bool seek_cur(__int64 offset);
   virtual bool seek_end(__int64 offset);

   virtual HRESULT CreateTexture(IDirect3DTexture9**, RectInfo*);
   virtual HRESULT TakeTexture(IDirect3DTexture9**, RectInfo*, bool* update);
   virtual HRESULT ReleaseTexture();
   virtual HRESULT SwapTexture(IDirect3DTexture9**, RectInfo*, bool* update);

private:
   bool loadW(const wchar_t* filename, VideoInfo*);
   void clear();
   bool seek0(LONGLONG offset);
   DWORD m_rot;

   IGraphBuilder* m_pGraph;
   IMediaControl* m_pControl;
   IMediaSeeking* m_pSeeking;
   TexRenderer* m_pRenderer;
   LONGLONG m_begin, m_end;

   IDirect3DDevice9* m_pDev;
   D3DPOOL m_pool;
};

}

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
