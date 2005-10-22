#ifndef _BITS_MATHINLINE_H
#if defined(__i386__) || defined(__PPC__)
/*A bit of gcc magic:) */
#include <math.h>
#ifndef __GLIBC__
#define __MATH_INLINE extern inline
#define __inline_mathcode(func, arg, code) \
  __inline_mathcode_ (double, func, arg, code)                                \
  __inline_mathcode_ (float, __CONCAT(func,f), arg, code)                     \
  __inline_mathcode_ (long double, __CONCAT(func,l), arg, code)
#define __inline_mathop(func, op) \
    __inline_mathop_ (double, func, op)                                         \
    __inline_mathop_ (float, __CONCAT(func,f), op)                              \
    __inline_mathop_ (long double, __CONCAT(func,l), op)
#define __inline_mathop_(float_type, func, op) \
    __inline_mathop_decl_ (float_type, func, op, "0" (__x))
#define __inline_mathcode_(float_type, func, arg, code) \
    __MATH_INLINE float_type func (float_type);                                 \
    __MATH_INLINE float_type func (float_type arg)                              \
    {                                                                           \
      code;                                                                     \
    }
#define __inline_mathop_decl_(float_type, func, op, params...) \
    __MATH_INLINE float_type func (float_type);                                 \
    __MATH_INLINE float_type func (float_type __x)                                      \
{                                                                           \
  register float_type __result;                                             \
    __asm __volatile__ (op : "=t" (__result) : params);               \
    return __result;                                                          \
}


#if defined __GNUC__ && (__GNUC__ > 2 || __GNUC__ == 2 && __GNUC_MINOR__ >= 8)
__inline_mathcode_ (long double, __fabsl, __x, return __builtin_fabsl (__x))
__inline_mathcode_ (double, fabs, __x, return __builtin_fabs (__x))
__inline_mathcode_ (float, fabsf, __x, return __builtin_fabsf (__x))
#else
#ifndef DJGPP
__inline_mathop (fabs, "fabs")
#endif
__inline_mathop_ (long double, __fabsl, "fabs")
#define fabsf(x) fabs((double)(x))
#endif
#endif
#endif
#endif

#include <aconfig.h>
#ifdef HAVE_LONG_DOUBLE
#ifndef __OPTIMIZE__
#define myabs(x) ((x)>0?(x):-(x))
#else
#define myabs(x) __fabsl((number_t)(x))
#endif
#else
#define myabs(x) fabs((number_t)(x))
#endif
