/*
    Copyright © 1996-2001, Irmen de Jong
    Copyright © 2001, The AROS Development Team 

    Operating system module implementation for AmigaOS and AROS, adapted 
    from posixmodule.c; implements as much of the functionality of that
    module as possible. Should compile with SAS/C version 6.58 and 
    GCC 2.95.2+ (inside the AROS build environment).  
    
    Originally written by Irmen de Jong (irmen@bigfoot.com).
    Updated and adapted for AROS by Adam Chodorowski (chodorowski@aros.org)
    
    HISTORY

    27-Mar-96: Added a lot of AmiTCP functions!               
     2-Apr-96: Many small fixes & enhancements.               
    11-Apr-96: Totally rewritten the environment handling.    
               Now creates 4 separate dictionaries.           
               Fixed link(), added symlink() and readlink().  
    29-May-96: Added filenote() and fullpath()                
    11-Jun-96: Moved filenote() to doslib/SetComment          
    12-Jun-96: Removed execv                                  
    29-Aug-96: fixed getcwd(), some minor errno fixes         
    26-Dec-96: upgraded to 1.4: added putenv(), remove()      
               fixed mkdir: default protbits (0777)           
     6-Apr-97: fixed bug in readlink (lock was incorrect)     
     6-Nov-97: added uname()                                   
    12-Jan-98: upgraded to 1.5: fixed includes, new names,    
               uses new Amiga/.../unixemul.c                 
    28-Mar-98: fixed buffer overflow bug in convertenviron()  
    27-Sep-98: added crc32 function                           
    24-Dec-98: moved bunch of #defines to config.h            
    25-Dec-98: added I-Net 225 support                        
    30-Sep-01: Updated and adapted for AROS by Adam Chodorowski.
               A bunch of different fixes and updates to synchronize
	       with posixmodule in Python 2.0+. See the CVS log for
	       details and further history.
	                                                      
    TODO                                                        
    
    + Implement execv(e) and threads (if possible)
    + Update to functionality in 2.0+ (new functions, changed semantics?)
    + Implement LARGEFILE support (lseek(), ftruncate()) 
    
    NOTES
                                                            
    Don't forget about the __io2errno conversion!             
*/

#include "Python.h"
#include "osdefs.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef AROS
#include <stat.h>
#endif
#if !defined INET225 && !defined AROS
#include <dos.h>
#endif
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos/var.h>
#include <dos/dostags.h>
#include <exec/execbase.h>

#ifdef HAVE_UTIME_H
#include <utime.h>
#endif

#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)

#ifdef AMITCP
#include <clib/netlib_protos.h>
#endif

#ifdef INET225
#include <proto/socket.h>
static int _OSERR;
#define getegid() getgid()
#define geteuid() getuid()
static __inline int dup(int oldsd) { return s_dup(oldsd); }
static __inline int dup2(int oldsd, int newsd) { return s_dup2(oldsd, newsd); }
#endif

#ifndef AROS
#include "protos.h"
#endif

#ifdef AROS
#include <fcntl.h>
#include <unistd.h>

#define __aligned 
extern int IoErr2errno( int );
#define __io2errno(x) IoErr2errno(x) 
static int _OSERR;  // FIXME: What's this for, really?
#endif

/* Return a dictionary corresponding to the AmigaDOS environment table. */
/* That is, scan ENV: for global environment variables.                 */
/* The local shell environment variables are put into another table.    */

static int
convertenviron(PyObject **glob, PyObject **loc,
			   PyObject **both, PyObject **aliases)
{
    BPTR dlok;
    struct FileInfoBlock __aligned fib;
    PyObject *v;
    char *dynbuf;
    struct LocalVar *lvar;
    struct List *localvars;

    *glob=PyDict_New();
    *loc=PyDict_New();
    *both=PyDict_New();
    *aliases=PyDict_New();

    if(!*glob || !*loc || !*both || !*aliases)
    {
    	if(*glob) Py_DECREF(*glob);
    	if(*loc) Py_DECREF(*loc);
    	if(*both) Py_DECREF(*both);
    	if(*aliases) Py_DECREF(*aliases);
    	return 0;
    }

    /* Read global vars from ENV: */
    /* Put them in 'glob' and in 'both'. */

    if(dlok=Lock("ENV:",ACCESS_READ))
    {
    	if(Examine(dlok,&fib))
    	{
    	    while(ExNext(dlok,&fib))
    	    {
    	    	if(fib.fib_DirEntryType<0)
    	    	{
    	    	    if(dynbuf=malloc(fib.fib_Size+1))
    	    	    {
    	    	    	int len=GetVar(fib.fib_FileName,dynbuf,fib.fib_Size+1,GVF_GLOBAL_ONLY);
    	    	    	if(len>=0 && (v=PyString_FromString(dynbuf)))
    	    	    	{
    	    	    	    PyDict_SetItemString(*glob,fib.fib_FileName,v);
    	    	    	    PyDict_SetItemString(*both,fib.fib_FileName,v);
    	    	    	    Py_DECREF(v);
    	    	    	}
    	    	        free(dynbuf);
    	    	    }
    	    	}
    	    }
    	}
    }

    if(dlok) UnLock(dlok);

    /* Scan the local shell environment, including "RC" and "Result2"!   */
    /* Put shell vars in 'loc' and 'both', and aliases in 'aliases'. */
    /* Because of the fact that the inserting of local vars into 'both' */
    /* happens AFTER the insertion of global vars, the formor overwrite */
    /* the latter, and thus have higher priority (as it should be). */

    localvars = (struct List*) &((struct Process*)FindTask(0))->pr_LocalVars;

    if(!IsListEmpty(localvars))
    {
    	lvar = (struct LocalVar*) localvars->lh_Head;
    	do {
    	    if(dynbuf=malloc(lvar->lv_Len+1))
    	    {
    	    	strncpy(dynbuf,lvar->lv_Value,lvar->lv_Len);
    	    	dynbuf[lvar->lv_Len]=0;

    	    	if(v=PyString_FromString(dynbuf))
    	    	{
    	    	    if(lvar->lv_Node.ln_Type==LV_VAR)
    	    	    {
    	    	    	PyDict_SetItemString(*loc,lvar->lv_Node.ln_Name,v);
    	    	    	PyDict_SetItemString(*both,lvar->lv_Node.ln_Name,v);
    	    	    }
    	    	    else if(lvar->lv_Node.ln_Type==LV_ALIAS) 
		    {
    	    	    	PyDict_SetItemString(*aliases,lvar->lv_Node.ln_Name,v);
    	    	    }
		    
    	    	    Py_DECREF(v);
    	    	}
    	    	free(dynbuf);
    	    }
    	} while((lvar=(struct LocalVar*)lvar->lv_Node.ln_Succ)->lv_Node.ln_Succ);
    }

    return 1;
}


