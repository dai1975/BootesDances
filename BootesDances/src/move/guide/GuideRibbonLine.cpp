#include "GuideRibbonLine.h"
#include "GuideRibbonLine.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

//const std::string GuideRibbonLine::TYPE = "GuideRibbonLine";

GuideRibbonLine::GuideRibbonLine()
   : GuideRibbon(GuideRibbon::SUBID_LINE)
{
   _edit_points.resize(2);
   init(0.1f, 0.1f, 0.9f, 0.9f);
}

GuideRibbonLine::GuideRibbonLine(const GuideRibbonLine& r)
  : GuideRibbon(r)
{
}

GuideRibbonLine::~GuideRibbonLine()
{
}

/*
bool GuideRibbonLine::idealize(::pb::Guide* pOut) const
{
   ::pb::GuideRibbonLine obj;
   {
      const IGuide::t_points& points = getEditPoints();
      for (size_t i=0; i<points.size(); ++i) {
         pb::Point* p = obj.add_points();
         p->set_x( points[i].x );
         p->set_y( points[i].y );
      }
   }

   {
      google::protobuf::io::StringOutputStream os(pOut->mutable_code());
      if (! google::protobuf::TextFormat::Print(obj, &os)) {
         return false;
      }
      pOut->set_type(GuideRibbonLine::TYPE);
   }

   return true;
}

bool GuideRibbonLine::realize(const ::pb::GuideRibbon& in)
{
   if (in.type().compare(GuideRibbonLine::TYPE) != 0) { return false; }

   ::pb::GuideRibbonLine idea;
   google::protobuf::io::ArrayInputStream is(in.code().data(), in.code().size());
   if (! google::protobuf::TextFormat::Parse(&is, &idea)) {
      return false;
   }

   if (idea.points_size() < 1) { return false; }

   IGuide::t_points points;
   points.resize(idea.points_size());
   for (size_t i=0; i<points.size(); ++i) {
      points[i].x = idea.points(i).x();
      points[i].y = idea.points(i).y();
      points[i].z = 0;
   }
   this->init(points);

   return true;
}
*/

bool GuideRibbonLine::idealize(::pb::GuideRibbon* pOut) const
{
   ::pb::GuideRibbon::Line* idea = pOut->mutable_line();
   {
      const IGuide::t_points& points = getEditPoints();
      for (size_t i=0; i<points.size(); ++i) {
         pb::Point* p = idea->add_points();
         p->set_x( points[i].x );
         p->set_y( points[i].y );
      }
   }
   return true;
}

bool GuideRibbonLine::realize(const ::pb::GuideRibbon& in)
{
   if (! in.has_line()) { return false; }

   const ::pb::GuideRibbon::Line& idea = in.line();
   if (idea.points_size() < 1) { return false; }

   IGuide::t_points points;
   points.resize(idea.points_size());
   for (size_t i=0; i<points.size(); ++i) {
      points[i].x = idea.points(i).x();
      points[i].y = idea.points(i).y();
      points[i].z = 0;
   }
   this->init(points);

   return true;
}


void GuideRibbonLine::init(float x0, float y0, float x1, float y1)
{
   _edit_points[0].x = x0;
   _edit_points[0].y = y0;
   _edit_points[0].z = 0.0f;
   _edit_points[1].x = x1;
   _edit_points[1].y = y1;
   _edit_points[1].z = 0.0f;
   calcPlayPoints();
}
void GuideRibbonLine::init(const t_points& points)
{
   _edit_points = points;
   calcPlayPoints();
}

bool GuideRibbonLine::setBegin(const D3DXVECTOR3& v)
{
   return replace(0, v);
}
bool GuideRibbonLine::setEnd(const D3DXVECTOR3& v)
{
   return replace(1, v);
}

bool GuideRibbonLine::replace(size_t i, const D3DXVECTOR3& v)
{
   if (! GuideRibbon::replace(i, v)) { return false; }
   _play_points[i] = _edit_points[i];
   return true;
}

bool GuideRibbonLine::transform(const D3DXMATRIX& mat)
{
   if (! GuideRibbon::transform(mat)) { return false; }
   calcPlayPoints();
   return true;
}

bool GuideRibbonLine::insert(size_t i, const D3DXVECTOR3& v)
{
   if (! GuideRibbon::insert(i, v)) { return false; }
   calcPlayPoints();
   return true;
}

bool GuideRibbonLine::erase(size_t i)
{
   if (! GuideRibbon::erase(i)) { return false; }
   calcPlayPoints();
   return true;
}

void GuideRibbonLine::calcPlayPoints()
{
   _play_points.resize(0);
   _play_points.reserve( _edit_points.size() );
   std::copy(_edit_points.begin(), _edit_points.end(), std::back_inserter(_play_points));
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
