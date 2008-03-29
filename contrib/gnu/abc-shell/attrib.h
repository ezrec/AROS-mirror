#ifdef __GNUC__
#define GCC_FUNC_ATTR(x)        __attribute__((x))
#define GCC_FUNC_ATTR2(x,y)     __attribute__((x,y))
#endif

#ifdef __VBCC__
#define GCC_FUNC_ATTR(x)
#define GCC_FUNC_ATTR2(x,y)
#endif
