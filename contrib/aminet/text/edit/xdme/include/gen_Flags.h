/* MACHINE GENERATED FILE - DO NOT EDIT! */

#define DEFFLAG(date,name,init) ULONG _ ## name : 1;

struct genflags {
    DEFFLAG( 93-04-09, MathInfix, 0 )
    DEFFLAG( 94-09-09, VSTK_Internal, 0 )
    DEFFLAG( 94-09-19, APIC_Active, 0 )
    DEFFLAG( 94-09-21, CMDSH_Active, 0 )
    DEFFLAG( 94-09-21, CMDSH_ErrorsOut, 0 )
    DEFFLAG( 94-09-21, CMDSH_WarningsOut, 0 )
};
 extern struct genflags genflags;

#undef  DEFFLAG
#define DEFFLAG(date,name,init)

#define MathInfix genflags._MathInfix
#define VSTK_Internal genflags._VSTK_Internal
#define APIC_Active genflags._APIC_Active
#define CMDSH_Active genflags._CMDSH_Active
#define CMDSH_ErrorsOut genflags._CMDSH_ErrorsOut
#define CMDSH_WarningsOut genflags._CMDSH_WarningsOut
