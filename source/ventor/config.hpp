#ifndef CONFIG 
#define CONFIG

#define ENABLE  1
#define DISABLE 0

#define CONTEXT_WINDOW    ENABLE // Enable use X11 without opengl or Disable with opengl

#if CONTEXT_WINDOW
    #define CONTEXT_NAKED DISABLE
#else 
    #define CONTEXT_NAKED ENABLE
#endif

#if !CONTEXT_NAKED
    #define CONTEXT_OPENGL ENABLE
#else
    #define CONTEXT_OPENGL DISABLE
#endif

#endif 