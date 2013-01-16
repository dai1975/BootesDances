#include "MoveConverter.h"

IMoveModel* IMoveModel::Realize(const pb::Move* idea)
{
   IMoveModel* rea = NULL;
   if (idea->has_line()) {
      rea = RealizeLine(&idea->line());
   } else if (idea->has_ellipse()) {
      rea = RealizeEllipse(&idea->ellipse());
   } else if (idea->has_spline()) {
      rea = RealizeSpline(&idea->spline());
   } else {
      return NULL;
   }
   if (rea) {
      for (size_t i=0; i < idea->motions_size(); ++i) {
         Motion* m = Motion::Realize(&idea->motions(i));
         static_cast< MoveModel* >(rea)->setMotion(m); //own
      }
      rea->setTime(idea->time0(), idea->time1()); //update motion's duration
   }
   return rea;
}

pb::Move* IMoveModel::Idealize(const IMoveModel* rea)
{
   pb::Move* idea = NULL;
   switch (rea->getType()) {
   case IMoveModel::T_LINE:
      idea = IdealizeLine(static_cast< const MoveModelLine* >(rea));
      break;
   case IMoveModel::T_ELLIPSE:
      idea = IdealizeEllipse(static_cast< const MoveModelEllipse* >(rea));
      break;
   case IMoveModel::T_SPLINE:
      idea = IdealizeSpline(static_cast< const MoveModelSpline* >(rea));
      break;
   }
   if (idea) {
      idea->set_time0(rea->getBeginTime());
      idea->set_time1(rea->getEndTime());

      const Motion* mot;
      const MoveModel* mov = static_cast< const MoveModel* >(rea);
      if (mot = mov->getMotion(Motion::WIIMOTE)) { mot->idealize(idea->add_motions()); }
   }
   return idea;
}

namespace {

   template <typename IT, typename RT>
   void RealizeByPoints(RT* rea, const IT* idea)
   {
      IMoveModel::t_points points;
      points.resize(idea->points_size());

      for (size_t i=0; i<points.size(); ++i) {
         D3DXVECTOR3& v = points[i];
         v.x = idea->points(i).x();
         v.y = idea->points(i).y();
         v.z = 0;
      }
      rea->init(points);
   }

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

}

MoveModelLine* RealizeLine(const pb::Move::Line* idea)
{
   if (idea->points_size() < 2) { return NULL; }

   MoveModelLine* rea = new MoveModelLine();
   RealizeByPoints(rea, idea);
   return rea;
}
pb::Move* IdealizeLine(const MoveModelLine* rea)
{
   pb::Move* idea = new pb::Move();
   IdealizeByPoints(idea->mutable_line(), rea);
   return idea;
}

MoveModelSpline* RealizeSpline(const pb::Move::Spline* idea)
{
   if (idea->points_size() < 3) { return NULL; }
   if (idea->points_size() % 2 != 1) { return NULL; }

   MoveModelSpline* rea = new MoveModelSpline();
   RealizeByPoints(rea, idea);
   return rea;
}
pb::Move* IdealizeSpline(const MoveModelSpline* rea)
{
   pb::Move* idea = new pb::Move();
   IdealizeByPoints(idea->mutable_spline(), rea);
   return idea;
}

MoveModelEllipse* RealizeEllipse(const pb::Move::Ellipse* idea)
{
   MoveModelEllipse *rea = new MoveModelEllipse();

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

pb::Move* IdealizeEllipse(const MoveModelEllipse* rea)
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

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
