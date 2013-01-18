#include "StageRealizer.h"
#include "../../move/MoveRealizer.h"

bool StageRealizer::Idealize(::pb::Stage2* pOut, const Stage* pIn)
{
   pOut->clear_moves();
   pOut->set_version(pIn->version);
   pOut->set_name(pIn->name);
   pOut->set_moviepath(pIn->moviepath);

   const MoveSequence& seq = pIn->seq;
   for (MoveSequence::const_iterator i = seq.begin(); i != seq.end(); ++i) {
      ::pb::Move2* p = pOut->add_moves();
      MoveRealizer::Idealize(p, *i);
      p->set_chainnext( seq.isChainNext(i) );
   }
   return true;
}

bool StageRealizer::Realize(Stage**, const pb::Stage2*)
{
   return true;
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
