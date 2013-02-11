#ifndef BOOTESDANCES_EV_STAGE_H
#define BOOTESDANCES_EV_STAGE_H

#include <bootes/lib/booteslib.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include "EvMovie.h"
#include "VideoInfo.h"
#include "Stage.h"

// 検索を実行する
class EvSearchStage: public ::bootes::lib::framework::EventTmpl< EvSearchStage >
{
public:
   static const char* GetEventName() { return "SearchStage"; }

   EvSearchStage() { }
   EvSearchStage(const EvSearchStage& r) { operator=(r); }
   EvSearchStage& operator=(const EvSearchStage& r) {
      return *this;
   }
};

// 検索命令の結果を返す
class EvSearchStageResult: public ::bootes::lib::framework::EventTmpl< EvSearchStageResult >
{
public:
   static const char* GetEventName() { return "SearchStageResult"; }

   bool _result;
   int _index;
   boost::shared_ptr< Stage > _pStage; //NULLなら終端

   EvSearchStageResult() { }
   EvSearchStageResult(const EvSearchStageResult& r) { operator=(r); }
   EvSearchStageResult& operator=(const EvSearchStageResult& r) {
      _result = r._result;
      _index  = r._index;
      _pStage = r._pStage;
      
      return *this;
   }
};

// 新規ステージのロードを指示する
class EvLoadStage: public ::bootes::lib::framework::EventTmpl< EvLoadStage >
{
public:
   static const char* GetEventName() { return "LoadStage"; }

   std::basic_string< TCHAR > _basename;

   EvLoadStage() { }
   ~EvLoadStage() { }
   EvLoadStage(const EvLoadStage& r) { operator=(r); }
   EvLoadStage& operator=(const EvLoadStage& r) {
      _basename  = r._basename;
      return *this;
   }
};

class EvLoadStageResult: public ::bootes::lib::framework::EventTmpl< EvLoadStageResult >
{
public:
   static const char* GetEventName() { return "LoadStageResult"; }

   bool _result;
   std::basic_string< TCHAR > _basename;
   VideoInfo _videoInfo;

   EvLoadStageResult() { }
   ~EvLoadStageResult() { }
   EvLoadStageResult(const EvLoadStageResult& r) { operator=(r); }
   EvLoadStageResult& operator=(const EvLoadStageResult& r) {
      _result = r._result;
      _videoInfo = r._videoInfo;
      _basename = r._basename;
      return *this;
   }
};

class EvNewStage: public ::bootes::lib::framework::EventTmpl< EvNewStage >
{
public:
   static const char* GetEventName() { return "NewStage"; }

   std::basic_string< TCHAR > _moviepath;

   EvNewStage() { }
   ~EvNewStage() { }
   EvNewStage(const EvNewStage& r) { operator=(r); }
   EvNewStage& operator=(const EvNewStage& r) {
      _moviepath = r._moviepath;
      return *this;
   }
};

class EvNewStageResult: public ::bootes::lib::framework::EventTmpl< EvNewStageResult >
{
public:
   static const char* GetEventName() { return "NewStageResult"; }

   bool _result;
   std::basic_string< TCHAR > _basename;
   std::basic_string< TCHAR > _moviepath;
   VideoInfo _videoInfo;

   EvNewStageResult() { }
   ~EvNewStageResult() { }
   EvNewStageResult(const EvNewStageResult& r) { operator=(r); }
   EvNewStageResult& operator=(const EvNewStageResult& r) {
      _result    = r._result;
      _videoInfo = r._videoInfo;
      _basename  = r._basename;
      _moviepath = r._moviepath;
      return *this;
   }
};

class EvSaveStage: public ::bootes::lib::framework::EventTmpl< EvSaveStage >
{
public:
   static const char* GetEventName() { return "SaveStage"; }

   std::basic_string< TCHAR > _basename;
   bool _new;

   EvSaveStage() { }
   ~EvSaveStage() { }
   EvSaveStage(const EvSaveStage& r) { operator=(r); }
   EvSaveStage& operator=(const EvSaveStage& r) {
      _basename = r._basename;
      _new      = r._new;
      return *this;
   }
};

class EvSaveStageResult: public ::bootes::lib::framework::EventTmpl< EvSaveStageResult >
{
public:
   static const char* GetEventName() { return "SaveStageResult"; }

   bool _result;
   std::basic_string<TCHAR> _basename;

   EvSaveStageResult() { }
   ~EvSaveStageResult() { }
   EvSaveStageResult(const EvSaveStageResult& r) { operator=(r); }
   EvSaveStageResult& operator=(const EvSaveStageResult& r) {
      _result   = r._result;
      _basename = r._basename;
      return *this;
   }
};

inline void Register_EvStage(::bootes::lib::framework::EventManager* m)
{
   assert(m->registerEvent< EvSearchStage >());
   assert(m->registerEvent< EvSearchStageResult >());
   assert(m->registerEvent< EvLoadStage >());
   assert(m->registerEvent< EvLoadStageResult >());
   assert(m->registerEvent< EvNewStage >());
   assert(m->registerEvent< EvNewStageResult >());
   assert(m->registerEvent< EvSaveStage >());
   assert(m->registerEvent< EvSaveStageResult >());
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
