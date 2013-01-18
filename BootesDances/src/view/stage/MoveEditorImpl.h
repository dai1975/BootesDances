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

   bool initStage(MoveSequence*);

public:
   virtual size_t getModels(std::vector< const IMove* >* pRet);
   virtual bool addModel(IMove*, __int64 t0, __int64 t1);

   virtual bool isChainPrev(const IMove*) const;
   virtual bool isChainNext(const IMove*) const;
   virtual bool chainPrev(const IMove*, bool chain);
   virtual bool chainNext(const IMove*, bool chain);

   virtual void editClear();

   virtual bool editeeSelect(const MoveEditee&);
   virtual bool editeeSelected(MoveEditee*) const;
   virtual bool editeeDeleteSelected();

   virtual bool editBegin();
   virtual bool editing(MoveEditee*, IMove**) const;
   virtual bool editCommit();
   virtual void editRollback();

private:
   MoveSequence* _moves;
   MoveEditee _select;
   MoveEditee _edit_src;
   IMove *_edit_move;
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
