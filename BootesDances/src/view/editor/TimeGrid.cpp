#include "TimeGrid.h"
#include <bootes/cegui/DynamicFont.h>
#include <bootes/dances/IStageManager.h>
#include <iostream>
#include <algorithm>
#include <math.h>

namespace bootes { namespace dances { namespace editor {

const CEGUI::String TimeGrid::WidgetTypeName = "BootesDancesTimeGrid";

TimeGrid::TimeGrid(const CEGUI::String& type,
                   const CEGUI::String& name)
: CEGUI::Window(type, name)
{
   CEGUI::Renderer* p = CEGUI::System::getSingleton().getRenderer();
   p->destroyGeometryBuffer(*d_geometry);

   CEGUI::Direct3D9Renderer* p9 = static_cast< CEGUI::Direct3D9Renderer* >(p);
   _mygeo = new ::bootes::cegui::Direct3D9GeometryBuffer(*p9, p9->getDevice());
   d_geometry = _mygeo;
   _tex_grid = &p->createTexture("grid32.png", "imagesets");
   _tex_timecell = &p->createTexture("timecell32.png", "imagesets");
   _resolution = 20;
   _max_resolution = 20;
   _min_resolution = 20;
   _videoInfo.size_100ns = 0;

   _menuModel = NULL;
}

TimeGrid::~TimeGrid()
{
   CEGUI::Renderer* pRenderer = CEGUI::System::getSingleton().getRenderer();
   if (_tex_grid) {
      pRenderer->destroyTexture(*_tex_grid);
      _tex_grid = NULL;
   }
   if (_tex_timecell) {
      pRenderer->destroyTexture(*_tex_timecell);
      _tex_timecell = NULL;
   }
   for (t_lTextTex::iterator i = _lTextTex.begin(); i != _lTextTex.end(); ++i) {
      pRenderer->destroyTexture(*(*i));
   }
   _lTextTex.clear();
   if (_mygeo) {
      delete _mygeo;
      _mygeo = NULL;
      d_geometry = NULL;
   }

   if (_gridMenu) { delete _gridMenu; }
}

bool TimeGrid::init()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   {
      _gridMenu = static_cast< CEGUI::PopupMenu* >(wm.loadWindowLayout("TimeGridContextMenu.layout"));
      _gridMenu->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      this->addChildWindow(_gridMenu);

      wm.getWindow("TimeGrid/ContextMenu/ChainPrev")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&TimeGrid::onMenuChainPrev, this));
      wm.getWindow("TimeGrid/ContextMenu/ChainNext")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&TimeGrid::onMenuChainNext, this));
      wm.getWindow("TimeGrid/ContextMenu/BreakPrev")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&TimeGrid::onMenuBreakPrev, this));
      wm.getWindow("TimeGrid/ContextMenu/BreakNext")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&TimeGrid::onMenuBreakNext, this));
   }

#define SUBSCRIBE(EVENT, METHOD)\
   this->subscribeEvent(\
   CEGUI::DefaultWindow:: EVENT,\
   CEGUI::Event::Subscriber(\
   &TimeGrid:: METHOD, this));

   SUBSCRIBE(EventMouseMove, onMouseMove);
   SUBSCRIBE(EventMouseButtonDown, onMouseDown);
   SUBSCRIBE(EventMouseButtonUp, onMouseUp);
#undef SUBSCRIBE

   return true;
}

/*
 _resolution == n: 一目盛りが n[sec]
*/
unsigned int TimeGrid::getMaxResolution() const
{
   return _max_resolution;
}
unsigned int TimeGrid::getMinResolution() const
{
   return _min_resolution;
}
unsigned int TimeGrid::getResolution() const
{
   return _resolution;
}

void TimeGrid::setResolution(unsigned int r)
{
   if (r < _min_resolution) {
      _resolution = _min_resolution;
   } else if (_max_resolution < r) {
      _resolution = _max_resolution;
   } else {
      _resolution = r;
   }

   {
      float lensec  = ((float)_videoInfo.size_100ns) / (10*1000*1000);
      int nGrid = static_cast< int >(lensec / _resolution) + 1;
      int width = PIXEL_GRID_X * nGrid + PIXEL_GRID_TEXT_X;

      CEGUI::UVector2 size = getMinSize();
      size.d_x = CEGUI::UDim(0, (float)width);
      setSize(size);
   }
}

