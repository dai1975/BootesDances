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
   virtual void onRender(const Scene* scene, const MoveSequence* moves, double currentTime, int elapsedTime);

public:
   size_t getRenderedModels(std::vector<const IMoveModel*>* v) const;
   bool presentLocated(float rx, float ry, ModelEditee* o) const;
   bool presentNearest(float rx, float ry, ModelEditee* o) const;
   bool presentNearestEdge(float rx, float ry, ModelEditee* o) const;

private:
   struct Rendered {
      const IMoveModel* model;
      std::vector< TriangleVertex > ribbon_vtx;
      std::vector< PointVertex > edit_vtx;
      Rendered(): model(NULL) { }
      Rendered(const Rendered& r):model(r.model) {
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
