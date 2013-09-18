#ifndef BOOTESDANCES_VIEW_STAGE_STAGEMANAGERPROXY_H
#define BOOTESDANCES_VIEW_STAGE_STAGEMANAGERPROXY_H

#include "../../include.h"
#include "../../move/MoveSequence.h"
#include <bootes/dances/Stage.h>
#include "StageRealizer.h"
#include "MoviePlayer.h"
#include "MovePresenterImpl.h"
#include "MoveEditorImpl.h"
#include "WiimoteHandlerImpl.h"
#include <bootes/dances/EvMovie.h>
#include <bootes/dances/EvStage.h>

class StageManagerProxy: public ::bootes::lib::framework::EventListener
{
 public:
   StageManagerProxy();
   virtual ~StageManagerProxy();
   bool init(const TCHAR* dir, bool editable, D3DPOOL pool);

 public:
   inline bool isEnabled() const { return _enabled; }
   //inline ::pb::Stage* getStage() const { return _pStage; }
   IMove* createMove(IGuide* p) const;
   inline ISceneSequencer*   getSceneSequencer() { return _pMoviePlayer; }
   inline IMovePresenter*    getMovePresenter()  { return _pMovePresenter; }
   inline IMoveEditor*       getMoveEditor()     { return _pMoveEditor; }
   inline IWiimoteHandler*   getWiimoteHandler() { return _pWiimoteHandler; }

   inline MoviePlayer*       getMoviePlayer()       { return _pMoviePlayer; }
   inline MovePresenterImpl* getMovePresenterImpl() { return _pMovePresenter; }
   inline MoveSequence*      getMoveSequence()      { return _pSeq; }

 public:
   static DWORD WINAPI ThreadProc(LPVOID);
   bool start(); //!< start thread
   void stopLater(); //!< make thread to be stopped
   void stop(); //!< stopLater and join thread is stop

 private:
   DWORD run(); //!< thread function

 public:
   void onSubscribe(::bootes::lib::framework::EventManager*);
   void onUnsubscribe(::bootes::lib::framework::EventManager*);
   void onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev); //!< receive interesting events and queues it.

 protected:
   bool onEvent0(const ::bootes::lib::framework::Event* ev); //!< receive interesting events and queues it.
   void doSearch(); //!< search stage files and emit SearchStageResult events.
   void doSave(const TCHAR* basename, bool neu); //!< save stage to file and emit a LoadStageResult event.
   void doNew(const TCHAR* path); //!< load movie and create new stage
   void doLoad(const TCHAR* basename); //!< load stage

   void doPlay(const EvPlayMovie*);
   void doPause(const EvPauseMovie*);
   void doSeek(const EvSeekMovie*);

 private:
   bool doLoadMovie(const TCHAR* path, Stage*, MoveSequence*);
   //bool initStage();

   HANDLE _thread_handle;
   DWORD  _thread_id;
   bool   _run;
   HANDLE _mutex;

   ::bootes::lib::framework::Event *_command;

   std::basic_string< TCHAR > _dir;
   std::basic_string< TCHAR > _filename;
   //pb::Stage2* _pStage;
   bool _editable;
   bool _enabled;

   StageRealizer::MotionGuideList _mgl;
   MoveSequence       *_pSeq;
   Stage              *_pStage;
   MoviePlayer        *_pMoviePlayer;
   MovePresenterImpl  *_pMovePresenter;
   MoveEditorImpl     *_pMoveEditor;
   WiimoteHandlerImpl *_pWiimoteHandler;
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
