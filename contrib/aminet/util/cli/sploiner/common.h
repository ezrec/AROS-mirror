#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usage.h"
#include "getopt.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef SEEK_SET
#define SEEK_SET        0       /* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define SEEK_CUR        1       /* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define SEEK_END        2       /* set file offset to EOF plus offset */
#endif

int FileSize(FILE *fp);
void OutputName(char *name, char *outfile);
extern const char Version[];
