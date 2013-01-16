#ifndef BOOTESDANCES_VIEW_CEGUITIMEGRID_H
#define BOOTESDANCES_VIEW_CEGUITIMEGRID_H

#include "../../include.h"
#include <map>
#include <string>
#include <list>
#include <bootes/dances/VideoInfo.h>
#include <bootes/dances/IMoveModel.h>
#include <bootes/cegui/Direct3D9GeometryBuffer.h>

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

class TimeGrid: public CEGUI::Window
{
public:
   static const CEGUI::String WidgetTypeName;

   TimeGrid(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~TimeGrid();
   bool init();

   unsigned int getMaxResolution() const;
   unsigned int getMinResolution() const;
   void setResolution(unsigned int r);
   unsigned int getResolution() const;

public:
   void onLoad(const VideoInfo&);

public:
   virtual void drawSelf(const CEGUI::RenderingContext& ctx);

   bool onMouseMove(const CEGUI::EventArgs&);
   bool onMouseUp(const CEGUI::EventArgs&);
   bool onMouseDown(const CEGUI::EventArgs&);
   bool onMouseClick(const CEGUI::EventArgs&);

   bool onMenuChainPrev(const CEGUI::EventArgs&);
   bool onMenuChainNext(const CEGUI::EventArgs&);
   bool onMenuBreakPrev(const CEGUI::EventArgs&);
   bool onMenuBreakNext(const CEGUI::EventArgs&);

private:
   bool showMenu(float x, float y);
   bool hideMenu();

   void drawGrid(const CEGUI::Rect& rect);
   void drawAxis(const CEGUI::Rect& rect);
   void drawText(const char* str, const CEGUI::Rect& rect);
   const IMoveModel* getModel(float x, float y);
   const IMoveModel* getModelEdge(float x, float y, bool* left);
   __int64 getTimeAt(float x);
   CEGUI::Rect getCellRect(const IMoveModel* model);

   enum {
      PIXEL_GRID_X = 200,
      PIXEL_GRID_TEXT_X = 50,
      AXIS_HEIGHT = 12,
   };
   unsigned int _resolution, _max_resolution, _min_resolution;

   CEGUI::PopupMenu* _gridMenu;
   const IMoveModel* _menuModel;

   CEGUI::Texture *_tex_grid, *_tex_timecell;
   ::bootes::cegui::Direct3D9GeometryBuffer* _mygeo;
   typedef std::list< CEGUI::Texture* > t_lTextTex;
   t_lTextTex _lTextTex;

   VideoInfo _videoInfo;
   bool _edit_edge;
   bool _edit_left;
   __int64 _edit_t0;
};

} } }

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
