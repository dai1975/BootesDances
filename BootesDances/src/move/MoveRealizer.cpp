#include "MoveRealizer.h"
#include "Move.h"
#include "guide/GuideRealizer.h"
#include "guide/GuideRibbonLine.h"
#include "guide/GuideRibbonEllipse.h"
#include "guide/GuideRibbonSpline.h"

namespace {
GuideRibbonLine* RealizeLine(const pb::Move::Line* idea);
//pb::Move* IdealizeLine(const MoveModelLine* rea);

GuideRibbonEllipse* RealizeEllipse(const pb::Move::Ellipse* idea);
//pb::Move* IdealizeEllipse(const MoveModelEllipse* rea);

GuideRibbonSpline* RealizeSpline(const pb::Move::Spline* idea);
//pb::Move* IdealizeSpline(const MoveModelSpline* rea);
}

bool MoveRealizer::Realize(IMove** ppOut, const pb::Move* pIn)
{
   IGuide* guide = NULL;
   if (pIn->has_line()) {
      guide = RealizeLine(&pIn->line());
   } else if (pIn->has_ellipse()) {
      guide = RealizeEllipse(&pIn->ellipse());
   } else if (pIn->has_spline()) {
      guide = RealizeSpline(&pIn->spline());
   } else {
      return false;
   }

   Move* move = new Move();
   if (guide) {
      move->setGuide(guide);
   }
   move->setTime(pIn->time0(), pIn->time1());
   move->setUuid(pIn->uuid().c_str());

   *ppOut = move;
   return true;
}

bool MoveRealizer::Idealize(::pb::Move2* pOut, const IMove* pIn)
{
   if (pIn->getGuide()) {
      GuideRealizer::Idealize(pOut->mutable_guide(), pIn->getGuide());
   }

   pOut->set_uuid(pIn->getUuid());
   pOut->set_time0(pIn->getBeginTime());
   pOut->set_time1(pIn->getEndTime());

   return true;
}

namespace {

   template <typename IT, typename RT>
   void RealizeByPoints(RT* rea, const IT* idea)
   {
      IGuide::t_points points;
      points.resize(idea->points_size());

      for (size_t i=0; i<points.size(); ++i) {
         D3DXVECTOR3& v = points[i];
         v.x = idea->points(i).x();
         v.y = idea->points(i).y();
         v.z = 0;
      }
      rea->init(points);
   }
/*
   template <typename IT, typename RT>
   void IdealizeByPoints(IT* idea, const RT* rea)
   {      
      const IMoveModel::t_points& points = rea->getEditPoints();
      for (size_t i=0; i<points.size(); ++i) {
         pb::Point* p = idea->add_points();
         p->set_x( points[i].x );
         p->set_y( points[i].y );
      }
   }
*/
}

namespace {
GuideRibbonLine* RealizeLine(const pb::Move::Line* idea)
{
   if (idea->points_size() < 2) { return NULL; }

   GuideRibbonLine* rea = new GuideRibbonLine();
   RealizeByPoints(rea, idea);
   return rea;
}
/*
pb::Move* IdealizeLine(const GuideRibbonLine* rea)
{
   pb::Move* idea = new pb::Move();
   IdealizeByPoints(idea->mutable_line(), rea);
   return idea;
}
*/
GuideRibbonSpline* RealizeSpline(const pb::Move::Spline* idea)
{
   if (idea->points_size() < 3) { return NULL; }
   if (idea->points_size() % 2 != 1) { return NULL; }

   GuideRibbonSpline* rea = new GuideRibbonSpline();
   RealizeByPoints(rea, idea);
   return rea;
}
/*
pb::Move* IdealizeSpline(const GuideRibbonSpline* rea)
{
   pb::Move* idea = new pb::Move();
   IdealizeByPoints(idea->mutable_spline(), rea);
   return idea;
}
*/
GuideRibbonEllipse* RealizeEllipse(const pb::Move::Ellipse* idea)
{
   GuideRibbonEllipse *rea = new GuideRibbonEllipse();

   float x  = idea->center().x();
   float y  = idea->center().y();
   float rx = idea->radius().x();
   float ry = idea->radius().y();
   float a0 = idea->angle0();
   float a1 = idea->angle1();
   bool dir = idea->direction();
   rea->init(x,y,rx,ry,a0,a1,dir);

   return rea;
}
/*
pb::Move* IdealizeEllipse(const GuideRibbonEllipse* rea)
{
   pb::Move* idea = new pb::Move();
   pb::Move::Ellipse* p = idea->mutable_ellipse();

   p->mutable_center()->set_x( rea->getCenterX() );
   p->mutable_center()->set_y( rea->getCenterY() );
   p->mutable_radius()->set_x( rea->getRadiusX() );
   p->mutable_radius()->set_y( rea->getRadiusY() );
   p->set_angle0( rea->getBeginAngle() );
   p->set_angle1( rea->getEndAngle() );
   p->set_direction( rea->getDirection() );

   return idea;
}
*/
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
