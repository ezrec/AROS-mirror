/****************************************************************************
 * getopt():	Return the next user option on each iteration. 
 *		This is a clone of the usual UNIX getopt() function.
 *		If you have never used a getopt() before, you'll have to
 * 		 read about it on any UNIX machine or other C system that
 * 		 documents it.
 *
 * Author:	Daniel J. Barrett, barrett@cs.umass.edu.
 *
 * License:	This code is placed in the Public Domain.
 *		Give it away to anybody for free!
 *		Use it for any purpose you like!
 *
 *		If you use this code in a program, please give me credit
 *		for my work.  Thanks!
 *
 * Why I wrote it:
 *
 *		Because every other getopt() function I have ever seen
 *		 had source code that was difficult to understand.
 *		I wrote this code to be very modular and readable.
 *		I hope you find it instructive and/or helpful.
 *
 * BUGS
 *		"--JUNK" is treated as "--", the "end of options" flag.
 *
 * REVISION HISTORY:
 *
 *	Version:	1.3
 *	Date:		January 21, 1993
 *	Comments:	Bugfix.  Option "-:" was accepted as OK.  Not anymore.
 *
 *	Version:	1.2a
 *	Date:		January 21, 1993
 *	Comments:	Improved the TESTME main program.  More useful output.
 *			Rewrote Error() to eliminate redundancy.
 *
 *	Version:	1.2
 *	Date:		January 21, 1993
 *	Comments:	If a legal option is not followed by a required
 *			argument, getopt() previously returned the option's
 *			character.  It should have returned UNKNOWN_OPT.
 *			This has been fixed.  Note that HandleArgument()
 *			now returns an int value rather than void.
 *
 *	Version:	1.1b
 *	Date:		December 17, 1991.
 *	Comments:	Minor change of NULL to '\0' in 1 spot.
 *
 *	Version:	1.1
 *	Date:		February 20, 1991.
 *	Comments:	Bug fix in Pass().  Forgot to check that the
 *			current argument is non-empty and starts with
 *			a DASH.
 *
 *			Got rid of the unnecessary "g_" at the beginning
 *			of each function name.  Since they're static, we
 *			don't have to worry about duplication of names
 *			by the calling program.
 *
 *	Version:	1.0
 *	Date:		April 12, 1990.
 *	Comments:	First released version.
 *
 ****************************************************************************/

#ifndef __STDIO_H	/* If we haven't already included stdio.h, do it. */
# include <stdio.h>	/* Maybe someday I'll eliminate this. */
#endif

#ifndef __STRING_H
# include <string.h>	/* For strchr() */
#endif

/************************************************************************
* Some constants.
************************************************************************/

#define	DASH		'-'	/* This preceeds an option. */
#define	ARG_COMING	':'	/* In the option string, this indicates that
				 * that the option requires an argument. */
#define	UNKNOWN_OPT	'?'	/* The char returned for unknown option. */

/************************************************************************
* Internal error codes.
************************************************************************/

#define	ERROR_BAD_OPTION	1
#define	ERROR_MISSING_ARGUMENT	2

/************************************************************************
* ANSI function prototypes.
************************************************************************/

int		getopt(int argc, char *argv[], char *optString);
static int	NextOption(char *argv[], char *optString);
static int	RealOption(char *argv[], char *str, int *skip, int *ind,
			     int opt);
static int	HandleArgument(char *argv[], int *optind, int *skip);
static void	Error(int err, int c);
static void	Pass(char *argv[], int *optind, int *optsSkipped);

/************************************************************************
* Global variables.  You must declare these externs in your program
* if you want to see their values!
************************************************************************/
	
char	*optarg	= NULL;	/* This will point to a required argument, if any. */
int	optind	= 1;	/* The index of the next argument in argv. */
int	opterr	= 1;	/* 1 == print internal error messages.  0 else. */
int	optopt;		/* The actual option letter that was found. */


int getopt(int argc, char *argv[], char *optString)
{
	optarg = NULL;
	if (optind < argc)		/* More arguments to check. */
		return(NextOption(argv, optString));
	else				/* We're done. */
		return(EOF);
}


/* If the current argument does not begin with DASH, it is not an option.
 * Return EOF.
 * If we have ONLY a DASH, and nothing after it... return EOF as well.
 * If we have a DASH followed immediately by another DASH, this is the
 * special "--" option that means "no more options follow."  Return EOF.
 * Otherwise, we have an actual option or list of options.  Process it. */
	
