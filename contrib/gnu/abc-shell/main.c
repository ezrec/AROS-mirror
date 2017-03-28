/*
 * startup, main loop, environments and error handling
 */

#define EXTERN                          /* define EXTERNs in sh.h */

#define DEBUG 0
#include <aros/debug.h>

#include <sys/stat.h>
#include <pwd.h>
#include "sh.h"
#include "ksh_time.h"

extern char **environ;

/*
 * global data
 */

static void     reclaim(void);
static void     remove_temps(struct temp *tp);

/*
 * shell initialization
 */

static const char initifs[] = "IFS= \t\n";

static const char initsubs[] = "${PS2=> } ${PS3=#? } ${PS4=+ }";

static const char *const initcoms [] = {
        "typeset", "-r", "KSH_VERSION", NULL,
        "typeset", "-x", "SHELL", "PATH", "HOME", "TMPDIR", "tmp", "LOGNAME",
        "USER", "HISTFILE", "MAKE", "PREFIX", "PATH_SEPARATOR", "DIR_SEPARATOR", "LD", NULL,
        "typeset", "-i", "PPID", NULL,
        "typeset", "-i", "OPTIND=1", NULL,
        "alias",
         /* Standard ksh aliases */
          "hash=alias -t",      /* not "alias -t --": hash -r needs to work */
          "type=whence -v",
          "autoload=typeset -fu",
          "functions=typeset -f",
#ifdef HISTORY
          "history=fc -l",
#endif /* HISTORY */
          "integer=typeset -i",
          "local=typeset",
          "r=fc -e -",
         /* Aliases that are builtin commands in at&t */
          "newgrp=exec newgrp",
         /* Aliases that are AmigaOS4 specific - coreutils renames */
#ifdef __amigaos4__
          "date=gdate", "dir=gdir", "install=ginstall", "env=genv",
          "make=gmake", "info=ginfo", 
#endif
	"/dev/null=/nil",
          NULL,
        /* this is what at&t ksh seems to track, with the addition of emacs */
        "alias", "-tU",
          "cat", "cc", "chmod", "cp", "date", "ed", "grep", "ls",
          "make", "mv", "pr", "rm", "sed", "sh", "who",
          NULL,
        NULL
};

#define version_param  (initcoms[2])

const char *amiversion __attribute__((used)) = "$VER: abc-shell " ABC_VERSION " (" RELEASE_DATE ") " RELEASE_COMMENT "\0";
const char * stack_cookie __attribute__((used)) = "$STACK: " STACK_SIZE "\0";

