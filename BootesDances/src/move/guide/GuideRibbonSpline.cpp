#include "GuideRibbonSpline.h"
#include "GuideRibbonSpline.pb.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

const std::string GuideRibbonSpline::TYPE = "GuideRibbonSpline";

#define SPLIT_NUM 21 //奇数が良い

GuideRibbonSpline::GuideRibbonSpline()
{
   _edit_points.resize(3);
   _edit_points[0] = D3DXVECTOR3(0.2f, 0.8f, 0.0f);
   _edit_points[1] = D3DXVECTOR3(0.5f, 0.2f, 0.0f);
   _edit_points[2] = D3DXVECTOR3(0.8f, 0.8f, 0.0f);
   _play_points.resize(SPLIT_NUM);
   calcPlayPoints(0, 2);
}

GuideRibbonSpline::GuideRibbonSpline(const GuideRibbonSpline& r)
: GuideRibbon(r)
{
}

GuideRibbonSpline::~GuideRibbonSpline()
{
}

void GuideRibbonSpline::init(const t_points& points)
{
   if (points.size() < 3) {
      _edit_points[0] = D3DXVECTOR3(0.2f, 0.8f, 0.0f);
      _edit_points[1] = D3DXVECTOR3(0.5f, 0.2f, 0.0f);
      _edit_points[2] = D3DXVECTOR3(0.8f, 0.8f, 0.0f);
      _play_points.resize(SPLIT_NUM);

   } else if (points.size() % 2 == 1) {
      _edit_points = points;

   } else {
      _edit_points.resize(points.size() + 1);
      _edit_points.assign(points.begin(), points.end());
      _edit_points.back() = D3DXVECTOR3(0,0,0);
   }

   int n = (_edit_points.size() - 1) / 2;
   _play_points.resize(SPLIT_NUM * n);
   calcPlayPoints(0, _edit_points.size()-1);
}

bool GuideRibbonSpline::idealize(::pb::Guide* pOut) const
{
   ::pb::GuideRibbonSpline obj;
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
      pOut->set_type(GuideRibbonSpline::TYPE);
   }

   return true;
}

