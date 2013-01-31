#include "Guide.h"

Guide::Guide(int subid)
   : _pMove(NULL)
   , _subid(subid)
{ }

Guide::~Guide()
{ }

Guide::Guide(const Guide& r)
   : _pMove(r._pMove)
   , _subid(r._subid)
{ }

void Guide::setMove(IMove* p)
{
   assert(_pMove == NULL);
   _pMove = p;
}

int Guide::getSubId() const
{
   return _subid;
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