int
main(int argc, char *argv[])
{
        int i;
        int argi;
        Source *s;
        struct block *l;
        int errexit;
        char **wp;
        struct env env;
        pid_t ppid;

        /* make sure argv[] is sane */
        if (!*argv) {
                static const char *empty_argv[] = {"sh", (char *) 0};

                argv = (char **) empty_argv;
                argc = 1;
        }

        kshname = *argv;

        aperm = &perm_space;
        ainit(aperm);          /* initialize permanent Area */

        /* set up base environment */
        memset(&env, 0, sizeof(env));
        env.type = E_NONE;
        ainit(&env.area);
        e = &env;
        newblock();             /* set up global l->vars and l->funs */

        /* Do this first so output routines (eg, errorf, shellf) can work */
        initio();

        initvar();

        initctypes();

        inittraps();

        coproc_init();

        /* allocate tables */

        taliases = malloc(sizeof(struct table));
        aliases  = malloc(sizeof(struct table));
        homedirs = malloc(sizeof(struct table));

        /* set up variable and command dictionaries */
        ktinit(taliases, APERM, 0);
        ktinit(aliases, APERM, 0);
        ktinit(homedirs, APERM, 0);

        /* define shell keywords */
        initkeywords();

        /* define built-in commands */
        ktinit(&builtins, APERM, 64); /* must be 2^n (currently 40 builtins) */
        for (i = 0; shbuiltins[i].name != NULL; i++)
                builtin(shbuiltins[i].name, shbuiltins[i].func);
        for (i = 0; kshbuiltins[i].name != NULL; i++)
                builtin(kshbuiltins[i].name, kshbuiltins[i].func);

        init_histvec();

#if defined(__AROS__)
        def_path = "/Developer/bin:/Developer/usr/bin:/C:.";
#else
        def_path = "/gcc/bin:/SDK/C:/SDK/Local/C:/SDK/Local/newlib/bin:/SDK/Local/clib2/bin:/C:.";
#endif

        /* Set PATH to def_path (will set the path global variable).
         * (import of environment below will probably change this setting).
         */
        {
                struct tbl *vp = global("PATH");
                /* setstr can't fail here */
                setstr(vp, def_path, KSH_RETURN_ERROR);
        }

        /* Set PATH_SEPARATOR and DIR_SEPARATOR. */
        {
                struct tbl *vp = global("PATH_SEPARATOR");
                /* setstr can't fail here */
                setstr(vp, ":", KSH_RETURN_ERROR);
        }

	{
                struct tbl *vp = global("DIR_SEPARATOR");
                /* setstr can't fail here */
                setstr(vp, "/", KSH_RETURN_ERROR);
        }

        /* Set DISABLE_COMMANDLINE_WILDCARD_EXPANSION - to disable
	 * wildcard expansion in applications using clib2 >= 1.201
	 */
        {
                struct tbl *vp = global("DISABLE_COMMANDLINE_WILDCARD_EXPANSION");
                /* setstr can't fail here */
                setstr(vp, "true", KSH_RETURN_ERROR);
        }

        /* Set SHELL. */
        {
                struct tbl *vp = global("SHELL");
                /* setstr can't fail here */
#if defined(__AROS__)
                setstr(vp, "/Developer/bin/sh", KSH_RETURN_ERROR);
#else
                setstr(vp, "/SDK/C/sh", KSH_RETURN_ERROR);
#endif
        }

        /* Set HOME. */
        {
                struct tbl *vp = global("HOME");
                /* setstr can't fail here */
                setstr(vp, "/home", KSH_RETURN_ERROR);
        }

        /* Set TMPDIR. */
        {
                struct tbl *vp = global("TMPDIR");
                /* setstr can't fail here */
                setstr(vp, "/T", KSH_RETURN_ERROR);
        }

        /* Set tmp. */
        {
                struct tbl *vp = global("tmp");
                /* setstr can't fail here */
                setstr(vp, "/T", KSH_RETURN_ERROR);
        }

        /* Set LOGNAME. */
        {
                struct tbl *vp = global("LOGNAME");
                /* setstr can't fail here */
                setstr(vp, "root", KSH_RETURN_ERROR);
        }

        /* Set USER - deprecated synonym of LOGNAME. */
        {
                struct tbl *vp = global("USER");
                /* setstr can't fail here */
                setstr(vp, "root", KSH_RETURN_ERROR);
        }

        /* Set HISTFILE - history file. */
        {
                struct tbl *vp = global("HISTFILE");
                /* setstr can't fail here */
#if defined(__AROS__)
                setstr(vp, "/Developer/Data/abc-shell/history", KSH_RETURN_ERROR);
#else
                setstr(vp, "/SDK/Data/abc-shell/history", KSH_RETURN_ERROR);
#endif
        }

#ifndef __AROS__
        /* Set MAKE. */
        {
                struct tbl *vp = global("MAKE");
                /* setstr can't fail here */
                setstr(vp, "/SDK/C/gmake", KSH_RETURN_ERROR);
        }

        /* Set LD. */
        {
                struct tbl *vp = global("LD");
                /* setstr can't fail here */
                setstr(vp, "ld", KSH_RETURN_ERROR);
        }
#endif
        
        /* Turn on brace expansion by default.  At&t ksh's that have
         * alternation always have it on.  BUT, posix doesn't have
         * brace expansion, so set this before setting up FPOSIX
         * (change_flag() clears FBRACEEXPAND when FPOSIX is set).
         */

        Flag(FBRACEEXPAND) = 1;

        /* set posix flag just before environment so that it will have
         * exactly the same effect as the POSIXLY_CORRECT environment
         * variable.  If this needs to be done sooner to ensure correct posix
         * operation, an initial scan of the environment will also have
         * done sooner.
         */
        change_flag(FPOSIX, OF_SPECIAL, 1);

        /* import environment */
        if (environ != NULL)
        {
            for (wp = environ; *wp != NULL; wp++)
            {
                if(strncmp("_=",*wp,2))
                {
                    typeset(*wp, IMPORTV|EXPORTV, 0, 0, 0);
                }
                else
                {
                    typeset(*wp,IMPORTV, 0, 0, 0);
                }
            }
        }
        kshpid = procpid = getpid();
        typeset(initifs, 0, 0, 0, 0);   /* for security */

        /* assign default shell variable values */
        substitute(initsubs, 0);

        /* Figure out the current working directory and set $PWD */
        {
                struct stat s_pwd, s_dot;
                struct tbl *pwd_v = global("PWD");
                char *pwd = str_val(pwd_v);
                char *pwdx = pwd;

                /* Try to use existing $PWD if it is valid */
                if (pwd[0] != '/' ||
                    stat(pwd, &s_pwd) < 0 || stat(".", &s_dot) < 0 ||
                    s_pwd.st_dev != s_dot.st_dev ||
                    s_pwd.st_ino != s_dot.st_ino)
                        pwdx = (char *) 0;
                set_current_wd(pwdx);
                if (current_wd[0])
                        simplify_path(current_wd);
                /* Only set pwd if we know where we are or if it had a
                 * bogus value
                 */
                if (current_wd[0] || pwd != null)
                        /* setstr can't fail here */
                        setstr(pwd_v, current_wd, KSH_RETURN_ERROR);
        }
        ppid = getppid();
        setint(global("PPID"), (long) ppid);
        setint(global("RANDOM"), (long) (time((time_t *)0) * kshpid * ppid));
        /* setstr can't fail here */
        setstr(global(version_param), ksh_version, KSH_RETURN_ERROR);

        /* execute initialization statements */
        for (wp = (char**) initcoms; *wp != NULL; wp++) {
                shcomexec(wp);
                for (; *wp != NULL; wp++)
                        ;
        }

        ksheuid = geteuid();
        kshuid = getuid();
        kshgid = getgid();
        kshegid = getegid();
        safe_prompt = "$PWD> ";
        {
                struct tbl *vp = global("PS1");

                /* Set PS1 if it isn't set, or we are root and prompt doesn't
                 * contain a #.
                 */
                if (!(vp->flag & ISSET))
                        /* setstr can't fail here */
                        setstr(vp, safe_prompt, KSH_RETURN_ERROR);
        }

        argi = parse_args(argv, OF_CMDLINE, (int *) 0);
        if (argi < 0)
                exit(1);

        if (Flag(FCOMMAND)) {
                s = pushs(SSTRING, ATEMP);
                if (!(s->start = s->str = argv[argi++]))
                        errorf("-c requires an argument");
                if (argv[argi])
                        kshname = argv[argi++];
        } else if (argi < argc && !Flag(FSTDIN)) {
                s = pushs(SFILE, ATEMP);
                s->file = argv[argi++];
                s->u.shf = shf_open(s->file, O_RDONLY, 0, SHF_MAPHI|SHF_CLEXEC);
                if (s->u.shf == NULL) {
                        exstat = 127; /* POSIX */
                        errorf("%s: %s", s->file, strerror(errno));
                }
                kshname = s->file;
        } else {
                Flag(FSTDIN) = 1;
                s = pushs(SSTDIN, ATEMP);
                s->file = "<stdin>";
                s->u.shf = shf_fdopen(0, SHF_RD | can_seek(0),
                        (struct shf *) 0);
                if (isatty(0) && isatty(2)) {
                        Flag(FTALKING) = Flag(FTALKING_I) = 1;
                        /* The following only if isatty(0) */
                        s->flags |= SF_TTY;
                        s->u.shf->flags |= SHF_INTERRUPT;
                        s->file = (char *) 0;
                }
        }

        /* This bizarreness is mandated by POSIX */
        {
                struct stat s_stdin;

                if (fstat(0, &s_stdin) >= 0 && S_ISCHR(s_stdin.st_mode) &&
                        Flag(FTALKING))
                        reset_nonblock(0);
        }

        j_init(i);

        l = e->loc;
        l->argv = &argv[argi - 1];
        l->argc = argc - argi;
        l->argv[0] = (char *) kshname;
        getopts_reset(1);

        errexit = Flag(FERREXIT);
        Flag(FERREXIT) = 0;

        if (!current_wd[0] && Flag(FTALKING))
                warningf(false, "Cannot determine current working directory");

        char *env_file;

        /* include $ENV */
        env_file = str_val(global("ENV"));

        /* If env isn't set, include default environment */
        if (env_file == null)
#if defined(__AROS__)
                env_file = strdup("/Developer/Data/abc-shell/variables");
#else
                env_file = strdup("/SDK/Data/abc-shell/variables");
#endif
	
        env_file = substitute(env_file, DOTILDE);
        if (*env_file != '\0')
                include(env_file, 0, (char **) 0, 1);

        if (errexit)
                Flag(FERREXIT) = 1;

        if (Flag(FTALKING)) {
                hist_init(s);
                alarm_init();
        } else
                Flag(FTRACKALL) = 1;    /* set after ENV */

        shell(s, true); /* doesn't return */
        return 0;
}

