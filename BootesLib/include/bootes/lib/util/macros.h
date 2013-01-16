#ifndef FELIX_UTIL_MACROS_H
#define FELIX_UTIL_MACROS_H

#include "../macros.h"

#define FELIX_UTIL_NS_BEGIN FELIX_NS_BEGIN namespace util {
#define FELIX_UTIL_NS_END   } FELIX_NS_END  

#define FELIX_UTIL(NAME) FELIX( util::NAME )

#define FELIX_UTIL_H(FILE) <felix/util/FILE>

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
