#ifndef GETOPT_H
#define GETOPT_H
/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

 */

extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
int getopt(int argc, char * const argv[], char const *opts);

#endif /* GETOPT_H */
