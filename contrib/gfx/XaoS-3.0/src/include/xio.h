#ifndef XIO1_H
#define XIO1_H 1
#include <config.h>

#ifdef USE_STDIO
#include <stdio.h>
typedef char *xio_path;
typedef char xio_pathdata[4096];
typedef FILE *xio_file;
  /*returns xio_file or XIO_FAILED in case it failed*/
#define xio_ropen(fname) fopen(fname,"r")
#define xio_wopen(fname) fopen(fname,"w")
#define xio_rbopen(fname) fopen(fname,"rb")
#define xio_wbopen(fname) fopen(fname,"wb")
  /*returns non zero in case of error*/
#define xio_close(fname) fclose(fname)
#define XIO_FAILED NULL
#define XIO_EMPTYPATH "./" /*Should be also call to currentdir function*/
#define XIO_EOF EOF

#define xio_addfname(destination, dirrectory, filename) \
{strcpy(destination,dirrectory);if (destination[strlen(destination)-1]!='/') strcat(destination,"/");strcat(destination,filename);}
#define xio_addextension(destination,extension) strcat(destination,extension)

#define myputs(s) ((fputs(s,uih->savec->file)==EOF)?outputerror(uih),1:0)
#define myputc(s) ((putc(s,uih->savec->file)==EOF)?outputerror(uih),1:0)
#define myprintf(s,s1) ((fprintf(uih->savec->file,s,s1)<=0)?outputerror(uih),1:0)

#define xio_getc(s) getc(s)
#define xio_ungetc(s,c) ungetc(s,c)
#define xio_feof(s) feof(s)

xio_path xio_getdirectory(xio_path name);
xio_path xio_getfilename(char *base, char *extension);
xio_file xio_getrandomexample(xio_path name);
xio_file xio_getcatalog(char *name);
xio_file xio_gettutorial(char *name,xio_path result);
 /*look trought directory with examples, choose one file, open it (and return
  *descriptor+put name into name parameter*/

#ifdef _plan9_
#define xio_errorstring() errstr
#else
#define xio_errorstring() strerror(errno)
#endif /*plan9*/

#endif/*USE_STDIO*/

#endif
