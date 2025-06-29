#ifndef CONFIG 
#define CONFIG

#define HIGH 1  
#define LOW  0

#define ENABLE   HIGH
#define DISABLE  LOW

#define CONTEXT_WINDOW    ENABLE  // Enable use X11 without opengl or Disable with opengl
#define X11_SCRATCH       DISABLE // Enable use X11 from scratch or Disable use xlib
#define ENGINE_DEBUG      DISABLE // Enable for debug engine or Disable 

/////////////////////////////////////////////////////////////////

#if CONTEXT_WINDOW
#define CONTEXT_OPENGL ENABLE
#else 
#define CONTEXT_OPENGL DISABLE
#endif

/////////////////////////////////////////////////////////////////
// See x11 document

#if X11_SCRATCH
#define FROM_SCRATCH ENABLE
#else 
#define FROM_SCRATCH DISABLE
#endif

/////////////////////////////////////////////////////////////////   

#if ENGINE_DEBUG
// ..
#endif

/////////////////////////////////////////////////////////////////   
    #endif // config.h
/////////////////////////////////////////////////////////////////