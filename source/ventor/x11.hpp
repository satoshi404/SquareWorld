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

//////////////////////////////////////////////////////////////////

typedef char* XPointer;
typedef unsigned long XID;

//////////////////////////////////////////////////////////////////

typedef XID XWindow;

//////////////////////////////////////////////////////////////////

typedef struct XExtData 
{
	int number;             		// Extencion number
	struct XExtData *next;  		// Next item in list
	int (*free_activate)(
		struct XExtData *extencion
	); 								// Function for free private data
	XPointer pointer; 				// Data pointer
};

//////////////////////////////////////////////////////////////////

typedef struct XDisplay 
{
	XExtData* data; 		 // Hook extencion data
	void* private1; 		 // ..
	int fd;         		 // NetworkSocket
	int private2;   		 // ..
	int proto_major_version; // major version of server's X protocol 
	int proto_minor_version; // minor version of servers X protocol 
	char *vendor;		     // vendor of the server hardware 
};

/////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////

extern int XDestroyWindow(
	XDisplay*,      // Display
	XWindow,        // Parent
);

///////////////////////////////////////////////////////////////////

extern int XCloseDisplay(
	XDisplay*       // Display
);

///////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////
	#endif //// x11.hpp ////
///////////////////////////////////////////////////////////////////