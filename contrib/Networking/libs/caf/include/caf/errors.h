#ifndef ERRORS_H
#define ERRORS_H

char *GetErrnoDesc(int err_code);

struct ErrnoEntry 
{
	int error_code;
	char *error_desc;
};

#endif
