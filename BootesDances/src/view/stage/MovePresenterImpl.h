#ifndef BOOTESDANCES_VIEW_STAGE_MOVEPRESENTERIMPL_H
#define BOOTESDANCES_VIEW_STAGE_MOVEPRESENTERIMPL_H

#include "../../include.h"
#include "renderer/MoveRendererResource.h"
#include "renderer/MoveRendererPlay.h"
#include "renderer/MoveRendererEdit.h"
#include <bootes/dances/EvMovie.h>
#include <bootes/dances/IMoveEditor.h>
#include "../../move/MoveSequence.h"

class MovePresenterImpl: public IMovePresenter, public ::bootes::lib::framework::EventListener
{
public:
   explicit MovePresenterImpl(bool editable);
   virtual ~MovePresenterImpl();

   virtual bool initStage(MoveSequence*, IMoveEditor*);
   //void clear();

 public:
   void onLostDevice();
   void onResetDevice();
   void onRender(const ::bootes::lib::framework::GameTime* gt, const Scene* scene);

 public:
   virtual size_t getRenderedModels(std::vector<const IMove*>*) const;
   virtual bool   presentLocated(float rx, float ry, MoveEditee* o) const;
   virtual bool   presentNearest(float rx, float ry, MoveEditee* o) const;
   virtual bool   presentNearestEdge(float rx, float ry, MoveEditee* o) const;

 protected:
   virtual void onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev);

 private:
   bool _editable;
   bool _playing;
   MoveSequence* _moves;

   MoveRendererResource _rend_res;
   MoveRendererPlay _rend_play;
   MoveRendererEdit _rend_edit;
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
