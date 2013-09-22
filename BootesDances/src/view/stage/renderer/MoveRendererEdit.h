#ifndef BOOTESDANCES_VIEW_STAGE_MOVERENDEREREDIT_H
#define BOOTESDANCES_VIEW_STAGE_MOVERENDEREREDIT_H

#include "../../../include.h"
#include "MoveRenderer.h"
#include <vector>
#include <bootes/dances/IMovePresenter.h> //for types used by virtual functions
#include <bootes/dances/IMoveEditor.h>

class MoveRendererEdit: public MoveRenderer
{
public:
   MoveRendererEdit();
   virtual ~MoveRendererEdit();

   virtual bool init(IMoveEditor*);
   virtual void clear();

public:
   virtual void onLostDevice();
   virtual void onResetDevice();
   virtual void onRender(const ::bootes::lib::framework::GameTime* gt, const Scene* scene, const MoveSequence* moves);

public:
   size_t getRenderedModels(std::vector<const IMove*>* v) const;
   bool presentLocated(float rx, float ry, MoveEditee* o) const;
   bool presentNearest(float rx, float ry, MoveEditee* o) const;
   bool presentNearestEdge(float rx, float ry, MoveEditee* o) const;

private:
   struct Rendered {
      const IMove* pMove;
      std::vector< TriangleVertex > ribbon_vtx;
      std::vector< PointVertex > edit_vtx;
      Rendered(): pMove(NULL) { }
      Rendered(const Rendered& r): pMove(r.pMove) {
         std::copy(r.ribbon_vtx.begin(), r.ribbon_vtx.end(), ribbon_vtx.begin());
         std::copy(r.edit_vtx.begin(), r.edit_vtx.end(), edit_vtx.begin());
      }
   };
   typedef std::vector< Rendered > t_rendered;
   t_rendered _rendered;
   int _width, _height;
   IMoveEditor *_pEditor;
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
