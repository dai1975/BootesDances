#include "Guide.h"

Guide::Guide()
   : _pMove(NULL)
{ }

Guide::~Guide()
{ }

Guide::Guide(const Guide& r)
   : _pMove(r._pMove)
{ }

void Guide::setMove(IMove* p)
{
   assert(_pMove == NULL);
   _pMove = p;
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
