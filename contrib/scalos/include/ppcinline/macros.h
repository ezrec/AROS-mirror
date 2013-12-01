#if !defined(__INLINE_MACROS_H_SCALOS)
#define __INLINE_MACROS_H_SCALOS

#include_next "ppcinline/macros.h"

// this definition is missing in SDK macros.h
#define LP8FP(offs, rt, name, t1, v1, r1, t2, v2, r2, t3, v3, r3, t4, v4, r4, t5, v5, r5, t6, v6, r6, t7, v7, r7, t8, v8, r8, bt, bn, fpt, cm1, cs1, cl1, cm2, cs2, cl2 )   \
({                                                              \
	  typedef fpt;						\
	  t1 _##name##_v1 = v1;					\
	  t2 _##name##_v2 = v2;					\
	  t3 _##name##_v3 = v3;					\
	  t4 _##name##_v4 = v4;					\
	  t5 _##name##_v5 = v5;					\
	  t6 _##name##_v6 = v6;					\
	  t7 _##name##_v7 = v7;					\
	  t8 _##name##_v8 = v8;					\
	  rt _##name##_re;					\
	  REG_##r1           = (ULONG) _##name##_v1;		\
	  REG_##r2           = (ULONG) _##name##_v2;		\
	  REG_##r3           = (ULONG) _##name##_v3;		\
	  REG_##r4           = (ULONG) _##name##_v4;		\
	  REG_##r5           = (ULONG) _##name##_v5;		\
	  REG_##r6           = (ULONG) _##name##_v6;		\
	  REG_##r7           = (ULONG) _##name##_v7;		\
	  REG_##r8           = (ULONG) _##name##_v8;		\
	  REG_A6             = (ULONG) (bn);			\
	  _##name##_re = (rt) (*MyEmulHandle->EmulCallDirectOS)(-offs);\
	  _##name##_re;						\
})

#endif //!defined(__INLINE_MACROS_H_SCALOS)

