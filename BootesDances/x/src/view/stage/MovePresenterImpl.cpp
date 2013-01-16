#include "MovePresenterImpl.h"

MovePresenterImpl::MovePresenterImpl(bool editable)
{
   _moves = NULL;
   _playing = false;
   _editable = editable;

   _rend_play.setResource(&_rend_res);
   if (_editable) { _rend_edit.setResource(&_rend_res); }

   g_pFnd->getEventManager()->subscribe< EvMoviePlay >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePause >(this);
}

MovePresenterImpl::~MovePresenterImpl()
{
}

bool MovePresenterImpl::init(MoveSequence* moves, IMoveEditor* editor)
{
   _moves = moves;
   if (! _rend_play.init()) { return false; }
   if (_editable) {
      if (! _rend_edit.init(editor)) { return false; }
   }
   _playing = false;
   return true;
}

void MovePresenterImpl::clear()
{
}

void MovePresenterImpl::onEvent(const ::bootes::lib::framework::Event* ev)
{
   int eid = ev->getEventId();
   if (eid == EvMoviePlay::GetEventId()) {
      _playing = true;
   } else if(eid == EvMoviePause::GetEventId()) {
      _playing = false;
   }
}

void MovePresenterImpl::onLostDevice()
{
   _rend_res.onLostDevice();
   _rend_play.onLostDevice();
   if (_editable) { _rend_edit.onLostDevice(); }
}

void MovePresenterImpl::onResetDevice()
{
   _rend_res.onResetDevice();
   _rend_play.onResetDevice();
   if (_editable) { _rend_edit.onResetDevice(); }
}

void MovePresenterImpl::onRender(const Scene* scene, double currentTime, int elapsedTime)
{
   if (! scene->isValid()) { return; }
   if (_editable && !_playing) {
      _rend_edit.onRender(scene, _moves, currentTime, elapsedTime);
   } else {
      _rend_play.onRender(scene, _moves, currentTime, elapsedTime);
   }
}

size_t MovePresenterImpl::getRenderedModels(std::vector<const IMoveModel*>* ret) const
{
   if (_editable && !_playing) {
      return _rend_edit.getRenderedModels(ret);
   } else {
      return 0;
   }
}

bool MovePresenterImpl::presentLocated(float rx, float ry, ModelEditee* o) const
{
   if (_editable && !_playing) {
      return _rend_edit.presentLocated(rx, ry, o);
   } else {
      return false;
   }
}

bool MovePresenterImpl::presentNearest(float rx, float ry, ModelEditee* o) const
{
   if (_editable && !_playing) {
      return _rend_edit.presentNearest(rx, ry, o);
   } else {
      return false;
   }
}

bool MovePresenterImpl::presentNearestEdge(float rx, float ry, ModelEditee* o) const
{
   if (!_editable || _playing) { return false; }
   if (_editable && !_playing) {
      return _rend_edit.presentNearestEdge(rx, ry, o);
   } else {
      return false;
   }
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
