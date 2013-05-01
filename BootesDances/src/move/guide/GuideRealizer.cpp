#include "GuideRealizer.h"
#include "GuideRibbonLine.h"
#include "GuideRibbonSpline.h"
#include "GuideRibbonEllipse.h"
#include <bootes/lib/util/TChar.h>
#include <sstream>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>

namespace {

class Static
{
public:
   GuideRealizer::Registry registry;

   Static() {
   }

   ~Static() {
      for (GuideRealizer::Registry::iterator i = registry.begin(); i != registry.end(); ++i) {
         GuideRealizer* p = const_cast< GuideRealizer* >(i->second);
         delete p;
      }
      registry.clear();
   }
};
Static _static;

}

const GuideRealizer::Registry& GuideRealizer::GetRegistry()
{
   return _static.registry;
}

bool GuideRealizer::Register(GuideRealizer* p)
{
   Registry& registry = _static.registry;
   const char* name = p->getGuideName();
   Registry::iterator i = registry.find(name);
   if (i != registry.end()) {
      return false;
   }
   registry.insert( Registry::value_type(name,p) );
   return true;
}

const GuideRealizer* GuideRealizer::GetRealizer(const char* name)
{
   Registry& registry = _static.registry;
   Registry::iterator i = registry.find(std::string(name));
   if (i == registry.end()) {
      return NULL;
   }
   return i->second;
}

GuideRealizer::GuideRealizer() { }
GuideRealizer::~GuideRealizer() { }

/*
bool GuideRealizer::Idealize(::pb::Guide* pOut, const IGuide& in)
{
   return in.idealize(pOut);
}

bool GuideRealizer::Realize(IGuide** ppOut, const ::pb::Guide& in)
{
   IGuide* p = NULL;
   const std::string& type = in.type();

   if (false) {
      ;
   } else if (type.compare(GuideRibbonLine::TYPE) == 0) {
      p = new GuideRibbonLine();
   } else if (type.compare(GuideRibbonEllipse::TYPE) == 0) {
      p = new GuideRibbonEllipse();
   } else if (type.compare(GuideRibbonSpline::TYPE) == 0) {
      p = new GuideRibbonSpline();
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

std::basic_string< TCHAR > GuideRealizer::GetFilePath(const TCHAR* dir, const TCHAR* name, const TCHAR* guide)
{
   std::basic_ostringstream< TCHAR > o;
   o << dir << _T("\\") << name << _T("\\guide-") << guide << _T(".txt");
   return o.str();
}

bool GuideRealizer::IsExist(const TCHAR* dir, const TCHAR* name, const TCHAR* guide)
{
   std::basic_string< TCHAR > path = GetFilePath(dir, name, guide);
   DWORD attr = GetFileAttributes(path.c_str());
   if (attr == INVALID_FILE_ATTRIBUTES || ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)) {
      return false;
   }
   return true;
}

bool GuideRealizer::load(std::list< GuideData >* pOut, const TCHAR* dir, const TCHAR* name) const
{
   int fd = -1;
   std::basic_string< TCHAR > path = GetFilePath(dir, name, getGuideNameT());
   errno_t err = _tsopen_s(&fd, path.c_str(), 
                           _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
   if (err != 0) { return false; }
   if (fd < 0) { return false; }

   bool ret = load(pOut, fd);
   close(fd);
   return ret;
}

bool GuideRealizer::save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq) const
{
   std::basic_string< TCHAR > path, tmppath, oldpath;
   path = GetFilePath(dir, name, getGuideNameT());
   tmppath.append(path).append(_T(".tmp"));
   oldpath.append(path).append(_T(".old"));

   {
      int fd = -1;
      errno_t err = _tsopen_s(&fd, tmppath.c_str(), 
                              _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYWR, _S_IREAD|_S_IWRITE);
      if (err != 0) { return false; }
      if (fd < 0) { return false; }

      bool r = save(fd, seq);
      close(fd);
      if (!r) { return false; }
   }

   if (! DeleteFile(oldpath.c_str())) {
      DWORD err = GetLastError();
      if (err != ERROR_FILE_NOT_FOUND) { return false; }
   }
   MoveFile(path.c_str(), oldpath.c_str());
   if (! MoveFile(tmppath.c_str(), path.c_str())) { return false; }

   return true;
}

/*
bool GuideRealizer::Save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq)
{
   const GuideFactory* reg = NULL;
   std::basic_string< TCHAR > path, tmppath, oldpath;
   {
      reg = seq.getGuideFactory();
      if (reg == NULL) { return false; }
      const char* guide_name = reg->getGuideName();

      TCHAR* guide_tname = ::bootes::lib::util::TChar::C2T(guide_name);
      path = GetFilePath(dir, name, guide_tname);
      delete[] guide_tname;
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
*/

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