bool GuideRibbonSpline::realize(const ::pb::Guide* pIn)
{
   if (pIn->type().compare(GuideRibbonSpline::TYPE) != 0) { return false; }

   ::pb::GuideRibbonSpline idea;
   google::protobuf::io::ArrayInputStream in(pIn->code().data(), pIn->code().size());
   if (! google::protobuf::TextFormat::Parse(&in, &idea)) {
      return false;
   }

   if (idea.points_size() < 3) { return false; }
   if (idea.points_size() % 2 != 1) { return false; }

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

bool GuideRibbonSpline::setOnlinePoint(size_t idx, float x, float y)
{
   D3DXVECTOR3 p(x,y,0);
   size_t i = idx * 2;
   if (_edit_points.size() <= i) { return false; }
   return replace(i, p);
}
bool GuideRibbonSpline::setOfflinePoint(size_t idx, float x, float y)
{
   D3DXVECTOR3 p(x,y,0);
   size_t i = idx * 2 + 1;
   if (_edit_points.size() <= i) { return false; }
   return replace(i, p);
}

bool GuideRibbonSpline::replace(size_t idx, const D3DXVECTOR3& v)
{
   if (! GuideRibbon::replace(idx, v)) { return false; }
   calcPlayPoints(idx);
   return true;
}

bool GuideRibbonSpline::transform(const D3DXMATRIX& mat)
{
   if (! GuideRibbon::transform(mat)) { return false; }
   calcPlayPoints(0, _edit_points.size());
   return true;
}

bool GuideRibbonSpline::insert(size_t idx, const D3DXVECTOR3& v)
{
   size_t beg,end;
   D3DXVECTOR3 points[2];
   if ((idx & 1) == 0) { //オンライン点
      points[0] = v;
      D3DXVECTOR3 p = _edit_points[idx];
      points[1].x = (p.x + v.x)/2;
      points[1].y = (p.y + v.y)/2;
      points[1].z = 0.0f;
      beg = (idx == 0)? 0: idx-2;
      end = (idx == 0)? 2: beg+4;

   } else { //オフライン点。追加も idx==edit_points.size() でここ。
      if (_edit_points.size() <= idx) { idx = _edit_points.size(); }
      points[1] = v;
      D3DXVECTOR3 p = _edit_points[idx-1];
      points[0].x = (p.x + v.x)/2;
      points[0].y = (p.y + v.y)/2;
      points[0].z = 0.0f;
      beg = idx - 1;
      end = (idx == _edit_points.size())? idx+1: beg+4;
   }
   if (! GuideRibbon::insert(idx, points[1])) { return false; }
   if (! GuideRibbon::insert(idx, points[0])) {
      GuideRibbon::erase(idx);
      return false;
   }

   t_points::iterator i = _play_points.begin() + ((beg / 2) * SPLIT_NUM);
   _play_points.insert(i, SPLIT_NUM, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
   calcPlayPoints(beg, end);
   return true;
}
bool GuideRibbonSpline::erase(size_t idx)
{
   D3DXVECTOR3 prev(0.0f, 0.0f, 0.0f);
   if (idx == 0) {
      ;
   } else if ((idx & 1) == 0) { //オンライン点。前後の中点を制御点に残す。
      prev.x = (_edit_points[idx-1].x + _edit_points[idx+1].x)/2;
      prev.y = (_edit_points[idx-1].y + _edit_points[idx+1].y)/2;
      prev.z = 0.0f;
   }

   {
      D3DXVECTOR3 tmp = _edit_points[idx];
      if (! GuideRibbon::erase(idx)) { return false; }
      if (! GuideRibbon::erase(idx)) {
         GuideRibbon::insert(idx, tmp);
         return false;
      }
   }

   if (idx == 0) {
      t_points::iterator ite = _play_points.begin() + 0;
      _play_points.erase(ite, ite+SPLIT_NUM);
      calcPlayPoints(0, 2);
   } else if ((idx & 1) == 0) {
      _edit_points[idx-1] = prev;
      size_t i = (idx - 2) / 2;
      t_points::iterator ite = _play_points.begin() + (i * SPLIT_NUM);
      _play_points.erase(ite, ite+SPLIT_NUM);
      calcPlayPoints(idx-2, i+2);
   } else {
      size_t i = (idx - 1) / 2;
      t_points::iterator ite = _play_points.begin() + (i * SPLIT_NUM);
      _play_points.erase(ite, ite+SPLIT_NUM);
      calcPlayPoints(idx-1, i+2);
   }
   return true;
}

void GuideRibbonSpline::calcPlayPoints(size_t idx)
{
   if (idx < 2) {
      calcPlayPoints(0, 2);
   } else if ((idx & 1) == 0) {
      calcPlayPoints(idx-2, idx+2);
   } else {
      calcPlayPoints(idx-1, idx+1);
   }
}

void GuideRibbonSpline::calcPlayPoints(size_t beg, size_t end)
{
   float ds = 1.0f / (SPLIT_NUM - 1);

   beg = beg & ~1; //最下位ビットをゼロにする。0->0, 1->0, 2->2, 3->2, ...
   end = (end+1) & ~1; //繰上げる。0->0, 1->2, 2->2, 3->4, ...
   if (_edit_points.size() < end) {
      end = _edit_points.size() - 1;
   }
   for (size_t i=beg; i<end; i+=2) {
      size_t pi = (i/2) * SPLIT_NUM;
      const D3DXVECTOR3& p1 = _edit_points[i];
      const D3DXVECTOR3& p6 = _edit_points[i+1];
      const D3DXVECTOR3& p3 = _edit_points[i+2];
      D3DXVECTOR3 p2 = 2*p6 - p1/2 - p3/2;

      _play_points[pi] = p1;
      _play_points[pi + SPLIT_NUM - 1] = p3;
      float s = 0;
      for (size_t j=1; j<SPLIT_NUM-1; ++j) {
         s += ds; //0..1 への等幅パラメータ
         // 中心部ほど密度を高く。y=-x^2, y=x^2 を組み合わせるか
         // x,yを1ずつ平行移動し、半分に縮める
         // 2y-1 = (2x-1)^2
         float u = 2*s - 1.0f;
         float t = (j <= SPLIT_NUM/2)? (-u*u +1.0f)/2.0f: (u*u +1.0f)/2.0f;
         float tt = t*t;
         //x = (1-t)^2 * x1 + 2t(1-t)*x2 + t^2*x3
         //  = (x1 -2*x2 +x3)*t^2 + (-2*x1 +2*x2)*t + x1
         float x = tt*(p1.x -2*p2.x +p3.x) + (-2*p1.x +2*p2.x)*t + p1.x;
         float y = tt*(p1.y -2*p2.y +p3.y) + (-2*p1.y +2*p2.y)*t + p1.y;
         _play_points[pi+j].x = x;
         _play_points[pi+j].y = y;
         _play_points[pi+j].z = 0.0f;
      }
   }
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