/* Set a Amiga-specific error from errno, and return NULL */

static PyObject * amiga_error(void)
{
    return PyErr_SetFromErrno(PyExc_OSError);
}
static PyObject * amiga_error_with_filename(char *name)
{
    return PyErr_SetFromErrnoWithFilename(PyExc_OSError, name);
}


/* AMIGA generic methods */

static PyObject *
amiga_1str( PyObject *args, char *format, int (*func)(const char *) )
{
    char *path1;
    int res;
    
    if( !PyArg_ParseTuple( args, format, &path1 ) ) return NULL;
    
    Py_BEGIN_ALLOW_THREADS
    res = (*func)(path1);
    Py_END_ALLOW_THREADS
    if (res < 0)
    	return amiga_error_with_filename(path1);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
amiga_2str( PyObject *args, char *format, int (*func)(const char *, const char *) )
{
    char *path1, *path2;
    int res;
    
    if( !PyArg_ParseTuple( args, format, &path1, &path2 ) ) return NULL;

    Py_BEGIN_ALLOW_THREADS
    res = (*func)(path1, path2);
    Py_END_ALLOW_THREADS
    if (res < 0)
    	return amiga_error();
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
amiga_strint(PyObject *args, char *format, int (*func)(const char *, int))
{
    char *path;
    int i;
    int res;
    if( !PyArg_ParseTuple( args, format, &path, &i ) ) return NULL;

    Py_BEGIN_ALLOW_THREADS
    res = (*func)(path, i);
    Py_END_ALLOW_THREADS
    if (res < 0)
    	return amiga_error_with_filename(path);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
amiga_do_stat(PyObject *self, PyObject *args, char *format, int (*statfunc)(const char *, struct stat *))
{
	struct stat st;
	char *path;
	int res;
	if (!PyArg_ParseTuple(args, format, &path))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	res = (*statfunc)(path, &st);
	Py_END_ALLOW_THREADS
	if (res != 0)
		return amiga_error_with_filename(path);
	return Py_BuildValue("(llllllllll)",
			(long)st.st_mode,
			(long)st.st_ino,
			(long)st.st_dev,
			(long)st.st_nlink,
			(long)st.st_uid,
			(long)st.st_gid,
			(long)st.st_size,
			(long)st.st_atime,
			(long)st.st_mtime,
			(long)st.st_ctime);
}


/* AMIGA methods */

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_chdir__doc__[] =
"chdir(path) -> None\n\
Change the current working directory to the specified path.";
#else
#define amiga_chdir__doc__ NULL
#endif

static PyObject *
amiga_chdir(PyObject *self, PyObject *args)
{
    return amiga_1str(args, "s:chdir", chdir);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_chmod__doc__[] =
"chmod(path, mode) -> None\n\
Change the access permissions of a file.";
#else
#define amiga_chmod__doc__ NULL
#endif 

static PyObject *
amiga_chmod(PyObject *self, PyObject *args)
{
    return amiga_strint(args, "si:chmod", chmod);
}

#ifdef HAVE_CHOWN
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_chown__doc__[] =
"chown(path, uid, gid) -> None\n\
Change the owner and group id of path to the numeric uid and gid.";
#else
#define amiga_chown__doc__ NULL
#endif

static PyObject *
amiga_chown(PyObject *self, PyObject *args)
{
    char *path;
    int   owner, group, result;
    
    if( !(PyArg_ParseTuple( args, "sii:chown", &path, &owner, &group ) ) ) return NULL;
    
    Py_BEGIN_ALLOW_THREADS
    result = chown( path, owner, group );
    Py_END_ALLOW_THREADS
    
    if( result < 0 ) 
    {
    	return amiga_error_with_filename( path );
    } 
    else 
    {
    	Py_INCREF( Py_None );
	return Py_None;
    }
}
#endif /* HAVE_CHOWN */

#ifdef HAVE_GETCWD
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getcwd__doc__[] =
"getcwd() -> path\n\
Return a string representing the current working directory.";
#else
#define amiga_getcwd__doc__ NULL
#endif

static PyObject *
amiga_getcwd(PyObject *self, PyObject *args)
{
	char buf[MAXPATHLEN];
	char *res;
	if (!PyArg_ParseTuple(args,":getcwd"))
			return NULL;
	Py_BEGIN_ALLOW_THREADS
	res = getcwd(buf, sizeof buf);
	Py_END_ALLOW_THREADS
	if (res == NULL)
			return amiga_error();
	return PyString_FromString(buf);
}
#endif

#ifdef HAVE_LINK
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_link__doc__[] =
"link(src, dst) -> None\n\
Create a hard link to a file.";
#else
#define amiga_link__doc__ NULL
#endif 

static PyObject *
amiga_link(PyObject *self, PyObject *args)
{
	return amiga_2str(args, "ss:link", link);
}
#endif

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_listdir__doc__[] =
"listdir(path) -> list_of_strings\n\
Return a list containing the names of the entries in the directory.\n\
\n\
	path: path of directory to list\n\
\n\
The list is in arbitrary order.";
#else
#define amiga_listdir__doc__ NULL
#endif

static PyObject *
amiga_listdir(PyObject *self, PyObject *args)
{
    BPTR dlok;
    char *name;
    struct FileInfoBlock __aligned fib;
    PyObject *d;

    if (!PyArg_ParseTuple(args, "s:listdir", &name)) return NULL;

    if ((d = PyList_New(0)) == NULL) return NULL;

    if(dlok=Lock(name,ACCESS_READ))
    {
    	if(Examine(dlok,&fib))
    	{
    	    while(ExNext(dlok,&fib))
    	    {
    	    	PyObject *v = PyString_FromString(fib.fib_FileName);
    	    	if(v==NULL)
    	    	{
    	    	    Py_DECREF(d); d=NULL; break;
    	    	}

    	    	if(PyList_Append(d,v)!=0)
    	    	{
    	    	    Py_DECREF(v); Py_DECREF(d); d=NULL; break;
    	    	}
    	    	Py_DECREF(v);
    	    }
    	}
    	UnLock(dlok);
    }

    if(IoErr()==ERROR_NO_MORE_ENTRIES) return d;

    Py_DECREF(d);
    errno=__io2errno(_OSERR=IoErr());
    return amiga_error();
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_mkdir__doc__[] =
"mkdir(path [, mode=0777]) -> None\n\
Create a directory.";
#else
#define amiga_mkdir__doc__ NULL
#endif

static PyObject *
amiga_mkdir(PyObject *self, PyObject *args)
{
    int res;
    char *path;
    int mode = 0777;
    if (!PyArg_ParseTuple(args, "s|i:mkdir", &path, &mode)) return NULL;
    Py_BEGIN_ALLOW_THREADS
#if defined INET225 || defined AROS
    res = mkdir(path, mode);
#else
    res = my_mkdir(path, mode);
#endif
    Py_END_ALLOW_THREADS
    if (res < 0) return amiga_error_with_filename(path);
    Py_INCREF(Py_None); return Py_None;
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_rename__doc__[] =
"rename(old, new) -> None\n\
Rename a file or directory.";
#else
#define amiga_rename__doc__ NULL
#endif

static PyObject *
amiga_rename(PyObject *self, PyObject *args)
{
    return amiga_2str(args, "ss:rename", rename);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_rmdir__doc__[] =
"rmdir(path) -> None\n\
Remove a directory.";
#else
#define amiga_rmdir__doc__ NULL
#endif

static PyObject *
amiga_rmdir(PyObject *self, PyObject *args)
{
    return amiga_1str(args, "s:rmdir", rmdir);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_stat__doc__[] =
"stat(path) -> (mode,ino,dev,nlink,uid,gid,size,atime,mtime,ctime)\n\
Perform a stat system call on the given path.";
#else
#define amiga_stat__doc__ NULL
#endif

static PyObject *
amiga_stat(PyObject *self, PyObject *args)
{
	return amiga_do_stat(self, args, "s:stat", stat);
}

#ifdef HAVE_SYSTEM
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_system__doc__[] =
"system(command) -> exit_status\n\
Execute the command (a string) in a subshell.";
#else
#define amiga_system__doc__ NULL
#endif

static PyObject *
amiga_system(PyObject *self, PyObject *args)
{
	char *command;
	long sts;
	if (!PyArg_ParseTuple(args, "s:system", &command))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	sts = system(command);
	Py_END_ALLOW_THREADS
	return PyInt_FromLong(sts);
}
#endif

#if defined AMITCP || defined INET225 || defined AROS
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_umask__doc__[] =
"umask(new_mask) -> old_mask\n\
Set the current numeric umask and return the previous umask.";
#else
#define amiga_umask__doc__
#endif

static PyObject *
amiga_umask(PyObject *self, PyObject *args)
{
	int i;
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_ParseTuple(args,"i:umask",&i))
		return NULL;
	i = umask(i);
	if (i < 0)
		return amiga_error();
	return PyInt_FromLong((long)i);
}
#endif

#ifdef HAVE_UNAME
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_uname__doc__[] =
"uname() -> (sysname, nodename, release, version, machine)\n\
Return a tuple identifying the current operating system.";
#else
#define amiga_uname__doc__
#endif

static PyObject *
amiga_uname(PyObject *self, PyObject *args)
{
        struct utsname u;
        int res;
        if (!PyArg_ParseTuple(args, ":uname"))
                return NULL;
        Py_BEGIN_ALLOW_THREADS
        res = uname(&u);
        Py_END_ALLOW_THREADS
        if (res < 0)
                return amiga_error();
        return Py_BuildValue("(sssss)",
                             u.sysname,
                             u.nodename,
                             u.release,
                             u.version,
                             u.machine);
}
#endif

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_unlink__doc__[] =
"unlink(path) -> None\n\
Remove a file (same as remove(path)).";

static char amiga_remove__doc__[] =
"remove(path) -> None\n\
Remove a file (same as unlink(path)).";
#else
#define amiga_unlink__doc__ NULL
#define amiga_remove__doc__ NULL
#endif

static PyObject *
amiga_unlink(PyObject *self, PyObject *args)
{
    return amiga_1str(args, "s:remove", unlink);
}

#if defined AMITCP  || defined INET225 || defined AROS
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_utime__doc__[] =
"utime(path, (atime, utime)) -> None\n\
utime(path, None) -> None\n\
Set the access and modified time of the file to the given values.  If the\n\
second form is used, set the access and modified times to the current time.";
#else
#define amiga_utime__doc__
#endif

static PyObject *
amiga_utime(PyObject *self, PyObject *args)
{
	char *path;
	long atime, mtime;
	int res;
	PyObject* arg;

/* XXX should define struct utimbuf instead, above */
#ifdef HAVE_UTIME_H
	struct utimbuf buf;
#define ATIME buf.actime
#define MTIME buf.modtime
#define UTIME_ARG &buf
#else /* HAVE_UTIME_H */
	time_t buf[2];
#define ATIME buf[0]
#define MTIME buf[1]
#define UTIME_ARG buf
#endif /* HAVE_UTIME_H */

	if (!PyArg_ParseTuple(args, "sO:utime", &path, &arg))
		return NULL;
	if (arg == Py_None) {
		/* optional time values not given */
		Py_BEGIN_ALLOW_THREADS
		res = utime(path, NULL);
		Py_END_ALLOW_THREADS
	}
	else if (!PyArg_Parse(arg, "(ll)", &atime, &mtime)) {
		PyErr_SetString(PyExc_TypeError,
				"utime() arg 2 must be a tuple (atime, mtime)");
		return NULL;
	}
	else {
		ATIME = atime;
		MTIME = mtime;
		Py_BEGIN_ALLOW_THREADS
		res = utime(path, UTIME_ARG);
		Py_END_ALLOW_THREADS
	}

	if (res < 0)
		return amiga_error_with_filename(path);
	Py_INCREF(Py_None);
	return Py_None;
#undef UTIME_ARG
#undef ATIME
#undef MTIME
}
#endif


/* Process operations */

/* XXX Removed _exit. You are VERY STUPID if you used this. (2-apr-96) */

/* XXX Removed execv. You must use system/exit combination instead. */
/*     Maybe one day I'll implement a REAL execv ?? */


#ifdef HAVE_GETEGID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getegid__doc__[] =
"getegid() -> egid\n\
Return the current process's effective group id.";
#else
#define amiga_getegid__doc__ NULL
#endif

static PyObject *
amiga_getegid(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_Parse(args,":getegid"))
		return NULL;
	return PyInt_FromLong((long)getegid());
}
#endif

#ifdef HAVE_GETEUID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_geteuid__doc__[] =
"geteuid() -> euid\n\
Return the current process's effective user id.";
#else
#define amiga_geteuid__doc__ NULL
#endif

static PyObject *
amiga_geteuid(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_Parse(args,":geteuid"))
		return NULL;
	return PyInt_FromLong((long)geteuid());
}
#endif

#ifdef HAVE_GETGID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getgid__doc__[] =
"getgid() -> gid\n\
Return the current process's group id.";
#else
#define amiga_getgid__doc__ NULL
#endif

static PyObject *
amiga_getgid(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif 
#endif
	if (!PyArg_Parse(args,":getgid"))
		return NULL;
	return PyInt_FromLong((long)getgid());
}
#endif

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getpid__doc__[] =
"getpid() -> pid\n\
Return the current process id";
#else
#define amiga_getpid__doc__ NULL
#endif

static PyObject *
amiga_getpid(PyObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args,":getpid"))
		return NULL;
	return PyInt_FromLong((long)FindTask(0));
}

#ifdef HAVE_GETPGRP
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getpgrp__doc__[] =
"getpgrp() -> pgrp\n\
Return the current process group id.";
#else
#define amiga_getpgrp__doc__ NULL
#endif

static PyObject *
amiga_getpgrp(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_ParseTuple(args,":getpgrp"))
		return NULL;
#ifdef GETPGRP_HAVE_ARG
	return PyInt_FromLong((long)getpgrp(0));
#else /* GETPGRP_HAVE_ARG */
	return PyInt_FromLong((long)getpgrp());
#endif /* GETPGRP_HAVE_ARG */
}
#endif /* HAVE_GETPGRP */

#ifdef HAVE_SETPGRP
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_setpgrp__doc__[] =
"setpgrp() -> None\n\
Make this process a session leader.";
#else 
#define amiga_setpgrp__doc__ NULL
#endif

static PyObject *
amiga_setpgrp(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_ParseTuple(args,":setpgrp"))
		return NULL;
#ifdef SETPGRP_HAVE_ARG
	if (setpgrp(0, 0) < 0)
#else /* SETPGRP_HAVE_ARG */
	if (setpgrp() < 0)
#endif /* SETPGRP_HAVE_ARG */
		return amiga_error();
	Py_INCREF(Py_None);
	return Py_None;
}

#endif /* HAVE_SETPGRP */

#ifdef HAVE_GETPPID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getppid__doc__[] =
"getppid() -> ppid\n\
Return the parent's process id.";
#else
#define amiga_getppid__doc__ NULL
#endif

static PyObject *
amiga_getppid(PyObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args,":getppid"))
		return NULL;
	return PyInt_FromLong((long)getppid());
}
#endif

