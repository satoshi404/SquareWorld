#ifndef CONFLICT_QUARD
#define CONFLICT_QUARD

	#define Window  XWindow
	#define Display XDisplay
	#define Screen  XScreen
	#define Visual  XVisual
#else
	#undef Window
	#undef Display
	#undef Screen
	#undef Visual

#undef CONFLICT_QUARD
#endif