#pragma once

#include <cstdio>
#include <cstring>

#if defined(_WIN32)
  #include <direct.h>
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef PATH_MAX
    #define PATH_MAX 260
  #endif
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
  #define getcwd _getcwd
#endif
