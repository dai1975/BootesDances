#include <bootes/dances/Scene.h>

Scene::Scene()
   : _valid(false)
   , _pTex(NULL)
   , _videoInfo()
   , _clock()
   , _play(false)
{ }

Scene::Scene(const Scene& r)
   : _valid(false)
   , _pTex(NULL)
   , _videoInfo()
   , _clock()
{
   operator=(r);
}

Scene::~Scene()
{
   if (_pTex) {
      _pTex->Release();
   }
}

Scene& Scene::operator=(const Scene& r)
{
   _valid     = r._valid;
   _videoInfo = r._videoInfo;
   _clock     = r._clock;
   _play      = r._play;

   if (_pTex) { _pTex->Release(); }
   _pTex = r._pTex;
   if (_pTex) { _pTex->AddRef(); }
   
   return *this;
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
