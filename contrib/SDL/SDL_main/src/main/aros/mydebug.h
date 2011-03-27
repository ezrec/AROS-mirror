#ifndef MYDEBUG

#ifdef AMIGADEBUG
#define D(x) x
#define bug kprintf
extern void kprintf(const char *,...);
#else
#define D(x)
#endif

#define MAKE_HIT() {char *a=NULL,b;b=*a;}

#define MYDEBUG
#endif
