/*
# Extended Environment module.
#
# Intended for use on the Amiga. Made by Irmen de Jong.
# Adapted for AROS by Adam Chodorowski.
#
# This module contains all sorts of functions that operate on the global
# or local environment of Python. Global environment = ENV: -vars,
# local environment = the local shell variables.
#
#  11-Apr-96:    Rewrite. This module does not have to be posix compliant
#                or portable, so...
#  12-Jun-96:    Fixed zero-length var bugs. Renamed to new symbolnames.
#  18-jan-98:    Updated for Python1.5
#   4-Oct-01:    Ported to AROS-
*/

#include "Python.h"
#include <stdlib.h>
#include <string.h>

/* protos */
static PyObject *put_environ Py_PROTO((PyObject *self, PyObject *args));
static PyObject *get_environ Py_PROTO((PyObject *self, PyObject *args));
static PyObject *set_environ Py_PROTO((PyObject *self, PyObject *args));
static PyObject *unset_environ Py_PROTO((PyObject *self, PyObject *args));
static PyObject *get_var Py_PROTO((PyObject *self, PyObject *args));
static PyObject *set_var Py_PROTO((PyObject *self, PyObject *args));
static PyObject *unset_var Py_PROTO((PyObject *self, PyObject *args));

 
static struct PyMethodDef environment_methods[] = {
        {"putenv", put_environ,1},
        {"setenv", set_environ,1},
        {"getenv", get_environ,1},
        {"unsetenv", unset_environ,1},
#if defined(_AMIGA) || defined(AROS)
        {"setvar", set_var,1},
        {"getvar", get_var,1},
        {"unsetvar", unset_var,1},
#endif
        {NULL, NULL}
};
 

void initenvironment Py_PROTO((void))
{
	(void)Py_InitModule("environment", environment_methods);
}
   
/*
**  putenv("name=value")	-- this is putenv(3)
*/
static PyObject *put_environ(PyObject *self, PyObject *args)
{
	char *string;
	if(PyArg_ParseTuple(args,"s",&string))
	{
		if ( putenv( string ) )
		{
			PyErr_SetString(PyExc_SystemError, "Error in system putenv call");
			return NULL;
		}
		Py_INCREF(Py_None); return Py_None;
	}
	return NULL;
}

/*
** setenv("name","value",overwrite?)
*/
static PyObject *set_environ(PyObject *self, PyObject *args)
{
	char *name, *value;
	int overwrite;

	if(PyArg_ParseTuple(args,"ssi",&name,&value,&overwrite))
	{
		if ( setenv(name,value,overwrite) )
		{
			PyErr_SetString(PyExc_SystemError, "Error in system setenv call");
			return NULL;
		}
		Py_INCREF(Py_None); return Py_None;
	}
	return NULL;
}

/*
** value = getenv("name")
*/
static PyObject *get_environ(PyObject *self, PyObject *args)
{
	char *name, *val;
	if(PyArg_ParseTuple(args,"s",&name))
	{
		if(val=getenv(name))
		{
			PyObject *s = PyString_FromString(val);
			free(val);
			return s; /* ok if s=NULL */
		}
		Py_INCREF(Py_None); return Py_None; /* var not found in env. */
	}
	return NULL;
}

/*
** unsetenv("name")
*/
static PyObject *unset_environ(PyObject *self, PyObject *args)
{
	char *string;
	if(PyArg_ParseTuple(args,"s",&string))
	{
		unsetenv(string);
		Py_INCREF(Py_None); return Py_None;
	}
	return NULL;
}

#if defined(_AMIGA) || defined(AROS)
#include <proto/dos.h>
#include <dos/var.h>

/*
** setvar("name","value",overwrite?)
*/
static PyObject *set_var(PyObject *self, PyObject *args)
{
	char *name, *value;
	int overwrite;

	if(PyArg_ParseTuple(args,"ssi",&name,&value,&overwrite))
	{
		if(!overwrite && FindVar(name,GVF_LOCAL_ONLY))
		{
			Py_INCREF(Py_None); return Py_None;
		}

		if(SetVar(name,value,-1,GVF_LOCAL_ONLY))
		{
			Py_INCREF(Py_None); return Py_None;
		}		
		PyErr_SetString(PyExc_SystemError, "Error in dos SetVar call");
		return NULL;
	}
	return NULL;
}

/*
** value = getvar("name")
*/
static PyObject *get_var(PyObject *self, PyObject *args)
{
	char *name;
	char buf[200];

	if(PyArg_ParseTuple(args,"s",&name))
	{
		if(GetVar(name,buf,200,GVF_LOCAL_ONLY)>=0)
		{
			PyObject *s = PyString_FromString(buf);
			return s; /* ok if s=NULL */
		}
		Py_INCREF(Py_None); return Py_None; /* var not found */
	}
	return NULL;
}

/*
** unsetvar("name")
*/
static PyObject *unset_var(PyObject *self, PyObject *args)
{
	char *string;
	if(PyArg_ParseTuple(args,"s",&string))
	{
		DeleteVar(string,GVF_LOCAL_ONLY);
		Py_INCREF(Py_None); return Py_None;
	}
	return NULL;
}

#endif
