#pragma once

#ifdef _WIN32
    #define _USE_MATH_DEFINES
    #include <cmath>

    #ifndef M_PI
        #define M_PI 3.14159265358979323846
    #endif

    typedef unsigned int uint;
#endif