int
include(const char *name, int argc, char **argv, int intr_ok)
{
        Source *volatile s = NULL;
        struct shf *shf;
        char **volatile old_argv;
        volatile int old_argc;
        int i;

        shf = shf_open(name, O_RDONLY, 0, SHF_MAPHI|SHF_CLEXEC);
        if (shf == NULL)
                return -1;

        if (argv) {
                old_argv = e->loc->argv;
                old_argc = e->loc->argc;
        } else {
                old_argv = (char **) 0;
                old_argc = 0;
        }
        newenv(E_INCL);
        i = ksh_sigsetjmp(e->jbuf, 0);
        if (i) {
                quitenv(s ? s->u.shf : NULL);
                if (old_argv) {
                        e->loc->argv = old_argv;
                        e->loc->argc = old_argc;
                }
                switch (i) {
                  case LRETURN:
                  case LERROR:
                        return exstat & 0xff; /* see below */
                  case LINTR:
                        if (intr_ok && (exstat - 128) != SIGTERM)
                                return 1;
                        /* FALLTHROUGH */
                  case LEXIT:
                  case LLEAVE:
                  case LSHELL:
                        unwind(i);
                        /* NOTREACHED */
                  default:
                        internal_errorf(1, "include: %d", i);
                        /* NOTREACHED */
                }
        }
        if (argv) {
                e->loc->argv = argv;
                e->loc->argc = argc;
        }
        s = pushs(SFILE, ATEMP);
        s->u.shf = shf;
        s->file = str_save(name, ATEMP);
        i = shell(s, false);
        quitenv(s->u.shf);
        if (old_argv) {
                e->loc->argv = old_argv;
                e->loc->argc = old_argc;
        }
        return i & 0xff;        /* & 0xff to ensure value not -1 */
}

