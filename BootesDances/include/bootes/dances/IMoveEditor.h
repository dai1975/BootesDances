#ifndef BOOTESDANCES_STAGE_IMOVEEDITOR_H
#define BOOTESDANCES_STAGE_IMOVEEDITOR_H

#include "MoveEditee.h"
#include <vector>

class IMoveEditor
{
public:
   virtual size_t getModels(std::vector< const IMoveModel* >*) = 0;
   virtual bool addModel(IMoveModel*, __int64 t0, __int64 t1) = 0;

   virtual bool isChainPrev(const IMoveModel*) const = 0;
   virtual bool isChainNext(const IMoveModel*) const = 0;

   virtual bool chainPrev(const IMoveModel*, bool chain) = 0;
   virtual bool chainNext(const IMoveModel*, bool chain) = 0;

   virtual void editClear() = 0;

   virtual bool editeeSelect(const ModelEditee&) = 0;
   virtual bool editeeSelected(ModelEditee*) const = 0;
   virtual bool editeeDeleteSelected() = 0;

   virtual bool editBegin() = 0;
   virtual bool editing(ModelEditee*, IMoveModel**) const = 0;
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
