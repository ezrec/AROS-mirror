#include <aros/asmcall.h>

#include "m68k.h"
#include "callfuncs.h"

extern struct M68k CPU;

#define CPU_A0	CPU.A[0]
#define CPU_A1	CPU.A[1]
#define CPU_A2	CPU.A[2]
#define CPU_A3	CPU.A[3]
#define CPU_A4	CPU.A[4]
#define CPU_A5	CPU.A[5]
#define CPU_A6	CPU.A[6]
#define CPU_A7	CPU.A[7]

#define CPU_D0	CPU.D[0]
#define CPU_D1	CPU.D[1]
#define CPU_D2	CPU.D[2]
#define CPU_D3	CPU.D[3]
#define CPU_D4	CPU.D[4]
#define CPU_D5	CPU.D[5]
#define CPU_D6	CPU.D[6]
#define CPU_D7	CPU.D[7]

#define func1(reg) 					\
VOID call_##reg##(int (*func)()) 			\
{							\
  CPU.D[0] = AROS_UFC2(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg##, reg),	\
                AROS_UFCA(ULONG, CPU.A[6]   , A6));	\
}

#define func1_VOID(reg) 				\
VOID call_##reg##_VOID(int (*func)()) 			\
{							\
  CPU.D[0] = AROS_UFC2(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg##, reg),	\
                AROS_UFCA(ULONG, CPU.A[6]   , A6));	\
}

#define func2(reg1,reg2) 				\
VOID call_##reg1##_##reg2##(int (*func)()) 		\
{							\
  CPU.D[0] = AROS_UFC3(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func2_VOID(reg1,reg2) 				\
VOID call_##reg1##_##reg2##_VOID(int (*func)()) 	\
{							\
          AROS_UFC3(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func3(reg1,reg2,reg3) 				\
VOID call_##reg1##_##reg2##_##reg3##(int (*func)()) 	\
{							\
  CPU.D[0] = AROS_UFC4(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU_##reg3##, reg3),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func3_VOID(reg1,reg2,reg3) 			\
VOID call_##reg1##_##reg2##_##reg3##_VOID(int (*func)())\
{							\
          AROS_UFC4(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU_##reg3##, reg3),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func4(reg1,reg2,reg3,reg4) 			\
VOID call_##reg1##_##reg2##_##reg3##_##reg4##(int (*func)()) 	\
{							\
  CPU.D[0] = AROS_UFC5(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU_##reg3##, reg3),	\
                AROS_UFCA(ULONG, CPU_##reg4##, reg4),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func4_VOID(reg1,reg2,reg3,reg4) 		\
VOID call_##reg1##_##reg2##_##reg3##_##reg4##_VOID(int (*func)())\
{							\
          AROS_UFC5(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU_##reg3##, reg3),	\
                AROS_UFCA(ULONG, CPU_##reg4##, reg4),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func5(reg1,reg2,reg3,reg4,reg5) 		\
VOID call_##reg1##_##reg2##_##reg3##_##reg4##_##reg5##(int (*func)()) 	\
{							\
  CPU.D[0] = AROS_UFC6(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU_##reg3##, reg3),	\
                AROS_UFCA(ULONG, CPU_##reg4##, reg4),	\
                AROS_UFCA(ULONG, CPU_##reg5##, reg5),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}

#define func5_VOID(reg1,reg2,reg3,reg4,reg5) 		\
VOID call_##reg1##_##reg2##_##reg3##_##reg4##_##reg5##_VOID(int (*func)())\
{							\
          AROS_UFC6(ULONG, func,			\
                AROS_UFCA(ULONG, CPU_##reg1##, reg1),	\
                AROS_UFCA(ULONG, CPU_##reg2##, reg2),	\
                AROS_UFCA(ULONG, CPU_##reg3##, reg3),	\
                AROS_UFCA(ULONG, CPU_##reg4##, reg4),	\
                AROS_UFCA(ULONG, CPU_##reg5##, reg5),	\
                AROS_UFCA(ULONG, CPU.A[6]    , A6));	\
}


VOID call_VOID(int (*func)())
{
}

VOID call_NOPARM(int (*func)())
{
  CPU.D[0] = AROS_UFC1(ULONG, func,
                AROS_UFCA(ULONG, CPU.A[6], A6));
}

func1(A0)
func1(D0)
func1(A1)
func1(D1)

func1_VOID(A0)
func1_VOID(A1)
func1_VOID(A5)

func1_VOID(D0)
func1_VOID(D1)
func1_VOID(D7)

/* 2 arguments */
func2(A1,D0)
func2(D0,D1)
func2(D0,A1)
func2(A1,D1)
func2(A0,D0)
func2(A0,A1)

func2_VOID(A0, A1)
func2_VOID(A1, D0)
func2_VOID(D0, D1)
func2_VOID(D0, A1)

/* 3 arguments */
func3_VOID(A1,A2,D0)
func3_VOID(A0,A1,D0)

func3(A0,A1,A2)
func3(A1,A2,A3)
func3(A1,A0,D0)

/* 4 arguments */
func4(A0,D0,A1,D1)

/* 5 arguments */
func5(A0,A1,A2,D0,D1)
