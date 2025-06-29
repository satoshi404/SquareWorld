#ifndef PLATFORM
#define PLATFORM

#define HIGH 1  
#define LOW  0

#if defined(__linux__)
    #define PLATFORM_LINUX      (HIGH)
    #define PLATFORM_WINDOWS    (LOW)
#elif defined(__WIN32) || defined(__WIN64)
    #define PLATFORM_LINUX      (LOW)
    #define PLATFORM_WINDOWS    (HIGH)
#else
#error "PLATFORM NOT SUPPORTED"
#endif

#if PLATFORM_LINUX
#define LINUX_IMPL   (HIGH)
#elif PLATFORM_WINDOWS
#define WINDOWS_IMPL (HIGH)
#else
#error "PLATFORM NOT SUPPORTED"
#endif

#endif