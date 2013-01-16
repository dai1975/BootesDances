#ifndef BOOTESDANCES_EV_MOVIE_H
#define BOOTESDANCES_EV_MOVIE_H

#include <bootes/lib/booteslib.h>
#include <string>

class EvLoadMovie: public ::bootes::lib::framework::EventTmpl< EvLoadMovie >
{
public:
   static const char* GetEventName() { return "LoadMovie"; }

   std::basic_string< TCHAR > _filename;

   EvLoadMovie(): _filename() { }
   EvLoadMovie(const EvLoadMovie& r) { operator=(r); }
   EvLoadMovie& operator=(const EvLoadMovie& r) {
      _filename = r._filename;
      return *this;
   }
};

class EvPlayMovie: public ::bootes::lib::framework::EventTmpl< EvPlayMovie >
{
public:
   static const char* GetEventName() { return "PlayMovie"; }
   EvPlayMovie() { }
   EvPlayMovie(const EvPlayMovie& r) { operator=(r); }
   EvPlayMovie& operator=(const EvPlayMovie& r) { return *this; }
};

class EvPauseMovie: public ::bootes::lib::framework::EventTmpl< EvPauseMovie >
{
public:
   static const char* GetEventName() { return "PauseMovie"; }
   EvPauseMovie() { }
   EvPauseMovie(const EvPauseMovie& r) { operator=(r); }
   EvPauseMovie& operator=(const EvPauseMovie& r) { return *this; }
};

class EvSeekMovie: public ::bootes::lib::framework::EventTmpl< EvSeekMovie >
{
public:
   static const char* GetEventName() { return "SeekMovie"; }
   enum ORIGIN {
      SET, CUR, END,
   };
   ORIGIN  _origin;
   __int64 _offset;
   bool _play;

   EvSeekMovie(): _origin(CUR), _offset(0), _play(false) { }
   EvSeekMovie(ORIGIN origin, __int64 offset): _origin(origin), _offset(offset), _play(false) { }
   EvSeekMovie(const EvSeekMovie& r) { operator=(r); }
   EvSeekMovie& operator=(const EvSeekMovie& r) {
      _origin = r._origin;
      _offset = r._offset;
      _play   = r._play;
      return *this;
   }
};

/*
class EvMovieLoad: public ::bootes::lib::framework::EventTmpl< EvMovieLoad >
{
public:
   static const char* GetEventName() { return "MovieLoad"; }
   EvMovieLoad()
      : _complete(0.0f)
      , _source(NULL)
      , _presenter(NULL)
   { }
   EvMovieLoad(const EvMovieLoad& r) { operator=(r); }
   EvMovieLoad& operator=(const EvMovieLoad& r) {
      _evLoad     = r._evLoad;
      _complete   = r._complete;
      _videoInfo  = r._videoInfo;
      _source     = r._source;
      _presenter  = r._presenter;
      return *this;
   }   

   EvLoadMovie _evLoad;
   float _complete;
   VideoInfo _videoInfo;
   ModelSource *_source;
   ModelPresenter *_presenter;
};
*/

class EvMoviePlay: public ::bootes::lib::framework::EventTmpl< EvMoviePlay >
{
public:
   static const char* GetEventName() { return "MoviePlay"; }
   EvMoviePlay() { }
   EvMoviePlay(const EvMoviePlay& r) { operator=(r); }
   EvMoviePlay& operator=(const EvMoviePlay& r) {
      _time = r._time;
      return *this;
   }

   __int64 _time;
};

class EvMoviePause: public ::bootes::lib::framework::EventTmpl< EvMoviePause >
{
public:
   static const char* GetEventName() { return "MoviePause"; }
   EvMoviePause() { }
   EvMoviePause(const EvMoviePause& r) { operator=(r); }
   EvMoviePause& operator=(const EvMoviePause& r) {
      _time = r._time;
      return *this;
   }

   __int64 _time;
};

class EvMovieSeek: public ::bootes::lib::framework::EventTmpl< EvMovieSeek >
{
public:
   static const char* GetEventName() { return "MovieSeek"; }
   EvMovieSeek() { }
   EvMovieSeek(const EvMovieSeek& r) { operator=(r); }
   EvMovieSeek& operator=(const EvMovieSeek& r) {
      _time = r._time;
      return *this;
   }

   __int64 _time;
};

inline void Register_EvMovie(::bootes::lib::framework::EventManager* m)
{
   assert(m->registerEvent< EvLoadMovie >());
   assert(m->registerEvent< EvPlayMovie >());
   assert(m->registerEvent< EvPauseMovie >());
   //   assert(m->registerEvent< EvMovieLoad >());
   assert(m->registerEvent< EvMoviePlay >());
   assert(m->registerEvent< EvMoviePause >());
   assert(m->registerEvent< EvMovieSeek >());
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
