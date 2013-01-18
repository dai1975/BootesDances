#include "MoveEditorImpl.h"
#include "../../move/MoveSequence.h"

MoveEditorImpl::MoveEditorImpl()
{
   _edit_move = NULL;
   editClear();
}

MoveEditorImpl::~MoveEditorImpl()
{ }

bool MoveEditorImpl::initStage(MoveSequence* moves)
{
   _moves = moves;
   editClear();
   return true;
}
/*
void MoveEditorImpl::clear()
{
   std::vector< IMove* > v;
   _moves->clear(&v);
   for (size_t i=0; i<v.size(); ++i) { delete v[i]; }
   editClear();
}
*/

size_t MoveEditorImpl::getModels(std::vector< const IMove* >* pRet)
{
   size_t n = _moves->size();
   if (pRet == NULL) {
      return n;
   }
   pRet->resize(n);
   
   size_t i = 0;
   for (MoveSequence::iterator ite = _moves->begin(); ite != _moves->end(); ++ite) {
      pRet->at(i++) = *ite;
   }
   return n;
}

bool MoveEditorImpl::addModel(IMove* m, __int64 t0, __int64 t1)
{
   MoveSequence::iterator i;
   for (i = _moves->begin(); i != _moves->end(); ++i) {
      __int64 s0,s1;
      (*i)->getTime(&s0,&s1);
      if (t0 < s0) {
         if (s0 < t1) {
            t1 = s0;
         }
         break;
      }
   }
   m->setTime(t0,t1);

   return (_moves->add(m) != _moves->end());
}

bool MoveEditorImpl::isChainPrev(const IMove* m) const
{
   MoveSequence::const_iterator i = const_cast<const MoveSequence*>(_moves)->search(m);
   return _moves->isChainPrev(i);
}
bool MoveEditorImpl::isChainNext(const IMove* m) const
{
   MoveSequence::const_iterator i = const_cast<const MoveSequence*>(_moves)->search(m);
   return _moves->isChainNext(i);
}
bool MoveEditorImpl::chainPrev(const IMove* m, bool chain)
{
   MoveSequence::iterator i = _moves->search(m);
   return _moves->chainPrev(i, chain);
}
bool MoveEditorImpl::chainNext(const IMove* m, bool chain)
{
   MoveSequence::iterator i = _moves->search(m);
   return _moves->chainNext(i, chain);
}

void MoveEditorImpl::editClear()
{
   editRollback();
   _select.pMove = NULL;
}

bool MoveEditorImpl::editeeSelect(const MoveEditee& e)
{
   editRollback();
   _select.pMove = NULL;
   if (e.pMove == NULL) { return true; }

   if (_moves->search(e.pMove) == _moves->end()) { return false; }
   _select.pMove = e.pMove;
   _select.joint = e.joint;
   return true;
}

bool MoveEditorImpl::editeeSelected(MoveEditee* o) const
{
   if (_select.pMove) {
      if (o) {
         *o = _select;
      }
      return true;
   } 
   return false;
}

bool MoveEditorImpl::editeeDeleteSelected()
{
   if (!_select.pMove) { return false; }
   editRollback();

   MoveSequence::iterator i = _moves->search(_select.pMove);
   IMove* m = _moves->remove(i);
   if (!m) { return false; }
   delete m;
   return true;
}

bool MoveEditorImpl::editBegin()
{
   if (_select.pMove == NULL) { return false; }
   editRollback();
   _edit_src   = _select;
   _edit_move = _select.pMove->clone();
   return true;
}

bool MoveEditorImpl::editing(MoveEditee* oe, IMove** om) const
{
   if (_edit_move) {
      if (oe) { *oe = _edit_src; }
      if (om) { *om = _edit_move; }
      return true;
   }
   return false;
}

bool MoveEditorImpl::editCommit()
{
   if (_edit_move == NULL) { return false; }

   bool ret = true;
   if (_select.pMove == NULL) {
      _moves->add(_edit_move);
      _select.pMove = _edit_move;

   } else {
      MoveSequence::iterator i = _moves->search(_edit_src.pMove);
      IMove* m = _moves->replace(i, _edit_move);
      if (m != NULL) {
         delete m;
         _select.pMove = _edit_move;
      } else {
         delete _edit_move;
         ret = false;
      }
   }
   _edit_move = NULL;
   _edit_src.pMove = NULL;
   return ret;
}

void MoveEditorImpl::editRollback()
{
   if (_edit_move != NULL) {
      delete _edit_move;
      _edit_move = NULL;
   }
   _edit_src.pMove = NULL;
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
