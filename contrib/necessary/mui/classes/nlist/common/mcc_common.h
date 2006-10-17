/*
** common header
*/

#ifdef MORPHOS
 #define DoMethod(MyObject, tags...) \
	({ULONG _tags[] = { tags }; DoMethodA((MyObject), (APTR)_tags);})
 #define DoSuperMethod(MyClass, MyObject, tags...) \
	({ULONG _tags[] = { tags }; DoSuperMethodA((MyClass), (MyObject), (APTR)_tags);})
 #define CoerceMethod(MyClass, MyObject, tags...) \
	({ULONG _tags[] = { tags }; CoerceMethodA((MyClass), (MyObject), (APTR)_tags);})

 #define MADTRAP(x)	struct EmulLibEntry x = { TRAP_LIB, 0, (void (*)(void)) x ## _gate }
#else
 #define MADTRAP(x)
#endif


#ifdef SAVEDS
#undef SAVEDS
#endif

#ifdef __SASC

#	define REG( r )				register __ ## r
#	define GNUCREG( r )
#	define SAVEDS				__saveds
#	define ASM					__asm
#	define REGARGS				__regargs
#	define STDARGS				__stdargs
#	define STACKARGS			__stdargs
#	define ALIGNED				__aligned
#	define FAR					__far
#	define INLINE				__inline

#else
#	ifdef __GNUC__
#    ifdef MORPHOS
#		define REG( r )
#		define GNUCREG( r )
#		define SAVEDS
#		define ASM
#		define REGARGS			__regargs
#		define STDARGS			__stdargs
#		define SACKARGS			__stdargs
#		define ALIGNED			__aligned
#		define FAR				__far
#		define INLINE			__inline
#		define NO_PRAGMAS
#		define USE_GLUE
#    else

#		define REG( r )
#		define GNUCREG( r )		__asm( #r )
#		define SAVEDS			__saveds
#		define ASM
#		define REGARGS			__regargs
#		define STDARGS			__stdargs
#		define STACKARGS		__stdargs
#		define ALIGNED			__aligned
#		define FAR				__far
#		define INLINE
#		define NO_PRAGMAS
#    endif

#	else /* Any other compiler. */

#		define REG( r )
#		define GNUCREG( r )
#		define SAVEDS
#		define ASM
#		define REGARGS
#		define STDARGS
#		define SACKARGS
#		define ALIGNED
#		define FAR
#		define INLINE

#	endif /* __GNUC__ */
#endif /* __SASC */