#ifdef HAVE_GETUID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_getuid__doc__[] =
"getuid() -> uid\n\
Return the current process's user id.";
#else
#define amiga_getuid__doc__ NULL
#endif

static PyObject *
amiga_getuid(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_ParseTuple(args,":getuid"))
		return NULL;
	return PyInt_FromLong((long)getuid());
}
#endif

#ifdef HAVE_POPEN
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_popen__doc__[] =
"popen(command [, mode='r' [, bufsize]]) -> pipe\n\
Open a pipe to/from a command returning a file object.";
#else 
#define amiga_popen__doc__ NULL
#endif

static PyObject *
amiga_popen(PyObject *self, PyObject *args)
{
	char *name;
	char *mode = "r";
	int bufsize = -1;
	FILE *fp;
	PyObject *f;
	if (!PyArg_ParseTuple(args, "s|si:popen", &name, &mode, &bufsize))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	fp = popen(name, mode);
	Py_END_ALLOW_THREADS
	if (fp == NULL)
		return amiga_error();
	f = PyFile_FromFile(fp, name, mode, pclose);
	if (f != NULL)
		PyFile_SetBufSize(f, bufsize);
	return f;
}
#endif

#ifdef HAVE_SETUID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_setuid__doc__[] =
"setuid(uid) -> None\n\
Set the current process's user id.";
#else
#define amiga_setuid__doc__ NULL
#endif

