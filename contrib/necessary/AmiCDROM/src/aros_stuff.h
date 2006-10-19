#ifndef __AROS__
#define dl_OldName dl_Name

#define AROS_BSTR_ADDR(s)        (((STRPTR)BADDR(s))+1)
#define AROS_BSTR_strlen(s)      (AROS_BSTR_ADDR(s)[-1])
#define AROS_BSTR_getchar(s,l)   (AROS_BSTR_ADDR(s)[l])

#endif

