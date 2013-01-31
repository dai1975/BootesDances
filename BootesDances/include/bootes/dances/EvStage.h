#ifndef BOOTESDANCES_EV_STAGE_H
#define BOOTESDANCES_EV_STAGE_H

#include <bootes/lib/booteslib.h>
#include <string>
#include "proto/stage.pb.h"
#include <boost/shared_ptr.hpp>
#include "EvMovie.h"
#include "VideoInfo.h"

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

   EvSearchStageResult() { }
   EvSearchStageResult(const EvSearchStageResult& r) { operator=(r); }
   EvSearchStageResult& operator=(const EvSearchStageResult& r) {
      _result = r._result;
      return *this;
   }
};

// 検索の内容を返す
class EvStageSearched: public ::bootes::lib::framework::EventTmpl< EvStageSearched >
{
public:
   static const char* GetEventName() { return "StageSearched"; }

   boost::shared_ptr< pb::Stage > _stage;

   EvStageSearched() { }
   EvStageSearched(const EvStageSearched& r) { operator=(r); }
   EvStageSearched& operator=(const EvStageSearched& r) {
      _stage = r._stage;
      return *this;
   }
};

// 新規ステージのロードを指示する
class EvLoadStage: public ::bootes::lib::framework::EventTmpl< EvLoadStage >
{
public:
   static const char* GetEventName() { return "LoadStage"; }

   boost::shared_ptr< pb::Stage > _stage;
   std::basic_string< TCHAR > _path;

   EvLoadStage() { }
   ~EvLoadStage() { }
   EvLoadStage(const EvLoadStage& r) { operator=(r); }
   EvLoadStage& operator=(const EvLoadStage& r) {
      _stage = r._stage;
      _path  = r._path;
      return *this;
   }
};

class EvLoadStageResult: public ::bootes::lib::framework::EventTmpl< EvLoadStageResult >
{
public:
   static const char* GetEventName() { return "LoadStageResult"; }

   bool _result;
   std::string _name;
   VideoInfo _videoInfo;

   EvLoadStageResult() { }
   ~EvLoadStageResult() { }
   EvLoadStageResult(const EvLoadStageResult& r) { operator=(r); }
   EvLoadStageResult& operator=(const EvLoadStageResult& r) {
      _result = r._result;
      _videoInfo = r._videoInfo;
      _name = r._name;
      return *this;
   }
};

class EvSaveStage: public ::bootes::lib::framework::EventTmpl< EvSaveStage >
{
public:
   static const char* GetEventName() { return "SaveStage"; }

   std::string _name;

   EvSaveStage() { }
   ~EvSaveStage() { }
   EvSaveStage(const EvSaveStage& r) { operator=(r); }
   EvSaveStage& operator=(const EvSaveStage& r) {
      _name = r._name;
      return *this;
   }
};

class EvSaveStageResult: public ::bootes::lib::framework::EventTmpl< EvSaveStageResult >
{
public:
   static const char* GetEventName() { return "SaveStageResult"; }

   bool _result;
   std::string _name;

   EvSaveStageResult() { }
   ~EvSaveStageResult() { }
   EvSaveStageResult(const EvSaveStageResult& r) { operator=(r); }
   EvSaveStageResult& operator=(const EvSaveStageResult& r) {
      _result = r._result;
      _name = r._name;
      return *this;
   }
};

inline void Register_EvStage(::bootes::lib::framework::EventManager* m)
{
   assert(m->registerEvent< EvSearchStage >());
   assert(m->registerEvent< EvSearchStageResult >());
   assert(m->registerEvent< EvStageSearched >());
   assert(m->registerEvent< EvLoadStage >());
   assert(m->registerEvent< EvLoadStageResult >());
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
