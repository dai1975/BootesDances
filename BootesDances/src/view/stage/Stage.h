#ifndef BOOTESDANCES_STAGE_H
#define BOOTESDANCES_STAGE_H

#include "../../include.h"
#include <string>
#include "../../move/MoveSequence.h"

struct Stage
{
   int version;
   std::string  name;
   std::string  moviepath;
   std::basic_string< TCHAR > tc_name;
   std::basic_string< TCHAR > tc_moviepath;
   MoveSequence seq;
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
