/*	$NetBSD: getconf.c,v 1.34 2008/08/27 08:56:49 christos Exp $	*/

/*-
 * Copyright (c) 1996, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by J.T. Conklin.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: getconf.c,v 1.34 2008/08/27 08:56:49 christos Exp $");
#endif /* not lint */

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct conf_variable
{
  const char *name;
  enum { SYSCONF, CONFSTR, PATHCONF, CONSTANT } type;
  long value;
};

static void print_longvar(const char *, long);
static void print_strvar(const char *, const char *);
static void printvar(const struct conf_variable *, const char *);
static void usage(void) __dead2;

static const struct conf_variable conf_table[] =
{
#if defined(_CS_PATH)
  { "PATH",			CONFSTR,	_CS_PATH		},
#endif

  /* Utility Limit Minimum Values */
#if defined(_POSIX2_BC_BASE_MAX)
  { "POSIX2_BC_BASE_MAX",	CONSTANT,	_POSIX2_BC_BASE_MAX	},
#endif
#if defined(_POSIX2_BC_DIM_MAX)
  { "POSIX2_BC_DIM_MAX",	CONSTANT,	_POSIX2_BC_DIM_MAX	},
#endif
#if defined(_POSIX2_BC_SCALE_MAX)
  { "POSIX2_BC_SCALE_MAX",	CONSTANT,	_POSIX2_BC_SCALE_MAX	},
#endif
#if defined(_POSIX2_BC_STRING_MAX)
  { "POSIX2_BC_STRING_MAX",	CONSTANT,	_POSIX2_BC_STRING_MAX	},
#endif
#if defined(_POSIX2_COLL_WEIGHTS_MAX)
  { "POSIX2_COLL_WEIGHTS_MAX",	CONSTANT,	_POSIX2_COLL_WEIGHTS_MAX },
#endif
#if defined(_POSIX2_EXPR_NEST_MAX)
  { "POSIX2_EXPR_NEST_MAX",	CONSTANT,	_POSIX2_EXPR_NEST_MAX	},
#endif
#if defined(_POSIX2_LINE_MAX)
  { "POSIX2_LINE_MAX",		CONSTANT,	_POSIX2_LINE_MAX	},
#endif
#if defined(_POSIX2_RE_DUP_MAX)
  { "POSIX2_RE_DUP_MAX",	CONSTANT,	_POSIX2_RE_DUP_MAX	},
#endif
#if defined(_POSIX2_VERSION)
  { "POSIX2_VERSION",		CONSTANT,	_POSIX2_VERSION		},
#endif

  /* POSIX.1 Minimum Values */
#if defined(_POSIX_AIO_LISTIO_MAX)
  { "_POSIX_AIO_LISTIO_MAX",	CONSTANT,	_POSIX_AIO_LISTIO_MAX	},  
#endif
#if defined(_POSIX_AIO_MAX)
  { "_POSIX_AIO_MAX",		CONSTANT,       _POSIX_AIO_MAX		},
#endif
#if defined(_POSIX_ARG_MAX)
  { "_POSIX_ARG_MAX",		CONSTANT,	_POSIX_ARG_MAX		},
#endif
#if defined(_POSIX_CHILD_MAX)
  { "_POSIX_CHILD_MAX",		CONSTANT,	_POSIX_CHILD_MAX	},
#endif
#if defined(_POSIX_LINK_MAX)
  { "_POSIX_LINK_MAX",		CONSTANT,	_POSIX_LINK_MAX		},
#endif
#if defined(_POSIX_MAX_CANON)
  { "_POSIX_MAX_CANON",		CONSTANT,	_POSIX_MAX_CANON	},
#endif
#if defined(_POSIX_MAX_INPUT)
  { "_POSIX_MAX_INPUT",		CONSTANT,	_POSIX_MAX_INPUT	},
#endif
#if defined(_POSIX_MQ_OPEN_MAX)
  { "_POSIX_MQ_OPEN_MAX",	CONSTANT,	_POSIX_MQ_OPEN_MAX	},
#endif
#if defined(_POSIX_MQ_PRIO_MAX)
  { "_POSIX_MQ_PRIO_MAX",	CONSTANT,	_POSIX_MQ_PRIO_MAX	},
#endif
#if defined(_POSIX_NAME_MAX)
  { "_POSIX_NAME_MAX",		CONSTANT,	_POSIX_NAME_MAX		},
#endif
#if defined(_POSIX_NGROUPS_MAX)
  { "_POSIX_NGROUPS_MAX",	CONSTANT,	_POSIX_NGROUPS_MAX	},
#endif
#if defined(_POSIX_OPEN_MAX)
  { "_POSIX_OPEN_MAX",		CONSTANT,	_POSIX_OPEN_MAX		},
#endif
#if defined(_POSIX_PATH_MAX)
  { "_POSIX_PATH_MAX",		CONSTANT,	_POSIX_PATH_MAX		},
#endif
#if defined(_POSIX_PIPE_BUF)
  { "_POSIX_PIPE_BUF",		CONSTANT,	_POSIX_PIPE_BUF		},
#endif
#if defined(_POSIX_SSIZE_MAX)
  { "_POSIX_SSIZE_MAX",		CONSTANT,	_POSIX_SSIZE_MAX	},
#endif
#if defined(_POSIX_STREAM_MAX)
  { "_POSIX_STREAM_MAX",	CONSTANT,	_POSIX_STREAM_MAX	},
#endif
#if defined(_POSIX_TZNAME_MAX)
  { "_POSIX_TZNAME_MAX",	CONSTANT,	_POSIX_TZNAME_MAX	},
#endif

  /* Symbolic Utility Limits */
#if defined(_SC_BC_BASE_MAX)
  { "BC_BASE_MAX",		SYSCONF,	_SC_BC_BASE_MAX		},
#endif
#if defined(_SC_BC_DIM_MAX)
  { "BC_DIM_MAX",		SYSCONF,	_SC_BC_DIM_MAX		},
#endif
#if defined(_SC_BC_SCALE_MAX)
  { "BC_SCALE_MAX",		SYSCONF,	_SC_BC_SCALE_MAX	},
#endif
#if defined(_SC_BC_STRING_MAX)
  { "BC_STRING_MAX",		SYSCONF,	_SC_BC_STRING_MAX	},
#endif
#if defined(_SC_COLL_WEIGHTS_MAX)
  { "COLL_WEIGHTS_MAX",		SYSCONF,	_SC_COLL_WEIGHTS_MAX	},
#endif
#if defined(_SC_EXPR_NEST_MAX)
  { "EXPR_NEST_MAX",		SYSCONF,	_SC_EXPR_NEST_MAX	},
#endif
#if defined(_SC_LINE_MAX)
  { "LINE_MAX",			SYSCONF,	_SC_LINE_MAX		},
#endif
#if defined(_SC_RE_DUP_MAX)
  { "RE_DUP_MAX",		SYSCONF,	_SC_RE_DUP_MAX		},
#endif

  /* Optional Facility Configuration Values */
#if defined(_SC_2_C_BIND)
  { "_POSIX2_C_BIND",		SYSCONF,	_SC_2_C_BIND		},
#endif
#if defined(_SC_2_C_DEV)
  { "POSIX2_C_DEV",		SYSCONF,	_SC_2_C_DEV		},
#endif
#if defined(_SC_2_CHAR_TERM)
  { "POSIX2_CHAR_TERM",		SYSCONF,	_SC_2_CHAR_TERM		},
#endif
#if defined(_SC_2_FORT_DEV)
  { "POSIX2_FORT_DEV",		SYSCONF,	_SC_2_FORT_DEV		},
#endif
#if defined(_SC_2_FORT_RUN)
  { "POSIX2_FORT_RUN",		SYSCONF,	_SC_2_FORT_RUN		},
#endif
#if defined(_SC_2_LOCALEDEF)
  { "POSIX2_LOCALEDEF",		SYSCONF,	_SC_2_LOCALEDEF		},
#endif
#if defined(_SC_2_SW_DEV)
  { "POSIX2_SW_DEV",		SYSCONF,	_SC_2_SW_DEV		},
#endif
#if defined(_SC_2_UPE)
  { "POSIX2_UPE",		SYSCONF,	_SC_2_UPE		},
#endif

  /* POSIX.1 Configurable System Variables */
#if defined(_SC_AIO_LISTIO_MAX)
  { "AIO_LISTIO_MAX",		SYSCONF,	_SC_AIO_LISTIO_MAX	},
#endif
#if defined(_SC_AIO_MAX)
  { "AIO_MAX",			SYSCONF,	_SC_AIO_MAX		},
#endif
#if defined(_SC_ARG_MAX)
  { "ARG_MAX",			SYSCONF,	_SC_ARG_MAX 		},
#endif
#if defined(_SC_CHILD_MAX)
  { "CHILD_MAX",		SYSCONF,	_SC_CHILD_MAX		},
#endif
#if defined(_SC_CLK_TCK)
  { "CLK_TCK",			SYSCONF,	_SC_CLK_TCK		},
#endif
#if defined(_SC_MQ_OPEN_MAX)
  { "MQ_OPEN_MAX",		SYSCONF,	_SC_MQ_OPEN_MAX		},
#endif
#if defined(_SC_MQ_PRIO_MAX)
  { "MQ_PRIO_MAX",		SYSCONF,	_SC_MQ_PRIO_MAX		},
#endif
#if defined(_SC_NGROUPS_MAX)
  { "NGROUPS_MAX",		SYSCONF,	_SC_NGROUPS_MAX		},
#endif
#if defined(_SC_OPEN_MAX)
  { "OPEN_MAX",			SYSCONF,	_SC_OPEN_MAX		},
#endif
#if defined(_SC_STREAM_MAX)
  { "STREAM_MAX",		SYSCONF,	_SC_STREAM_MAX		},
#endif
#if defined(_SC_TZNAME_MAX)
  { "TZNAME_MAX",		SYSCONF,	_SC_TZNAME_MAX		},
#endif
#if defined(_SC_JOB_CONTROL)
  { "_POSIX_JOB_CONTROL",	SYSCONF,	_SC_JOB_CONTROL 	},
#endif
#if defined(_SC_SAVED_IDS)
  { "_POSIX_SAVED_IDS",		SYSCONF,	_SC_SAVED_IDS		},
#endif
#if defined(_SC_VERSION)
  { "_POSIX_VERSION",		SYSCONF,	_SC_VERSION		},
#endif

#if defined(_PC_LINK_MAX)
  { "LINK_MAX",			PATHCONF,	_PC_LINK_MAX		},
#endif
#if defined(_PC_MAX_CANON)
  { "MAX_CANON",		PATHCONF,	_PC_MAX_CANON		},
#endif
#if defined(_PC_MAX_INPUT)
  { "MAX_INPUT",		PATHCONF,	_PC_MAX_INPUT		},
#endif
#if defined(_PC_NAME_MAX)
  { "NAME_MAX",			PATHCONF,	_PC_NAME_MAX		},
#endif
#if defined(_PC_PATH_MAX)
  { "PATH_MAX",			PATHCONF,	_PC_PATH_MAX		},
#endif
#if defined(_PC_PIPE_BUF)
  { "PIPE_BUF",			PATHCONF,	_PC_PIPE_BUF		},
#endif
#if defined(_PC_CHOWN_RESTRICTED)
  { "_POSIX_CHOWN_RESTRICTED",	PATHCONF,	_PC_CHOWN_RESTRICTED	},
#endif
#if defined(_PC_NO_TRUNC)
  { "_POSIX_NO_TRUNC",		PATHCONF,	_PC_NO_TRUNC		},
#endif
#if defined(_PC_VDISABLE)
  { "_POSIX_VDISABLE",		PATHCONF,	_PC_VDISABLE		},
#endif

  /* POSIX.1b Configurable System Variables */
#if defined(_SC_PAGESIZE)
  { "PAGESIZE",			SYSCONF,	_SC_PAGESIZE		},
#endif
#if defined(_SC_ASYNCHRONOUS_IO)
  { "_POSIX_ASYNCHRONOUS_IO",	SYSCONF,	_SC_ASYNCHRONOUS_IO	},
#endif
#if defined(_SC_FSYNC)
  { "_POSIX_FSYNC",		SYSCONF,	_SC_FSYNC		},
#endif
#if defined(_SC_MAPPED_FILES)
  { "_POSIX_MAPPED_FILES",	SYSCONF,	_SC_MAPPED_FILES	},
#endif
#if defined(_SC_MEMLOCK)
  { "_POSIX_MEMLOCK",		SYSCONF,	_SC_MEMLOCK		},
#endif
#if defined(_SC_MEMLOCK_RANGE)
  { "_POSIX_MEMLOCK_RANGE",	SYSCONF,	_SC_MEMLOCK_RANGE	},
#endif
#if defined(_SC_MEMORY_PROTECTION)
  { "_POSIX_MEMORY_PROTECTION",	SYSCONF,	_SC_MEMORY_PROTECTION	},
#endif
#if defined(_SC_MESSAGE_PASSING)
  { "_POSIX_MESSAGE_PASSING",	SYSCONF,	_SC_MESSAGE_PASSING	},
#endif
#if defined(_SC_MONOTONIC_CLOCK)
  { "_POSIX_MONOTONIC_CLOCK",	SYSCONF,	_SC_MONOTONIC_CLOCK	},
#endif
#if defined(_SC_PRIORITY_SCHEDULING)
  { "_POSIX_PRIORITY_SCHEDULING", SYSCONF,	_SC_PRIORITY_SCHEDULING },
#endif
#if defined(_SC_SEMAPHORES)
  { "_POSIX_SEMAPHORES",	SYSCONF,	_SC_SEMAPHORES		},
#endif
#if defined(_SC_SYNCHRONIZED_IO)
  { "_POSIX_SYNCHRONIZED_IO",	SYSCONF,	_SC_SYNCHRONIZED_IO	},
#endif
#if defined(_SC_TIMERS)
  { "_POSIX_TIMERS",		SYSCONF,	_SC_TIMERS		},
#endif

#if defined(_PC_SYNC_IO)
  { "_POSIX_SYNC_IO",		PATHCONF,	_PC_SYNC_IO		},
#endif

  /* POSIX.1c Configurable System Variables */
#if defined(_SC_LOGIN_NAME_MAX)
  { "LOGIN_NAME_MAX",		SYSCONF,	_SC_LOGIN_NAME_MAX	},
#endif
#if defined(_SC_THREADS)
  { "_POSIX_THREADS",		SYSCONF,	_SC_THREADS		},
#endif

  /* POSIX.1j Configurable System Variables */
#if defined(_SC_BARRIERS)
  { "_POSIX_BARRIERS",		SYSCONF,	_SC_BARRIERS		},
#endif
#if defined(_SC_READER_WRITER_LOCKS)
  { "_POSIX_READER_WRITER_LOCKS", SYSCONF,	_SC_READER_WRITER_LOCKS	},
#endif
#if defined(_SC_SPIN_LOCKS)
  { "_POSIX_SPIN_LOCKS",	SYSCONF,	_SC_SPIN_LOCKS		},
#endif

  /* XPG4.2 Configurable System Variables */
#if defined(_SC_IOV_MAX)
  { "IOV_MAX",			SYSCONF,	_SC_IOV_MAX		},
#endif
#if defined(_SC_PAGE_SIZE)
  { "PAGE_SIZE",		SYSCONF,	_SC_PAGE_SIZE		},
#endif
#if defined(_SC_XOPEN_SHM)
  { "_XOPEN_SHM",		SYSCONF,	_SC_XOPEN_SHM		},
#endif

  /* X/Open CAE Spec. Issue 5 Version 2 Configurable System Variables */
#if defined(_PC_FILESIZEBITS)
  { "FILESIZEBITS",		PATHCONF,	_PC_FILESIZEBITS	},
#endif

  /* POSIX.1-2001 XSI Option Group Configurable System Variables */
#if defined(_SC_ATEXIT_MAX)
  { "ATEXIT_MAX",		SYSCONF,	_SC_ATEXIT_MAX		},
#endif

  /* POSIX.1-2001 TSF Configurable System Variables */
#if defined(_SC_GETGR_R_SIZE_MAX)
  { "GETGR_R_SIZE_MAX",		SYSCONF,	_SC_GETGR_R_SIZE_MAX	},
#endif
#if defined(_SC_GETPW_R_SIZE_MAX)
  { "GETPW_R_SIZE_MAX",		SYSCONF,	_SC_GETPW_R_SIZE_MAX	},
#endif

#ifdef _NETBSD_SOURCE
  /* Commonly provided extensions */
  { "NPROCESSORS_CONF",		SYSCONF,	_SC_NPROCESSORS_CONF	},
  { "NPROCESSORS_ONLN",		SYSCONF,	_SC_NPROCESSORS_ONLN	},
#endif	/* _NETBSD_SOURCE */

  { NULL, CONSTANT, 0L }
};

