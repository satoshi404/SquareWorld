#ifndef X11
#define X11

#include "config.hpp"

#if ORIGINAL_HEADERS
	#include "conflict.hpp"
		#include <X11/Xlib.h>
		#include <X11/Xutil.h>
		#include <X11/Xatom.h>
		#include <X11/XKBlib.h>
	#include "conflict.hpp"
#else

typedef char* XPointer;

typedef struct XExtData 
{
	int number;             // Extencion number
	struct XExtData *next;   // Next item in list
	int (*free_activate)(
		struct XExtData *extencion
	); // Function for free private data
	XPointer pointer; // Data pointer
};

typedef struct XDisplay 
{
	XExtData* data; // Hook extencion data
	void* private1; // ..
	int fd;         // NetworkSocket
 };

//////////////////////////////////////////////////////////////////

extern XDisplay* XOpenDisplay(
	const char* // Display name
);

///////////////////////////////////////////////////////////////////

extern XWindow XCreateSimpleWindow(
	XDisplay*, 		// Display
	XWindow,   		// Parent
	int,       		// x
	int,       		// y
	unsigned int,   // width
	unsigned int,   // height
	unsigned int,   // border_width
	unsigned long,  // border
	unsigned long   // background
);

/////////////////////////////////////////////////////////////////

#endif

#endif // x11.hpp