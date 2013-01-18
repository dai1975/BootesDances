#ifndef BOOTESDANCES_IMOVEEDITOR_H
#define BOOTESDANCES_IMOVEEDITOR_H

#include "MoveEditee.h"
#include <vector>

class IMoveEditor
{
public:
   virtual size_t getModels(std::vector< const IMove* >*) = 0;
   virtual bool addModel(IMove*, __int64 t0, __int64 t1) = 0;

   virtual bool isChainPrev(const IMove*) const = 0;
   virtual bool isChainNext(const IMove*) const = 0;

   virtual bool chainPrev(const IMove*, bool chain) = 0;
   virtual bool chainNext(const IMove*, bool chain) = 0;

   virtual void editClear() = 0;

   virtual bool editeeSelect(const MoveEditee&) = 0;
   virtual bool editeeSelected(MoveEditee*) const = 0;
   virtual bool editeeDeleteSelected() = 0;

   virtual bool editBegin() = 0;
   virtual bool editing(MoveEditee*, IMove**) const = 0;
   virtual bool editCommit() = 0;
   virtual void editRollback() = 0;
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