void TimeGrid::onLoad(const VideoInfo& videoInfo)
{
   _videoInfo = videoInfo;
   {
      float lensec  = ((float)_videoInfo.size_100ns) / (10*1000*1000);
      float pixel   = getPixelSize().d_width;
      float grid_per_screen = pixel / PIXEL_GRID_X; //一度に表示できる目盛りの数
      float max_res         = lensec / grid_per_screen; //一度に全部表示する場合の一目盛りに必要な時間
      _max_resolution = static_cast< unsigned int >(ceil(max_res));

      _min_resolution = 1;
      if (_max_resolution < _min_resolution) {
         _max_resolution = _min_resolution;
      }
      {
         float total_grid = lensec / _min_resolution + 1; //minres で末端までに必要なグリッド数
         int total_pixel  = static_cast<int>(ceil(total_grid * PIXEL_GRID_X));
         setMaxSize(CEGUI::UVector2(CEGUI::UDim(0,total_pixel), CEGUI::UDim(0,total_pixel)));
      }
   }
   setResolution(_max_resolution);
}

void TimeGrid::drawSelf(const CEGUI::RenderingContext& ctx)
{
   CEGUI::Window::drawSelf(ctx);

   if (_mygeo) {
      const CEGUI::Size size = getPixelSize();
      float w = size.d_width;
      float h = size.d_height;

      CEGUI::Rect axis_rect(0, 0, w, AXIS_HEIGHT);
      CEGUI::Rect grid_rect(0, AXIS_HEIGHT, w, h);

      _mygeo->reset();
      CEGUI::Renderer* pRenderer = CEGUI::System::getSingleton().getRenderer();
      for (t_lTextTex::iterator i = _lTextTex.begin(); i != _lTextTex.end(); ++i) {
         pRenderer->destroyTexture(*(*i));
      }
      _lTextTex.clear();

      this->drawGrid(grid_rect);
      this->drawAxis(axis_rect);
   }
}

//! x 座標の示す時刻を返す
/**
 * x = (t * PIXEL_GRID_X) / _resolution;
 * t = x * _resolution / PIXEL_GRID_X;
 */
__int64 TimeGrid::getTimeAt(float x)
{
   float t = x * _resolution / PIXEL_GRID_X;
   t *= 10*1000*1000;
   return static_cast< __int64 >(t);
}

CEGUI::Rect TimeGrid::getCellRect(const IMoveModel* model)
{
   const CEGUI::Size size = getPixelSize();
   float w = size.d_width;
   float h = size.d_height;

   float t0 = ((float)model->getBeginTime()) / (10*1000*1000);
   float t1 = ((float)model->getEndTime()) / (10*1000*1000);
   float px0 = 0 + (t0 * PIXEL_GRID_X) / _resolution;
   float px1 = 0 + (t1 * PIXEL_GRID_X) / _resolution;

   return CEGUI::Rect(px0, AXIS_HEIGHT, px1, AXIS_HEIGHT+h);
}

const IMoveModel* TimeGrid::getModel(float x, float y)
{
   if (! g_pGame->getStageManager()->hasStage()) { return NULL; }

   CEGUI::Point point(x,y);
   std::vector< const IMoveModel* > models;
   g_pGame->getStageManager()->getMoveEditor()->getModels(&models);
   for (size_t i=0; i<models.size(); ++i) {
      CEGUI::Rect r = getCellRect(models[i]);
      if (r.isPointInRect(point)) {
         return models[i];
      }
   }
   return NULL;
}
const IMoveModel* TimeGrid::getModelEdge(float x, float y, bool* left)
{
   if (! g_pGame->getStageManager()->hasStage()) { return NULL; }

   std::vector< const IMoveModel* > models;
   g_pGame->getStageManager()->getMoveEditor()->getModels(&models);

   const IMoveModel* p0 = NULL;
   bool l0 = true;
   float x0;
   for (size_t i=0; i<models.size(); ++i) {
      CEGUI::Rect r = getCellRect(models[i]);
      if (y < r.d_top || r.d_bottom < y) { continue; }
      //近傍に複数の境界線がある場合は、
      //同じ側にある場合は近い方を選び、異なる側にある場合は選択不可とする
      if (r.d_left - 5 <= x && x < r.d_left) { //後方近傍に帯の左側がある
         if (p0 != NULL && x0 < x) { return NULL; } //前方後方ともにあるので選択しない
         if (p0 == NULL) { //すでに存在していた場合、それは必ず前方。そちらを優先。
            p0 = models[i];
            l0 = true;
            x0 = r.d_left;
         }
      }
      if (r.d_left <= x && x < r.d_left + 5) { //前方近傍に帯の左側がある
         if (p0 != NULL && x < x0) { return NULL; }
         if (true) { //自分に近い方、すなわち後方を優先
            p0 = models[i];
            l0 = true;
            x0 = r.d_left;
         }
      }
      if (r.d_right - 5 <= x && x < r.d_right) {
         if (p0 != NULL && x0 < x) { return NULL; } //後方近傍に帯の右側がある
         if (p0 == NULL) {
            p0 = models[i];
            l0 = false;
            x0 = r.d_right;
         }
      }
      if (r.d_right <= x && x < r.d_right + 5) { //前方近傍に帯の左側がある
         if (p0 != NULL && x < x0) { return NULL; }
         if (true) {
            p0 = models[i];
            l0 = false;
            x0 = r.d_right;
         }
      }
   }

   *left = l0;
   return p0;
}

