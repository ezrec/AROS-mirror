/* Copyright: Richard Joergensen                             */
/* Internet: ric@daimi.aau.dk (at last until summer 1995)    */
/* Disclaimer: I take NO responsibilities for this program.  */
/* Any use is on your own risk.                              */

#include "common.h"
#include "split.h"
#include "join.h"
#include "repair.h"

/* This is a versionstring for use with the Amiga-program "version" */
const char Version[]="$VER: Sploiner 1.01 ("__DATE__" "__TIME__")\0";

char *prgname;

int main(int argc, char *argv[])
{
  prgname = *argv++;
  if (argc-- < 2) usage();

  if (strcmp(*argv,"split")==0)
    split(--argc, ++argv);
  else if (strcmp(*argv,"join")==0)
    join(--argc, ++argv);
  else if (strcmp(*argv,"repair")==0)
    repair(--argc, ++argv);
  else if (strcmp(*argv,"example")==0)
    usage_example();
  else usage();
  return 0;
}

int FileSize(FILE *fp)
{
  long size, pos=ftell(fp);
  
  fseek(fp,0,SEEK_END);      /* Goto EOF */
  size=ftell(fp);            /* Read position */
  fseek(fp,pos,SEEK_SET);    /* Goto old position */
  return(size);
}

void OutputName(char *name, char *outfile)
{
  int ext;
  
  for(ext=0 ; ((outfile[ext]=name[ext]) != '\0') ; ext++);

  outfile[ext]='.';
  outfile[ext+1] = '0';
  outfile[ext+2] = '0';
  outfile[ext+3] = '0';
  outfile[ext+4] = '\0';
}

