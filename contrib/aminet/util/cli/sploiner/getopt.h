/*************************************************************************
* getopt.h:	Header file for using getopt().
* Author:	Daniel J. Barrett, barrett@cs.umass.edu
* Status:	Public domain.
*************************************************************************/


#ifndef _GETOPT_H		/* Insure we #include me only once. */
#define	_GETOPT_H 1


/*************************************************************************
* optarg
* This variable is READ-ONLY!  Do not set its value yourself!
*
* If an option must be followed by a string, then optarg is a pointer
* to that string.
*
* If an option should NOT be followed by an string, then optarg is
* NOT DEFINED.  It is an error to reference optarg if no argument is
* expected.
*************************************************************************/

	extern char *optarg;

/*************************************************************************
* optind
* This variable is READ-ONLY!  Do not set its value yourself!
*
* After getopt() returns EOF, this variable contains the index of the
* next unprocessed command-line argument.
*
* That is, argv[optind] is the first argument AFTER the options.
*
* If optind == argc-1, then there are no arguments after the options.
*************************************************************************/

	extern int optind;

/*************************************************************************
* optopt
* This variable is READ-ONLY!  Do not set its value yourself!
*
* After each call of getopt(), this variable contains the option character
* which was found on this call.
*
* Normally, you do not need to examine this variable because getopt()
* returns the value of the character it read.
* However, when getopt() discovers an illegal option, it returns the 
* character '?'.  You now examine optopt to find the actual character
* which getopt() read.
*************************************************************************/

	extern int optopt;

/*************************************************************************
* opterr
* This variable is READ/WRITE.  You may set its value yourself.
*
* If opterr != 0, then getopt() will print its own error messages
* on standard error (stderr).  Error messages are of the form:
*
*		-x: Illegal option.
*		-x: An argument is required, but missing.
*
* assuming that the illegal option "-x" was read by getopt().
*
* If opterr == 0, these error messages are suppressed.
* 
* By default, opterr == 1, meaning that error messages are printed.
*************************************************************************/

	extern int opterr;

/*************************************************************************
* getopt()
* The function prototype.
*************************************************************************/


#ifdef __STDC__
      /*  extern int getopt(int argc, char *argv[], char *optionString); */
     extern int getopt(int, char * const [], const char *);
#else
       extern int getopt();
#endif /* __STDC__ */


#endif /* _GETOPT_H */