void TimeGrid::drawGrid(const CEGUI::Rect& rect)
{
   if (! g_pGame->getStageManager()->hasStage()) { return; }

   {
      float w = rect.d_right - rect.d_left;
      CEGUI::Rect trect(0.0f, 0.5f, w/PIXEL_GRID_X, 1.5f);
      CEGUI::colour color(0.5f, 0.5f, 0.5f);
      _mygeo->appendBoard(rect, trect, color, _tex_grid);
   }

   std::vector< const IMoveModel* > models;
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   ISceneSequencer* pSceneSeq = g_pGame->getStageManager()->getSceneSequencer();
   pEditor->getModels(&models);

   {
      IMoveModel* pEditModel;
      ModelEditee srcEditee;
      if (! pEditor->editing(&srcEditee, &pEditModel)) {
         srcEditee.model = NULL;
         pEditModel = NULL;
      }

      for (size_t i=0; i<models.size(); ++i) {
         CEGUI::Rect pos = getCellRect(models[i]);
         CEGUI::Rect tpos(0.0f, 0.0f, 1.0f, 1.0f);
         CEGUI::colour color(0.7f, 0.7f, 0.7f);
         if (models[i] == srcEditee.model) {
            color = CEGUI::colour(1.0f, 1.0f, 1.0f);
         }
         _mygeo->appendBoard(pos, tpos, color, _tex_timecell);
      }

      if (pEditModel != NULL) {
         CEGUI::Rect pos = getCellRect(pEditModel);
         CEGUI::Rect tpos(0.0f, 0.0f, 1.0f, 1.0f);
         CEGUI::colour color(1.0f, 0.0f, 0.7f);
         _mygeo->appendBoard(pos, tpos, color, _tex_timecell);
      }
   }

   {
      float t = ((float)pSceneSeq->getStageClock().clock) / (10*1000*1000);
      float px = rect.d_left + (t * PIXEL_GRID_X) / _resolution;

      CEGUI::colour color(1.0f, 0.0f, 0.0f);
      CEGUI::Rect pos(px, rect.d_top, px + 10, rect.d_bottom);
      CEGUI::Rect tpos(0.0f, 0.1f, 0.1f, 0.9f);

      _mygeo->appendBoard(pos, tpos, color, _tex_grid);
   }
}

void TimeGrid::drawAxis(const CEGUI::Rect& rect)
{
   CEGUI::Rect r(rect);

   unsigned int cell_sec = (_resolution < 1)? 1: _resolution; //一目盛りの秒数
   for (size_t i=0; ; ++i) {
      float x0 = (float)(PIXEL_GRID_X * i);
      if (rect.d_right < x0) { break; }

      r.d_left = x0;
      r.d_right = x0 + PIXEL_GRID_TEXT_X;

      unsigned int sec = cell_sec * i;
      unsigned int min = sec / 60;
      sec %= 60;
      char buf[7];
      if (999 < min) {
         strcpy(buf, "---:--");
      } else {
         sprintf_s(buf, 7, "%03d:%02d", min, sec);
      }
      drawText(buf, r);
      //_mygeo->appendText(buf, 6, r); //うまく動かない
   }
}