int
command(const char *comm)
{
        Source *s;

        s = pushs(SSTRING, ATEMP);
        s->start = s->str = comm;
        return shell(s, false);
}

/*
 * run the commands from the input source, returning status.
 */
int
shell(Source *volatile s, volatile int toplevel)
{
        struct op *t;
        volatile int wastty = s->flags & SF_TTY;
        volatile int attempts = 13;
        volatile int interactive = Flag(FTALKING) && toplevel;
        Source *volatile old_source = source;
        int i;

        newenv(E_PARSE);
        if (interactive)
                really_exit = 0;
        i = ksh_sigsetjmp(e->jbuf, 0);
        if (i) {
                switch (i) {
                  case LINTR: /* we get here if SIGINT not caught or ignored */
                  case LERROR:
                  case LSHELL:
                        if (interactive) {
                                if (i == LINTR)
                                        shellf(newline);
                                /* Reset any eof that was read as part of a
                                 * multiline command.
                                 */
                                if (Flag(FIGNOREEOF) && s->type == SEOF &&
                                        wastty)
                                        s->type = SSTDIN;
                                /* Used by exit command to get back to
                                 * top level shell.  Kind of strange since
                                 * interactive is set if we are reading from
                                 * a tty...
                                 */
                                /* toss any input we have so far */
                                s->start = s->str = null;
                                break;
                        }
                        /* FALLTHROUGH */
                  case LEXIT:
                  case LLEAVE:
                  case LRETURN:
                        source = old_source;
                        quitenv(NULL);
                        unwind(i);      /* keep on going */
                        /* NOTREACHED */
                  default:
                        source = old_source;
                        quitenv(NULL);
                        internal_errorf(1, "shell: %d", i);
                        /* NOTREACHED */
                }
        }

        while (1) {
                if (trap)
                        runtraps(0);

                if (s->next == NULL) {
                        if (Flag(FVERBOSE))
                                s->flags |= SF_ECHO;
                        else
                                s->flags &= ~SF_ECHO;
                }

                if (interactive) {
                        j_notify();
                        set_prompt(PS1, s);
                }

                t = compile(s);
                if (t != NULL && t->type == TEOF) {
                        if (wastty && Flag(FIGNOREEOF) && --attempts > 0) {
                                shellf("Use `exit' to leave ksh\n");
                                s->type = SSTDIN;
                        } else if (wastty && !really_exit &&
                                j_stopped_running())
                        {
                                really_exit = 1;
                                s->type = SSTDIN;
                        } else {
                                /* this for POSIX, which says EXIT traps
                                 * shall be taken in the environment
                                 * immediately after the last command
                                 * executed.
                                 */
                                if (toplevel)
                                        unwind(LEXIT);
                                break;
                        }
                }

                if (t && (!Flag(FNOEXEC) || (s->flags & SF_TTY)))
                        exstat = execute(t, 0);

                if (t != NULL && t->type != TEOF && interactive && really_exit)
                        really_exit = 0;

                reclaim();
        }
        quitenv(NULL);
        source = old_source;
        return exstat;
}

