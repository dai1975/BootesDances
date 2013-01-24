#include "StageRealizer.h"
#include "../../move/MoveRealizer.h"
#include <bootes/lib/util/TChar.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>

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
      TCHAR* tmp;
      tmp = ::bootes::lib::util::TChar::C2T(pStage->name.c_str());
      if (tmp == NULL) { return false; }
      pStage->tc_name = tmp;
      delete[] tmp;

      tmp = ::bootes::lib::util::TChar::C2T(pStage->moviepath.c_str());
      if (tmp == NULL) { return false; }
      pStage->tc_moviepath = tmp;
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

bool StageRealizer::Save(const TCHAR* path_, const Stage* pIn)
{
   ::pb::Stage idea;
   if (! StageRealizer::Idealize(&idea, pIn)) {
      return false;
   }

   std::basic_string< TCHAR > path, tmppath, oldpath;
   path.append(path);
   tmppath.append(path).append(_T(".tmp"));
   oldpath.append(path).append(_T(".old"));

   {
      int fd = -1;
      errno_t err = _tsopen_s(&fd, tmppath.c_str(), 
                                 _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYWR, _S_IREAD|_S_IWRITE);
      if (err != 0) { goto fail; }
      
      google::protobuf::io::FileOutputStream out(fd);
      bool b = google::protobuf::TextFormat::Print(idea, &out);
      out.Flush();
      out.Close();

      if (!b) { goto fail; }
   }

   if (! DeleteFile(oldpath.c_str())) {
      DWORD err = GetLastError();
      if (err != ERROR_FILE_NOT_FOUND) { goto fail; }
   }

   MoveFile(path.c_str(), oldpath.c_str());
   if (! MoveFile(tmppath.c_str(), path.c_str())) { goto fail; }
   return true;

fail:
   return false;
}

bool StageRealizer::Load(::pb::Stage** ppOut, const TCHAR* path)
{
   int fd;
   errno_t err = _tsopen_s(&fd, path,
                           _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
   if (err != 0) {
      return false;
   }

   ::pb::Stage* idea = new ::pb::Stage();
   {
      google::protobuf::io::FileInputStream in(fd);
      bool parsed = google::protobuf::TextFormat::Parse(&in, idea);
      _close(fd);
      if (!parsed) {
         delete idea;
         return false;
      }
   }
   *ppOut = idea;
   return true;
}

bool StageRealizer::Load(Stage** ppOut, const TCHAR* path)
{
   ::pb::Stage* idea;
   if (! Load(&idea, path)) {
      return false;
   }
   bool ret = StageRealizer::Realize(ppOut, idea);
   delete idea;
   return ret;
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
