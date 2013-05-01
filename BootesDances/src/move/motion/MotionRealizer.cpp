#include "MotionRealizer.h"
#include "MotionWiimoteSimple.h"
#include <bootes/lib/util/TChar.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>
#include <sstream>

namespace {

class Static
{
public:
   MotionRealizer::Registry registry;

   Static() {
   }

   ~Static() {
      for (MotionRealizer::Registry::iterator i = registry.begin(); i != registry.end(); ++i) {
         MotionRealizer* p = const_cast< MotionRealizer* >(i->second);
         delete p;
      }
      registry.clear();
   }
};
Static _static;

}

const MotionRealizer::Registry& MotionRealizer::GetRegistry()
{
   return _static.registry;
}

bool MotionRealizer::Register(MotionRealizer* p)
{
   Registry& registry = _static.registry;
   const std::string name = p->getMotionName();
   Registry::iterator i = registry.find(name);
   if (i != registry.end()) {
      return false;
   }
   registry.insert( Registry::value_type(name,p) );
   return true;
}

const MotionRealizer* MotionRealizer::GetRealizer(const char* name)
{
   Registry& registry = _static.registry;
   Registry::iterator i = registry.find(std::string(name));
   if (i == registry.end()) {
      return NULL;
   }
   return i->second;
}

MotionRealizer::MotionRealizer()
{ }

MotionRealizer::~MotionRealizer()
{ }

/*
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
*/

std::basic_string< TCHAR > MotionRealizer::GetFilePath(const TCHAR* dir, const TCHAR* name, const TCHAR* motion)
{
   std::basic_ostringstream< TCHAR > o;
   o << dir << _T("\\") << name << _T("\\motion-") << motion << _T(".txt");
   return o.str();
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

bool MotionRealizer::load(std::list< MotionData >* pOut, const TCHAR* dir, const TCHAR* name) const
{
   int fd = -1;
   std::basic_string< TCHAR > path = GetFilePath(dir, name, getMotionNameT());
   errno_t err = _tsopen_s(&fd, path.c_str(), 
                           _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
   if (err != 0) { return false; }
   if (fd < 0) { return false; }

   bool ret = load(pOut, fd);
   close(fd);
   return ret;
}

bool MotionRealizer::save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq) const
{
   std::basic_string< TCHAR > path, tmppath, oldpath;
   path = MotionRealizer::GetFilePath(dir, name, getMotionNameT());
   tmppath.append(path).append(_T(".tmp"));
   oldpath.append(path).append(_T(".old"));

   {
      int fd = -1;
      errno_t err = _tsopen_s(&fd, tmppath.c_str(), 
                              _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYWR, _S_IREAD|_S_IWRITE);
      if (err != 0) { return false; }

      bool b = save(fd, seq);
      close(fd);
      if (!b) { return false; }
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
