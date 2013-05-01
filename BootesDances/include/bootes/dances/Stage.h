#ifndef BOOTESDANCES_STAGE_H
#define BOOTESDANCES_STAGE_H

#include <TCHAR.h>
#include <string>

struct Stage
{
   int version;
   std::string  name;
   std::string  basename;
   std::string  moviepath;
   std::basic_string< TCHAR > tc_basename;
   std::basic_string< TCHAR > tc_moviepath;
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