void TimeGrid::drawText(const char* str, const CEGUI::Rect& rect)
{
   ::bootes::lib::d3dx9::ResourceView* res = g_pFnd->getDxResource();
   if (res == NULL) { return; }

   size_t len = strlen(str);
   if (len < 1) { return; }

   CEGUI::Direct3D9Renderer* pRenderer;
   {
      CEGUI::Renderer* p = CEGUI::System::getSingleton().getRenderer();
      pRenderer = static_cast< CEGUI::Direct3D9Renderer* >(p);
   }

   float w = rect.d_right - rect.d_left;
   float y0 = rect.d_top;
   float y1 = rect.d_bottom;
   for (size_t i=0; i<len; ++i) {
      ::bootes::lib::d3dx9::Font font;
      CEGUI::Texture* tex;
      {
         if (! res->getFontTexture(str[i], &font)) {
            continue; 
         }
         //_lTextTex.push_back(font.pTex);
         tex = &pRenderer->createTexture(font.pTex);
         _lTextTex.push_back(tex); //delete用。内部の D3DTexture の ref 管理も CEGUI::Texture がやる
      }

      float x0 = w * (i+0) / len + rect.d_left;
      float x1 = w * (i+1) / len + rect.d_left;
#if 1
      CEGUI::Rect pos(x0,y0,x1,y1);
      CEGUI::Rect tpos(0.0f, 0.0f, 1.0f, 1.0f);
      CEGUI::colour col(1.0f, 1.0f, 1.0f);
      _mygeo->appendBoard(pos, tpos, col, tex);
#else
      CEGUI::Vertex v[6];
      for (size_t i=0; i<6; ++i) {
         v[i].colour_val = CEGUI::colour(1.0f, 1.0f, 1.0f);
      }
      v[0].position   = CEGUI::Vector3(x0, y1, 0);
      v[0].tex_coords = CEGUI::Vector2(0.0f, 1.0f);
      v[1].position   = CEGUI::Vector3(x0, y0, 0);
      v[1].tex_coords = CEGUI::Vector2(0.0f, 0.0f);
      v[2].position   = CEGUI::Vector3(x1, y1, 0);
      v[2].tex_coords = CEGUI::Vector2(1.0f, 1.0f);
      v[3].position   = v[1].position;
      v[3].tex_coords = v[1].tex_coords;
      v[4].position   = v[2].position;
      v[4].tex_coords = v[2].tex_coords;
      v[5].position   = CEGUI::Vector3(x1, y0, 0);
      v[5].tex_coords = CEGUI::Vector2(1.0f, 0.0f);
      _mygeo->setActiveTexture(tex);
      _mygeo->appendGeometry(&v[0], 6);
      //pRenderer->destroyTexture(*tex); //GeoBuf は IDirect3DTexture9* で保持。ref 操作しない。
#endif
   }
}

namespace {
inline bool _mouse_pos(const CEGUI::Window* win, const CEGUI::MouseEventArgs& e, float* x, float* y, float* rx, float *ry, float *w, float* h)
{
   CEGUI::Size size = win->getPixelSize();
   if (size.d_width < 1 || size.d_height < 1) {
      return false;
   }
   CEGUI::Vector2 pos = 
      CEGUI::CoordConverter::screenToWindow(*win, e.position);
   *x  = pos.d_x;
   *y  = pos.d_y;
   *w = size.d_width;
   *h = size.d_height;
   *rx = pos.d_x / size.d_width;
   *ry = pos.d_y / size.d_height;
   return true;
}
}