/* return to closest error handler or shell(), exit if none found */
void
unwind(int i)
{
        /* ordering for EXIT vs ERR is a bit odd (this is what at&t ksh does) */
        if (i == LEXIT || (Flag(FERREXIT) && (i == LERROR || i == LINTR) &&
                sigtraps[SIGEXIT_].trap))
        {
                runtrap(&sigtraps[SIGEXIT_]);
                i = LLEAVE;
        } else if (Flag(FERREXIT) && (i == LERROR || i == LINTR)) {
                runtrap(&sigtraps[SIGERR_]);
                i = LLEAVE;
        }
        while (1) {
                switch (e->type) {
                  case E_PARSE:
                  case E_FUNC:
                  case E_INCL:
                  case E_LOOP:
                  case E_ERRH:
                  case E_SUBSHELL:
                        ksh_siglongjmp(e->jbuf, i);
                        /*NOTREACHED*/

                  case E_NONE:
                        if (i == LINTR)
                                e->flags |= EF_FAKE_SIGDIE;
                        /* FALLTHROUGH */
                  default:
                        quitenv(NULL);
                }
        }
}

void
newenv(int type)
{
        struct env *ep;

        ep = (struct env *) alloc(sizeof(*ep), ATEMP);
        ep->type = type;
        ep->flags = 0;
        ainit(&ep->area);
        ep->loc = e->loc;
        ep->savefd = NULL;
        ep->oenv = e;
        ep->temps = NULL;
        e = ep;
}

void
quitenv(struct shf *shf)
{
        struct env *ep = e;
        int fd;

        if (ep->oenv && ep->oenv->loc != ep->loc)
                popblock();
        if (ep->savefd != NULL) {
                for (fd = 0; fd < NUFILE; fd++)
                        /* if ep->savefd[fd] < 0, means fd was closed */
                        if (ep->savefd[fd])
                                restfd(fd, ep->savefd[fd]);
                if (ep->savefd[2]) /* Clear any write errors */
                        shf_reopen(2, SHF_WR, shl_out);
        }

        /* Bottom of the stack.
         * Either main shell is exiting or cleanup_parents_env() was called.
         */
        if (ep->oenv == NULL) {
                if (ep->type == E_NONE) {       /* Main shell exiting? */
                        if (Flag(FTALKING))
                                hist_finish();
                        j_exit();
                        if (ep->flags & EF_FAKE_SIGDIE) {
                                int sig = exstat - 128;

                                /* ham up our death a bit (at&t ksh
                                 * only seems to do this for SIGTERM)
                                 * Don't do it for SIGQUIT, since we'd
                                 * dump a core..
                                 */
                                if (sig == SIGINT || sig == SIGTERM) {
                                        setsig(&sigtraps[sig], SIG_DFL,
                                                SS_RESTORE_CURR|SS_FORCE);
                                        kill(0, sig);
                                }
                        }
#ifdef MEM_DEBUG
                        chmem_allfree();
#endif /* MEM_DEBUG */
                }
                if (shf)
                        shf_close(shf);
                reclaim();
                exit(exstat);
        }
        if (shf)
                shf_close(shf);
        reclaim();

        e = e->oenv;
        afree(ep, ATEMP);
}

/* Called after a fork to cleanup stuff left over from parents environment */
void
cleanup_parents_env(void)
{
        struct env *ep;
        int fd;

        /* Don't clean up temporary files - parent will probably need them.
         * Also, can't easily reclaim memory since variables, etc. could be
         * anywhere.
         */

        /* close all file descriptors hiding in savefd */
        for (ep = e; ep; ep = ep->oenv) {
                if (ep->savefd) {
                        for (fd = 0; fd < NUFILE; fd++)
                                if (ep->savefd[fd] > 0)
                                        close(ep->savefd[fd]);
                        afree(ep->savefd, &ep->area);
                        ep->savefd = (short *) 0;
                }
        }
        e->oenv = (struct env *) 0;
}

/* Called just before an execve cleanup stuff temporary files */
void
cleanup_proc_env(void)
{
        struct env *ep;

        for (ep = e; ep; ep = ep->oenv)
                remove_temps(ep->temps);
}

/* remove temp files and free ATEMP Area */
static void
reclaim(void)
{
        remove_temps(e->temps);
        e->temps = NULL;
        afreeall(&e->area);
}

static void
remove_temps(struct temp *tp)
{
        for (; tp != NULL; tp = tp->next)
                if (tp->pid == procpid) {
                        unlink(tp->name);
                }
}
