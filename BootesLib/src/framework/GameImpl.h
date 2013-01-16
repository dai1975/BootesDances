#ifndef BOOTES_LIB_SRC_FRAMEWROK_GAMEIMPL_H
#define BOOTES_LIB_SRC_FRAMEWROK_GAMEIMPL_H

#include "../include.h"
#include <bootes/lib/framework/Game.h>
#include <deque>

namespace bootes { namespace lib { namespace framework {

class GameImpl
{
public:
   GameImpl(Game*);
   virtual ~GameImpl();

public:
   void onUpdate(double currentTime, int elapsedTime);
   void addView(View*);
   typedef Game::t_views t_views;
   inline const t_views& getViews() const { return _views; }

private:
   Game* _parent;
   t_views _views;
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
