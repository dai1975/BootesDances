#include "StageRealizer.h"
#include "../../move/MoveRealizer.h"
#include "../../move/guide/GuideRealizer.h"
#include "../../move/motion/MotionRealizer.h"
#include <bootes/lib/util/TChar.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>
#include <sstream>

bool StageRealizer::Idealize(::pb::Stage* pOut, const Stage& in)
{
   //pOut->clear_moves();
   pOut->set_version(in.version);
   pOut->set_name(in.name);
   pOut->set_moviepath(in.moviepath);

   return true;
}

bool StageRealizer::Realize(Stage** ppOut, const pb::Stage& in, const TCHAR* name, const TCHAR* dirpath)
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
   {
      char* tmp;
      tmp = ::bootes::lib::util::TChar::T2C(pStage->tc_basename.c_str());
      if (tmp == NULL) { goto fail; }
      pStage->basename = tmp;
      delete[] tmp;
   }

   if (pStage->tc_moviepath[0] == _T('\0') ||
       pStage->tc_moviepath[0] == _T('\\') ||
       (pStage->tc_moviepath[0] != _T('\0') && pStage->tc_moviepath[1] == _T(':'))
   ) {
      ;
   } else {
      std::basic_ostringstream< TCHAR > o;
      o << dirpath << _T("\\") << pStage->tc_moviepath;
      pStage->tc_moviepath = o.str();

      char* tmp;
      tmp = ::bootes::lib::util::TChar::T2C(pStage->tc_moviepath.c_str());
      if (tmp == NULL) { goto fail; }
      pStage->moviepath = tmp;
      delete[] tmp;
   }

   *ppOut = pStage;
   return true;

fail:
   delete pStage;
   return false;
}

namespace {

/*
 * create dedicated directory to the stage under global root directory:
 *   <basedir>/...../<basename>
 *
 * stage file is "stage.txt"
 *
 */

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
   o << GetDirPath(dir, name) << _T("\\stage.txt");
   return o.str();
}
}

bool StageRealizer::New(Stage** ppStage, MoveSequence** ppSeq, const MotionGuidePair& mg)
{
   const GuideRealizer*  pGuideRealizer = NULL;
   const MotionRealizer* pMotionRealizer = NULL;
   {
      char* c;
      c = ::bootes::lib::util::TChar::T2C(mg.guide.c_str());
      pGuideRealizer = GuideRealizer::GetRealizer(c);
      delete[] c;

      c = ::bootes::lib::util::TChar::T2C(mg.motion.c_str());
      pMotionRealizer = MotionRealizer::GetRealizer(c);
      delete[] c;
   }
   if (pGuideRealizer == NULL) { return false; }
   if (pMotionRealizer == NULL) { return false; }

   Stage* pStage = new Stage();
   MoveSequence* pSeq = new MoveSequence();
   pSeq->setGuideRealizer(pGuideRealizer);
   pSeq->setMotionRealizer(pMotionRealizer);

   *ppStage = pStage;
   *ppSeq   = pSeq;
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

bool StageRealizer::Load(Stage** ppStage, MoveSequence** ppSeq, const TCHAR* basepath, const TCHAR* subdir, const MotionGuideList& mgl)
{
   if (ppStage == NULL) { return false; }
   if (basepath == NULL || basepath[0] == _T('\0')) { return false; }
   if (subdir == NULL || subdir[0] == _T('\0')) { return false; }
   if (! IsExist(basepath, subdir)) { return false; }

   MotionGuidePair mg;
   if (! MoveRealizer::IsExist(&mg, basepath, subdir, mgl)) { return false; }

   Stage* pStage = NULL;
   MoveSequence* pSeq = NULL;

   {
      int fd;
      std::basic_string< TCHAR > path = GetFilePath(basepath, subdir);
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

      
      std::basic_string< TCHAR > dirpath = GetDirPath(basepath, subdir);
      if (! StageRealizer::Realize(&pStage, idea, subdir, dirpath.c_str())) {
         return false;
      }
   }

   if (ppSeq) {
      if (! MoveRealizer::Load(&pSeq, basepath, subdir, mg)) {
         goto fail;
      }
   }

   *ppStage = pStage;
   if (ppSeq) { *ppSeq = pSeq; }
   return true;

fail:
   if (pStage) { delete pStage; }
   if (pSeq) { delete pSeq; }
   if (ppStage) { *ppStage = NULL; }
   if (ppSeq) { *ppSeq = NULL; }
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

bool StageRealizer::Search(void(*cb)(bool,Stage*,void*), void* data, const TCHAR* basepath, const MotionGuideList& mgl)
{
   WIN32_FIND_DATA find;
   HANDLE hFind;
   {
      tc_string query;
      query.append(basepath).append(_T("\\*"));
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         (*cb)(false, 0, NULL);
         return false;
      }
   }

   std::list< std::basic_string< TCHAR > > subdirs;
   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      if ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
         if (find.cFileName[0] != _T('.')) {
            subdirs.push_back( std::basic_string< TCHAR >(find.cFileName) );
         }
      }
   } //for found
   FindClose(hFind);

   for (std::list< std::basic_string< TCHAR > >::iterator i = subdirs.begin(); i != subdirs.end(); ++i) {
      Search0(cb, data, basepath, i->c_str(), mgl);
   }

   (*cb)(true, NULL, data);
   return true;
}

bool StageRealizer::Search0(void(*cb)(bool,Stage*,void*), void* data,
                            const TCHAR* basepath, const TCHAR* subdir, const MotionGuideList& mgl)
{
   WIN32_FIND_DATA find;
   HANDLE hFind;
   {
      std::basic_string< TCHAR > query;
      if (subdir[0] != _T('\0')) {
         query.append(basepath).append(_T("\\")).append(subdir);
      } else {
         query.append(basepath);
      }
      query.append(_T("\\*"));

      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         (*cb)(false, 0, NULL);
         return false;
      }
   }

   std::list< std::basic_string< TCHAR > > dirs;
   bool has_stagefile = false;
   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      if ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
         if (find.cFileName[0] != _T('.')) {
            dirs.push_back(find.cFileName);
         }

      } else if ((find.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0) {
         continue;

      } else if (IsStageFileName(find.cFileName)) {
         has_stagefile = true;
      }
   } //for found
   FindClose(hFind);

   for (std::list< std::basic_string< TCHAR > >::iterator i = dirs.begin(); i != dirs.end(); ++i) {
      std::basic_string< TCHAR > sd;
      if (subdir[0] != _T('\0')) {
         sd.append(subdir).append(_T("\\")).append(i->c_str());
      } else {
         sd.append(i->c_str());
      }
      Search0(cb, data, basepath, sd.c_str(), mgl);
   }
   if (has_stagefile) {
      Stage* pStage = NULL;
      if (StageRealizer::Load(&pStage, NULL, basepath, subdir, mgl)) {
         (*cb)(true, pStage, data);
      }
   }

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
