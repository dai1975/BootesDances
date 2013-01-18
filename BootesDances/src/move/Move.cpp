#include "Move.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

Move::Move()
{
   _t0 = 0;
   _t1 = 0;
   _pGuide = NULL;
   _pMotion = NULL;
   genUuid();
}

Move::Move(const Move& r)
{
   _uuid = r._uuid;
   _t0 = r._t0;
   _t1 = r._t1;

   if (_pGuide == NULL) {
      _pGuide = NULL;
   } else {
      _pGuide = _pGuide->clone();
      static_cast< Guide* >(_pGuide)->setMove(this);
   }

   if (_pMotion == NULL) {
      _pMotion = NULL;
   } else {
      _pMotion = _pMotion->clone();
      static_cast< Motion* >(_pMotion)->setMove(this);
   }
}

IMove* Move::clone() const
{
   return new Move(*this);
}

Move::~Move()
{
   if (_pGuide) { delete _pGuide; }
   if (_pMotion) { delete _pMotion; }
}

IMotion* Move::getMotion()
{
   return _pMotion;
}

IGuide* Move::getGuide()
{
   return _pGuide;
}

const IMotion* Move::getMotion() const
{
   return _pMotion;
}

const IGuide* Move::getGuide() const
{
   return _pGuide;
}

void Move::setMotion(IMotion* m)
{
   assert(_pMotion == NULL);
   m->setMove(this);
   _pMotion = m;
}

void Move::setGuide(IGuide* g)
{
   assert(_pGuide == NULL);
   g->setMove(this);
   _pGuide = g;
}

void Move::setUuid(const char* uuid)
{
   _uuid = uuid;
}

void Move::genUuid()
{
   ::boost::uuids::random_generator gen;
   ::boost::uuids::uuid uuid = gen();
   ::std::ostringstream os;
   os << uuid;
   _uuid = os.str();
}

void Move::setTime(__int64 t0, __int64 t1)
{
   _t0 = t0;
   _t1 = t1;
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