static int NextOption(char *argv[], char *optString)
{
	static int optsSkipped = 0;	/* In a single argv[i]. */
	
	if ((argv[optind][0] == DASH)
	&&  ((optopt = argv[optind][1+optsSkipped]) != '\0'))
	{
		if (optopt == DASH)
		{
			optind++;
			return(EOF);
		}
		else
			return(RealOption(argv, optString, &optsSkipped,
					    &optind, optopt));
	}
	else
		return(EOF);
}


/* At this point, we know that argv[optind] is an option or list of
 * options.  If this is a list of options, *optsSkipped tells us how
 * many of those options have ALREADY been parsed on previous calls
 * to getopt().
 * If the option is not legal (not in optString), complain and return
 * UNKNOWN_OPT.
 * If the option requires no argument, just return the option letter.
 * If the option requires an argument, call HandleArgument and return
 * the option letter. */
	
static int RealOption(char *argv[], char *optString, int *optsSkipped,
			int *optind, int optopt)
{
	char *where;	/* Pointer to the letter in optString. */
	int argWasOK;	/* An indication that a required arg was found. */

	(*optsSkipped)++;

	if (optopt == ARG_COMING)	/* Special case of "-:" */
	{
		Error(ERROR_BAD_OPTION, optopt);
		Pass(argv, optind, optsSkipped);
		return(UNKNOWN_OPT);
	}

	else if ((where = strchr(optString, optopt)) != NULL)
	{
		argWasOK = 1;	/* Assume any argument will be found. */

		if (*(where+1) == ARG_COMING)
			argWasOK = HandleArgument(argv, optind, optsSkipped);

		Pass(argv, optind, optsSkipped);

		return(argWasOK ? optopt : UNKNOWN_OPT);
	}

	else
	{
		Error(ERROR_BAD_OPTION, optopt);
		Pass(argv, optind, optsSkipped);
		return(UNKNOWN_OPT);
	}
}


/* We have an option in argv[optind] that requires an argument.  If there
 * is no whitespace after the option letter itself, take the rest of
 * argv[optind] to be the argument.
 * If there IS whitespace after the option letter, take argv[optind+1] to
 * be the argument.
 * Otherwise, if there is NO argument, complain!
 * Return 1 on success, 0 on error.
 */

static int HandleArgument(char *argv[], int *optind, int *optsSkipped)
{
	int OK;

	if (argv[*optind][1+(*optsSkipped)])
	{
		optarg = argv[*optind] + 1 + (*optsSkipped);
		OK = 1;
	}
	else if (argv[(*optind)+1])
	{
		optarg = argv[(*optind)+1];
		(*optind)++;
		OK = 1;
	}
	else
	{
		Error(ERROR_MISSING_ARGUMENT, optopt);
		OK = 0;
	}

	(*optsSkipped) = 0;
	(*optind)++;
	return(OK);
}


/* Print an appropriate error message. */

static void Error(int err, int c)
{
	if (opterr)
	{
		fprintf(stderr, "-%c: ", c);
		switch (err)
		{
			case ERROR_BAD_OPTION:
				fprintf(stderr, "Illegal option.\n");
				break;
			case ERROR_MISSING_ARGUMENT:
				fprintf(stderr,
				  "An argument is required, but missing.\n");
				break;
			default:
				fprintf(stderr,
				  "An unknown error occurred.\n");
				break;
		}
	}
}


/* We have reached the end of argv[optind]... there are no more options
 * in it to parse.  Skip to the next item in argv. */

static void Pass(char *argv[], int *optind, int *optsSkipped)
{
	if (argv[*optind]
	&&  (argv[*optind][0] == DASH)
	&&  (argv[*optind][1+(*optsSkipped)] == '\0'))
	{
		(*optind)++;
		(*optsSkipped) = 0;
	}
}

/***************************************************************************
* A test program.  Compile this file with -DTESTME as an option.
***************************************************************************/

#ifdef TESTME
main(int argc, char *argv[])
{
	char c;
	int i;

	while ((c = getopt(argc, argv, "a:b:cde")) != EOF)
    	{
		printf("OPTION %c", c);
		if ((c == 'a') || (c == 'b'))
			printf(", arg=\"%s\"\n", optarg);
		else
			printf("\n");
	}

	for (i=optind; i<argc; i++)
	{
		printf("Arg %d (argv[%d]): \"%s\"\n",
			i - optind + 1,
			i,
			argv[i]);
	}

	exit(0);
}
#endif
