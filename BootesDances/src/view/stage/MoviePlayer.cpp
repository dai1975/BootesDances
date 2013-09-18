#include "MoviePlayer.h"

MoviePlayer::MoviePlayer(D3DPOOL pool)
{
   _pool = pool;
   _pPlayer = NULL;
   _scene_valid = false;
   _scene_playing = false;
   _scene_pTex  = NULL;
}

MoviePlayer::~MoviePlayer()
{
   if (_scene_pTex) {
      _scene_pTex->Release();
   }
}

void MoviePlayer::clear()
{
   clearSceneTexture();
   pause();
}

StageClock MoviePlayer::getStageClock() const
{
   return _scene_clock;
}

VideoInfo MoviePlayer::getVideoInfo() const
{
   return _scene_videoInfo;
}

Scene MoviePlayer::getScene(bool requireTexture) const
{
   Scene s;
   s.isValid() = _scene_valid;
   s.isPlaying() = _scene_playing;
   if (s.isValid()) {
      s.videoinfo() = _scene_videoInfo;
      s.clock()     = _scene_clock;
      if (requireTexture) {
         if (_scene_pTex) { _scene_pTex->AddRef(); }
         s.texture() = _scene_pTex;
      }
   }
   return s;
}

void MoviePlayer::clearSceneTexture()
{
   if (_scene_pTex) { _scene_pTex->Release(); }
   _scene_pTex = NULL;
}

bool MoviePlayer::load(const TCHAR* filename)
{
   if (_pPlayer == NULL) { return false; }
   _scene_valid = false;
   bool b = load0(filename);
   _scene_valid = true;
   return b;
}

bool MoviePlayer::load0(const TCHAR* filename)
{
   clearSceneTexture();
   DirectShowTexture::VideoInfo v;
   if (! _pPlayer->load(filename, &v)) {
      return false;
   }
   _scene_videoInfo.size_100ns = v.max_time;
   _scene_videoInfo.tpf_100ns  = v.tpf_100ns;
   _scene_videoInfo.width      = v.width;
   _scene_videoInfo.height     = v.height;
   _scene_clock.clock = 0;
   _scene_playing = false;

   if (FAILED(_pPlayer->CreateTexture(&_scene_pTex, &_rect))) {
      clearSceneTexture();
      return false;
   }
   return true;
}

//二重呼び出しのイベント送出防止と、再生開始までのラグを考え、
//Update() などで、「直前の状態と変わったら」イベント投げるべき
bool MoviePlayer::play()
{
   if (! _scene_valid) { return false; }
   if (! _pPlayer->play()) { return false; }
   return true;
}

bool MoviePlayer::pause()
{
   if (! _scene_valid) { return false; }
   if (! _pPlayer->pause()) { return false; }
   return true;
}
/*
bool MoviePlayer::suspend()
{
   if (! _scene_valid) { return false; }
   _resume_play = (_pPlayer->getState() == DirectShowTexture::Player::S_RUN);
   return true;
}
bool MoviePlayer::resume()
{
   if (! _scene_valid) { return false; }
   if (_resume_play) {
      return play();
   }
   return true;
}
*/

bool MoviePlayer::seek_set(__int64 offset)
{
   if (! _scene_valid) { return false; }
   _pPlayer->seek_set(offset);
   return true;
}
bool MoviePlayer::seek_cur(__int64 offset)
{
   if (! _scene_valid) { return false; }
   _pPlayer->seek_cur(offset);
   return true;
}
bool MoviePlayer::seek_end(__int64 offset)
{
   if (! _scene_valid) { return false; }
   _pPlayer->seek_end(offset);
   return true;
}

void MoviePlayer::onLostDevice()
{
   if (_scene_pTex) {
      //m_pTex->Release(); //MANAGED
      //m_pTex = NULL;
   }
}
void MoviePlayer::onResetDevice()
{
   _scene_valid = false;
   HRESULT hr;
   if (_pPlayer == NULL) {
      IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
      hr = DirectShowTexture::Player::CreateInstance(pDev, _pool, &_pPlayer);
      if (FAILED(hr)) {
         return;
      }
   }
   _scene_valid = (_scene_pTex != NULL);
}

void MoviePlayer::onRender(const ::bootes::lib::framework::GameTime* gt)
{
   if ( !_scene_valid || _scene_pTex == NULL || _pPlayer == NULL) {
      _scene_videoInfo.width  = 0;
      _scene_videoInfo.height = 0;
      _scene_clock.clock      = -1;
      return;
   }

   bool update;
   if (FAILED(_pPlayer->SwapTexture(&_scene_pTex, &_rect, &update))) { ; }
   _scene_videoInfo.width  = _rect.bmi.biWidth;
   _scene_videoInfo.height = _rect.bmi.biHeight;
   _scene_clock.clock      = _pPlayer->getTime();
   _scene_playing          = (_pPlayer->getState() == DirectShowTexture::Player::S_RUN);
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