static PyObject *
amiga_setuid(PyObject *self, PyObject *args)
{
	int uid;
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_ParseTuple(args, "i:setuid", &uid))
		return NULL;
	if (setuid(uid) < 0)
		return amiga_error();
	Py_INCREF(Py_None);
	return Py_None;
}
#endif /* HAVE_SETUID */

#ifdef HAVE_SETGID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_setgid__doc__[] =
"setgid(gid) -> None\n\
Set the current process's group id.";
#else
#define amiga_setgid__doc__ NULL
#endif

static PyObject *
amiga_setgid(PyObject *self, PyObject *args)
{
	int gid;
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#   endif
#endif
	if (!PyArg_ParseTuple(args, "i:setgid", &gid))
		return NULL;
	if (setgid(gid) < 0)
		return amiga_error();
	Py_INCREF(Py_None);
	return Py_None;
}
#endif /* HAVE_SETGID */

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_lstat__doc__[] =
"lstat(path) -> (mode,ino,dev,nlink,uid,gid,size,atime,mtime,ctime)\n\
Like stat(path), but do not follow symbolic links.";
#else
#define amiga_lstat__doc__ NULL
#endif

static PyObject *
amiga_lstat(PyObject *self, PyObject *args)
{
#ifdef HAVE_LSTAT
	return amiga_do_stat(self, args, "s:lstat", lstat);
#else /* !HAVE_LSTAT */
	return amiga_do_stat(self, args, "s:lstat", stat);
#endif /* !HAVE_LSTAT */
}

