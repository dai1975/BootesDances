#ifndef BOOTES_LIB_FRAMEWORK_GAME_H
#define BOOTES_LIB_FRAMEWORK_GAME_H

#include "macros.h"
#include "View.h"
#include <deque>

namespace bootes { namespace lib { namespace framework {

class Game
{
public:
   Game();
   virtual ~Game();

public:
   virtual void onUpdate(double currentTime, int elapsedTime);

   typedef std::deque< View* > t_views;
   const t_views getViews() const;
   void addView(View*);

private:
   void* _data;
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
