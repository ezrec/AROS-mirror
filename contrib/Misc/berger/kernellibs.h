/*
   kernellibs.h
   
   Include file for kernellibs.c
*/

struct LibCall
{
  int (*func)();
  STRPTR funcname;
};

struct Libbases
{
  struct Library * baseptr;
  struct LibCall * libcall;
};