#ifdef HAVE_READLINK
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_readlink__doc__[] =
"readlink(path) -> path\n\
Return a string representing the path to which the symbolic link points.";
#else
#define amiga_readlink__doc__ NULL
#endif

static PyObject *
amiga_readlink(PyObject *self, PyObject *args)
{
	char buf[MAXPATHLEN];
	char *path;
	int n;
	if (!PyArg_ParseTuple(args, "s:readlink", &path))
	        return NULL;
	Py_BEGIN_ALLOW_THREADS
	n = readlink(path, buf, (int) sizeof buf);
	Py_END_ALLOW_THREADS
	if (n < 0)
	        return amiga_error_with_filename(path);
	return PyString_FromStringAndSize(buf, n);
}
#endif

#ifdef HAVE_SYMLINK
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_symlink__doc__[] =
"symlink(src, dst) -> None\n\
Create a symbolic link.";
#else
#define amiga_symlink__doc__ NULL
#endif

static PyObject *
amiga_symlink(PyObject *self, PyObject *args)
{
    return amiga_2str(args, "ss:symlink", symlink);
}
#endif

#ifdef HAVE_SETSID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_setsid__doc__[] =
"setsid() -> None\n\
Call the system call setsid().";
#else 
#define amiga_setsid__doc__ NULL
#endif

static PyObject *
amiga_setsid(PyObject *self, PyObject *args)
{
#ifdef AMITCP
	if (!checkusergrouplib()) return NULL;
#else
#  ifdef INET225
	if (!checksocketlib()) return NULL;
#  endif
#endif
	if (!PyArg_ParseTuple(args,":setsid"))
		return NULL;
	if ((int)setsid() < 0)
		return amiga_error();
	Py_INCREF(Py_None);
	return Py_None;
}
#endif /* HAVE_SETSID */

#ifdef HAVE_SETPGID
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_setpgid__doc__[] =
"setpgid(pid, pgrp) -> None\n\
Call the system call setpgid().";
#else
#define amiga_setpgid__doc__
#endif

static PyObject *
amiga_setpgid(PyObject *self, PyObject *args)
{
	int pid, pgrp;
	if (!PyArg_ParseTuple(args, "ii:setpgid", &pid, &pgrp))
		return NULL;
	if (setpgid(pid, pgrp) < 0)
		return amiga_error();
	Py_INCREF(Py_None);
	return Py_None;
}
#endif /* HAVE_SETPGID */

/* Functions acting on file descriptors */

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_open__doc__[] =
"open(filename, flag [, mode=0777]) -> fd\n\
Open a file (for low level IO).";
#else
#define amiga_open__doc__ NULL
#endif

