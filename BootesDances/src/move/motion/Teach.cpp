#include "Teach.h"
#include <iterator>

TeachSequence::TeachSequence()
   : succeed(false)
{ }

TeachSequence::TeachSequence(const TeachSequence& r)
{
   operator=(r);
}

TeachSequence& TeachSequence::operator=(const TeachSequence& r)
{
   records.clear();
   ::std::copy(r.records.begin(), r.records.end(), ::std::back_inserter(records));
   succeed = r.succeed;
   return *this;
}

TeachSequence::~TeachSequence()
{ }

void TeachSequence::clear()
{
   succeed = false;
   records.clear();
}

void TeachSequence::add(int t, const ::bootes::lib::framework::WiimoteEvent& wev)
{
   records.push_back( TeachSequence::Record(t, wev) );
}

void TeachSequence::commit(bool succeed_)
{
   succeed = succeed_;
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
