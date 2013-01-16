#ifndef DIRECTSHOWTEXTURE_PLAYER_H
#define DIRECTSHOWTEXTURE_PLAYER_H

#include "DirectShowTextureRenderer.h"

namespace DirectShowTexture {


class Player
{
protected:
   Player();
public:
   typedef DirectShowTexture::RectInfo RectInfo;

   static HRESULT CreateInstance(IDirect3DDevice9*, D3DPOOL, Player**);
	virtual ~Player();

   enum STATE {
      S_UNLOAD, S_PAUSE, S_RUN, S_ERR,
   };

   // user must call CoInitialize
   virtual bool load(const TCHAR* filename, VideoInfo*) = 0;
   virtual bool unload() = 0;
   virtual bool play() = 0;
   virtual bool pause() = 0;
   virtual bool seek_set(__int64 offset) = 0;
   virtual bool seek_cur(__int64 offset) = 0;
   virtual bool seek_end(__int64 offset) = 0;

   virtual STATE getState() = 0;
   virtual LONGLONG getTime() = 0; //100ns

   virtual HRESULT CreateTexture(IDirect3DTexture9**, RectInfo*) = 0;
   virtual HRESULT SwapTexture(IDirect3DTexture9**, RectInfo*, bool* update) = 0;
   virtual HRESULT TakeTexture(IDirect3DTexture9**, RectInfo*, bool* update) = 0;
   virtual HRESULT ReleaseTexture() = 0;
};		

}

typedef DirectShowTexture::Player TexPlayer;

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 3
 * End:
 */
