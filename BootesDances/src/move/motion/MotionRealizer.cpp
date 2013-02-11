#include "MotionRealizer.h"
#include "MotionFactory.h"
#include "MotionWiimoteSimple.h"
#include <bootes/lib/util/TChar.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>
#include <sstream>

bool MotionRealizer::Idealize(::pb::Motion* pOut, const IMotion& in)
{
   return in.idealize(pOut);
}

bool MotionRealizer::Realize(IMotion** ppOut, const ::pb::Motion& in)
{
   IMotion* p = NULL;
   const std::string& type = in.type();

   if (false) {
      ;
   } else if (type.compare("MotionWiimoteSimple") == 0) {
      p = new MotionWiimoteSimple();
   } else {
      return false;
   }

   if (! p->realize(in)) {
      delete p;
      return false;
   }
   
   *ppOut = p;
   return true;
}


namespace {

std::basic_string< TCHAR > GetFilePath(const TCHAR* dir, const TCHAR* name, const TCHAR* motion)
{
   std::basic_ostringstream< TCHAR > o;
   o << dir << _T("\\") << name << _T("\\motion-") << motion << _T(".txt");
   return o.str();
}

}

bool MotionRealizer::IsExist(const TCHAR* dir, const TCHAR* name, const TCHAR* motion)
{
   std::basic_string< TCHAR > path = GetFilePath(dir, name, motion);
   DWORD attr = GetFileAttributes(path.c_str());
   if (attr == INVALID_FILE_ATTRIBUTES || ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)) {
      return false;
   }
   return true;
}

bool MotionRealizer::Load(MoveSequence* seq, const TCHAR* dir, const TCHAR* name, const TCHAR* motion)
{
   return false;
}

bool MotionRealizer::Save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq)
{
   const MotionFactory* reg = NULL;
   std::basic_string< TCHAR > path, tmppath, oldpath;
   {
      reg = seq.getMotionFactory();
      if (reg == NULL) { return false; }
      const char* motion_name = reg->getMotionName();

      TCHAR* motion_tname = ::bootes::lib::util::TChar::C2T(motion_name);
      path = GetFilePath(dir, name, motion_tname);
      delete[] motion_tname;
   }
   tmppath.append(path).append(_T(".tmp"));
   oldpath.append(path).append(_T(".old"));

   {
      int fd = -1;
      errno_t err = _tsopen_s(&fd, tmppath.c_str(), 
                              _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYWR, _S_IREAD|_S_IWRITE);
      if (err != 0) { return false; }
      
      google::protobuf::io::FileOutputStream out(fd);
      bool b = reg->save(out, seq);

      out.Flush();
      out.Close();

      if (!b) {
         return false;
      }
   }

   if (! DeleteFile(oldpath.c_str())) {
      DWORD err = GetLastError();
      if (err != ERROR_FILE_NOT_FOUND) { return false; }
   }

   MoveFile(path.c_str(), oldpath.c_str());
   if (! MoveFile(tmppath.c_str(), path.c_str())) { return false; }
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