bool TimeGrid::onMouseDown(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;
   if (! _mouse_pos(this, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   if (e.button == CEGUI::LeftButton && e.clickCount == 1) {
      const IMoveModel* pModel = getModelEdge(x, y, &_edit_left);
      if (pModel != NULL) {
         _edit_edge = true;
         CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseEsWeCursor");
      } else {
         _edit_edge = false;
         CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
         pModel = getModel(x,y);
      }
      if (pModel) {
         ModelEditee editee;
         editee.model = pModel;
         editee.joint = -1;
         if (! pEditor->editeeSelect(editee)) { return true; }
         if (! pEditor->editBegin()) { return true; }
         _edit_t0 = getTimeAt(x);
      }

   } else if (e.button == CEGUI::RightButton && e.clickCount == 1) {
      if (_menuModel) { hideMenu(); }
      _menuModel = getModel(x, y);
      if (_menuModel != NULL) {
         showMenu(x,y);
      }
   }
   return true;
}

bool TimeGrid::onMouseMove(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;

   if (! _mouse_pos(this, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   IMoveModel* pModel;
   ModelEditee editee;
   if (pEditor->editing(&editee, &pModel)) {
      __int64 t0, t1, dt;
      t1 = getTimeAt(x);
      dt = t1 - _edit_t0;
      editee.model->getTime(&t0, &t1);
      if (! _edit_edge) {
         pModel->setTime(t0+dt, t1+dt);

      } else {
         std::vector< const IMoveModel* > models;
         pEditor->getModels(&models);
         size_t index;
         for (index=0; index<models.size(); ++index) {
            if (models[index] == editee.model) {
               break;
            }
         }
         if (_edit_left) {
            t0 += dt;
            if (0 < index) {
               __int64 edge = models[index-1]->getEndTime();
               if (t0 < edge) { t0 = edge; }
            }
         } else {
            t1 += dt;
            if (index < models.size()-1) {
               __int64 edge = models[index+1]->getBeginTime();
               if (edge < t1) { t1 = edge; }
            }
         }
         pModel->setTime(t0, t1);
      }

   } else {
      bool left;
      const IMoveModel* pModel = getModelEdge(x, y, &left);
      if (pModel != NULL) {
         CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseEsWeCursor");
      } else {
         CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
      }
   }

   return true;
}

bool TimeGrid::onMouseUp(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;

   if (! _mouse_pos(this, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   if (e.button == CEGUI::LeftButton && e.clickCount == 1) {
      IMoveModel* pModel;
      ModelEditee editee;
      if (! pEditor->editing(&editee, &pModel)) { return true; }
      pEditor->editCommit();
   }
   return true;
}

bool TimeGrid::showMenu(float x, float y)
{
   if (_menuModel == NULL) { return true; }

   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   std::vector< const IMoveModel* > models;
   pEditor->getModels(&models);

   size_t index;
   for (index = 0; index<models.size(); ++index) {
      if (models[index] == _menuModel) { break; }
   }
   if (index == models.size()) { return true; }

   {
      CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

      if (index == 0) {
         wm.getWindow("TimeGrid/ContextMenu/ChainPrev")->setEnabled(false);
         wm.getWindow("TimeGrid/ContextMenu/BreakPrev")->setEnabled(false);
      } else {
         bool b = pEditor->isChainPrev(_menuModel);
         wm.getWindow("TimeGrid/ContextMenu/ChainPrev")->setEnabled(!b);
         wm.getWindow("TimeGrid/ContextMenu/BreakPrev")->setEnabled(b);
      }

      if (index+1 == models.size()) {
         wm.getWindow("TimeGrid/ContextMenu/ChainNext")->setEnabled(false);
         wm.getWindow("TimeGrid/ContextMenu/BreakNext")->setEnabled(false);
      } else {
         bool b = pEditor->isChainNext(_menuModel);
         wm.getWindow("TimeGrid/ContextMenu/ChainNext")->setEnabled(!b);
         wm.getWindow("TimeGrid/ContextMenu/BreakNext")->setEnabled(b);
      }
   }

   {
      CEGUI::Size size0 = this->getPixelSize();
      CEGUI::Size size1 = _gridMenu->getPixelSize();
      if (size0.d_width  < x + size1.d_width) { x = size0.d_width - size1.d_width; }
      if (size0.d_height < y + size1.d_height) { y = size0.d_height - size1.d_height; }

      CEGUI::UVector2 pos(CEGUI::UDim(0,x), CEGUI::UDim(0,y));
      _gridMenu->setPosition(pos);
      _gridMenu->show();
   }
   return true;
}
bool TimeGrid::hideMenu()
{
   _gridMenu->hide();
   _menuModel = NULL;
   return true;
}

bool TimeGrid::onMenuChainPrev(const CEGUI::EventArgs&)
{
   if (_menuModel == NULL) { return true; }
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   pEditor->chainPrev(_menuModel, true);
   return false;
}
bool TimeGrid::onMenuChainNext(const CEGUI::EventArgs&)
{
   if (_menuModel == NULL) { return true; }
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   pEditor->chainNext(_menuModel, true);
   return false;
}

bool TimeGrid::onMenuBreakPrev(const CEGUI::EventArgs&)
{
   if (_menuModel == NULL) { return true; }
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   pEditor->chainPrev(_menuModel, false);
   return false;
}
bool TimeGrid::onMenuBreakNext(const CEGUI::EventArgs&)
{
   if (_menuModel == NULL) { return true; }
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   pEditor->chainNext(_menuModel, false);
   return false;
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
