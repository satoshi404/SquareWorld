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
typedef unsigned long Atom;

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
	XPointer private_data; 		    // Data pointer
};

//////////////////////////////////////////////////////////////////

typedef struct ScreenFormat 
{
	XExtData* data;          // Hook extencion data
	int depth;               // Image depth
	int bits_per_pixel;      // Bits/pixel at this depth
	int scanline_pad;	     // scanline must padded to this multiple 
};

//////////////////////////////////////////////////////////////////

typedef struct XDisplay 
{
	XExtData* data; 		 // Hook extencion data
	void* private1; 	     // ..
	int fd;         		 // NetworkSocket
	int private2, private6, private8;   	     
	int proto_major_version; // major version of server's X protocol 
	int proto_minor_version; // minor version of servers X protocol 
	char *vendor;		     // vendor of the server hardware 
	XID private3, private4, private5; 
	XID (*resource_alloc) (
		struct XDisplay*
	);							   // Allocator function 
	int byte_order;				   // Screen byte order, LSBFirst, MSBFirst 
	int bitmap_unit;	    	   // Padding and data requirements 
	int bitmap_pad;		    	   // Padding requirements on bitmaps 
	int bitmap_bit_order;		   // LeastSignificant or MostSignificant 
	int nformats;		    	   // Number of pixmap formats in list 
	ScreenFormat *pixmap_format;   // Pixmap format list 
	int release;		           // Server release
};

/////////////////////////////////////////////////////////////////

extern "C" 
(

	XDisplay* XOpenDisplay(
		const char* 			// Display name
	);

	XWindow XCreateSimpleWindow(
		XDisplay*, 				// Display
		XWindow,   				// Parent
		int,       				// X
		int,       				// Y
		unsigned int,   		// Width
		unsigned int,   		// Height
		unsigned int,   		// Border_width
		unsigned long,  		// Border
		unsigned long   		// Background
	);

	XWindow XCreateWindow(
		XDisplay *, 			// Display
		XWindow,    			// Parent
		int,                    // X
		int,                    // Y
		unsigned int,           // Width
		unsigned int,           // Height
		unsigned int,           // Border_width
		int,                    // Depth
		unsigned int,           // Class
		XVisual *,              // Visual
		unsigned long,          // Valuemask
		XSetWindowAttributes *  // Attributes
    );

	int XDestroyWindow(
		XDisplay*,      		// Display
		XWindow,        		// Parent
	);

	int XCloseDisplay(
		XDisplay*       		// Display
	);
    
	int XPending(
		XDisplay *      		// Display
	);
	
	int XFree(
    	void*		   		    // Data
	);
);

///////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////
	#endif //// x11.hpp ////
///////////////////////////////////////////////////////////////////