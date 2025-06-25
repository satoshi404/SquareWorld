#ifndef X11
#define X11


#include "config.hpp"

#include <stdbool.h>

//////////////////////////////////////////////////////////////////

typedef int Bool;

typedef char* XPointer;
typedef unsigned long XID;
typedef unsigned long Atom;
typedef unsigned long VisualID;

//////////////////////////////////////////////////////////////////

typedef XID XWindow;
typedef XID Pixmap;
typedef XID Colormap;
typedef XID Cursor;

//////////////////////////////////////////////////////////////////

struct XExtData 
{
	int number;             	   // Extencion number
	struct XExtData *next;  	   // Next item in list
	int (*free_activate)(
		struct XExtData *extencion
	); 							   // Function for free private data
	XPointer private_data; 		   // Data pointer
};

//////////////////////////////////////////////////////////////////

struct ScreenFormat 
{
	XExtData* data;         	   // Hook extencion data
	int depth;              	   // Image depth
	int bits_per_pixel;     	   // Bits/pixel at this depth
	int scanline_pad;	    	   // scanline must padded to this multiple 
};

//////////////////////////////////////////////////////////////////

struct XDisplay 
{
	XExtData* data; 		 	   // Hook extencion data
	void* private1; 	     	   // ..
	int fd;         		 	   // NetworkSocket
	int private2;   	     	   
	int proto_major_version; 	   // major version of server's X protocol 
	int proto_minor_version; 	   // minor version of servers X protocol 
	char *vendor;		     	   // vendor of the server hardware 
	XID private3;
	XID private4;
	XID private5;
	int private6;
	int private8; 
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
	void* private9;
	void* private10;
	int qlen;	                   // Lenght of input event queue

};

/////////////////////////////////////////////////////////////////

struct XVisual 
{
	XExtData* data;                // Hook extencion data
	VisualID visualid;             // Visual id of this visual
	int cpp_class;                 // C++ class of screen 
	unsigned long red_mask;        // Mask value red
	unsigned long green_mask;      // Mask value green
	unsigned long blue_mask;	   // Mask value blue
	int bits_per_rgb;	           // log base 2 of distinct color values 
	int map_entries;			   // color map entries 
};

/////////////////////////////////////////////////////////////////

struct XSetWindowAttributes
{
    Pixmap background_pixmap;	    // Background or None or ParentRelative 
    unsigned long background_pixel;	// Background pixel 
    Pixmap border_pixmap;			// Border of the window 
    unsigned long border_pixel;		// Border pixel value 
    int bit_gravity;				// One of bit gravity values 
    int win_gravity;				// One of the window gravity values 
    int backing_store;				// NotUseful, WhenMapped, Always 
    unsigned long backing_planes;	// Planes to be preserved if possible 
    unsigned long backing_pixel;	// Value to use in restoring planes 
    Bool save_under;				// Should bits under be saved? (popups) 
    long event_mask;				// Set of events that should be saved 
    long do_not_propagate_mask;		// Set of events that should not propagate 
    Bool override_redirect;			// Boolean value for override-redirect 
    Colormap colormap;				// Color map to be associated with window 
    Cursor cursor;					// Cursor to be displayed (or None) 
};

/////////////////////////////////////////////////////////////////

extern "C" 
{

	XWindow XDefaultRootWindow(
    	XDisplay*			   // display 
	);

	int XMapWindow(
	    XDisplay*,			   // display 
   	    XWindow		     	   // window 
	);

	int XFlush(
    	XDisplay*			   // display 	
	);

	////////////////////////////////////////////////////////////
	
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
		XWindow         		// Parent
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

	int XStoreName(
		XDisplay*,              // Display
		XWindow,                // X
		const char* name       // Window name
	);
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
	#endif //// x11.hpp ////
///////////////////////////////////////////////////////////////////