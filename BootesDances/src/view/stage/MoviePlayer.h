#ifndef BOOTESDANCES_VIEW_STAGE_MOVIEPLAYER_H
#define BOOTESDANCES_VIEW_STAGE_MOVIEPLAYER_H

#include "../../include.h"
#include <bootes/dances/ISceneSequencer.h>
#include <bootes/lib/framework/GameTime.h>
#include <DirectShowTextureExport.h>

class MoviePlayer: public ISceneSequencer
{
public:
   explicit MoviePlayer(D3DPOOL pool); //SYSTEMMEM or MANAGED
   virtual ~MoviePlayer();

   inline D3DPOOL getPool() const { return _pool; }
   void onRender(const ::bootes::lib::framework::GameTime* gt);
   void onLostDevice();
   void onResetDevice();

public:
   inline bool isValid() const { return _scene_valid; }
   virtual Scene getScene(bool requireTexture) const;
   virtual StageClock getStageClock() const;
   virtual VideoInfo  getVideoInfo() const;
   void clear();

   bool load(const TCHAR* filename);
   bool play();
   bool pause();
//   bool suspend();
//   bool resume();
   bool seek_set(__int64 offset);
   bool seek_cur(__int64 offset);
   bool seek_end(__int64 offset);

private:
   bool load0(const TCHAR* filename);
   void clearSceneTexture();
   bool _scene_valid, _scene_playing;
   IDirect3DTexture9* _scene_pTex;
   VideoInfo _scene_videoInfo;
   StageClock _scene_clock;

   D3DPOOL _pool;
   DirectShowTexture::Player* _pPlayer;
   DirectShowTexture::RectInfo _rect;
//   bool _resume_play;
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
