#include "StageRealizer.h"
#include "../../move/MoveRealizer.h"
#include <bootes/lib/util/TChar.h>

bool StageRealizer::Idealize(::pb::Stage* pOut, const Stage* pIn)
{
   pOut->clear_moves();
   pOut->set_version(pIn->version);
   pOut->set_name(pIn->name);
   pOut->set_moviepath(pIn->moviepath);

   const MoveSequence& seq = pIn->seq;
   for (MoveSequence::const_iterator i = seq.begin(); i != seq.end(); ++i) {
      ::pb::Move* p = pOut->add_moves();
      MoveRealizer::Idealize(p, *i);
      p->set_chainnext( seq.isChainNext(i) );
   }
   return true;
}

bool StageRealizer::Realize(Stage** ppOut, const pb::Stage* pIn)
{
   Stage* pStage = new Stage();

   pStage->name      = pIn->name();
   pStage->moviepath = pIn->moviepath();
   pStage->version   = 1;

   {
      TCHAR* tmp = ::bootes::lib::util::TChar::C2T(pStage->moviepath.c_str());
      if (tmp == NULL) { return false; }
      pStage->tmoviepath = tmp;
      delete[] tmp;
   }

   // add move models
   pStage->seq.clear();
   bool chain0 = false;
   for (int i=0; i < pIn->moves_size(); ++i) {
      const ::pb::Move& idea = pIn->moves(i);
      IMove* rea = NULL;
      if (! MoveRealizer::Realize(&rea, &idea)) { goto fail; }

      MoveSequence::iterator ite = pStage->seq.add(rea); //pass ownership
      if (chain0) { pStage->seq.chainPrev(ite, true); }
      chain0 = idea.chainnext();
   }
   *ppOut = pStage;
   return true;

fail:
   delete pStage;
   return false;
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
