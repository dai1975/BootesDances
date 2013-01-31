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

bool MotionRealizer::Idealize(::pb::Motion* pOut, const IMotion* pIn)
{
   return pIn->idealize(pOut);
}

bool MotionRealizer::Realize(IMotion** ppOut, const ::pb::Motion* pIn)
{
   if (pIn == NULL) {
      *ppOut = new MotionWiimoteSimple();
      return true;
   }
   IMotion* p = NULL;
   const std::string& type = pIn->type();

   if (false) {
      ;
   } else if (type.compare("MotionWiimoteSimple") == 0) {
      p = new MotionWiimoteSimple();
   } else {
      return false;
   }

   if (! p->realize(pIn)) {
      delete p;
      return false; 
   }
   
   *ppOut = p;
   return true;
}

bool MotionRealizer::Save(const TCHAR* dir, const TCHAR* name, const MoveSequence* seq)
{
   const MotionFactory* reg = NULL;
   std::basic_string< TCHAR > path, tmppath, oldpath;
   {
      const char* motion_name = seq->getMotionName();
      reg = MotionFactory::GetFactory(motion_name);
      if (reg == NULL) { return false; }

      TCHAR* motion_tname = ::bootes::lib::util::TChar::C2T(motion_name);
      path.append(dir).append(_T("\\motion-")).append(motion_tname).append(_T(".txt"));
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

bool MotionRealizer::Load(MoveSequence* seq, const TCHAR* dir, const TCHAR* name)
{
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
