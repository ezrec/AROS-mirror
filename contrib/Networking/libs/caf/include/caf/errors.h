#ifndef ERRORS_H
#define ERRORS_H

#ifdef __MORPHOS__
char *GetErrnoDesc(int err_code);

struct ErrnoEntry 
{
	int error_code;
	char *error_desc;
};
#else
#include <string.h>
#define GetErrnoDesc strerror
#endif

#endif