static int a_flag = 0;		/* list all variables */

#if defined(__AROS__)
static size_t confstr(int name, char *buf, size_t len)
{
    errno = EINVAL;
    return -1;
}
#endif

int
main(int argc, char **argv)
{
	int ch;
	const struct conf_variable *cp;
	const char *varname, *pathname;
	int found;

#if !defined(__AROS__)
	setprogname(argv[0]);
#endif
	(void)setlocale(LC_ALL, "");

	while ((ch = getopt(argc, argv, "a")) != -1) {
		switch (ch) {
		case 'a':
			a_flag = 1;
			break;
		case '?':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (!a_flag) {
		if (argc == 0)
			usage();
		varname = argv[0];
		argc--;
		argv++;
	} else
		varname = NULL;

	if (argc > 1)
		usage();
	pathname = argv[0];	/* may be NULL */

	found = 0;
	for (cp = conf_table; cp->name != NULL; cp++) {
		if (a_flag || strcmp(varname, cp->name) == 0) {
			/*LINTED weird expression*/
			if ((cp->type == PATHCONF) == (pathname != NULL)) {
				printvar(cp, pathname);
				found = 1;
			} else if (!a_flag)
				errx(EXIT_FAILURE,
				    "%s: invalid variable type", cp->name);
		}
	}

	if (!a_flag && !found)
		errx(EXIT_FAILURE, "%s: unknown variable", varname);

	(void)fflush(stdout);
	return ferror(stdout) ? EXIT_FAILURE : EXIT_SUCCESS;
}

static void
print_longvar(const char *name, long value)
{
	if (a_flag)
		(void)printf("%s = %ld\n", name, value);
	else
		(void)printf("%ld\n", value);
}

static void
print_strvar(const char *name, const char *sval)
{
	if (a_flag)
		(void)printf("%s = %s\n", name, sval);
	else
		(void)printf("%s\n", sval);
}

static void
printvar(const struct conf_variable *cp, const char *pathname)
{
	size_t slen;
	char *sval;
	long val;

	switch (cp->type) {
	case CONSTANT:
		print_longvar(cp->name, cp->value);
		break;

	case CONFSTR:
		errno = 0;
		slen = confstr((int)cp->value, NULL, 0);
		if (slen == 0) {
			if (errno != 0)

out:			 	err(EXIT_FAILURE, "confstr(%ld)", cp->value);
			else
				print_strvar(cp->name, "undefined");
		}

		if ((sval = malloc(slen)) == NULL)
			err(EXIT_FAILURE, "Can't allocate %zu bytes", slen);

		errno = 0;
		if (confstr((int)cp->value, sval, slen) == 0) {
			if (errno != 0)
				goto out;
			else
				print_strvar(cp->name, "undefined");
		} else
			print_strvar(cp->name, sval);

		free(sval);
		break;

	case SYSCONF:
		errno = 0;
		if ((val = sysconf((int)cp->value)) == -1) {
			if (errno != 0)
				err(EXIT_FAILURE, "sysconf(%ld)", cp->value);
			print_strvar(cp->name, "undefined");
		} else
			print_longvar(cp->name, val);
		break;

	case PATHCONF:
		errno = 0;
		if ((val = pathconf(pathname, (int)cp->value)) == -1) {
			if (errno != 0) {
				if (a_flag && errno == EINVAL) {
					/* Just skip invalid variables */
					return;
				}
				err(EXIT_FAILURE, "pathconf(%s, %ld)",
				    pathname, cp->value);
				/* NOTREACHED */
			}

			print_strvar(cp->name, "undefined");
		} else
			print_longvar(cp->name, val);
		break;
	}
}


static void
usage(void)
{
	const char *p = _getprogname();
	(void)fprintf(stderr, "Usage: %s system_var\n\t%s -a\n"
	    "\t%s path_var pathname\n\t%s -a pathname\n", p, p, p, p);
	exit(EXIT_FAILURE);
}
