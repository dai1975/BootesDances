#ifndef BOOTESDANCES_STAGE_ISCENESEQUENCER_H
#define BOOTESDANCES_STAGE_ISCENESEQUENCER_H

#include "Scene.h"

class ISceneSequencer
{
public:
   virtual Scene      getScene(bool requireTexture) const = 0;
   virtual StageClock getStageClock() const = 0;
   virtual VideoInfo  getVideoInfo() const = 0;
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
