#include "../include.h"
#include <bootes/lib/framework/Game.h>
#include "GameImpl.h"

namespace bootes { namespace lib { namespace framework {

Game::Game()
{
   _data = new GameImpl(this);
}

Game::~Game()
{
   delete static_cast< GameImpl* >(_data);
}

void Game::onUpdate(const GameTime* gt)
{
   static_cast< GameImpl* >(_data)->onUpdate(gt);
}

void Game::addView(View* v)
{
   static_cast< GameImpl* >(_data)->addView(v);
}

const std::deque< View* > Game::getViews() const
{
   return static_cast< const GameImpl* >(_data)->getViews();
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