static PyObject *
amiga_open(PyObject *self, PyObject *args)
{
    char *file;
    int flag;
    int mode = 0777;
    int fd;
    if (!PyArg_ParseTuple(args, "si|i:open", &file, &flag)) return NULL;

    Py_BEGIN_ALLOW_THREADS
    fd = open(file, flag, mode);
    Py_END_ALLOW_THREADS
    if (fd < 0)
    	return amiga_error_with_filename(file);
    return PyInt_FromLong((long)fd);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_close__doc__[] =
"close(fd) -> None\n\
Close a file descriptor (for low level IO).";
#else
#define amiga_close__doc__ NULL
#endif

static PyObject *
amiga_close(PyObject *self, PyObject *args)
{
	int fd, res;
	if (!PyArg_ParseTuple(args, "i:close", &fd))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	res = close(fd);
	Py_END_ALLOW_THREADS
	if (res < 0)
		return amiga_error();
	Py_INCREF(Py_None);
	return Py_None;
}

#ifdef HAVE_FTRUNCATE
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_ftruncate__doc__[] =
"ftruncate(fd, length) -> None\n\
Truncate a file to a specified length.";
#else
#define amiga_ftruncate__doc__ NULL
#endif

static PyObject *amiga_ftruncate( PyObject *self, PyObject *args )
{
    int   fd, result;
    off_t length;
    
    if( !PyArg_ParseTuple( args, "il:ftruncate", &fd, &length ) ) return NULL;
    
    Py_BEGIN_ALLOW_THREADS
    result = ftruncate( fd, length );
    Py_END_ALLOW_THREADS
    
    if( fd != 0 )
    {
    	return amiga_error();
    }
    else
    {	
    	Py_INCREF( Py_None );
	return Py_None;
    }
}
#endif

#if defined AMITCP || defined INET225 || defined AROS
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_dup__doc__[] =
"dup(fd) -> fd2\n\
Return a duplicate of a file descriptor.";
#else
#define amiga_dup__doc__ NULL
#endif

static PyObject *
amiga_dup(PyObject *self, PyObject *args)
{
    int fd;

#ifndef AROS	
    if (!checksocketlib()) 
    { 
    	PyErr_Clear(); 
    	errno=EIO; 
    	return amiga_error(); 
    }
#endif

    if (!PyArg_ParseTuple(args, "i:dup", &fd))
    	return NULL;
    Py_BEGIN_ALLOW_THREADS
    fd = dup(fd);
    Py_END_ALLOW_THREADS
    if (fd < 0)
    	return amiga_error();
    return PyInt_FromLong((long)fd);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_dup2__doc__[] =
"dup2(fd, fd2) -> None\n\
Duplicate file descriptor.";
#else
#define amiga_dup2__doc__ NULL
#endif

static PyObject *
amiga_dup2(PyObject *self, PyObject *args)
{
    int fd, fd2, res;

#ifndef AROS
    if (!checksocketlib()) 
    { 
    	PyErr_Clear(); 
	errno=EIO; 
	return amiga_error(); 
    }
#endif
    
    if (!PyArg_ParseTuple(args, "ii:dup2", &fd, &fd2))
    	return NULL;
    Py_BEGIN_ALLOW_THREADS
    res = dup2(fd, fd2);
    Py_END_ALLOW_THREADS
    if (res < 0)
    	return amiga_error();
    Py_INCREF(Py_None);
    return Py_None;
}
#endif

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_lseek__doc__[] =
"lseek(fd, pos, how) -> newpos\n\
Set the current position of a file descriptor.";
#else
#define amiga_lseek__doc__ NULL
#endif

static PyObject *
amiga_lseek(PyObject *self, PyObject *args)
{
	int   fd, how, res;
	off_t pos;
	
	if (!PyArg_ParseTuple(args, "ili:lseek", &fd, &pos, &how))
		return NULL;
#ifdef SEEK_SET
	/* Turn 0, 1, 2 into SEEK_{SET,CUR,END} */
	switch (how) {
	case 0: how = SEEK_SET; break;
	case 1: how = SEEK_CUR; break;
	case 2: how = SEEK_END; break;
	}
#endif /* SEEK_END */
	Py_BEGIN_ALLOW_THREADS
	res = lseek(fd, pos, how);
	Py_END_ALLOW_THREADS
	if (res < 0)
		return amiga_error();
	return PyInt_FromLong(res);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_read__doc__[] =
"read(fd, buffersize) -> string\n\
Read a file descriptor.";
#else
#define amiga_read__doc__ NULL
#endif

static PyObject *
amiga_read(PyObject *self, PyObject *args)
{
	int fd, size;
	PyObject *buffer;
	if (!PyArg_ParseTuple(args, "ii:read", &fd, &size))
		return NULL;
	buffer = PyString_FromStringAndSize((char *)NULL, size);
	if (buffer == NULL)
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	size = read(fd, PyString_AsString(buffer), size);
	Py_END_ALLOW_THREADS
	if (size < 0) {
		Py_DECREF(buffer);
		return amiga_error();
	}
	_PyString_Resize(&buffer, size);
	return buffer;
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_write__doc__[] =
"write(fd, string) -> byteswritten\n\
Write a string to a file descriptor.";
#else
#define amiga_write__doc__ NULL
#endif

static PyObject *
amiga_write(PyObject *self, PyObject *args)
{
	int fd, size;
	char *buffer;
	if (!PyArg_ParseTuple(args, "is#:write", &fd, &buffer, &size))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	size = write(fd, buffer, size);
	Py_END_ALLOW_THREADS
	if (size < 0)
		return amiga_error();
	return PyInt_FromLong((long)size);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_fstat__doc__[]=
"fstat(fd) -> (mode, ino, dev, nlink, uid, gid, size, atime, mtime, ctime)\n\
Like stat(), but for an open file descriptor.";
#else 
#define amiga_fstat__doc__ NULL
#endif

static PyObject *
amiga_fstat(PyObject *self, PyObject *args)
{
	int fd;
	struct stat st;
	int res;
	if (!PyArg_ParseTuple(args, "i:fstat", &fd))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	res = fstat(fd, &st);
	Py_END_ALLOW_THREADS
	if (res != 0)
		return amiga_error();
	return Py_BuildValue("(llllllllll)",
			(long)st.st_mode,
			(long)st.st_ino,
			(long)st.st_dev,
			(long)st.st_nlink,
			(long)st.st_uid,
			(long)st.st_gid,
			(long)st.st_size,
			(long)st.st_atime,
			(long)st.st_mtime,
			(long)st.st_ctime);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_fdopen__doc__[] =
"fdopen(fd, [, mode='r' [, bufsize]]) -> file_object\n\
Return an open file object connected to a file descriptor.";
#else
#define amiga_fdopen__doc__ NULL
#endif

static PyObject *
amiga_fdopen(PyObject *self, PyObject *args)
{
	int fd;
	char *mode = "r";
	int bufsize = -1;
	FILE *fp;
	PyObject *f;
	if (!PyArg_ParseTuple(args, "i|si:fdopen", &fd, &mode, &bufsize))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	fp = fdopen(fd, mode);
	Py_END_ALLOW_THREADS
	if (fp == NULL)
		return amiga_error();
	f = PyFile_FromFile(fp, "(fdopen)", mode, fclose);
	if (f != NULL)
		PyFile_SetBufSize(f, bufsize);
	return f;
}

#if 0
/*** XXX pipe() is useless without fork() or threads ***/
/***     TODO: guess what.. implement threads! ***/
static int pipe(int *fildes)
{
	/* 0=ok, -1=err, errno=EMFILE,ENFILE,EFAULT */
	char buf[50];
	static int num = 1;
	
	sprintf(buf,"PIPE:Py%ld_%ld",FindTask(0),num++);
	fildes[0]=open(buf,O_RDONLY,0);
	if(fildes[0]>0)
	{
		fildes[1]=open(buf,O_WRONLY|O_CREAT,FIBF_OTR_READ|FIBF_OTR_WRITE);
		if(fildes[1]>0)
		{
			return 0;
		}
		close(fildes[0]);
	}
	return -1;
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_pipe__doc__[] =
"pipe() -> (read_end, write_end)\n\
Create a pipe.";
#else
#define amiga_pipe__doc__ NULL
#endif

static PyObject *
amiga_pipe(PyObject *self, PyObject *args)
{
	int fds[2];
	int res;
	if (!PyArg_ParseTuple(args, ""))
		return NULL;
	Py_BEGIN_ALLOW_THREADS
	res = pipe(fds);
	Py_END_ALLOW_THREADS
	if (res != 0)
		return amiga_error();
	return Py_BuildValue("(ii)", fds[0], fds[1]);
}
#endif


static PyObject *
amiga_fullpath(PyObject *self, PyObject *args)
{
	BOOL ok=FALSE;
	BPTR lk;
	char *path;
	char buf[MAXPATHLEN];

	if (!PyArg_ParseTuple(args, "s:fullpath", &path)) return NULL;

	Py_BEGIN_ALLOW_THREADS
	if(lk=Lock(path,SHARED_LOCK))
	{
		ok=NameFromLock(lk,buf,sizeof(buf));
		UnLock(lk);
	}
	Py_END_ALLOW_THREADS

	if(!ok)
	{
		errno=__io2errno(_OSERR=IoErr());
		return amiga_error();
	}
	else return PyString_FromString(buf);
}

#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_putenv__doc__[] =
"putenv(key, value) -> None\n\
Change or add an environment variable.";
#else
#define amiga_putenv__doc__ NULL
#endif

static PyObject *amiga_putenv(PyObject *self, PyObject *args)
{
	char *s1, *s2;

	if (!PyArg_ParseTuple(args, "ss:putenv", &s1, &s2)) return NULL;
	if(setenv(s1,s2,1))
	{
		amiga_error(); return NULL;
	}
	
	Py_INCREF(Py_None); return Py_None;
}

#ifdef HAVE_STRERROR
#ifndef PYTHON_EXCLUDE_DOCSTRINGS
static char amiga_strerror__doc__[] =
"strerror(code) -> string\n\
Translate an error code to a message string.";
#else
#define amiga_strerror__doc__ NULL
#endif

PyObject *
amiga_strerror(PyObject *self, PyObject *args)
{
	int code;
	char *message;
	if (!PyArg_ParseTuple(args, "i:strerror", &code))
		return NULL;
	message = strerror(code);
	if (message == NULL) {
		PyErr_SetString(PyExc_ValueError,
				"strerror code out of range");
		return NULL;
	}
	return PyString_FromString(message);
}
#endif /* strerror */

#ifndef AROS
// external function prototype:
unsigned long __asm CalcCRC32(register __d0 ULONG startcrc, register __a0 const void *data, register __d1 unsigned long size);

PyObject *
amiga_crc32(PyObject *self, PyObject *args)
{
	PyObject *py_str;
	int startcrc = 0;
	if(!PyArg_ParseTuple(args,"S|i",&py_str,&startcrc)) return NULL;
	return PyInt_FromLong(CalcCRC32(startcrc,PyString_AsString(py_str), PyString_Size(py_str)));
}
#endif

static struct PyMethodDef amiga_methods[] = {
    { "chdir",     amiga_chdir,     METH_VARARGS, amiga_chdir__doc__ 	 },
    { "chmod",     amiga_chmod,     METH_VARARGS, amiga_chmod__doc__ 	 },
#ifdef HAVE_CHOWN
    { "chown",     amiga_chown,     METH_VARARGS, amiga_chown__doc__ 	 },
#endif
#ifdef HAVE_GETCWD
    { "getcwd",    amiga_getcwd,    METH_VARARGS, amiga_getcwd__doc__ 	 },
#endif
    { "fullpath",  amiga_fullpath,  METH_VARARGS, NULL      	    	 },
#ifdef HAVE_LINK
    { "link",      amiga_link,      METH_VARARGS, amiga_link__doc__ 	 },
#endif
    { "listdir",   amiga_listdir,   METH_VARARGS, amiga_listdir__doc__   },
    { "lstat",     amiga_lstat,     METH_VARARGS, amiga_lstat__doc__ 	 },
    { "mkdir",     amiga_mkdir ,    METH_VARARGS, amiga_mkdir__doc__ 	 },
#ifdef HAVE_READLINK
    { "readlink",  amiga_readlink,  METH_VARARGS, amiga_readlink__doc__  },
#endif
    { "rename",    amiga_rename,    METH_VARARGS, amiga_rename__doc__ 	 },
    { "rmdir",     amiga_rmdir,     METH_VARARGS, amiga_rmdir__doc__ 	 },
    { "stat",      amiga_stat,      METH_VARARGS, amiga_stat__doc__ 	 },
#ifdef HAVE_SYMLINK
    { "symlink",   amiga_symlink,   METH_VARARGS, amiga_symlink__doc__   },
#endif
#ifdef HAVE_SYSTEM
    { "system",    amiga_system,    METH_VARARGS, amiga_system__doc__ 	 },
#endif
#if defined AMITCP || defined INET225 || defined AROS
    { "umask",     amiga_umask,     METH_VARARGS, amiga_umask__doc__ 	 },
#endif
#ifdef HAVE_UNAME
    { "uname",     amiga_uname,     METH_VARARGS, amiga_uname__doc__ 	 },
#endif
    { "unlink",    amiga_unlink,    METH_VARARGS, amiga_unlink__doc__ 	 },
    { "remove",    amiga_unlink,    METH_VARARGS, amiga_remove__doc__ 	 },
#if defined AMITCP || defined INET225 || defined AROS 
    { "utime",     amiga_utime,     METH_VARARGS, amiga_utime__doc__ 	 },
#endif
#ifdef HAVE_GETEGID
    { "getegid",   amiga_getegid,   METH_VARARGS, amiga_getegid__doc__   },
#endif
#ifdef HAVE_GETEUID
    { "geteuid",   amiga_geteuid,   METH_VARARGS, amiga_geteuid__doc__   },
#endif
#ifdef HAVE_GETGID
    { "getgid",    amiga_getgid,    METH_VARARGS, amiga_getgid__doc__ 	 },
#endif
    { "getpid",    amiga_getpid,    METH_VARARGS, amiga_getpid__doc__ 	 },
#ifdef HAVE_GETPGRP
    { "getpgrp",   amiga_getpgrp,   METH_VARARGS, amiga_getpgrp__doc__   },
#endif
#ifdef HAVE_GETPPID
    { "getppid",   amiga_getppid,   METH_VARARGS, amiga_getppid__doc__   },
#endif
#ifdef HAVE_GETUID
    { "getuid",    amiga_getuid,    METH_VARARGS, amiga_getuid__doc__ 	 },
#endif
#ifdef HAVE_POPEN
    { "popen",     amiga_popen,     METG_VARARGS, amiga_popen__doc__ 	 },
#endif
#ifdef HAVE_SETUID
    { "setuid",    amiga_setuid,    METH_VARARGS, amiga_setuid__doc__ 	 },
#endif
#ifdef HAVE_SETGID
    { "setgid",    amiga_setgid,    METH_VARARGS, amiga_setgid__doc__    },
#endif
#ifdef HAVE_SETPGRP
    { "setpgrp",   amiga_setpgrp,   METH_VARARGS, amiga_setpgrp__doc__   },
#endif
#ifdef HAVE_SETSID
    { "setsid",    amiga_setsid,    METH_VARARGS, amiga_setsid__doc__    },
#endif
#ifdef HAVE_SETPGID
    { "setpgid",   amiga_setpgid,   METH_VARARGS, amiga_setpgid__doc__   },
#endif
    { "open",      amiga_open,      METH_VARARGS, amiga_open__doc__ 	 },
    { "close",     amiga_close,     METH_VARARGS, amiga_close__doc__ 	 },
#if defined AMITCP || defined INET225 || defined AROS
    { "dup",       amiga_dup,       METH_VARARGS, amiga_dup__doc__  	 },
    { "dup2",      amiga_dup2,      METH_VARARGS, amiga_dup2__doc__ 	 },
#endif
    { "lseek",     amiga_lseek,     METH_VARARGS, amiga_lseek__doc__ 	 },
    { "read",      amiga_read,      METH_VARARGS, amiga_read__doc__ 	 },
    { "write",     amiga_write,     METH_VARARGS, amiga_write__doc__ 	 },
    { "fstat",     amiga_fstat,     METH_VARARGS, amiga_fstat__doc__ 	 },
    { "fdopen",    amiga_fdopen,    METH_VARARGS, amiga_fdopen__doc__    },
#ifdef HAVE_FTRUNCATE
    { "ftruncate", amiga_ftruncate, METH_VARARGS, amiga_ftruncate__doc__ },
#endif
#ifdef HAVE_PUTENV
    { "putenv",    amiga_putenv,    METH_VARARGS, amiga_putenv__doc__    },
#endif
#ifdef HAVE_STRERROR
    { "strerror",  amiga_strerror,  METH_VARARGS, amiga_strerror__doc__  },
#endif
#if 0
    /* XXX TODO: implement threads. Otherwise pipe() is useless. */
    {"pipe",       amiga_pipe,      METH_VARARGS, amiga_pipe__doc__      },
#endif
#ifndef AROS
    {"crc32",      amiga_crc32,     METH_VARARGS, NULL                   },
#endif

    {NULL,         NULL}
};


static int
ins(PyObject *d, char *symbol, long value)
{
        PyObject* v = PyInt_FromLong(value);
        if (!v || PyDict_SetItemString(d, symbol, v) < 0)
                return -1;                   /* triggers fatal error */

        Py_DECREF(v);
        return 0;
}

static int all_ins(PyObject *d)
{
#ifdef WNOHANG
        if (ins(d, "WNOHANG", (long)WNOHANG)) return -1;
#endif        
#ifdef O_RDONLY
        if (ins(d, "O_RDONLY", (long)O_RDONLY)) return -1;
#endif
#ifdef O_WRONLY
        if (ins(d, "O_WRONLY", (long)O_WRONLY)) return -1;
#endif
#ifdef O_RDWR
        if (ins(d, "O_RDWR", (long)O_RDWR)) return -1;
#endif
#ifdef O_NDELAY
        if (ins(d, "O_NDELAY", (long)O_NDELAY)) return -1;
#endif
#ifdef O_NONBLOCK
        if (ins(d, "O_NONBLOCK", (long)O_NONBLOCK)) return -1;
#endif
#ifdef O_APPEND
        if (ins(d, "O_APPEND", (long)O_APPEND)) return -1;
#endif
#ifdef O_DSYNC
        if (ins(d, "O_DSYNC", (long)O_DSYNC)) return -1;
#endif
#ifdef O_RSYNC
        if (ins(d, "O_RSYNC", (long)O_RSYNC)) return -1;
#endif
#ifdef O_SYNC
        if (ins(d, "O_SYNC", (long)O_SYNC)) return -1;
#endif
#ifdef O_NOCTTY
        if (ins(d, "O_NOCTTY", (long)O_NOCTTY)) return -1;
#endif
#ifdef O_CREAT
        if (ins(d, "O_CREAT", (long)O_CREAT)) return -1;
#endif
#ifdef O_EXCL
        if (ins(d, "O_EXCL", (long)O_EXCL)) return -1;
#endif
#ifdef O_TRUNC
        if (ins(d, "O_TRUNC", (long)O_TRUNC)) return -1;
#endif
#ifdef O_BINARY
        if (ins(d, "O_BINARY", (long)O_BINARY)) return -1;
#endif
#ifdef O_TEXT
        if (ins(d, "O_TEXT", (long)O_TEXT)) return -1;
#endif

#if defined(PYOS_OS2)
        if (insertvalues(d)) return -1;
#endif
        return 0;
}

void
initamiga(void)
{
	PyObject *m, *d, *globv, *locv, *bothv, *aliases;

	m = Py_InitModule("amiga", amiga_methods);
	d = PyModule_GetDict(m);
	
	/* Initialize amiga.environ dictionary */
	if(!convertenviron(&globv, &locv, &bothv, &aliases))
		Py_FatalError("can't read environment");

	if (PyDict_SetItemString(d, "environ", bothv) != 0)
		Py_FatalError("can't define amiga.environ");
	Py_DECREF(bothv);
	if (PyDict_SetItemString(d, "globalvars", globv) != 0)
		Py_FatalError("can't define amiga.globalvars");
	Py_DECREF(globv);
	if (PyDict_SetItemString(d, "shellvars", locv) != 0)
		Py_FatalError("can't define amiga.shellvars");
	Py_DECREF(locv);
	if (PyDict_SetItemString(d, "shellaliases", aliases ) != 0)
		Py_FatalError("can't define amiga.shellaliases");
	Py_DECREF(aliases);

	if(all_ins(d)) return;

	/* Initialize exception */
	PyDict_SetItemString(d, "error", PyExc_OSError);
}
