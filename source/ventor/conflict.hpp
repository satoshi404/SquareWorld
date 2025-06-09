#ifndef CONFLICT_QUARD
#define CONFLICT_QUARD

	#define Window XWindow
	#define Display XDisplay
	#define Screen XScreen

#else
	#undef Window
	#undef Display
	#undef Screen
#undef CONFLICT_QUARD
#endif