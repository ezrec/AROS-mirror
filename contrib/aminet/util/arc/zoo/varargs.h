/* varargs.h */
#ifndef __VARARGS_H
#define __VARARGS_H
typedef char *va_list;
#define va_dcl			int va_alist;
#define va_start(list)          list = (char *) &va_alist
#define va_end(list)
#define va_arg(list,type)       ((type*)(list+=sizeof(type)))[-1]
#endif


