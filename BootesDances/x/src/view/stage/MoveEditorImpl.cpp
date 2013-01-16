#include "MoveEditorImpl.h"
#include "../../move/MoveSequence.h"
#include "../../motion/WiimoteMotionSimple.h"

MoveEditorImpl::MoveEditorImpl()
{
   _edit_model = NULL;
   editClear();
}

MoveEditorImpl::~MoveEditorImpl()
{ }

bool MoveEditorImpl::init(MoveSequence* moves)
{
   _moves = moves;
   return true;
}

void MoveEditorImpl::clear()
{
   std::vector< IMoveModel* > v;
   _moves->clear(&v);
   for (size_t i=0; i<v.size(); ++i) { delete v[i]; }
   editClear();
}

size_t MoveEditorImpl::getModels(std::vector< const IMoveModel* >* pRet)
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

bool MoveEditorImpl::addModel(IMoveModel* m, __int64 t0, __int64 t1)
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

bool MoveEditorImpl::isChainPrev(const IMoveModel* m) const
{
   MoveSequence::const_iterator i = const_cast<const MoveSequence*>(_moves)->search(m);
   return _moves->isChainPrev(i);
}
bool MoveEditorImpl::isChainNext(const IMoveModel* m) const
{
   MoveSequence::const_iterator i = const_cast<const MoveSequence*>(_moves)->search(m);
   return _moves->isChainNext(i);
}
bool MoveEditorImpl::chainPrev(const IMoveModel* m, bool chain)
{
   MoveSequence::iterator i = _moves->search(m);
   return _moves->chainPrev(i, chain);
}
bool MoveEditorImpl::chainNext(const IMoveModel* m, bool chain)
{
   MoveSequence::iterator i = _moves->search(m);
   return _moves->chainNext(i, chain);
}

void MoveEditorImpl::editClear()
{
   editRollback();
   _select.model = NULL;
}

bool MoveEditorImpl::editeeSelect(const ModelEditee& e)
{
   editRollback();
   _select.model = NULL;
   if (e.model == NULL) { return true; }

   if (_moves->search(e.model) == _moves->end()) { return false; }
   _select.model = e.model;
   _select.joint = e.joint;
   return true;
}

bool MoveEditorImpl::editeeSelected(ModelEditee* o) const
{
   if (_select.model) {
      if (o) {
         *o = _select;
      }
      return true;
   } 
   return false;
}

bool MoveEditorImpl::editeeDeleteSelected()
{
   if (!_select.model) { return false; }
   editRollback();

   MoveSequence::iterator i = _moves->search(_select.model);
   IMoveModel* m = _moves->remove(i);
   if (!m) { return false; }
   delete m;
   return true;
}

bool MoveEditorImpl::editBegin()
{
   if (_select.model == NULL) { return false; }
   editRollback();
   _edit_src   = _select;
   _edit_model = _select.model->clone();
   return true;
}

bool MoveEditorImpl::editing(ModelEditee* oe, IMoveModel** om) const
{
   if (_edit_model) {
      if (oe) { *oe = _edit_src; }
      if (om) { *om = _edit_model; }
      return true;
   }
   return false;
}

bool MoveEditorImpl::editCommit()
{
   if (_edit_model == NULL) { return false; }

   bool ret = true;
   if (_select.model == NULL) {
      _moves->add(_edit_model);
      _select.model = _edit_model;

   } else {
      MoveSequence::iterator i = _moves->search(_edit_src.model);
      IMoveModel* m = _moves->replace(i, _edit_model);
      if (m != NULL) {
         delete m;
         _select.model = _edit_model;
      } else {
         delete _edit_model;
         ret = false;
      }
   }
   _edit_model = NULL;
   _edit_src.model = NULL;
   return ret;
}

void MoveEditorImpl::editRollback()
{
   if (_edit_model != NULL) {
      delete _edit_model;
      _edit_model = NULL;
   }
   _edit_src.model = NULL;
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
