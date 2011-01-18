/*
 *  good_double.h
 */

/*
 *	you may comment this out to save c. 200 bytes, if not interested
 *	in the MIDDLE button (just put /* at the start of the line)
 */
#define ALL_BUTTONS /* */

/*
 *	we have registerized function arguments if this #define below is on;
 *	have it commented out if you don't want registerized arguments
 *	(e.g. if your compiler doesn't support these)
 */
/* #define GDC_REGARGS /* */


#ifdef  ALL_BUTTONS
	/*
	 *	our macros
	 */
    #define LeftDouble(s,m)    GoodDouble(0,s,m)
    #define MiddleDouble(s,m)  GoodDouble(1,s,m)
    #define RightDouble(s,m)   GoodDouble(2,s,m)

    /*
	 *	function prototype
	 */
	#ifndef GDC_REGARGS
		BOOL GoodDouble( LONG, ULONG, ULONG );
	#else
		#ifdef _DCC				/* i.e. DICE */
    		__regargs BOOL GoodDouble( LONG, ULONG, ULONG );
		#else
			/*
			 *	typical non-DICE compiler wants it like that
			 */
			BOOL GoodDouble( register LONG, register ULONG, register ULONG );
		#endif
	#endif
#else
    /*
     * 	if you are only interested in LEFT & RIGHT buttons, and
     * 	want this object to be slightly smaller, use these instead of
     * 	the ones above
     */

     /*
	  *	our macros
	  */
     #define LeftDouble(s,m)    GoodDouble2(0,s,m)
     #define MiddleDouble(s,m)  GoodDouble2(1,s,m)   /* <--- dummy! */
     #define RightDouble(s,m)   GoodDouble2(2,s,m)

     /*
	  *	function prototype
	  */
	#ifndef GDC_REGARGS
		BOOL GoodDouble2( LONG, ULONG, ULONG );
	#else
		#ifdef _DCC				/* i.e. DICE */
    		__regargs BOOL GoodDouble2( LONG, ULONG, ULONG );
		#else
			/*
			 *	typical non-DICE compiler wants it like that
			 */
			BOOL GoodDouble2( register LONG, register ULONG, register ULONG );
		#endif
	#endif
#endif
