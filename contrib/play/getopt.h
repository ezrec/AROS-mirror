#ifndef GETOPT_H

#define GETOPT_H

extern int opterr;
extern int optind;

extern char *optarg;

extern int getopt(int argc, char *argv[], char *opstring);


#endif
