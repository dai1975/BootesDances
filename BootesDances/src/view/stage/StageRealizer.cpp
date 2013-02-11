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
#include <sstream>
#include "../../move/guide/GuideFactory.h"
#include "../../move/motion/MotionFactory.h"

bool StageRealizer::Idealize(::pb::Stage* pOut, const Stage& in)
{
   pOut->clear_moves();
   pOut->set_version(in.version);
   pOut->set_name(in.name);
   pOut->set_moviepath(in.moviepath);

   return true;
}

bool StageRealizer::Realize(Stage** ppOut, const pb::Stage& in, const TCHAR* name)
{
   Stage* pStage = new Stage();

   pStage->version     = 1;
   pStage->name        = in.name();
   pStage->moviepath   = in.moviepath();
   pStage->tc_basename = name;

   {
      TCHAR* tmp;
      tmp = ::bootes::lib::util::TChar::C2T(pStage->moviepath.c_str());
      if (tmp == NULL) { goto fail; }
      pStage->tc_moviepath = tmp;
      delete[] tmp;
   }
/*
   // add move models
   pStage->seq.clear();
   pStage->seq.setGuideName("GuideRibbon");
   pStage->seq.setMotionName("MotionWiimoteSimple");
   bool chain0 = false;
   for (int i=0; i < in.moves_size(); ++i) {
      const ::pb::Move& idea = in.moves(i);
      IMove* rea = NULL;
      if (! MoveRealizer::Realize(&rea, &idea)) { goto fail; }

      MoveSequence::iterator ite = pStage->seq.add(rea); //pass ownership
      if (chain0) { pStage->seq.chainPrev(ite, true); }
      chain0 = idea.chainnext();
   }
*/
   *ppOut = pStage;
   return true;

fail:
   delete pStage;
   return false;
}

namespace {

inline bool IsStageFileName(const TCHAR* name)
{
   size_t len = _tcslen(name);
   return (9 == len && (_tcscmp(_T("stage.txt"), name) == 0));
}

std::basic_string< TCHAR > GetDirPath(const TCHAR* dir, const TCHAR* name)
{
   std::basic_ostringstream< TCHAR > o;
   o << dir << _T("\\") << name;
   return o.str();
}
std::basic_string< TCHAR > GetFilePath(const TCHAR* dir, const TCHAR* name)
{
   std::basic_ostringstream< TCHAR > o;
   o << dir << _T("\\") << name << _T("\\stage.txt");
   return o.str();
}
}

bool StageRealizer::SetFactory(MoveSequence* pSeq, const MotionGuidePair& mg)
{
   char* c;
   c = ::bootes::lib::util::TChar::T2C(mg.guide.c_str());
   pSeq->setGuideFactory(GuideFactory::GetFactory(c));
   delete[] c;

   c = ::bootes::lib::util::TChar::T2C(mg.motion.c_str());
   pSeq->setMotionFactory(MotionFactory::GetFactory(c));
   delete[] c;
   return true;
}

bool StageRealizer::IsExist(const TCHAR* dir, const TCHAR* name)
{
   std::basic_string< TCHAR > path = GetFilePath(dir, name);
   DWORD attr = GetFileAttributes(path.c_str());
   if (attr == INVALID_FILE_ATTRIBUTES || ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)) {
      return false;
   }
   return true;
}

bool StageRealizer::Load(Stage** ppStage, MoveSequence** ppSeq, const TCHAR* dir, const TCHAR* name, const MotionGuideList& mgl)
{
   if (ppStage == NULL) { return false; }
   if (! IsExist(dir, name)) { return false; }

   MotionGuidePair mg;
   if (! MoveRealizer::IsExist(&mg, dir, name, mgl)) { return false; }

   {
      int fd;
      std::basic_string< TCHAR > path = GetFilePath(dir, name);
      errno_t err = _tsopen_s(&fd, path.c_str(),
                              _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
      if (err != 0) {
         return false;
      }

      ::pb::Stage idea;
      google::protobuf::io::FileInputStream in(fd);
      bool parsed = google::protobuf::TextFormat::Parse(&in, &idea);
      _close(fd);
      if (!parsed) {
         return false;
      }

      if (! StageRealizer::Realize(ppStage, idea, name)) {
         return false;
      }
   }

   if (ppSeq != NULL) {
      //MoveRealizer::Load(p);
      SetFactory(*ppSeq, mg);
   }

   return true;
}

bool StageRealizer::Save(std::basic_string<TCHAR>* o_basename, const TCHAR* dir, const TCHAR* basename0, bool neu, const Stage& stage, const MoveSequence& seq)
{
   std::basic_string< TCHAR > basename;
   {
      DWORD attr;
      std::basic_string< TCHAR > stagedir;

      basename = basename0;
      stagedir = GetDirPath(dir, basename0);
      attr = GetFileAttributes(stagedir.c_str());
      if (attr == -1) {
         ;
      } else if (neu) {
         int n = 0;
         const TCHAR* pc;
         for (pc = basename0; *pc != _T('\0'); ++pc);
         for (--pc; _T('0') <= *pc && *pc <= _T('9'); --pc) {
            n *= 10;
            n += (*pc - _T('0'));
         }
         ++pc;
         std::basic_string<TCHAR> alpha(basename0, pc-basename0);
         for (++n; attr != -1; ++n) {
            std::basic_ostringstream< TCHAR > o;
            o << alpha << n;
            basename = o.str();
            stagedir = GetDirPath(dir, basename.c_str());
            attr = GetFileAttributes(stagedir.c_str());
            if (attr == -1) { break; }
         }
      }

      if (attr == -1) {
         if (! CreateDirectory(stagedir.c_str(), NULL)) {
            return false;
         }
      } else if ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0) {
         return false;
      }
   }

   if (! MoveRealizer::Save(dir, basename.c_str(), seq)) {
      return false;
   }

   ::pb::Stage idea;
   if (! StageRealizer::Idealize(&idea, stage)) {
      return false;
   }

   std::basic_string< TCHAR > path, tmppath, oldpath;
   path = GetFilePath(dir, basename.c_str());
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
   *o_basename = basename;
   return true;

fail:
   return false;
}

bool StageRealizer::Search(void(*cb)(bool,int,Stage*), const TCHAR* dir, const MotionGuideList& mgl)
{
   WIN32_FIND_DATA find;
   HANDLE hFind;
   {
      tc_string query;
      query.append(dir).append(_T("\\*"));
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         (*cb)(false, 0, NULL);
         return false;
      }
   }

   int index = 0;
   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      if ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
         if (find.cFileName[0] != _T('.')) {
            std::basic_ostringstream< TCHAR > os;
            os << dir << _T('\\') << find.cFileName;
            Search(cb, os.str().c_str(), mgl); //recursive
         }
         continue;

      } else if ((find.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0) {
         continue;

      } else if (! IsStageFileName(find.cFileName)) {
         continue;

      } else {
         std::basic_ostringstream< TCHAR > os;
         os << dir << _T('\\') << find.cFileName;

         HANDLE hFile = INVALID_HANDLE_VALUE;
         int fd;
         errno_t err = _tsopen_s(&fd, os.str().c_str(),
                                 _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
         if (err != 0) {
            continue;
         }

         Stage* pStage = NULL;
         if (! StageRealizer::Load(&pStage, NULL, dir, find.cFileName, mgl)) {
            continue;
         }
         (*cb)(true, index++, pStage);
      }
   } //for found
   FindClose(hFind);

   (*cb)(true, index++, NULL);
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
