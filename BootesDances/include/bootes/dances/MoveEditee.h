#ifndef BOOTESDANCES_MOVE_MOVEEDITEE_H
#define BOOTESDANCES_MOVE_MOVEEDITEE_H

#include "IMoveModel.h"

struct ModelEditee
{
   const IMoveModel* model;
   int joint;
   inline ModelEditee(): model(NULL), joint(-1) { }
   inline ModelEditee(const ModelEditee& r) { operator=(r); }
   inline ModelEditee& operator=(const ModelEditee& r) {
      model = r.model;
      joint = r.joint;
      return *this;
   }
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
