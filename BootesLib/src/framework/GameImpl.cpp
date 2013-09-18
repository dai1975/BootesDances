#include "../include.h"
#include "GameImpl.h"

namespace bootes { namespace lib { namespace framework {

GameImpl::GameImpl(Game* parent)
{
   _parent = parent;
}

GameImpl::~GameImpl()
{
   for (t_views::iterator i = _views.begin(); i != _views.end(); ) {
      View* view = *i++;
      delete view;
   }
   _views.clear();
}

void GameImpl::addView(View* v)
{
   _views.push_back(v);
   v->onResetDevice();
}

void GameImpl::onUpdate(const GameTime* gt)
{
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
