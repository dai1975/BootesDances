#ifndef BOOTESDANCES_STAGE_ISTAGEMANAGER_H
#define BOOTESDANCES_STAGE_ISTAGEMANAGER_H

#include "ISceneSequencer.h"
#include "IMovePresenter.h"
#include "IMoveEditor.h"
#include "IWiimoteHandler.h"
#include "proto/stage.pb.h"

class IStageManager
{
public:
   virtual bool hasStage() const = 0;
   virtual const pb::Stage* getStage() const = 0;
   virtual ISceneSequencer* getSceneSequencer() = 0;
   virtual IMovePresenter*  getMovePresenter()  = 0;
   virtual IMoveEditor*     getMoveEditor()     = 0;
   virtual IWiimoteHandler* getWiimoteHandler() = 0;
};


#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
