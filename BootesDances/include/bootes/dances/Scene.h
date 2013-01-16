#ifndef BOOTESDANCES_STAGE_SCENE_H
#define BOOTESDANCES_STAGE_SCENE_H

#include <bootes/lib/booteslib.h> //D3D
#include "StageClock.h"
#include "VideoInfo.h"

class Scene
{
public:
   Scene();
   ~Scene();
   Scene(const Scene&);
   Scene& operator=(const Scene&);

   inline bool isValid() const { return _valid; }
   inline const VideoInfo&   videoinfo() const { return _videoInfo; }
   inline const StageClock&  clock() const { return _clock; }
   inline bool isPlaying() const { return _play; }
   inline IDirect3DTexture9* norefTexture() const { return _pTex; }
   inline IDirect3DTexture9* refTexture() const {
      if (_pTex) { _pTex->AddRef(); }
      return _pTex;
   }

   inline bool& isValid() { return _valid; }
   inline VideoInfo& videoinfo() { return _videoInfo; }
   inline StageClock& clock() { return _clock; }
   inline bool& isPlaying() { return _play; }
   inline IDirect3DTexture9*& texture() { return _pTex; }

private:
   bool _valid, _play;
   IDirect3DTexture9* _pTex;
   VideoInfo _videoInfo;
   StageClock _clock;
};

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
