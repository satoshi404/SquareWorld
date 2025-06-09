#ifndef PLATFORM
#define PLATFORM

#if defined(__linux__)
    #define PLATFORM_LINUX   1
    #define PLATFORM_WINDOWS 0
#elif defined(__WIN32) || defined(__WIN64)
    #define PLATFORM_LINUX   0
    #define PLATFORM_WINDOWS 1
#endif

#endif