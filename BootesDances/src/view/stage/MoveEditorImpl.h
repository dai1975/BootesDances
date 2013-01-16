#ifndef BOOTESDANCES_VIEW_STAGE_MOVEEDITORIMPL_H
#define BOOTESDANCES_VIEW_STAGE_MOVEEDITORIMPL_H

#include "../../include.h"
#include <bootes/dances/IMoveEditor.h>
#include "../../move/MoveSequence.h"

class MoveEditorImpl: public IMoveEditor
{
public:
   explicit MoveEditorImpl();
   virtual ~MoveEditorImpl();

   bool init(MoveSequence*);
   void clear();

public:
   virtual size_t getModels(std::vector< const IMoveModel* >* pRet);
   virtual bool addModel(IMoveModel*, __int64 t0, __int64 t1);

   virtual bool isChainPrev(const IMoveModel*) const;
   virtual bool isChainNext(const IMoveModel*) const;
   virtual bool chainPrev(const IMoveModel*, bool chain);
   virtual bool chainNext(const IMoveModel*, bool chain);

   virtual void editClear();

   virtual bool editeeSelect(const ModelEditee&);
   virtual bool editeeSelected(ModelEditee*) const;
   virtual bool editeeDeleteSelected();

   virtual bool editBegin();
   virtual bool editing(ModelEditee*, IMoveModel**) const;
   virtual bool editCommit();
   virtual void editRollback();

private:
   MoveSequence* _moves;
   ModelEditee _select;
   ModelEditee _edit_src;
   IMoveModel *_edit_model;
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
